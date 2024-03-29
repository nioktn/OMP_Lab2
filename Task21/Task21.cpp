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
	sum = 0;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
	int neg_elems = 0, pos_elems = 0, zero_elems = 0;

#pragma omp parallel private(i) num_threads(threads) if(enOmp)
	{
		int positive = 0, negative = 0, zero = 0;
#pragma omp for schedule(dynamic, chunk)
		for (i = 0; i < n*m; i++)
		{
			if (mass[i] > 0)
			{
				positive++;

			}
			else if (mass[i] < 0)
			{
				negative++;
			}
			else
			{
				zero++;
			}
		}
#pragma omp critical(neg_sum)
		neg_elems += negative;
#pragma omp critical(pos_sum)
		pos_elems += positive;
#pragma omp critical(zero_sum)
		zero_elems += zero;
	}

	QueryPerformanceCounter(&finish);
	if (!enOmp) printf("\n\n");
	printf("positive: %d  negative: %d  zero: %d ", pos_elems, neg_elems, zero_elems);
	double time1 = (finish.LowPart - start.LowPart) * 1000.0f / frequency.LowPart;
	if (enOmp) printf("Parallel execution time for %dx%d matrix with %d threads = %f\n", n, m, threads, time1);
	else printf("Consistent execution time for %dx%d matrix = %f\n", n, m, time1);
	return floor(time1 + 0.5);
}

int main()
{
	double *mass;
	int n, m, threads = 1;
	n = 5000, m = 5000;
	string path;
	int switchvar = 0;

	ofstream os("C:\\Users\\nniki\\source\\repos\\Univ\\OMP_Lab2\\CSVs\\task21.csv", ios::app);
	os << 4 << ";" << 12 << ";" << 20 << ";" << 28 << ";" << 36 << ";" << 44 << ";" << 1 << "\n";
	while (switchvar <= 6) {
		//switchvar = 1;
		switch (switchvar)
		{
		case(0): path = "C:\\Data\\Stuff\\OpenMP\\bin\\100.bin", n = 100, m = 100;
			break;
		case(1): path = "C:\\Data\\Stuff\\OpenMP\\bin\\1000.bin", n = 1000, m = 1000;
			break;
		case(2): path = "C:\\Data\\Stuff\\OpenMP\\bin\\2000.bin", n = 2000, m = 2000;
			break;
		case(3): path = "C:\\Data\\Stuff\\OpenMP\\bin\\4000.bin", n = 4000, m = 4000;
			break;
		case(4): path = "C:\\Data\\Stuff\\OpenMP\\bin\\6000.bin", n = 6000, m = 6000;
			break;
		case(5): path = "C:\\Data\\Stuff\\OpenMP\\bin\\8000.bin", n = 8000, m = 8000;
			break;
		case(6): path = "C:\\Data\\Stuff\\OpenMP\\bin\\10000.bin", n = 10000, m = 10000;
			break;
			//case(0): path = "D:\\Stuff\\OpenMP\\bin\\100.bin", n = 100, m = 100;
			//	break;
			//case(1): path = "D:\\Stuff\\OpenMP\\bin\\1000.bin", n = 1000, m = 1000;
			//	break;
			//case(2): path = "D:\\Stuff\\OpenMP\\bin\\2000.bin", n = 2000, m = 2000;
			//	break;
			//case(3): path = "D:\\Stuff\\OpenMP\\bin\\4000.bin", n = 4000, m = 4000;
			//	break;
			//case(4): path = "D:\\Stuff\\OpenMP\\bin\\6000.bin", n = 6000, m = 6000;
			//	break;
			//case(5): path = "D:\\Stuff\\OpenMP\\bin\\8000.bin", n = 8000, m = 8000;
			//	break;
			//case(6): path = "D:\\Stuff\\OpenMP\\bin\\10000.bin", n = 10000, m = 10000;
			//	break;
		}

		//Execute consistent
		mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(mass, n*m, path);
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
	//os << "\n";
	//os.close();
}