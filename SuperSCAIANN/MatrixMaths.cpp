#include "MatrixMaths.h"

vector<vector<float>> MatrixMaths::MatrixTranspose(const vector<vector<float>>& LHS)
{
	vector<vector<float>> returnMat;

	// error conditions
	const size_t rowSize = LHS.size();
	if (rowSize <= 0)
	{
		return returnMat;
	}
	const size_t colSize = LHS[0].size();

	// size inits
	returnMat.resize(colSize);
	for (size_t row = 0; row < colSize; ++row)
	{
		returnMat[row].resize(rowSize);
	}

	for (size_t row = 0; row < rowSize; ++row)
	{
		for (size_t col = 0; col < colSize; ++col)
		{
			returnMat[col][row] = LHS[row][col];
		}
	}

	return (returnMat);
}

// Return a matrix that does element-wise addition (needs matrices of same sizes)
vector<vector<float>> MatrixMaths::MatrixAdd(const vector<vector<float>>& LHS, const vector<vector<float>>& RHS)
{
	vector<vector<float>> returnMat;

	// error conditions
	const size_t rowSize = LHS.size();
	if (rowSize <= 0)
	{
		return returnMat;
	}
	const size_t colSize = LHS[0].size();
	if (rowSize != RHS.size() || colSize != RHS[0].size())
	{
		return returnMat;
	}

	// size inits
	returnMat.resize(rowSize);
	for (size_t row = 0; row < returnMat.size(); ++row)
	{
		returnMat[row].resize(colSize);
	}

	for (size_t row = 0; row < rowSize; ++row)
	{
		for (size_t col = 0; col < colSize; ++col)
		{
			returnMat[row][col] = LHS[row][col] + RHS[row][col];
		}
	}

	return (returnMat);
}

// return a matrix with same size as LHS, with each element multiplied by a scalar RHS
vector<vector<float>> MatrixMaths::ScalarMultiply(const vector<vector<float>>& LHS, int RHS)
{
	vector<vector<float>> returnMat;

	// error conditions
	const size_t rowSize = LHS.size();
	if (rowSize <= 0)
	{
		return returnMat;
	}
	const size_t colSize = LHS[0].size();

	for (size_t row = 0; row < rowSize; ++row)
	{
		for (size_t col = 0; col < colSize; ++col)
		{
			returnMat[row][col] *= RHS;
		}
	}

	return (returnMat);
}

// Your usual matrix multiplication
vector<vector<float>> MatrixMaths::MatrixMultiply(const vector<vector<float>>& LHS, const vector<vector<float>>& RHS)
{
	vector<vector<float>> returnMat;

	// size inits
	const size_t rowSize = LHS.size();
	const size_t colSize = RHS[0].size();
	returnMat.resize(rowSize);
	for (size_t row = 0; row < returnMat.size(); ++row)
	{
		returnMat[row].resize(colSize);
	}

	for (size_t row = 0; row < rowSize; ++row)
	{
		for (size_t col = 0; col < colSize; ++col)
		{
			for (size_t inner = 0; inner < LHS[0].size(); inner++) {
				returnMat[row][col] += LHS[row][inner] * RHS[inner][col];
			}
		}
	}

	return (returnMat);
}

// Return a matrix that does element-wise multplication (needs matrices of same sizes)
vector<vector<float>> MatrixMaths::HadamardMultiply(const vector<vector<float>>& LHS, const vector<vector<float>>& RHS)
{
	vector<vector<float>> returnMat;
	
	// error conditions
	const size_t rowSize = LHS.size();
	if (rowSize <= 0)
	{
		return returnMat;
	}
	const size_t colSize = LHS[0].size();
	if (rowSize != RHS.size() || colSize != RHS[0].size())
	{
		return returnMat;
	}

	// size inits
	returnMat.resize(rowSize);
	for (size_t row = 0; row < returnMat.size(); ++row)
	{
		returnMat[row].resize(colSize);
	}

	for (size_t row = 0; row < rowSize; ++row)
	{
		for (size_t col = 0; col < colSize; ++col)
		{
			returnMat[row][col] = LHS[row][col] * RHS[row][col];
		}
	}

	return (returnMat);
}