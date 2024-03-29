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

double FindMinValue(bool enOmp, int num_of_elems, int threads, double *mass)
{
	int chunk = num_of_elems % threads == 0 ? num_of_elems / threads : num_of_elems / 2 + 1;
	double *minValues = (double*)calloc(threads, sizeof(double));
	double min = 0;
#pragma omp parallel num_threads(threads) if(enOmp)
	{
#pragma omp for schedule(static, chunk)
		for (int i = 0; i < num_of_elems; i++)
		{
			if (mass[i] < minValues[omp_get_thread_num()]) minValues[omp_get_thread_num()] = mass[i];
		}

#pragma omp flush(min)
		if (minValues[omp_get_thread_num()] < min)
		{
#pragma omp critical (minvalue)
			{
				if (minValues[omp_get_thread_num()] < min) min = minValues[omp_get_thread_num()];
			}
		}
	}
	return min;
}

double properMinValue(double *arr, int n)
{
	double min = 200;
	for (int i = 0; i < n; i++)
	{
		if (arr[i] < min) min = arr[i];
	}
	//printf("\n\nRight min value = %f\n\n", min);
	return min;
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
	int chunk = n * m / threads;
	LARGE_INTEGER frequency, start, finish;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
	double min = FindMinValue(enOmp, n*m, threads, mass);
	QueryPerformanceCounter(&finish);
	printf("\nmin element = %f\n", min);
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

		//Execute consistent		
		mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(mass, n*m, path);
		properMinValue(mass, n*m);
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