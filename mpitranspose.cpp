// Name : Anshul Bahukhandi
// Asuid : 1207178527
// MPI implementation of matrix transpose for symmetric matrix 

// Input Arguments : nsize = number of rows =number of columns 
//NOTE:MATRIX IS ARRANGED IN ROWISE-FASHION

#include<iostream>
#include<stdlib.h>
#include<random>
#include<ctime>
#include<mpi.h>
#include<iomanip>
using namespace std; 

//.........Function to generate matrix.............. 

void matrix (int* mat ,  int N )			//mat is thhe matrix whose transpose will be done , N is the size of matrix 
{
srand(time(NULL));
//COMMENT OUT TO SEE RESULTS
/*for ( int i=0 ; i<N ; i++)
	{	
		for ( int j=0 ; j<N ;j++)
			{
				mat[i*N+j]=rand()/100000;
				cout<<setw(7)<<mat[i*N+j] <<"  ";
			}
			cout<<"\n";
	} */
}



//...........Main Function...........
int main( int argc ,  char * argv[])
{	
MPI_Init(&argc , &argv);

int nsize = atoi(argv[1]);
int nprocess;
MPI_Comm_size(MPI_COMM_WORLD , &nprocess);
int rank; 
MPI_Comm_rank(MPI_COMM_WORLD , &rank);
int rr= nsize/nprocess;					//Number of rows to take care of by each process
//Each process will have vector to receive the elements
int * rec = (int*)malloc(rr*nsize*sizeof(int));

//Generating the matrix at RANK 0 
if(rank==0)
	{
		clock_t begin= clock();
		cout<<"\nMatrix Generated at rank : "<<rank<<endl<<endl;
		int* mat= (int*)malloc(nsize*nsize*sizeof(int));
		matrix(mat , nsize);
		//Keeping first rr rows with rank 0 
		for(int j=0 ;j<rr*nsize ; j++)
			rec[j]=mat[j];
		//rank 0 will scatter rr rows to other processes 
		for( int i=1 ; i<nprocess ; i++)
			MPI_Send(mat+(i*rr*nsize) , rr*nsize,MPI_INT,i,1,MPI_COMM_WORLD);
		//process 0 has rr rows and hence fills rr columns  , No Need of communication as we are assembling on process 0
		for( int r=0 ; r<rr;r++){
		for ( int k=0  ; k<nsize ; k++)
			{mat[k*nsize+r]=rec[r*nsize+k];}
			}
		// Receive used to place the values coming from different processes to their respective place .	
		 for ( int l=1 ; l<nprocess; l++)
		 	{
		 	for( int r=0 ; r<rr;r++)
		 		{
		 				MPI_Status status;
		 				for ( int q=0;q<nsize;q++)
		 				{	
		 					int n; 
		 					MPI_Recv(&n, 1, MPI_INT ,l,r*nsize+q , MPI_COMM_WORLD,&status);
		 					mat[q*nsize+rr*l+r]=n;
		 				}	
				}	
			}
			
			//FINALLY MATRIX TRANSPOSED AT RANK 0
			cout<<"\nTransposed matrix asssembled automatically at Rank 0 :\n";
			//COMMENT OUT TO SEE RESULTS
				/*	for ( int i=0 ; i<nsize ; i++)
						{	
						for ( int j=0 ; j<nsize ;j++)
								{
									cout<<setw(7)<<mat[i*nsize+j] <<"  ";
								}
							cout<<"\n";
						}*/ 
		 clock_t end = clock();
	cout<<"Total time taken to transpose a "<<nsize<<"*"<<nsize<<" matrix by "<<nprocess<<" processors :"<<(double)(end-begin)/CLOCKS_PER_SEC<<endl;
	}	


else{
		MPI_Status status;
		MPI_Recv((void*)rec ,nsize*rr, MPI_INT ,0 ,1,MPI_COMM_WORLD , &status);
		//sending the received value one by one at their appropiate place in process 0 using non blocking
		MPI_Request request;
		for ( int j= 0 ; j<rr ; j++){ 
		for ( int i=0 ; i<nsize; i++)
			{MPI_Isend(&rec[j*nsize+i] , 1 ,MPI_INT , 0 ,j*nsize+i,MPI_COMM_WORLD,&request);}
		}
}

MPI_Finalize();
return 0 ;
}















