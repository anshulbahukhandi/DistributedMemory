/* Name : Anshul Bahukhandi
   Asuid : 1207178527
APM 525 : QuickSort using MPI */


#include<iostream>
#include<mpi.h>
#include<random>
#include<ctime>
#include<stdlib.h>
using namespace std;

//...............................................HELPER SWAP FUNCTION.............................................
void swap ( int * a , int * b)
{
	int temp ; 
	temp=* a; 
	*a=*b;
	*b=temp ;
}

//...............................................MERGE FUNCTION FOR TWO SORTED ARRAYS.............................

void  merge( int * parray1 , int size1 , int* parray2  , int size2 , int * result)
{
        int small;
        int large;
        int *psmall=new int [size1];
        int * plarge = new int[size2]; 
        
        if(size1<size2) {   small=size1; psmall=parray1; plarge=parray2; large=size2;}
        else 			{ 	small=size2; psmall=parray2;  plarge=parray1; large=size1;}

                        int i=0 , j=0 , k=0 , count=0;
                        while( count!=small && j!=large && i!=small )
                        {
                        	if(psmall[i]<plarge[j]) { result[k]=psmall[i];count++; i++; k++;}
                        	else{result[k]=plarge[j]; k++; j++;}
                        } 

						if( i!=small)
							{
                        		while(k<size1 + size2)
                        			{
                        				result[k]=psmall[i]; j++; k++;
                        			}
                        	}
                        if( j!=large)
							{
                        		while(k<size1 + size2)
                        			{
                        
                        				result[k]=plarge[j]; j++; k++;
                        			}
    
                       	}                 
		
}


//.......................................RECURSIVE QUICKSORT...........................................................
void quicksort( int * pelms , int nelms)
{	
	
	if(nelms>1)
{
	int nleft=nelms-1;
	int nright=0;
	
	int* uend=pelms+nelms-2;
        int* ustart=pelms;
	int pivot =pelms[nelms-1];
	int* ppivot=pelms+nelms-1;
	while( uend!=ustart)
		{
			if(*ustart > pivot)
				{
					swap(ustart , uend);
					swap(ppivot,uend);
					uend=uend-1;
					ppivot=ppivot-1;					
					nright++;
					nleft--;
				}
			else ustart=ustart+1;
		}
	if(pivot<*uend) 
		{
			swap(ppivot,uend);
			ppivot=ppivot-1;
			uend=uend+1;
			ustart=ustart+1;
			nright++;
                        nleft--;
		}
	//right subarray
	quicksort(ppivot+1 , nright);	

	//left subarray
	quicksort(pelms , nleft);
}	
}
	
//........................................MAIN FUNCTION................................................................

int main(int argc , char* argv[])
{

	clock_t begin = clock() ;			//keeping track of time
	int Nelemnt;
	int Nprocess;
	int Lelement;
	int Rank;
	MPI_Init(&argc , &argv);

	Nelemnt= atoi(argv[1]);
	MPI_Comm_size(MPI_COMM_WORLD , &Nprocess);
	MPI_Comm_rank(MPI_COMM_WORLD , &Rank);
	 srand(time(NULL)+Rank);
	 
	Lelement=Nelemnt/Nprocess;
	int* pelement= new int[Lelement];

	
	for ( unsigned int i = 0 ; i<Lelement ; i++)
		 pelement[i]=rand()/1000;

	quicksort(pelement  , Lelement);
	
	//................send data from all processes to rank 0 process
	
	if ( Rank != 0 )  
	{
	MPI_Request req;
	MPI_Isend( (void*)pelement ,Lelement , MPI_INT , 0 ,1,MPI_COMM_WORLD,&req);
	}
	
	//......receive data on rank zero using blocking operation 
	//......buffer at recieve end may not be sufficient to store all the  data from all the process
	//......Moreover nonblock wont make much difference as merge operation will stil take same time for any two data set 
	//......and sending is anyways happeneing at end of each process once  they have finished sorting of local sub arrays
	
	if(Rank==0)
	{
	
	int * start=new int [Nelemnt];
	start=pelement;
	int * finalresult=new int [Nelemnt];
	for ( int i = 1 ; i<Nprocess ; i++)
		{
			void * temp = malloc(Lelement*sizeof(MPI_INT));
			MPI_Status status;
			MPI_Recv(temp , Lelement , MPI_INT ,i ,1, MPI_COMM_WORLD , &status);
			int* temp1 =(int*)temp;
			merge(start ,i*Lelement, (int*)temp,Lelement,finalresult);			
			start=finalresult;
		}			
	clock_t end = clock() ;			//keeping track of time
	cout<<"Total Time taken to sort "<<Nelemnt<<" points with "<<Nprocess<<" processors :"<<double(end-begin)/CLOCKS_PER_SEC<<endl;
	}
	
	MPI_Finalize();

return 0; 
} 
