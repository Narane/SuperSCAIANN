#include "ANN.h"
#include "MatrixMaths.h"

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