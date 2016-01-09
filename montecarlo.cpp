#include<iostream>
#include<mpi.h>
#include<ctime>
#include<random>
#include<time.h>

using namespace std; 

struct coord
{
	long double x; 
	long double y;
};
//Funtion to compute the number of hits 
//Input is total number of hits for each process  , output is the successfull hits for that process 
long double  hitcount(long double total, int rnk ) 		//taking rank just so it can seed random generators differently
{
long double tot = total ; 
long double max = RAND_MAX;
long double count = 0;
srand(rnk);
for ( int i= 0 ; i<tot ; i++)
	{
		
		coord  point; 
		point.x=rand()/max;
		point.y=rand()/max; 
		//cout<<point.x<<"  ,  "<<point.y<<"\n";
		if( point.x * point.x + point.y * point.y<=1 )
			count++; 					
	}	
	return count; 
}



int main(int argc , char * argv[])
{
static const long double N =500000000 ;  // Total hits that'll be used for the computation
MPI_Init(&argc , &argv);
int rank;
MPI_Comm_rank(MPI_COMM_WORLD , &rank);
int nprocess;
MPI_Comm_size(MPI_COMM_WORLD , &nprocess);
//Data to create window on each process
MPI_Win window ;
long double nsize = N/(nprocess-1);

if ( rank==0)					//Master process is Rank 0 , will receive result from worker process		
	{
		clock_t begin=clock();
		long double  res =0;
		MPI_Win_create (&res , sizeof(long double)*(nprocess-1),sizeof(long double),MPI_INFO_NULL,MPI_COMM_WORLD,&window);
		
		
		MPI_Win_fence(0,window);
		 
		
		MPI_Win_fence(0,window);		
		clock_t end=clock();
		cout<<"Master says value of pi is : "<<(long double)4*res/N<<endl;
		cout<<"Time taken to calculate the value by " <<nprocess-1<<" processors :"<<double(end-begin)/CLOCKS_PER_SEC<<endl;
	}
	
else	//all the worker processes
	{
		 long double nsuccess=hitcount(nsize , rank);
		 MPI_Win_create (NULL,0,1,MPI_INFO_NULL,MPI_COMM_WORLD,&window);
		 MPI_Win_fence(0,window);
		 MPI_Accumulate(&nsuccess , 1 ,MPI_LONG_DOUBLE,0,0,1,MPI_LONG_DOUBLE,MPI_SUM,window);
		 MPI_Win_fence(0,window);
		  
	}	
	
MPI_Win_free(&window);
MPI_Finalize();
return 0 ;
}

		















