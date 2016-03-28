// basic file operations
#include "Generator.h"
#include <iostream>
#include <fstream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
using namespace std;

/*
void saveModel(vector <vector<float> > model)
{
ofstream outputFile;
outputFile.open(MODEL_FILE_LOC);

outputFile << model.size() << endl;
outputFile << model[0].size() << endl;

for (vector<float> modelRow : model)
{
for (float val : modelRow)
{
outputFile << val << " ";
}
outputFile << endl;
}
outputFile.close();
}*/

Generator::Generator()
{
}

Generator::~Generator()
{
}

std::vector<std::string>& Generator::split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> Generator::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

map< vector<vector<float> >, vector<vector<float> > > Generator::loadData()
{
	ifstream inputFile(mypath);

	int numRows;
	int numCols;
	int numOutputs;

	string line, token;
	getline(inputFile, line);

	vector<string> tokens = split(line, ',');
	numRows = atoi(tokens[0].c_str());
	numCols = atoi(tokens[1].c_str());
	numOutputs = atoi(tokens[2].c_str());

	map< vector<vector<float> >, vector<vector<float> > > model;
	vector<vector<float> > inputMatrix;
	vector<vector<float> > outputMatrix;


	vector<float> ip(numCols - 1);
	inputMatrix.push_back(ip);

	vector<float> ip2(numOutputs);
	outputMatrix.push_back(ip2);

	int tokenIndex = 0;

	for (int i = 0; i < numRows; i++)
	{
		//cout << line << " ";
		getline(inputFile, line);
		tokenIndex = 0;
		tokens = split(line, ',');
		for (int j = 0; j < numCols - 1; j++)
		{
			float val = atof(tokens[j].c_str());
			//cout << val << " ";
			inputMatrix[0][j] = val;
		}

		vector<float> ip2(numOutputs);
		int val = atoi(tokens[numCols - 1].c_str());
		ip2[val] = 1;
		outputMatrix[0] = ip2;
		if (model.find(inputMatrix) != model.end())
		{
			int ifddfg = 52;
			ifddfg += 3;
		}
		model[inputMatrix] = outputMatrix;

		//cout << endl;
	}

	inputFile.close();
	return model;
}

void Generator::generate() {
	ofstream myfile;
	myfile.open(mypath);

	//Num training samples, num cols
	myfile << NUM_SAMPLES * 3 << "," << 8 << "," << 2 << "\n";

	srand(time(NULL));


	//    class,        %,        ???            0 to MAX_STR,     0 to masStr,     [0,1]
	//Threat rating, Range, MyHealth, MyShield, pctOfTeamHealth, numAtkingUs, OurStrength, TheirStrength, Output Goal [0 = AtkMove, 1 = back?]

	//Zealot
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		float ourStr = ((i % 40) * MAX_STR) / 40.f;
		float theirStr = ((i % 80) * MAX_STR) / 80.f;
		float myHealth = (float)(i % 160) / 160.f;
		float myShield = (float)(i % 320) / 320.f;
		float pctOfTeamHealth = (float)(i % 640) / 640.f;
		int goal = 0;

		float numAtkingUs = (i % ((int)(MAX_COMBATANTS)+1)) / MAX_COMBATANTS;
		if (myHealth < 0.1f && myShield < 0.1f && pctOfTeamHealth < 0.5f && (numAtkingUs * MAX_COMBATANTS) >= 4)
		{
			goal = 1;
		}

		//Lol.
		if (rand() % 100 > RANDOM_GLITCH)
		{
			goal = goal ^ 1;
		}
		myfile << (float)3 / MAX_THREAT << "," << 1 / MAX_RANGE << "," << myHealth << "," << myShield << "," << pctOfTeamHealth << "," << numAtkingUs << "," << (double)ourStr / MAX_STR << "," << (double)theirStr / MAX_STR << "," << goal << endl;
	}


	//Dragoon
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		float ourStr = ((i % 40) * MAX_STR) / 40.f;
		float theirStr = ((i % 80) * MAX_STR) / 80.f;
		float myHealth = (float)(i % 160) / 160.f;
		float myShield = (float)(i % 320) / 320.f;
		float pctOfTeamHealth = (float)(i % 640) / 640.f;
		int goal = 0;

		float numAtkingUs = (i % ((int)(MAX_COMBATANTS)+1)) / MAX_COMBATANTS;
		if (myHealth < 1.0f && myShield < 0.2f && pctOfTeamHealth < 0.8f && (numAtkingUs * MAX_COMBATANTS) >= 1 && (ourStr / theirStr) < 1.2f)
		{
			goal = 1;
		}

		//Lol.
		if (rand() % 100 > RANDOM_GLITCH)
		{
			goal = goal ^ 1;
		}
		myfile << (float)8 / MAX_THREAT << "," << 4 / MAX_RANGE << "," << myHealth << "," << myShield << "," << pctOfTeamHealth << "," << numAtkingUs << "," << (double)ourStr / MAX_STR << "," << (double)theirStr / MAX_STR << "," << goal << endl;
	}


	//Archon
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		float ourStr = ((i % 40) * MAX_STR) / 40.f;
		float theirStr = ((i % 80) * MAX_STR) / 80.f;
		float myHealth = (float)(i % 160) / 160.f;
		float myShield = (float)(i % 320) / 320.f;
		float pctOfTeamHealth = (float)(i % 640) / 640.f;
		int goal = 0;

		float numAtkingUs = (i % ((int)(MAX_COMBATANTS)+1)) / MAX_COMBATANTS;
		if (myHealth < 1.0f && myShield <= 0.3f)
		//if (myHealth < 0.75f)
		{
			goal = 1;
		}

		//Lol.
		if (rand() % 100 > RANDOM_GLITCH)
		{
			goal = goal ^ 1;
		}
		myfile << (float)12 / MAX_THREAT << "," << 1.5 / MAX_RANGE << "," << myHealth << "," << myShield << "," << pctOfTeamHealth << "," << numAtkingUs << "," << (double)ourStr / MAX_STR << "," << (double)theirStr / MAX_STR << "," << goal << endl;
	}

	myfile.close();

	//map< vector<vector<float> >, vector<vector<float> > > trainingSet = loadData();

	/*
	for (map< vector<vector<float> >, vector<vector<float> > >::iterator it = model.begin(); it != model.end(); it++) {
		vector<float> inputMatrix = it->first[0];
		vector<float> outputMatrix = it->second[0];

		for (int i = 0; i < inputMatrix.size(); i++)
		{
			cout << inputMatrix[i] << " ";
		}

		for (int i = 0; i < outputMatrix.size(); i++)
		{
			cout << inputMatrix[i] << " ";
		}
		cout << endl;
	}*/

	//string str;
	//cin >> str;
}