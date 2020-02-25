#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <pthread.h>
#include <mpi.h>

using namespace std;

void Multiply_serial(float *A, float *B, float *C, int m, int n, int p)//mn*np=mp
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < p; j++)
		{
			C[i*p + j] = 0;
			for (int k = 0; k < n; k++)
				C[i*p + j] += A[i*n + k] * B[k*p + j];
		}
	}
}

int IsEqual(float *A, float *B, int m, int n)
{
	for (int i = 0; i < m; i++i)
	{
		for (int j = 0; j < n; j++)
		{
			if(A[i*n + j] !=B[i*n + j])
				return 0;
		}
	}
	return 1;
}

int main(int argc, char **argv) 
{

	int processes,rank;
	float *A;
	float *B;
	float *C;
	int n;
	int m;

	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i;
	if(rank==0)
	{
		for(i=1;i<processes;i++)
		{
			MPI_Send(&A[i*n*m/processes],n*m/p , MPI_FLOAT, i, 0, MPI_COMM_WORLD);
			MPI_Send(B,m*n , MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}		 
	}
	else
	{
		MPI_Recv(&A[rank*n*m/processes],n*m/p , MPI_FLOAT, i, 0, MPI_COMM_WORLD, 0);
		MPI_Recv(B,m*n , MPI_FLOAT, i, 0, MPI_COMM_WORLD, 0);
	}

	for (int i = rank*n/processes; i < (rank+1)*n/processes; i++)
	{
		for (int j = 0; j < n; j++)
		{
			C[i*n + j] = 0;
			for (int k = 0; k < m; k++)
				C[i*n + j] += A[i*m + k] * B[k*n + j];
		}
	}

	if (rank != 0 )
	{
		MPI_Send(&C[(rank) *n * n/processes][0], n*n/processes, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		for (i=1; i<processes; i++)
		{
			MPI_Recv(&C[i *n *n/processes], n*n / processes, MPI_FLOAT, i, 0, MPI_COMM_WORLD, 0);
		}
	}


	// MPI_Barrier(MPI_COMM_WORLD);



	MPI_Finalize();
	return 0;
}