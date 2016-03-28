#include <vector>
#include <map>
#include <math.h>
using std::vector;
using std::map;

class ANN
{
public:
	// Init shenanigans
	void BuildModel();
	// Run this on a loop for training
	void TrainWithData(const vector<float>& TrainInputs, const vector<float>& TrainOutputs);
	// Given that the weights matrix is filled, write the results to the output vectors shown below
	void CalculateLayers(const vector<float>& InputValues);

	// Debugging and setup stage speed goes to hell with multiple hidden layers so let's go with 1 layer
	vector<float> HiddenLayerValues;
	vector<float> OutputLayerValues;
	vector<float> HiddenLayerTot; //HiddenLayerValues before the activation function
	vector<float> OutputLayerTot; //OutputLayerValues before the activation function

	// weightmatrix[input side][output side]
	vector<vector<float>> WeightsInputToHidden;
	vector<vector<float>> WeightsHiddenToOutput;

	float ActivationFunction(float X);
	float ActivationFunctionDrv(float X);

	// relic from doing matrix-based calculations to deal with multiple hidden layers
	vector<vector<float>> ActivationMatrix(vector<vector<float>> const& InputMat);
	vector<vector<float>> ActivationMatrixDrv(vector<vector<float>> const& InputMat);

	// The weights should be 0~1; the upperbound is beacause it's nice to keep starting weights low
	void RandomizeWeights(vector<vector<float>>& WeightMatrix, float UpperBound, size_t LeftNodes, size_t RightNodes);
	void InitializeBias(vector<vector<float>>& BiasVector, size_t NodeCount);

private:
	// Hard coded here, adjust as necessary
	const size_t HiddenLayerNodeCount = 5;
	
	// Dynamically gathered from the Generator test files
	size_t InputNodeCount;
	size_t OutputNodeCount;

	// Gradient descent parameters
	const float epsilon = 0.4f; // learning rate
	const float eta = 0.01f; // regularization power

	map< vector< vector<float> >, vector< vector<float> > > LoadedInputOutputData;
};