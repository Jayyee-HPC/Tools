#include "mpi.h"
#include <iostream>
#include <stdlib.h>

//~/test/op_equal/mpich/_inst/bin/mpicxx -o prog  -std=c++17 put_get_2processes.cpp
//~/test/op_equal/mpich/_inst/bin/mpirun -np 2 ./prog

int main(int argc, char ** argv)
{
	//MPI initial
	MPI_Init(&argc,&argv);

    int myWorldRank, targetRank;
    int numWorldNodes;
    long unsigned int *my_base = NULL;
    long unsigned int *putBuf = NULL;
    long unsigned int *getBuf = NULL;
    MPI_Win global_win;

    MPI_Comm_size(MPI_COMM_WORLD,&numWorldNodes);
    MPI_Comm_rank(MPI_COMM_WORLD,&myWorldRank);

    if(myWorldRank == 0)
    	targetRank = 1;
    else
    	targetRank = 0;

    //my_base = (long unsigned int *)malloc(sizeof(long unsigned int) * ELEM_PER_PROC);
    MPI_Alloc_mem(sizeof(long unsigned int) * 2, MPI_INFO_NULL, &my_base);
    MPI_Alloc_mem(sizeof(long unsigned int) * 2, MPI_INFO_NULL, &putBuf);
    MPI_Alloc_mem(sizeof(long unsigned int) * 2, MPI_INFO_NULL, &getBuf);

    my_base[0] = myWorldRank;
    my_base[1] = myWorldRank;

    getBuf[0] = getBuf[1] = 0;
    /* Allocate ELEM_PER_PROC integers for each process */
    putBuf[0] = (myWorldRank+101) * 1;
    putBuf[1] = (myWorldRank+101) * 2;

    MPI_Win_create(my_base, sizeof(long unsigned int) * 2, sizeof(long unsigned int), MPI_INFO_NULL, MPI_COMM_WORLD,
                   &global_win);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, targetRank, 0, global_win);

    MPI_Put(putBuf, 2, MPI_UNSIGNED_LONG, targetRank,
                    0, 2, MPI_UNSIGNED_LONG, global_win);

    MPI_Win_flush(targetRank, global_win);

	MPI_Get(getBuf, 2, MPI_UNSIGNED_LONG, targetRank,
                    0, 2, MPI_UNSIGNED_LONG, global_win);
	
	MPI_Win_flush(targetRank, global_win);
	std::cout<<myWorldRank<<" "<<getBuf[0]<<" "<<getBuf[1]<<std::endl;

    MPI_Win_unlock(targetRank, global_win);


	std::cout<<myWorldRank<<" "<<my_base[0]<<" "<<my_base[1]<<std::endl;
    putBuf[0] = putBuf[1] = myWorldRank + 10;
    //MPI_Win_lock(MPI_LOCK_EXCLUSIVE, myWorldRank, 0, global_win);

    MPI_Finalize();

	return 0;
}
