#include <vector>
#include <math.h>
using std::vector;

class ANN
{
public:
	void BuildModel();
	void Update(const vector<vector<float>>& X);

	vector<float> NNInputs;

	float ActivationFunction(float x);
	float ActivationFunctionDrv(float x);

	vector<vector<float>> ActivationMatrix(vector<vector<float>> const& inputMat);
	vector<vector<float>> ActivationMatrixDrv(vector<vector<float>> const& inputMat);

	vector<vector<float>> WeightsInputToHidden;
	vector<vector<float>> WeightsHiddenToOutput;

	vector<vector<float>> BiasInputToHidden;
	vector<vector<float>> BiasHiddenToOutput;

	// The weights should be 0~1; the upperbound is beacause it's nice to keep starting weights low
	void RandomizeWeights(vector<vector<float>>& WeightMatrix, float UpperBound, size_t InputNodeCount, size_t HiddenLayerNodeCount);
	void InitializeBias(vector<vector<float>>& BiasVector, size_t NodeCount);
};