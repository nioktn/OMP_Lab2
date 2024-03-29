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

void PairwiseSum(double* array, int arrSize, bool useOmp)
{
	int resultSize = (arrSize % 2 == 0) ? (arrSize / 2) : (arrSize / 2 + 1);
	double* result = (double*)malloc(resultSize * sizeof(double));
	int i, resultIterator = 0;
	double sum = 0;


#pragma omp parallel private(i, sum) if(useOmp)
	{
#pragma omp for1
		for (i = 0; i < arrSize; i += 2)
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

	if (resultSize == 1)
	{
		printf("\n>>>sum = %f<<<\n", result[0]);
	}
	else PairwiseSum(result, resultSize, useOmp);
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
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	double* tempResults = (double*)calloc(threads, sizeof(double));
#pragma omp parallel if(enOmp) num_threads(threads)
	{
#pragma omp for schedule(dynamic, chunk)
		for (int i = 0; i < n * m; i++)
		{
			tempResults[omp_get_thread_num()] += mass[i];
		}
	}
	PairwiseSum(tempResults, threads, enOmp);

	QueryPerformanceCounter(&finish);
	if (!enOmp) printf("\n\n");
	double time1 = (finish.LowPart - start.LowPart) * 1000.0f / frequency.LowPart;
	if (enOmp) printf("Parallel execution time for %dx%d matrix with %d threads = %f\n", n, m, threads, time1);
	else printf("Consistent execution time for %dx%d matrix = %f\n", n, m, time1);
	return floor(time1 + 0.5);
}

int main()
{
	double *mass;
	int n, m, threads = 1;
	string path;
	int switchvar = 0;

	ofstream os("C:\\Users\\nniki\\source\\repos\\Univ\\OMP_Lab2\\CSVs\\task22.csv", ios::app);
	os << 1 << 4 << ";" << 12 << ";" << 20 << ";" << 28 << ";" << 36 << ";" << 44 << ";" << "\n";
	while (switchvar <= 6)
	{
		switch (switchvar)
		{
		//case(0): path = "C:\\Data\\Stuff\\OpenMP\\bin\\100.bin", n = 100, m = 100;
		//	break;
		//case(1): path = "C:\\Data\\Stuff\\OpenMP\\bin\\1000.bin", n = 1000, m = 1000;
		//	break;
		//case(2): path = "C:\\Data\\Stuff\\OpenMP\\bin\\2000.bin", n = 2000, m = 2000;
		//	break;
		//case(3): path = "C:\\Data\\Stuff\\OpenMP\\bin\\4000.bin", n = 4000, m = 4000;
		//	break;
		//case(4): path = "C:\\Data\\Stuff\\OpenMP\\bin\\6000.bin", n = 6000, m = 6000;
		//	break;
		//case(5): path = "C:\\Data\\Stuff\\OpenMP\\bin\\8000.bin", n = 8000, m = 8000;
		//	break;
		//case(6): path = "C:\\Data\\Stuff\\OpenMP\\bin\\10000.bin", n = 10000, m = 10000;
		//	break;

			case(0): path = "D:\\Stuff\\OpenMP\\bin\\100.bin", n = 100, m = 100;
				break;
			case(1): path = "D:\\Stuff\\OpenMP\\bin\\1000.bin", n = 1000, m = 1000;
				break;
			case(2): path = "D:\\Stuff\\OpenMP\\bin\\2000.bin", n = 2000, m = 2000;
				break;
			case(3): path = "D:\\Stuff\\OpenMP\\bin\\4000.bin", n = 4000, m = 4000;
				break;
			case(4): path = "D:\\Stuff\\OpenMP\\bin\\6000.bin", n = 6000, m = 6000;
				break;
			case(5): path = "D:\\Stuff\\OpenMP\\bin\\8000.bin", n = 8000, m = 8000;
				break;
			case(6): path = "D:\\Stuff\\OpenMP\\bin\\10000.bin", n = 10000, m = 10000;
				break;
		}

		mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(mass, n*m, path);
		double testSum = 0;
		for (int i = 0; i < n*m; i++)
		{
			testSum += mass[i];
		}
		printf("\n###### proper testSum = %f####\n", testSum);

		//Execute consistent
		double time1 = Execute(false, n, m, threads, mass);
		os << time1 << ";";
		free(mass);

		//Execution parallel
		for (threads = 4; threads <= 44; threads += 8)
		{
			mass = (double*)malloc(n * m * sizeof(double));
			ReadArray(mass, n*m, path);
			double time1 = Execute(true, n, m, threads, mass);
			os << time1 << ";";
			free(mass);
		}
		os << "\n";
		switchvar++;
	}
}