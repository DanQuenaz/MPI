#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

void geraVetor(int vet[], int n){//Gera vetor con n valores aleatórios.
	srand(time(NULL));
	for(int i=0; i<n; ++i){
		vet[i] = rand()%1000;
	}
}

void encontraMaiores(int vet[], int inicio, int fim, int &pmaior, int &smaior){
	smaior = pmaior = 0;
	for(int i=inicio; i<fim; ++i){
		if(vet[i]>=pmaior){
			smaior = pmaior;
			pmaior = vet[i];
		}
		if( vet[i]>=smaior && vet[i]<pmaior ){
			smaior = vet[i];
		}
	}
}

void imprimeVetor(int vet[], int n){
	for(int i=0; i<n; ++i){
		cout<<vet[i]<<" ";
	}
	cout<<endl;
}

int main( int argc, char **argv)
{
	int rank, inicio, fim, passo, rest, pmaior, smaior, n;
	int *vet;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	
	
	if (rank == 0) {
		int p1maior, s1maior, p2maior, s2maior, aux;

		cout<<"Quantidade: "; cin>>n;//Quantidade de elementos

		//Envia quantidade de elementos para os outros nós
		MPI_Send( &n, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send( &n, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		
		//Aloca vetor
		vet = new int[n];

		//Gera n valores aleatórios
		geraVetor(vet, n);
		imprimeVetor(vet, n);

		//Divisão de intervalos para cada nó
		passo = n/2;
		rest = n%2;
		inicio = 0;
		fim = passo;

		//Envia intervalo e vetor para o primeiro nó
		MPI_Send( &inicio, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send( &fim, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send( vet, n, MPI_INT, 1, 0, MPI_COMM_WORLD);

		inicio = inicio+passo;
		fim = fim + passo + rest;

		//Envia intervalo e vetor para o segundo nó
		MPI_Send( &inicio, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send( &fim, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send( vet, n, MPI_INT, 2, 0, MPI_COMM_WORLD);

		//Recebe maiores valores encontrados pelo primeiro nó
		MPI_Recv( &p1maior, 1, MPI_INT, 1, 0, MPI_COMM_WORLD,&status );
		MPI_Recv( &s1maior, 1, MPI_INT, 1, 0, MPI_COMM_WORLD,&status );

		//Recebe maiores valores encontrados pelo segundo nó
		MPI_Recv( &p2maior, 1, MPI_INT, 2, 0, MPI_COMM_WORLD,&status );
		MPI_Recv( &s2maior, 1, MPI_INT, 2, 0, MPI_COMM_WORLD,&status );

		//Econtra os 2 maiores dentre os 4 recebidos
		pmaior = p1maior >= p2maior ? p1maior : p2maior;
		smaior = s1maior >= s2maior ? s1maior : s2maior;
		p1maior >= p2maior ? (smaior = smaior >= p2maior ? smaior : p2maior) : smaior = smaior >= p1maior ? smaior : p1maior;

		cout<<"1 - "<<pmaior<<"\n2 - "<<smaior<<endl;

	}else if (rank == 1) {
		//Recebe quantidade de elementos e aloca vetor
		MPI_Recv( &n, 1, MPI_INT,0,0,MPI_COMM_WORLD,&status );
		vet = new int[n];

		//Recebe intervalos e valores do vetor
		MPI_Recv( &inicio, 1, MPI_INT,0,0,MPI_COMM_WORLD,&status );
		MPI_Recv( &fim, 1, MPI_INT,0,0,MPI_COMM_WORLD,&status );
		MPI_Recv( vet, n, MPI_INT,0,0,MPI_COMM_WORLD,&status );

		//Encontra os dois maiores do intervalo
		encontraMaiores(vet, inicio, fim, pmaior, smaior);

		//Envia para o nó principal os valores encontrados
		MPI_Send( &pmaior, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send( &smaior, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

		// cout<<"1: "<<pmaior<<" "<<smaior<<endl;
	}else{
		//Recebe quantidade de elementos e aloca vetor
		MPI_Recv( &n, 1, MPI_INT,0,0,MPI_COMM_WORLD,&status );
		vet = new int[n];

		//Recebe intervalos e valores do vetor
		MPI_Recv( &inicio, 1, MPI_INT,0,0,MPI_COMM_WORLD,&status );
		MPI_Recv( &fim, 1, MPI_INT,0,0,MPI_COMM_WORLD,&status );
		MPI_Recv( vet, n, MPI_INT,0,0,MPI_COMM_WORLD,&status );

		//Encontra os dois maiores do intervalo
		encontraMaiores(vet, inicio, fim, pmaior, smaior);

		//Envia para o nó principal os valores encontrados
		MPI_Send( &pmaior, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send( &smaior, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

		// cout<<"2: "<<pmaior<<" "<<smaior<<endl;
	}
	MPI_Finalize();

	return 0;
}
