
// basic file operations
#include <iostream>
#include <fstream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <map>
using namespace std;

int NUM_SAMPLES = 1000;
int MAX_STR = 1800;
double RANDOM_GLITCH = 0.02;

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

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

map< vector<vector<float> >, vector<vector<float> > > loadData()
{
	ifstream inputFile("example.txt");

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
		model[inputMatrix] = outputMatrix;

		//cout << endl;
	}

	inputFile.close();
	return model;
}

int main() {
	ofstream myfile;
	myfile.open("example.txt");

	//Num training samples, num cols
	myfile << NUM_SAMPLES * 3 << "," << 6 << "," << 2 << "\n";




	srand(time(NULL));


	//0 Marine, 32 Firebat, 34 Medic
	//    class,        %,        ???            0 to MAX_STR,     0 to masStr,     [0,1]
	//Unit Type, MyHealth, numAtkingUs, OurStrength, TheirStrength, Output Goal [0 = AtkMove, 1 = back?]
	//Marine
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		int ourStr = i*MAX_STR / NUM_SAMPLES;
		int theirStr = ((i % 20) * MAX_STR) / 20;
		float myHealth = 1.0f * i / NUM_SAMPLES;
		int goal = 0;

		int numAtkingUs = i % 5;
		if (myHealth < 0.5f && numAtkingUs >= 1 && ourStr < theirStr && ourStr > 800)
		{
			goal = 1;
		}

		//Lol.
		if (rand() % 100 > RANDOM_GLITCH)
		{
			goal = goal ^ 1;
		}
		myfile << 0 << "," << myHealth << "," << numAtkingUs << "," << ourStr << "," << theirStr << "," << goal << endl;
	}


	//FB
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		int ourStr = i*MAX_STR / NUM_SAMPLES;
		int theirStr = ((i % 20) * MAX_STR) / 20;
		float myHealth = 1.0f * i / NUM_SAMPLES;
		int goal = 0;

		int numAtkingUs = i % 5;
		if (myHealth < 0.3f && numAtkingUs >= 1 && ourStr < theirStr && ourStr > 600)
		{
			goal = 1;
		}

		//Lol.
		if (rand() % 100 > RANDOM_GLITCH)
		{
			goal = goal ^ 1;
		}
		myfile << 32 << "," << myHealth << "," << numAtkingUs << "," << ourStr << "," << theirStr << "," << goal << endl;
	}


	//MEDIC
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		int ourStr = i*MAX_STR / NUM_SAMPLES;
		int theirStr = ((i % 20) * MAX_STR) / 20;
		float myHealth = 1.0f * i / NUM_SAMPLES;
		int goal = 0;

		int numAtkingUs = i % 5;
		if (myHealth < 0.75f && numAtkingUs >= 1)
		{
			goal = 1;
		}

		//Lol.
		if (rand() % 100 > RANDOM_GLITCH)
		{
			goal = goal ^ 1;
		}
		myfile << 34 << "," << myHealth << "," << numAtkingUs << "," << ourStr << "," << theirStr << "," << goal << endl;
	}

	myfile.close();

	map< vector<vector<float> >, vector<vector<float> > > trainingSet = loadData();

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
	return 0;
}