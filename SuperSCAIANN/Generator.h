#pragma once
#include <vector>
#include <map>
using namespace std;

static int NUM_SAMPLES = 1000;
static int MAX_STR = 1800;
static double RANDOM_GLITCH = 0.02;
static string mypath = "C:\\Program Files (x86)\\StarCraft\\bwapi-data\\AI\\trainingfiles.txt";

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