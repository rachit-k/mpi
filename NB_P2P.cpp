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
	MPI_Request request1, request2;
	MPI_Status status;
    int i;
	bool sparse=false;
    if(rank==0)
	{
		//vars[0]=n;
		//vars[1]=m;
		for (int i = 0; i < n; i++)
		{  
			for (int j = 0; j < m; j++)
			{
				float sparse1=1.0;
				float sparse2=1.0;
				if(sparse){
					float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					if(r>0.2)
						sparse1=0.0;
					r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					if(r>0.2)
						sparse2=0.0;
				}
				A[i*m+j] = (float (rand()%10)+0.1)*sparse1;
				B[j+i*m] = (float (rand()%10)+0.1)*sparse2;
			}
		}
	}
	std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
	if(rank==0)
	{
		for(i=1;i<processes;i++)
		{
			//cout<<"Sent"<<endl;
			MPI_Isend(&A[i*n*m/processes],n*m/processes , MPI_FLOAT, i, 0, MPI_COMM_WORLD, &request1);
			MPI_Isend(B,m*n , MPI_FLOAT, i, 0, MPI_COMM_WORLD, &request1);
		}		 
	}
	else
	{
		//cout<<"Received for rank "<<rank<<endl;
		MPI_Irecv(&A[rank*n*m/processes],n*m/processes , MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &request2);
		//MPI_Wait(&request2, &status);
		MPI_Irecv(B,m*n , MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &request2);
		MPI_Wait(&request2, &status);
		//cout<<"parallel time till receiving: for rank "<<rank<<" :"<< (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;
		//cout<<"Out"<<endl;
	}
	std::chrono::time_point<std::chrono::system_clock> multiply = std::chrono::system_clock::now();
	for (int i = rank*n/processes; i < (rank+1)*n/processes; i++)
	{
		for (int j = 0; j < n; j++)
		{
			C[i*n + j] = 0;
			for (int k = 0; k < m; k++)
				C[i*n + j] += A[i*m + k] * B[k*n + j];
		}
	}
	cout<<"mult"<<rank<<" "<< (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - multiply)).count()<<endl;
	if (rank != 0 )
	{
		int ind=rank * n * n/processes;
		MPI_Isend(& (C[ind]), n*n/processes, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &request2);
		//MPI_Wait(&request1, &status);
	}
	else
	{
		//cout<<"Here"<<endl;
		for (i=1; i<processes; i++)
		{
			MPI_Irecv(&C[i *n *n/processes], n*n / processes, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &request1);
			
		}
		
	}

    cout<<"parallel_time:"<<rank<<" "<< (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;
    if(rank==0)
    {
		MPI_Wait(&request1, &status);
    	std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
    	Multiply_serial(A,B,C_serial,n,m,n); 
    	cout<<"serial_time: "<< (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;

    	cout<<"IsEqual: "<<IsEqual(C,C_serial,n,n)<<endl;
    	//print_matrix(C,vars[0],vars[0]);


    }

	// MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}