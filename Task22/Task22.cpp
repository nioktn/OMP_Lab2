#include "pch.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <locale.h>
#include <string>

using namespace std;

double PairwiseSum(double* array, int arrSize, bool useOmp)
{
	double* result = (double*)malloc(arrSize * sizeof(double));
	int resultIterator = 0, i;
	double sum = 0;
#pragma omp parallel private(i) if(useOmp)
	{
		i = 0;
#pragma omp for
		for (i = 0; i < arrSize; i + 2)
		{
			if (i + 1 == arrSize)
				sum = array[i];
			else
				sum = array[i] + array[i + 1];

#pragma omp critical (sum)
			{
				result[resultIterator] = sum;
				resultIterator++;
			}
		}
	}
	if (resultIterator != 1) PairwiseSum(result, resultIterator, useOmp);
	else return result[0];
}

void ReadArray(double* pdata, size_t nm, string file_path)
{
	ifstream is(file_path, ios::binary | ios::in);
	if (!is.is_open())
		return;
	is.read((char*)(pdata), nm * sizeof(double));
	is.close();
}

double Execute(bool enOmp, int n, int m, int threads, double *mass)
{
	int i, j;
	double sum;
	int chunk = n * m / threads;
	LARGE_INTEGER frequency, start, finish;

	// =========
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	sum = PairwiseSum(mass, threads, enOmp);
	QueryPerformanceCounter(&finish);
	if (!enOmp) printf("\n\n");
	printf("sum: %d", sum);
	double time1 = (finish.LowPart - start.LowPart) * 1000.0f / frequency.LowPart;
	if (enOmp) printf("Parallel execution time for %dx%d matrix with %d threads = %f\n", n, m, threads, time1);
	else printf("Consistent execution time for %dx%d matrix = %f\n", n, m, time1);
	return floor(time1 + 0.5);
}

int main()
{
	double *mass;
	int n, m, threads;
	n = 3, m = 3;
	//Execute consistent
	mass = (double*)malloc(n * m * sizeof(double));
	mass[0] = 1;
	mass[1] = 2;
	mass[2] = 3;
	mass[3] = 4;
	mass[4] = 5;
	mass[5] = 6;
	mass[6] = 7;
	mass[7] = 8;
	mass[8] = 9;

	threads = 4;
	//ReadArray(mass, n*m, path);
	double time1 = Execute(false, n, m, threads, mass);
	//os << time1 << ";";
	//free(mass);

	//Execution parallel
	//for (threads = 4; threads <= 44; threads += 8)
	//{
		//mass = (double*)malloc(n * m * sizeof(double));
		//ReadArray(mass, n*m, path);
	double time1 = Execute(true, n, m, threads, mass);
	//os << time1 << ";";
//	free(mass);
//}
//switchvar++;
//}
//os << "\n";
//os.close();
}

/*
#include "Header.h"

std::vector<double> PairwiseSum(std::vector<double> vector, bool useOmp);
double PairwiseSumLoop(double* arr, int n, bool useOmp);

void LaunchPairwiseSum(int n, bool useOmp)
{
	if (useOmp)
		Log("Pairwise Sum                                         OMP ON");
	else
		Log("Pairwise Sum                                         OMP OFF");

	Log("------------------------------------------------------------------------------");
	Log("Array length: " + to_string(n));

	double *arr = RandomArray(n);
	Log("Result: " + to_string(PairwiseSumLoop(arr, n, useOmp)));

	delete[] arr;

	Log("------------------------------------------------------------------------------");
}

std::vector<double> PairwiseSum(std::vector<double> vector, bool useOmp)
{
	std::vector<double> pairVector;
	#pragma omp parallel if(useOmp)
	{
		#pragma omp for
		for (int i = 0; i < vector.size(); i+=2)
		{
			double sum = 0;
			if (i + 1 == vector.size())
				sum = vector[i];
			else
				sum = vector[i] + vector[i + 1];

			#pragma omp critical
			pairVector.push_back(sum);
		}
	}

	return pairVector;
}

double PairwiseSumLoop(double* arr, int n, bool useOmp)
{
	Log("Starting calculations...");
	System::Diagnostics::Stopwatch^ watch = gcnew System::Diagnostics::Stopwatch();

	watch->Start();

	std::vector<double> vector;
	vector.assign(arr, arr + n);

	while (vector.size() != 1)
	{
		vector = PairwiseSum(vector, useOmp);
	}

	Log("Time ellapsed: " + to_string(watch->ElapsedTicks));
	watch->Reset();

	return vector.at(0);
}


*/


/*
#include "Header.h"

std::vector<double> PairwiseSum(std::vector<double> vector, bool useOmp);
double PairwiseSumLoop(double* arr, int n, bool useOmp);

void LaunchPairwiseSum(int n, bool useOmp)
{
	if (useOmp)
		Log("Pairwise Sum                                         OMP ON");
	else
		Log("Pairwise Sum                                         OMP OFF");

	Log("------------------------------------------------------------------------------");
	Log("Array length: " + to_string(n));

	double *arr = RandomArray(n);
	Log("Result: " + to_string(PairwiseSumLoop(arr, n, useOmp)));

	delete[] arr;

	Log("------------------------------------------------------------------------------");
}

std::vector<double> PairwiseSum(std::vector<double> vector, bool useOmp)
{
	std::vector<double> pairVector;
	#pragma omp parallel if(useOmp)
	{
		#pragma omp for
		for (int i = 0; i < vector.size(); i+=2)
		{
			double sum = 0;
			if (i + 1 == vector.size())
				sum = vector[i];
			else
				sum = vector[i] + vector[i + 1];

			#pragma omp critical
			pairVector.push_back(sum);
		}
	}

	return pairVector;
}

double PairwiseSumLoop(double* arr, int n, bool useOmp)
{
	Log("Starting calculations...");
	System::Diagnostics::Stopwatch^ watch = gcnew System::Diagnostics::Stopwatch();

	watch->Start();

	std::vector<double> vector;
	vector.assign(arr, arr + n);

	while (vector.size() != 1)
	{
		vector = PairwiseSum(vector, useOmp);
	}

	Log("Time ellapsed: " + to_string(watch->ElapsedTicks));
	watch->Reset();

	return vector.at(0);
}