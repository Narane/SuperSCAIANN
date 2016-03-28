#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ANN.h"
#include "MatrixMaths.h"
#include "Generator.h"

// Init shenanigans
//  Since the main AI module keeps an instance around (preconstructed) for singleton-like usage purposes, call this on Start for setup
void ANN::BuildModel()
{
	unsigned int iseed = (unsigned int)time(NULL);
	srand(iseed);

	Generator::generate();
	LoadedInputOutputData = Generator::loadData();
	size_t dataCount = LoadedInputOutputData.size();

	size_t trainingCount = dataCount * 3 / 4;
	//LoadedInputOutputData.begin();

	map<vector<vector<float>>, vector<vector<float>>> trainingSet;
	map<vector<vector<float>>, vector<vector<float>>> validationSet;
	
	//ah whatever.
	for (map<vector<vector<float>>, vector<vector<float>>>::const_iterator it = LoadedInputOutputData.begin()
			; it != LoadedInputOutputData.end()
			; ++it
			)
	{
		if (rand() / RAND_MAX > 0.25)
		{
			trainingSet[it->first] = it->second;
		}
		else
		{
			validationSet[it->first] = it->second;
		}
	}

	// "this is so ugly I cannot describe how ugly it is" - "Jeffrey Tang"
	InputNodeCount = LoadedInputOutputData.begin()->first[0].size();
	OutputNodeCount = LoadedInputOutputData.begin()->second[0].size();

	RandomizeWeights(WeightsInputToHidden, 0.3f, InputNodeCount, HiddenLayerNodeCount);
	RandomizeWeights(WeightsHiddenToOutput, 0.3f, HiddenLayerNodeCount, OutputNodeCount);

	HiddenLayerTot.resize(HiddenLayerNodeCount);
	HiddenLayerValues.resize(HiddenLayerNodeCount);
	OutputLayerValues.resize(OutputNodeCount);
	OutputLayerTot.resize(OutputNodeCount);

	for (size_t trainedTimes = 0; trainedTimes < 50; ++trainedTimes)
	{
		for (map<vector<vector<float>>, vector<vector<float>>>::const_iterator it = trainingSet.begin()
			; it != trainingSet.end()
			; ++it
			)
		{
			const vector<float>& inputs = it->first[0];
			const vector<float>& outputs = it->second[0];
			TrainWithData(inputs, outputs);
		}
	}


	// Testing
	
	int correct = 0;
	int numValidation = 0;
	for (map<vector<vector<float>>, vector<vector<float>>>::const_iterator it = validationSet.begin()
		; it != validationSet.end()
		; ++it
		)
	{
		const vector<float>& inputs = it->first[0];
		const vector<float>& outputs = it->second[0];

		CalculateLayers(inputs);

		int index1, index2;
		float val1;
		float val2;
		val1 = argMaxIndex(OutputLayerValues, index1);
		val2 = argMaxIndex(OutputLayerValues, index2);
		//actualOutputs.push_back(OutputLayerValues);
		//expectedOutputs.push_back(outputs);
		if (index1 == index2)
		{
			correct++;
		}
		numValidation++;
	}
	//Validation correct 
	float percentage = (double)correct/numValidation;
	
}

float ANN::argMaxIndex(vector<float> outputVector, int &index)
{
	index = 0;
	float max = -9999;//whatever.
	for (int i = 0; i < OutputNodeCount; ++i)
	{
		if (outputVector[i] > max)
		{
			index = i;
			max = outputVector[i];
		}
	}
	return max;
}

// Run this on a loop for training
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

// Given that the weights matrix is filled, write the results to the output vectors shown below
void ANN::CalculateLayers(const vector<float>& InputValues)
{
	if (InputValues.size() != InputNodeCount)
	{
		// think about putting a breakpoint here because this is sad
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
//   (input)											  (output)
// (leftnode x)---weight from leftnode to rightnode---->(rightnode y)
//                   (WeightMatrix[x][y])
void ANN::RandomizeWeights(vector<vector<float>>& WeightMatrix, float UpperBound, size_t LeftNodes, size_t RightNodes)
{
	WeightMatrix.resize(LeftNodes);
	for (size_t row = 0; row < LeftNodes; ++row)
	{
		WeightMatrix[row].resize(RightNodes);
	}

	for (size_t row = 0; row < LeftNodes; ++row)
	{
		for (size_t col = 0; col < RightNodes; ++col)
		{
			const float tang = rand();
			const float jeff = (tang / RAND_MAX) * UpperBound;
			WeightMatrix[row][col] = jeff;
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