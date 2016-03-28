#pragma once
#include <vector>
#include <map>
using namespace std;

static int NUM_SAMPLES = 3200;
static int RANDOM_GLITCH = 97;

static int MAX_STR = 1800;
static float MAX_THREAT = 12.0f;
static float MAX_RANGE = 8.0f;
static float MAX_COMBATANTS = 4.0f;

static string mypath = "C:\\trainingfiles.txt";

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