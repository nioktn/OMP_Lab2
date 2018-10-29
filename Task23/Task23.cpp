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

#pragma omp parallel default(none) private(i, mynum, last) shared(arr, partial, temp, num_threads, work, n) if(enOmp)
	{
#pragma omp single
		{
			num_threads = threads;
			if (!(partial = (double *)malloc(sizeof(double) * num_threads))) exit(-1);
			if (!(temp = (double *)malloc(sizeof(double) * num_threads))) exit(-1);
			work = n / num_threads + 1; // sets length of sub-arrays
		}

		mynum = omp_get_thread_num();
		//calculate prefix-sum for each subarray
		for (i = work * mynum + 1; i < work * mynum + work && i < n; i++)
			arr[i] += arr[i - 1];
		partial[mynum] = arr[i - 1];

#pragma omp barrier
		//calculate prefix sum for the array that was made from last elements of each of the previous sub-arrays
		for (i = 1; i < num_threads; i <<= 1) {
			if (mynum >= i)
				temp[mynum] = partial[mynum] + partial[mynum - i];
#pragma omp barrier
#pragma omp single
			memcpy(partial + 1, temp + 1, sizeof(double) * (num_threads - 1));
		}
		//update original array
		for (i = work * mynum; i < (last = work * mynum + work < n ? work * mynum + work : n); i++)
			arr[i] += partial[mynum] - arr[last - 1];
	}

	//for (i = 0; i < n; i++)
		printf("%f\n", arr[num_of_elems-1]);
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

int main()
{
	double *mass;
	int n, m, threads = 1;
	string path;
	int switchvar = 0;

	//ofstream os("C:\\Users\\nniki\\source\\repos\\Univ\\OMP_Lab2\\CSVs\\task22.csv", ios::app);
	//os << 1 << 4 << ";" << 12 << ";" << 20 << ";" << 28 << ";" << 36 << ";" << 44 << ";" << "\n";
	while (switchvar <= 6)
	{
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

		mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(mass, n*m, path);
		//Execute consistent
		double time1 = Execute(mass, n, m, threads, false);
		//os << time1 << ";";
		free(mass);

		//Execution parallel
		for (threads = 4; threads <= 36; threads += 8)
		{
			mass = (double*)malloc(n * m * sizeof(double));
			ReadArray(mass, n*m, path);
			double time1 = Execute(mass, n, m, threads, true);
			//os << time1 << ";";
			free(mass);
		}
		//os << "\n";
		switchvar++;
	}

	//Version1
	/*int n, ar[2][100], *t, i, toread, size, j;
	printf("Enter length:");
	scanf_s("%d", &n);
	printf("Enter numbers:\n", n);
	for (i = 0; i < n; i++)
		scanf_s("%d", &ar[0][i]);
	//set up complement to array that holds values
	toread = 1;
	//copy first value, since it is not copied by the code
	ar[1][0] = ar[0][0];
	size = 0;
	//following loop aims to get log2 of size, but can be avoided as in 2nd program
	while (i) {
		size++;
		i >>= 1;
	}
	//following code implements algorithm
	for (j = 0; j < size; j++) {
		toread = !toread;
		if (toread) t = ar[0];
		else t = ar[1];
#pragma omp parallel for default(none) private(i) shared(n, j, t, ar, toread) if(false)
		for (i = 1; i < n; i++) {
			if (i - (1 << j) >= 0)
				t[i] = ar[toread][i] + ar[toread][i - (1 << j)];
			else t[i] = ar[toread][i];
		}
	}
	toread = !toread;
	for (i = 0; i < n; i++)
		printf("%d\n", *(*(ar + toread) + i));
}*/
	//int n = 100, m = 100;
	//double *arr = (double*)malloc(n * m * sizeof(double));
	//ReadArray(arr, n*m, "C:\\Data\\Stuff\\OpenMP\\bin\\100.bin");
	//arr[0] = 1;
	//arr[1] = 2;
	//arr[2] = 3;
	//arr[3] = 4;
	//arr[4] = 5;
	//arr[5] = 6;
	//arr[6] = 7;
	//arr[7] = 8;
	//arr[8] = 9;

	//PrefixSum(arr, n*m, 4, true);
	//for (int i = 0; i < n; i++)
	//	printf("%f\n", arr[i]);
}

/*int *arr, *partial, *temp;
int num_threads, work, n;
int i, mynum, last;
printf("Enter length:");
scanf_s("%d", &n);
if (!(arr = (int *)malloc(sizeof(int) * n))) return -1;
printf("Enter numbers:\n");
for (i = 0; i < n; i++)
	scanf_s("%d", arr + i);
#pragma omp parallel default(none) private(i, mynum, last) shared(arr, partial, temp, num_threads, work, n) if(enOmp)
{
#pragma omp single
	{
		num_threads = omp_get_num_threads();
		if (!(partial = (int *)malloc(sizeof(int) * num_threads))) exit(-1);
		if (!(temp = (int *)malloc(sizeof(int) * num_threads))) exit(-1);
		work = n / num_threads + 1; // sets length of sub-arrays
	}

	mynum = omp_get_thread_num();
	//calculate prefix-sum for each subarray
	for (i = work * mynum + 1; i < work * mynum + work && i < n; i++)
		arr[i] += arr[i - 1];
	partial[mynum] = arr[i - 1];

#pragma omp barrier
	//calculate prefix sum for the array that was made from last elements of each of the previous sub-arrays
	for (i = 1; i < num_threads; i <<= 1) {
		if (mynum >= i)
			temp[mynum] = partial[mynum] + partial[mynum - i];
#pragma omp barrier
#pragma omp single
		memcpy(partial + 1, temp + 1, sizeof(int) * (num_threads - 1));
	}
	//update original array
	for (i = work * mynum; i < (last = work * mynum + work < n ? work * mynum + work : n); i++)
		arr[i] += partial[mynum] - arr[last - 1];
}
for (i = 0; i < n; i++)
	printf("%d\n", arr[i]);
}*/

//ReadArray(mass, n*m, path);

//threads = 4;

//double testSum = 0;
//for (int i = 0; i < n*m; i++)
//{
//	testSum += mass[i];
//}
//printf("\n###### testSum = %f####\n", testSum);
//double time1 = Execute(false, n, m, threads, mass);
////os << time1 << ";";
////free(mass);

////Execution parallel
////for (threads = 4; threads <= 44; threads += 8)
////{
//	//mass = (double*)malloc(n * m * sizeof(double));
//	//ReadArray(mass, n*m, path);
//time1 = Execute(true, n, m, threads, mass);
////os << time1 << ";";
//	free(mass);
//}
//switchvar++;
//}
//os << "\n";
//os.close();

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
*/