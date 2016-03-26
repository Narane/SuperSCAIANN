#include "ExampleAIModule.h"
#include "ANN.h"
#include <iostream>
#include <string>

using namespace BWAPI;
using namespace Filter;

const float ExampleAIModule::GAS_MINERAL_COST_RATIO = 1.5f;

void ExampleAIModule::onStart()
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

void ExampleAIModule::FillAggroTable()
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

void ExampleAIModule::ClearAggroTable()
{
	for (size_t it = 0; it < aggroTable.size(); ++it)
	{
		aggroTable[it] = 0;
	}
}


Position ExampleAIModule::UnitRunAway(BWAPI::Unit unit, int radius)
{
	Position endPosition = unit->getPosition();
	int count = 0;
	Position enemyPosition = BWAPI::Positions::Origin;
	for (auto &u : unit->getUnitsInRadius(radius, !BWAPI::Filter::IsOwned))
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


void ExampleAIModule::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }

  delete ANNInstance;
}

float ExampleAIModule::evaluateArmy(const BWAPI::Unitset &units)
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

void ExampleAIModule::onFrame()
{
  // Called once every game frame
	frameCounter += 1;
	Broodwar->drawTextScreen(200, 40, "ALLY SCORE: %f ENEMY SCORE: %f", lastAllyStrength, lastEnemyStrength);

  // Display the game frame rate as text in the upper left area of the screen
  Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
  Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

  // Return if the game is a replay or is paused
  if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
    return;

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
    return;

  if (frameCounter > frameCountForAIEval)
  {
	  SetMaxEnemyRange();
	  PopulateTeamScores();
	  FillAggroTable();
  }
}

void ExampleAIModule::SetMaxEnemyRange()
{
	// greatestEnemyAttackRange not set yet; start it up
	if (greatestEnemyAttackRange < 0)
	{
		BWAPI::Unitset enemyUnits = Broodwar->enemies().getUnits();
		for (const BWAPI::Unit &unit : enemyUnits)
		{
			const WeaponType gw = unit->getType().groundWeapon();
			const WeaponType aw = unit->getType().airWeapon();
			if (gw.maxRange() > greatestEnemyAttackRange)
			{
				greatestEnemyAttackRange = gw.maxRange();
			}
			if (aw.maxRange() > greatestEnemyAttackRange)
			{
				greatestEnemyAttackRange = aw.maxRange();
			}
		}
	}
}

void ExampleAIModule::PopulateTeamScores()
{
	frameCounter = 0;
	BWAPI::Unitset allyUnits = Broodwar->self()->getUnits();
	BWAPI::Unitset enemyUnits = Broodwar->enemies().getUnits();

	lastAllyStrength = evaluateArmy(allyUnits);
	lastEnemyStrength = evaluateArmy(enemyUnits);
}

void ExampleAIModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
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

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
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

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
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

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
}

