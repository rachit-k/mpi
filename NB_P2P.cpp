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

void print_matrix(float *A, int m,int n)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout<<A[i*n+j]<<" ";
		}
		cout<<endl;
	}
}

int main(int argc, char **argv) 
{
	int processes,rank;
	//int vars[2];

	float *A;//n*m
	float *B;//m*n
	float *C;//n*n
	float *C_serial;//n*n
	int n;
	int m;
	n=stoi(argv[1]);
	m=32;

	A=new float[n*m];
	B=new float[m*n];
	C=new float[n*n];
	C_serial=new float[n*n];

	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i;
    if(rank==0)
	{
		//vars[0]=n;
		//vars[1]=m;
		for (int i = 0; i < n; i++)
		{  
			for (int j = 0; j < m; j++)
			{
				A[i*m+j] = float (rand()%10)+0.1;
				B[j+i*m] = float (rand()%10)+0.1;
			}
		}
	}
	std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
	if(rank==0)
	{
		for(i=1;i<processes;i++)
		{
			MPI_Isend(&A[i*n*m/processes],n*m/processes , MPI_FLOAT, i, 0, MPI_COMM_WORLD);
			MPI_Isend(B,m*n , MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}		 
	}
	else
	{
		MPI_Irecv(&A[rank*n*m/processes],n*m/processes , MPI_FLOAT, i, 0, MPI_COMM_WORLD, 0);
		MPI_Irecv(B,m*n , MPI_FLOAT, i, 0, MPI_COMM_WORLD, 0);
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
		int ind=rank * n * n/processes;
		MPI_Isend(& (C[ind]), n*n/processes, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		for (i=1; i<processes; i++)
		{
			MPI_Irecv(&C[i *n *n/processes], n*n / processes, MPI_FLOAT, i, 0, MPI_COMM_WORLD, 0);
		}
	}
    cout<<"parallel time: "<< (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;
    if(rank==0)
    {
    	std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
    	Multiply_serial(A,B,C_serial,n,m,n); 
    	cout<<"serial time: "<< (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;

    	cout<<"IsEqual: "<<IsEqual(C,C_serial,n,n)<<endl;
    	//print_matrix(C,vars[0],vars[0]);

    }

	// MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}