#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>

using namespace std;

//Tamanho das matrizes -> A[l1][c1] X B[l2][c2] = C[l1][c2]
#define l1  2
#define c1  3
#define l2  3
#define c2  2

void preencheMatrizes(int a[][c1], int b[][c2], int c[][c2], int d[][c2]){

    for(int i=0; i<l1; ++i){
        for(int j=0; j<c1; ++j){
            a[i][j] = rand()%10;
        }
    }

    for(int i=0; i<l2; ++i){
        for(int j=0; j<c2; ++j){
            b[i][j] = rand()%10;
        }
    }

    for(int i=0; i<l1; ++i){
        for(int j=0; j<c2; ++j){
            c[i][j] = 0;
            d[i][j] = 0;
        }
    }
}

void imprimeResultado(int a[][c1], int b[][c2], int c[][c2], int d[][c2]){
    cout<<"MATRIZ A: "<<endl;
    for(int i=0; i<l1; ++i){
        for(int j=0; j<c1; ++j){
            cout<<a[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl<<endl;

    cout<<"MATRIZ B: "<<endl;
    for(int i=0; i<l2; ++i){
        for(int j=0; j<c2; ++j){
            cout<<b[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl<<endl;

    cout<<"MATRIZ D = A x B: "<<endl;
    for(int i=0; i<l1; ++i){
        for(int j=0; j<c2; ++j){
            cout<<d[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl<<endl;
}

void pirntC(int c[][c2]){
    cout<<"MATRIZ C: "<<endl;
    for(int i=0; i<l1; ++i){
        for(int j=0; j<c2; ++j){
            cout<<c[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl<<endl;
}

int main( int argc, char **argv ){
    int rank, size;

    int intervalos[3] = {0, 0, 0};

    //a x b = d
    int a[l1][c1];
    int b[l2][c2];
    int c[l1][c2];
    int d[l1][c2];

    MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    srand(816823);

    preencheMatrizes(a, b, c, d);

    if(rank==0){
        int totalOp = l1*c2;
        int line = 0;
        int col = 0;
        int node = 1;

        for(int i=0; i<totalOp; ++i){
            intervalos[0] = line;
            intervalos[1] = col++;
            intervalos[2] = node++; if(node >= size) node = 1;
            if(col >= c2){
                col = 0;
                line++;
            }
            MPI_Bcast(intervalos, 3, MPI_INT, 0, MPI_COMM_WORLD); 
        }
        intervalos[0] = -1;
        MPI_Bcast(intervalos, 3, MPI_INT, 0, MPI_COMM_WORLD); 
    }else{
        while(intervalos[0] != -1){
            MPI_Bcast(intervalos, 3, MPI_INT, 0, MPI_COMM_WORLD);
            if(intervalos[2] == rank && intervalos[0] != -1){
                for(int i=0; i<c1; ++i){
                    c[intervalos[0]][intervalos[1]] += a[intervalos[0]][i] * b[i][intervalos[1]];
                }
            }
        }
    }

    for(int i = 0; i<l1; ++i){
        for(int j=0; j<c2; ++j){
            MPI_Reduce(&(c[i][j]), &(d[i][j]), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        }
    }

    if(rank==0){
        imprimeResultado(a, b, c, d);
    }

    MPI_Finalize();
}