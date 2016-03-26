#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ANN.h"
#include "MatrixMaths.h"



void ANN::BuildModel()
{
	RandomizeWeights(WeightsInputToHidden, 0.3f, InputNodeCount, HiddenLayerNodeCount);
	RandomizeWeights(WeightsHiddenToOutput, 0.3f, HiddenLayerNodeCount, OutputNodeCount);
	InitializeBias(BiasInputToHidden, HiddenLayerNodeCount);
	InitializeBias(BiasHiddenToOutput, OutputNodeCount);

	HiddenLayerValues.resize(HiddenLayerNodeCount);
	OutputLayerValues.resize(OutputNodeCount);
}

//
void ANN::TrainWithData(const vector<float>& TrainInputs, const vector<float>& TrainOutputs)
{
	CalculateLayers(TrainInputs);

	//Backprop

	// First we get the error of the output
	vector<float> outputError;
	outputError.resize(OutputNodeCount);
	float totalError = 0;
	for (size_t outputN = 0; outputN < OutputNodeCount; ++outputN)
	{
		outputError[outputN] = TrainOutputs[outputN] - OutputLayerValues[outputN];
		totalError += outputError[outputN];
	}

	// New weights for hidden to output
	vector<float> outputDeltas;
	outputDeltas.resize(OutputNodeCount);
	for (size_t outputN = 0; outputN < OutputNodeCount; ++outputN)
	{
		outputDeltas[outputN] = outputError[outputN] * ActivationFunctionDrv(OutputLayerTot[outputN]);
		for (size_t hiddenN = 0; hiddenN < HiddenLayerNodeCount; ++hiddenN)
		{
			const float diff = epsilon * outputDeltas[outputN] * HiddenLayerValues[hiddenN];
			WeightsHiddenToOutput[hiddenN][outputN] += diff;
		}
	}

	// New weights for input to hidden
	vector<float> inputDeltas;
	inputDeltas.resize(HiddenLayerNodeCount);
	for (size_t hiddenN = 0; hiddenN < HiddenLayerNodeCount; ++hiddenN)
	{
		float temp = 0;
		for (size_t outputN = 0; outputN < OutputNodeCount; ++outputN)
		{
			temp += WeightsHiddenToOutput[hiddenN][outputN] * outputDeltas[outputN];
		}
		inputDeltas[hiddenN] = temp * ActivationFunctionDrv(HiddenLayerTot[hiddenN]);
		for (size_t inputN = 0; inputN < InputNodeCount; ++inputN)
		{
			const float diff = epsilon * inputDeltas[hiddenN] * TrainInputs[inputN];
			WeightsInputToHidden[inputN][hiddenN] += diff;
		}
	}
}

void ANN::CalculateLayers(const vector<float>& InputValues)
{
	if (InputValues.size() != InputNodeCount)
	{
		// uh oh
		return;
	}

	// Input layer to hidden layer
	for (size_t hiddenN = 0; hiddenN < HiddenLayerNodeCount; ++hiddenN)
	{
		HiddenLayerValues[hiddenN] = 0.0f;
		for (size_t inputN = 0; inputN < InputNodeCount; ++inputN)
		{
			HiddenLayerValues[hiddenN] += WeightsInputToHidden[inputN][hiddenN] * InputValues[inputN];
		}
		HiddenLayerTot[hiddenN] = HiddenLayerValues[hiddenN];
		HiddenLayerValues[hiddenN] = ActivationFunction(HiddenLayerValues[hiddenN]);
	}

	for (size_t outputN = 0; outputN < OutputNodeCount; ++outputN)
	{
		OutputLayerValues[outputN] = 0;
		for (size_t hiddenN = 0; hiddenN < HiddenLayerNodeCount; ++hiddenN)
		{
			OutputLayerValues[outputN] += WeightsHiddenToOutput[hiddenN][outputN] * HiddenLayerValues[hiddenN];
		}
		OutputLayerTot[outputN] = OutputLayerValues[outputN];
		OutputLayerValues[outputN] = ActivationFunction(OutputLayerValues[outputN]);
	}
}


float ANN::ActivationFunction(const float X)
{
	return (tanh(X) + 1.0f) / 2.0f;
}

float ANN::ActivationFunctionDrv(const float X)
{
	const float tanout = tanh(X);
	return (1.0f - tanout*tanout) / 2.0f;
}

// Take a matrix, return a copy matrix that applies the activation functions element-wise
vector<vector<float>> ANN::ActivationMatrix(vector<vector<float>> const& InputMat)
{
	vector<vector<float>> returnMat;
	const size_t rowCount = InputMat.size();
	if (InputMat.size() == rowCount)
	{
		return returnMat;
	}

	const size_t colCount = InputMat[0].size();
	for (size_t row = 0; row < returnMat.size(); ++row)
	{
		returnMat[row].resize(colCount);
	}

	for (size_t row = 0; row < rowCount; ++row)
	{
		for (size_t col = 0; col < colCount; ++col)
		{
			returnMat[row][col] = ActivationFunction(InputMat[row][col]);
		}
	}
}

// Take a matrix, return a copy matrix that applies the derivative of the activation functions element-wise
vector<vector<float>> ANN::ActivationMatrixDrv(vector<vector<float>> const& InputMat)
{
	vector<vector<float>> returnMat;
	const size_t rowCount = InputMat.size();
	if (InputMat.size() == rowCount)
	{
		return returnMat;
	}

	const size_t colCount = InputMat[0].size();
	for (size_t row = 0; row < returnMat.size(); ++row)
	{
		returnMat[row].resize(colCount);
	}

	for (size_t row = 0; row < rowCount; ++row)
	{
		for (size_t col = 0; col < colCount; ++col)
		{
			returnMat[row][col] = ActivationFunctionDrv(InputMat[row][col]);
		}
	}
}

//
//   layer 1                                              layer 2
// 
// (leftnode x)---weight from leftnode to rightnode---->(rightnode y)
//                   (WeightMatrix[x][y])
void ANN::RandomizeWeights(vector<vector<float>>& WeightMatrix, float UpperBound, size_t LeftNodes, size_t RightNodes)
{
	unsigned int iseed = (unsigned int)time(NULL);
	srand(iseed);

	WeightMatrix.resize(LeftNodes);
	for (size_t row = 0; row < LeftNodes; ++row)
	{
		WeightMatrix[row].resize(LeftNodes);
	}

	for (size_t row = 0; row < LeftNodes; ++row)
	{
		for (size_t col = 0; col < RightNodes; ++col)
		{
			WeightMatrix[row][col] = (rand() / RAND_MAX) * UpperBound;
		}
	}
}

void ANN::InitializeBias(vector<vector<float>>& BiasVector, size_t NodeCount)
{
	BiasVector.resize(NodeCount);
	for (size_t row = 0; row < NodeCount; ++row)
	{
		BiasVector.resize(1);
		BiasVector[row][0] = 0;
	}
}