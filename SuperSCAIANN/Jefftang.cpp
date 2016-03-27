#include "Jefftang.h"
#include "ANN.h"
#include "Helpers.h"
#include <iostream>
#include <string>

// do not ask why this is called jefftang

using namespace BWAPI;
using namespace Filter;

const float Jefftang::GAS_MINERAL_COST_RATIO = 1.5f;

void Jefftang::onStart()
{
	greatestEnemyAttackRange = -1;

	frameCounter = 0;
	aggroTable.resize(4096);

	ANNInstance = new ANN();
	ANNInstance->BuildModel();

	// Hello World!
	Broodwar->sendText("Hello world!");

	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

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

	/*
	// Announce the players in the replay
	Broodwar << "The following players are in this replay:" << std::endl;
    
	// Iterate all the players in the game using a std:: iterator
	Playerset players = Broodwar->getPlayers();
	for(auto p : players)
	{
		// Only print the player if they are not an observer
		if ( !p->isObserver() )
		Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
	}
	*/
	}
	else // if this is not a replay
	{
	// Retrieve you and your enemy's races. enemy() will just return the first enemy.
	// If you wish to deal with multiple enemies then you must use enemies().
		if (Broodwar->enemy()) // First make sure there is an enemy
		{
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
		}

		//You're a cheater.

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
			aggroTable[unitID] += 1;
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
	Position endPosition = unit->getPosition();
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
	}

	endPosition.x = 2 * endPosition.x - enemyPosition.x;
	endPosition.y = 2 * endPosition.y - enemyPosition.y;

	return endPosition;
}


void Jefftang::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }

  delete ANNInstance;
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

		std::string hi = unit->getType().getName();

		int gasPrice = unit->getType().gasPrice();
		int mineralPrice = unit->getType().mineralPrice();
		float totalPrice = mineralPrice + GAS_MINERAL_COST_RATIO * gasPrice;
		int hp = unit->getHitPoints();
		int maxhp = unit->getType().maxHitPoints();

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

	BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();
	for (const BWAPI::Unit &unit : allyUnits)
	{
		//Threat rating, Range, MyHealth, numAtkingUs, OurStrength, TheirStrength, Output Goal [0 = AtkMove, 1 = back?]
		vector<float> inputData;

		int threatRating = Helpers::GetUnitThreatRating(unit->getType());
		inputData.push_back((float)threatRating);

		inputData.push_back((float)Helpers::GetMaxUnitAttackRange(unit->getType()));

		inputData.push_back((float)unit->getHitPoints() / unit->getType().maxHitPoints());

		const int unitID = unit->getID();
		inputData.push_back((float)aggroTable[unitID]);

		inputData.push_back((float)lastAllyStrength);

		inputData.push_back((float)lastEnemyStrength);

		ANNInstance->CalculateLayers(inputData);

		vector<float> results = ANNInstance->OutputLayerValues;

		// React to the output vector
		if (results[0] > results[1])
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
		else
		{
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

void Jefftang::PopulateTeamScores()
{
	frameCounter = 0;
	BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();
	BWAPI::Unitset enemyUnits = Broodwar->enemies().getUnits();

	lastAllyStrength = evaluateArmy(allyUnits);
	lastEnemyStrength = evaluateArmy(enemyUnits);
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

