#include <vector>
#include <math.h>
using std::vector;

class ANN
{
public:
	vector<float> NNInputs;

	float ActivationFunction(float x);
	float ActivationFunctionDrv(float x);

	vector<vector<float>> ActivationMatrix(vector<vector<float>> const& inputMat);
	vector<vector<float>> ActivationMatrixDrv(vector<vector<float>> const& inputMat);
};