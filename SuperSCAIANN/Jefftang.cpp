#include "Jefftang.h"
#include "ANN.h"
#include "Helpers.h"
#include "Generator.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using std::string;

// do not ask why this is called jefftang

using namespace BWAPI;
using namespace Filter;

const float Jefftang::GAS_MINERAL_COST_RATIO = 1.5f;

void Jefftang::onStart()
{
	originalAllyStrength = 0;
	originalEnemyStrength = 0;
	greatestEnemyAttackRange = -1;

	frameCounter = 0;
	aggroTable.resize(4096);
	alreadyRanTable.resize(4096);

	ANNInstance = new ANN();
	ANNInstance->BuildModel();

	// Hello World!
	Broodwar->sendText("We're going SUPER SAIANN!");

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	if ( Broodwar->isReplay() )
	{
		return;
	}
	else // if this is not a replay
	{
		//Disable fog of war
		Broodwar->sendText("war aint what it used to be");

		//Full vision of map
		Broodwar->sendText("black sheep wall");

	}
}

void Jefftang::FillAggroTable()
{
	ClearAggroTable();
	BWAPI::Unitset enemyUnits = Broodwar->enemies().getUnits();
	for (Unitset::const_iterator i = enemyUnits.begin(); i != enemyUnits.end(); ++i) {
		const Unit attackTarget = (*i)->getOrderTarget();
		if (attackTarget != nullptr /*&& attackTarget->getPlayer()->getID() == Broodwar->self()->getID()*/)
		{
			const int unitID = attackTarget->getID();
			if (unitID >= 0 && unitID < aggroTable.size())
			{
				aggroTable[unitID] += 1;
			}
		}
	}
}

void Jefftang::ClearAggroTable()
{
	for (size_t it = 0; it < aggroTable.size(); ++it)
	{
		aggroTable[it] = 0;
	}
}


BWAPI::Position Jefftang::UnitRunAway(BWAPI::Unit unit, int radius)
{
	// Just for the test map, a simpler location choosing
	Position endPosition = unit->getPosition();
	endPosition.x -= (0 > radius * 32) ? 0 : radius * 32;
	/*
	int count = 0;
	Position enemyPosition = BWAPI::Positions::Origin;
	for (auto &u : unit->getUnitsInRadius(radius, BWAPI::Filter::IsEnemy))
	{
		++count;
		enemyPosition.x += u->getPosition().x;
		enemyPosition.y += u->getPosition().y;
	}

	if (count != 0)
	{
		enemyPosition.x /= count;
		enemyPosition.y /= count;

		endPosition.x = 2 * endPosition.x - enemyPosition.x;
		endPosition.y = 2 * endPosition.y - enemyPosition.y;
	}
	*/
	return endPosition;
}


void Jefftang::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }

  float a = ANNInstance->validationPercentage;
  
  BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();
  BWAPI::Unitset enemyUnits = Broodwar->enemies().getUnits();

  float allyStr = evaluateArmy(allyUnits);
  float enemyStr = evaluateArmy(enemyUnits);

  std::ostringstream out;
  out << "File: " << Broodwar->mapFileName() << ", VALIDATION PERCENTAGE: " << a << " ORIGINAL ALLY STR " << originalAllyStrength << " ORIGIN ENEMY STR " << originalEnemyStrength << " ALLY STR " << allyStr << " ENEMY STR " << enemyStr << endl;

	logResult(out.str());
  delete ANNInstance;
}

void Jefftang::logResult(string result)
{
	ofstream myfile;
	myfile.open("C:\\Users\\Ornalth\\Desktop\\bwapiAI\\SuperSCAIANN\\Data\\results" + std::to_string(ANNInstance->HiddenLayerNodeCount) + string(".txt"), std::fstream::out | std::fstream::app);
	myfile << result;
	myfile.close();
}

float Jefftang::evaluateArmy(const BWAPI::Unitset &units)
{
	float score = 0;
	for (const BWAPI::Unit &unit : units)
	{
		
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!unit->exists())
			continue;

		
		int gasPrice = unit->getType().gasPrice();
		int mineralPrice = unit->getType().mineralPrice();

		//Archons apparently have 0 cost according to API, this is the cost of the 2 HTs used to merge for it.
		if (unit->getType() == BWAPI::UnitTypes::Protoss_Archon)
		{
			gasPrice = 300;
			mineralPrice = 100;
		}


		float totalPrice = mineralPrice + GAS_MINERAL_COST_RATIO * gasPrice;
		int hp = unit->getHitPoints();
		int maxhp = unit->getType().maxHitPoints();
		hp += unit->getShields();
		maxhp += unit->getType().maxShields();

		float hpPercent = (float)hp / maxhp;
		score += (hpPercent * totalPrice);
	}

	return score;
}

void Jefftang::onFrame()
{
  // Called once every game frame
	frameCounter += 1;
	Broodwar->drawTextScreen(200, 40, "ALLY SCORE: %f ENEMY SCORE: %f", lastAllyStrength, lastEnemyStrength);

  // Display the game frame rate as text in the upper left area of the screen
  Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
  Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

  DoDebugDraws();

  // Return if the game is a replay or is paused
  if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
    return;

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
    return;

  if (frameCounter > frameCountForAIEval)
  {
	  IssueOrders();
  }
}

