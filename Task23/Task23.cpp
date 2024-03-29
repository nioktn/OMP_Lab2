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


void PrefixSum(double* arr, int num_of_elems, int threads, bool enOmp)
{
	double *partial, *temp;
	int num_threads, work, n = num_of_elems;
	int i, mynum, last;

	//double *arr = (double *)malloc(sizeof(double) * num_of_elems);
	//for (int i = 0; i < num_of_elems; i++)
	//{
	//	arr[i] = arr[i];
	//}

#pragma omp parallel default(none) private(i, mynum, last) shared(arr, partial, temp, num_threads, work, n) num_threads(threads) if(enOmp)
	{
#pragma omp single
		{
			num_threads = threads;
			if (!(partial = (double *)malloc(sizeof(double) * num_threads))) exit(-1);
			if (!(temp = (double *)malloc(sizeof(double) * num_threads))) exit(-1);
			work = n / num_threads + 1; // sets length of sub-arrays
		}

		mynum = omp_get_thread_num();
		//printf("%d", omp_get_num_threads());
		//calculate prefix-sum for each subarray
		for (i = work * mynum + 1; i < work * mynum + work && i < n; i++) {
			arr[i] += arr[i - 1];
			//printf("\nthread=%d > arr[%d] += arr[%d](%f) = %f\n", mynum, i, i - 1, arr[i-1], arr[i]);
		}
		//printf("\ni=%d\n", i);
		partial[mynum] = arr[i - 1]; // get last element from each sub-array
#pragma omp barrier

		//calculate prefix sum for the array that was made from last elements of each of the previous sub-arrays
		for (i = 1; i < num_threads; i <<= 1) {
			if (mynum >= i)
				temp[mynum] = partial[mynum] + partial[mynum - i];
#pragma omp barrier
#pragma omp single
			{
				//printf("partial+1 = %d, temp + 1 = %d, num_threads -1 = %d", partial + 1, temp + 1, num_threads - 1);
				memcpy(partial + 1, temp + 1, sizeof(double) * (num_threads - 1));
				//printf("partial+1 = %d, temp + 1 = %d, num_threads -1 = %d", partial + 1, temp + 1, num_threads - 1);
			}

		}
		//update original array
		for (i = work * mynum; i < (last = work * mynum + work < n ? work * mynum + work : n); i++) {
			arr[i] += partial[mynum] - arr[last - 1];
			//printf("mynum = %d >> last = %d, arr[last-1] = %f", mynum, last, arr[last - 1]);
		}

	}

	//for (i = 0; i < n; i++)
	printf("%f\n", arr[num_of_elems - 1]);
}

void ReadArray(double* pdata, size_t nm, string file_path)
{
	ifstream is(file_path, ios::binary | ios::in);
	if (!is.is_open())
		return;
	is.read((char*)(pdata), nm * sizeof(double));
	is.close();
}


double Execute(double* arr, int n, int m, int threads, bool enOmp)
{
	LARGE_INTEGER frequency, start, finish;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
	PrefixSum(arr, n*m, threads, enOmp);
	QueryPerformanceCounter(&finish);
	if (!enOmp) printf("\n\n");
	double time1 = (finish.LowPart - start.LowPart) * 1000.0f / frequency.LowPart;
	if (enOmp) printf("Parallel execution time for %dx%d matrix with %d threads = %f\n", n, m, threads, time1);
	else printf("Consistent execution time for %dx%d matrix = %f\n", n, m, time1);
	return floor(time1 + 0.5);
}

void nocascade(double *arr, int N)
{
	double sum = 0;
	for (int i = 0; i < N; i++)
	{
		sum += arr[i];
	}
	printf("\nAnswer: %f\n\n", sum);
}

int main()
{
	double *mass;
	int n, m, threads;
	string path;
	int switchvar = 0;

	ofstream os("C:\\Users\\Nikita\\source\\repos\\Univ\\OpenMP\\OMP_Lab2\\CSVs\\task23.csv", ios::app);
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
	nocascade(mass, n*m);
	//Execute consistent
	threads = 1;
	double time1 = Execute(mass, n, m, threads, false);
	os << time1 << ";";
	free(mass);

	//Execution parallel
	for (threads = 4; threads <= 44; threads += 8)
	{
		mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(mass, n*m, path);
		double time1 = Execute(mass, n, m, threads, true);
		os << time1 << ";";
		free(mass);
	}

	//mass = (double*)malloc(16 * sizeof(double));
	//for (int i = 0; i < 16; i++)
	//{
	//	mass[i] = i;
	//}
	//double time1 = Execute(mass, 4, 4, 4, true);

	os << "\n";
	switchvar++;
	}
}