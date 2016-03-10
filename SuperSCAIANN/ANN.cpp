#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ANN.h"
#include "MatrixMaths.h"



void ANN::BuildModel()
{
	const size_t HiddenLayerNodeCount = 5;
	const size_t InputNodeCount = 6;
	const size_t OutputNodeCount = 2;

	// Gradient descent parameters
	const float epsilon = 0.01; // learning rate
	const float eta = 0.01; // regularization power

	RandomizeWeights(WeightsInputToHidden, 0.3f, InputNodeCount, HiddenLayerNodeCount);
	RandomizeWeights(WeightsHiddenToOutput, 0.3f, InputNodeCount, HiddenLayerNodeCount);
	InitializeBias(BiasInputToHidden, HiddenLayerNodeCount);
	InitializeBias(BiasHiddenToOutput, OutputNodeCount);
}

void ANN::Update(const vector<vector<float>>& X)
{
	vector<vector<float>> z1 = MatrixMaths::MatrixAdd(MatrixMaths::MatrixMultiply(X, WeightsInputToHidden), BiasInputToHidden);
	vector<vector<float>> a1 = ActivationMatrix(z1);
	vector<vector<float>> z2 = MatrixMaths::MatrixAdd(MatrixMaths::MatrixMultiply(a1, WeightsHiddenToOutput), BiasHiddenToOutput);
	MatrixMaths::MatrixInPlaceExp(z2);
	


	/*
	# Forward propagation
	z1 = X.dot(W1) + b1
		a1 = np.tanh(z1)
		z2 = a1.dot(W2) + b2
		exp_scores = np.exp(z2)
		probs = exp_scores / np.sum(exp_scores, axis = 1, keepdims = True)

		# Backpropagation
		delta3 = probs
		delta3[range(num_examples), y] -= 1
		dW2 = (a1.T).dot(delta3)
		db2 = np.sum(delta3, axis = 0, keepdims = True)
		delta2 = delta3.dot(W2.T) * (1 - np.power(a1, 2))
		dW1 = np.dot(X.T, delta2)
		db1 = np.sum(delta2, axis = 0)

		# Add regularization terms(b1 and b2 don't have regularization terms)
		dW2 += reg_lambda * W2
		dW1 += reg_lambda * W1

		# Gradient descent parameter update
		W1 += -epsilon * dW1
		b1 += -epsilon * db1
		W2 += -epsilon * dW2
		b2 += -epsilon * db2

		# Assign new parameters to the model
		model = { 'W1': W1, 'b1' : b1, 'W2' : W2, 'b2' : b2 }

		# Optionally print the loss.
		# This is expensive because it uses the whole dataset, so we don't want to do it too often.
		if print_loss and i % 1000 == 0:
		print "Loss after iteration %i: %f" % (i, calculate_loss(model))
	*/
}


float ANN::ActivationFunction(const float x)
{
	return (tanh(x) + 1.0f) / 2.0f;
}

float ANN::ActivationFunctionDrv(const float x)
{
	const float tanout = tanh(x);
	return (1.0f - tanout*tanout) / 2.0f;
}

// Take a matrix, return a copy matrix that applies the activation functions element-wise
vector<vector<float>> ANN::ActivationMatrix(vector<vector<float>> const& inputMat)
{
	vector<vector<float>> returnMat;
	const size_t rowCount = inputMat.size();
	if (inputMat.size() == rowCount)
	{
		return returnMat;
	}

	const size_t colCount = inputMat[0].size();
	for (size_t row = 0; row < returnMat.size(); ++row)
	{
		returnMat[row].resize(colCount);
	}

	for (size_t row = 0; row < rowCount; ++row)
	{
		for (size_t col = 0; col < colCount; ++col)
		{
			returnMat[row][col] = ActivationFunction(inputMat[row][col]);
		}
	}
}

// Take a matrix, return a copy matrix that applies the derivative of the activation functions element-wise
vector<vector<float>> ANN::ActivationMatrixDrv(vector<vector<float>> const& inputMat)
{
	vector<vector<float>> returnMat;
	const size_t rowCount = inputMat.size();
	if (inputMat.size() == rowCount)
	{
		return returnMat;
	}

	const size_t colCount = inputMat[0].size();
	for (size_t row = 0; row < returnMat.size(); ++row)
	{
		returnMat[row].resize(colCount);
	}

	for (size_t row = 0; row < rowCount; ++row)
	{
		for (size_t col = 0; col < colCount; ++col)
		{
			returnMat[row][col] = ActivationFunctionDrv(inputMat[row][col]);
		}
	}
}

void ANN::RandomizeWeights(vector<vector<float>>& WeightMatrix, float UpperBound, size_t InputNodeCount, size_t HiddenLayerNodeCount)
{
	unsigned int iseed = (unsigned int)time(NULL);
	srand(iseed);

	WeightMatrix.resize(InputNodeCount);
	for (size_t row = 0; row < InputNodeCount; ++row)
	{
		WeightMatrix[row].resize(HiddenLayerNodeCount);
	}

	for (size_t row = 0; row < InputNodeCount; ++row)
	{
		for (size_t col = 0; col < HiddenLayerNodeCount; ++col)
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