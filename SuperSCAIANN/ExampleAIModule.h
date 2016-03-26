#pragma once
#include <BWAPI.h>
#include <vector>

using std::vector;

class ANN;

// Remember not to use "Broodwar" in any global class constructor!

class ExampleAIModule : public BWAPI::AIModule
{
public:
	class ANN *ANNInstance;

	// everything above is also safe to modify :-D
	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);

	float evaluateArmy(const BWAPI::Unitset &units);
	void PopulateTeamScores();

	const static float GAS_MINERAL_COST_RATIO;
	// Everything below this line is safe to modify.

private:
	size_t frameCountForAIEval = 120; //wait X frames to rethink AI
	size_t frameCounter = 0;

	float lastAllyStrength;
	float lastEnemyStrength;

	vector<int> aggroTable;
	void FillAggroTable();
	void ClearAggroTable();

	Position UnitRunAway(BWAPI::Unit unit, int radius);

	int greatestEnemyAttackRange;
	void SetMaxEnemyRange();
};
