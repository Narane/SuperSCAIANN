#include <vector>
#include <math.h>
using std::vector;

class ANN
{
public:
	void BuildModel();
	void TrainWithData(const vector<float>& TrainInputs, const vector<float>& TrainOutputs);
	void CalculateLayers(const vector<float>& InputValues);

	vector<float> HiddenLayerValues;
	vector<float> OutputLayerValues;
	vector<float> HiddenLayerTot; //HiddenLayerValues before the activation function
	vector<float> OutputLayerTot; //OutputLayerValues before the activation function

	vector<float> NNInputs;

	float ActivationFunction(float X);
	float ActivationFunctionDrv(float X);

	vector<vector<float>> ActivationMatrix(vector<vector<float>> const& InputMat);
	vector<vector<float>> ActivationMatrixDrv(vector<vector<float>> const& InputMat);

	vector<vector<float>> WeightsInputToHidden;
	vector<vector<float>> WeightsHiddenToOutput;

	// The weights should be 0~1; the upperbound is beacause it's nice to keep starting weights low
	void RandomizeWeights(vector<vector<float>>& WeightMatrix, float UpperBound, size_t LeftNodes, size_t RightNodes);
	void InitializeBias(vector<vector<float>>& BiasVector, size_t NodeCount);

private:
	const size_t HiddenLayerNodeCount = 5;
	const size_t InputNodeCount = 6;
	const size_t OutputNodeCount = 2;

	// Gradient descent parameters
	const float epsilon = 0.05f; // learning rate
	const float eta = 0.01f; // regularization power
};