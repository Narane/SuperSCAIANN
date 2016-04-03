#pragma once
#include <vector>
#include <map>
using namespace std;

static int NUM_SAMPLES = 1600;
static int RANDOM_GLITCH = 97;

static int MAX_STR = 2400;
static float MAX_THREAT = 12.0f;
static float MAX_RANGE = 6.0f;
static float MAX_COMBATANTS = 4.0f;

static string mypath = "C:\\Users\\Ornalth\\Desktop\\bwapiAI\\SuperSCAIANN\\Data\\trainingfiles.txt";

class Generator
{
public:
	Generator();
	~Generator();

	static void generate();
	static map< vector<vector<float> >, vector<vector<float> > > loadData();

private:
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	static std::vector<std::string> split(const std::string &s, char delim);
};