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
#include <mpi.h>
#include <chrono>

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
	for (int i = 0; i < m; ++i)
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
	int n;
	int m;
	cin>>n;
	m=32;
	int p;
	float *A=new float[n*m];//n*m
	float *B=new float[m*n];//m*n
	float *C=new float[n*n];//n*n
	float *C_serial=new float[n*n];//n*n

	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i;
	std::chrono::time_point<std::chrono::system_clock> parallel = std::chrono::system_clock::now();
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
		int ind=(rank) *n * n/processes;
		MPI_Send(& (C[ind]), n*n/processes, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		for (i=1; i<processes; i++)
		{
			MPI_Recv(&C[i *n *n/processes], n*n / processes, MPI_FLOAT, i, 0, MPI_COMM_WORLD, 0);
		}
	}
	cout<< (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - parallel)).count()<<endl;
	if(rank==0)
    {
    	std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
    	Multiply_serial(A,B,C_serial,n,m,n); 
    	cout<< (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;

    	cout<<IsEqual(C,C_serial,n,n)<<endl;
    	cout<< (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;

    }


	// MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}