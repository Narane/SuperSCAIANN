#include <vector>
using std::vector;

class MatrixMaths
{
public:
	static vector<vector<float>> MatrixTranspose(const vector<vector<float>>& LHS);
	static vector<vector<float>> MatrixAdd(const vector<vector<float>>& LHS, const vector<vector<float>>& RHS);
	static vector<vector<float>> ScalarMultiply(const vector<vector<float>>& LHS, int RHS);
	static vector<vector<float>> MatrixMultiply(const vector<vector<float>>& LHS, const vector<vector<float>>& RHS);
	static vector<vector<float>> HadamardMultiply(const vector<vector<float>>& LHS, const vector<vector<float>>& RHS);
};