void Jefftang::IssueOrders()
{
	// Set up the data
	SetMaxEnemyRange();
	PopulateTeamScores();
	FillAggroTable();
	SetTeamHealthPct();

	BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();
	for (const BWAPI::Unit &unit : allyUnits)
	{
		//Threat rating, Range, MyHealth, MyShield, pctOfTeamHealth, numAtkingUs, OurStrength, TheirStrength, Output Goal [0 = AtkMove, 1 = back?]
		vector<float> inputData;

		int threatRating = Helpers::GetUnitThreatRating(unit->getType());
		inputData.push_back((float)threatRating / MAX_THREAT);

		inputData.push_back((float)Helpers::GetMaxUnitAttackRange(unit->getType()) / MAX_RANGE);

		const float myHealthPct = (float)unit->getHitPoints() / unit->getType().maxHitPoints();
		inputData.push_back(myHealthPct);

		inputData.push_back((float)unit->getShields() / unit->getType().maxShields());

		float myHealthVsTeam = myHealthPct / teamAverageHealthiness;
		myHealthVsTeam = myHealthVsTeam > 1.0f ? 1.0f : myHealthVsTeam;
		inputData.push_back(myHealthVsTeam);

		const int unitID = unit->getID();
		inputData.push_back((MAX_COMBATANTS < aggroTable[unitID] ? MAX_COMBATANTS : (float)aggroTable[unitID]) / MAX_COMBATANTS);

		inputData.push_back((float)lastAllyStrength / MAX_STR);

		inputData.push_back((float)lastEnemyStrength / MAX_STR);

		ANNInstance->CalculateLayers(inputData);

		vector<float> results = ANNInstance->OutputLayerValues;

		// React to the output vector
		if (results[0] > results[1] || alreadyRanTable[unit->getID()] == true)
		{
			if (!unit->isAttacking())
			{
				const UnitType typeee = unit->getType();

				// Attack
				Unit foundEnemy = unit->getClosestUnit(BWAPI::Filter::IsEnemy);
				if (unit->canAttack() && typeee != BWAPI::UnitTypes::Terran_Medic)
				{
					unit->attack(foundEnemy);
				}
				else if (foundEnemy != nullptr)
				{
					unit->attack(foundEnemy->getPosition());
				}
			}
		}
		else
		{
			alreadyRanTable[unit->getID()] = true;
			unit->move(UnitRunAway(unit, greatestEnemyAttackRange));
		}
	}
}

void Jefftang::DoDebugDraws()
{
	BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();
	for (const BWAPI::Unit &unit : allyUnits)
	{
		Position startPos = unit->getPosition();
		Position endPos;
		BWAPI::Color colorVal = BWAPI::Color(0, 255, 0);
		if (unit->getOrderTarget() != nullptr)
		{
			endPos = unit->getOrderTargetPosition();
			if (!unit->isAttacking())
			{
				colorVal = BWAPI::Color(0, 0, 255);
			}
			else
			{
				colorVal = BWAPI::Color(255, 0, 0);
			}
		}
		else
		{
			endPos = unit->getTargetPosition();
		}
		Broodwar->drawLine(BWAPI::CoordinateType::Map, startPos.x, startPos.y, endPos.x, endPos.y, colorVal);
	}
}

void Jefftang::SetMaxEnemyRange()
{
	// greatestEnemyAttackRange not set yet; start it up
	if (greatestEnemyAttackRange < 0)
	{
		BWAPI::Unitset enemyUnits = Broodwar->enemies().getUnits();
		for (const BWAPI::Unit &unit : enemyUnits)
		{
			int range = Helpers::GetMaxUnitAttackRange(unit->getType());
			if (range > greatestEnemyAttackRange)
			{
				greatestEnemyAttackRange = Helpers::GetMaxUnitAttackRange(unit->getType());
			}
		}
	}
}

void Jefftang::SetTeamHealthPct()
{
	teamAverageHealthiness = 1.0f;
	BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();

	size_t unitCounter = 0;
	float cumulHealthPct = 0.0f;
	for (const BWAPI::Unit &unit : allyUnits)
	{
		++unitCounter;
		cumulHealthPct += ((float)unit->getHitPoints() / unit->getType().maxHitPoints());
	}
	teamAverageHealthiness = (cumulHealthPct / (float)unitCounter);
}

void Jefftang::PopulateTeamScores()
{
	frameCounter = 0;
	BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();
	BWAPI::Unitset enemyUnits = Broodwar->enemies().getUnits();

	lastAllyStrength = evaluateArmy(allyUnits);
	lastEnemyStrength = evaluateArmy(enemyUnits);

	if (originalAllyStrength < 1)
	{
		originalAllyStrength = lastAllyStrength;
	}
	if (originalEnemyStrength < 1)
	{
		originalEnemyStrength = lastEnemyStrength;
	}
}

void Jefftang::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void Jefftang::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void Jefftang::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void Jefftang::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void Jefftang::onUnitDiscover(BWAPI::Unit unit)
{
}

void Jefftang::onUnitEvade(BWAPI::Unit unit)
{
}

void Jefftang::onUnitShow(BWAPI::Unit unit)
{
}

void Jefftang::onUnitHide(BWAPI::Unit unit)
{
}

void Jefftang::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void Jefftang::onUnitDestroy(BWAPI::Unit unit)
{
}

void Jefftang::onUnitMorph(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void Jefftang::onUnitRenegade(BWAPI::Unit unit)
{
}

void Jefftang::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void Jefftang::onUnitComplete(BWAPI::Unit unit)
{
}

