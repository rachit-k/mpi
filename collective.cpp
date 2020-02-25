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
	int n;
	int m;
	cin>>n;
	m=32;
	float *A=new float[n*m];//n*m
	float *B=new float[m*n];//m*n
	float *C=new float[n*n];//n*n
	float *C_serial=new float[n*n];//n*n

	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i;
    std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
	if(rank==0)
	{
		for (int i = 0; i < n; i++)
		{  
			for (int j = 0; j < m; j++)
			{
				A[i*m+j] = double (rand()%10)+0.1;
				B[j+i*m] = double (rand()%10)+0.1;
			}
		}


	}
	MPI_Bcast(A, n*m , MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B, m*n , MPI_FLOAT, 0, MPI_COMM_WORLD);		

	int startrow = rank * ( n / processes);
	int endrow = ((rank + 1) * ( n / processes)) -1;

	float *C_temp=new float[n*n/processes];// n*n/processes

    for (int i = startrow; i <= endrow; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
        	C_temp[i*n+j]=0;
            for (int k = 0; k < m; k++) 
            {
                C_temp[i*n+j] += A[ (i*m + k) ] * B[ (k*n + j) ];
            }
        }
    }

    MPI_Gather(C_temp, n*n/processes, MPI_FLOAT, C, n*n/processes,  MPI_FLOAT, 0, MPI_COMM_WORLD);
    cout<< (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin)).count()<<endl;

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