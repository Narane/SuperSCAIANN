#pragma once
#include <BWAPI.h>
#include <vector>
#include <string>

// do not ask why this is called jefftang

using std::vector;
using std::string;

class ANN;


class Jefftang : public BWAPI::AIModule
{
public:
	class ANN *ANNInstance;

	// Leaving these in because of callbacks
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
	void IssueOrders();
	void DoDebugDraws();
	
size_t frameCountForAIEval = 15; //wait X frames to rethink AI
	size_t frameCounter = 0;

	float lastAllyStrength;
	float lastEnemyStrength;

	float originalAllyStrength;
	float originalEnemyStrength;

	vector<int> aggroTable;
	void FillAggroTable();
	void ClearAggroTable();

	vector<bool> alreadyRanTable;

	BWAPI::Position UnitRunAway(BWAPI::Unit unit, int radius);

	int greatestEnemyAttackRange;
	void SetMaxEnemyRange();

	// the average % health remaining in the team
	float teamAverageHealthiness;
	void SetTeamHealthPct();

	void logResult(string s);
};
