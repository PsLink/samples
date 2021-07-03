// mpicxx test.cc 
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <mpi.h>
#include <stdio.h>

#define DType       uint64_t
#define MPI_DTYPE   MPI_UINT64_T

using namespace std;

void Drawline(int rank)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
    cout << "-----------------------------------------------------------------" << endl;
}

void Display(
    DType *buffer, 
    size_t b_size, 
    int rank,
    const char *info) 
{
    cout << info << ' ' << "Rank_" << rank << ' : ';
    for (int i = 0; i < b_size; i++) {
        cout << buffer[i] << ' ';
    }
    cout << endl << flush;
}

int main() {
    int rank;
    int size;
    int name_len;
    char name[100];

    int root = 0;
    srand(time(NULL));

    // Initialize -----------------------------------------------------------------

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(name, &name_len);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Bcast -----------------------------------------------------------------
    // From root to all ------------------------------------------------------
    // MPI_Bcast(
    // void* data,
    // int count,
    // MPI_Datatype datatype,
    // int root,
    // MPI_Comm communicator)

    // MPI_Barrier(MPI_COMM_WORLD);
    // if (rank == root) {
    //     cout << "Bcast ";   
    // }
    // Drawline(rank);
    // MPI_Barrier(MPI_COMM_WORLD);

    size_t b_size = 10;
    DType *buffer = new DType[b_size];
    memset(buffer, 0, b_size * sizeof(DType));

    if (rank == root) {
        for (int i = 0; i < b_size; i++) {
            buffer[i] = rand();
        }
    }

    Display(buffer, b_size, rank, "before Bcast, buffer:");

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(buffer, b_size, MPI_DTYPE, root, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // Drawline(rank);
    Display(buffer, b_size, rank, "after Bcast, buffer:");
  

    // Gather -----------------------------------------------------------------
    // From all to root ------------------------------------------------------
    // MPI_Gather(
    // void* send_data,
    // int send_count,
    // MPI_Datatype send_datatype,
    // void* recv_data,
    // int recv_count,
    // MPI_Datatype recv_datatype,
    // int root,
    // MPI_Comm communicator)

    // MPI_Barrier(MPI_COMM_WORLD);
    // if (rank == root) {
    //     cout << "Gather ";
    // }
    // Drawline(rank);
    // MPI_Barrier(MPI_COMM_WORLD);

    size_t l_size = b_size/size;
    DType *local_buffer = new DType[l_size];

    for (int i = 0; i < l_size; i++) local_buffer[i] = rank + 1;
  
    memset(buffer, 0, b_size * sizeof(DType));

    
    // Drawline(rank);
    Display(local_buffer, l_size, rank, "before gather, local buffer:");

    // Drawline(rank);
    Display(buffer, b_size, rank, "before gather, buffer:");

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(local_buffer, l_size, MPI_DTYPE, buffer, l_size, MPI_DTYPE, root, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // Drawline(rank);
    Display(buffer, b_size, rank, "after gather, buffer:");

    // Gather to all threads ------------------------------------------------
    // MPI_Allgather(
    // void* send_data,
    // int send_count,
    // MPI_Datatype send_datatype,
    // void* recv_data,
    // int recv_count,
    // MPI_Datatype recv_datatype,
    // MPI_Comm communicator)
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == root) {
        cout << "Allgather ";   
    }
    // Drawline(rank);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(local_buffer, l_size, MPI_DTYPE, buffer, l_size, MPI_DTYPE, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // Drawline(rank);
    Display(buffer, b_size, rank, "after allgather, buffer");
    MPI_Barrier(MPI_COMM_WORLD);
    // Gatherv -----------------------------------------------------------------
    // Gather different size from different rank -------------------------------
    // int MPI_Gatherv(void* buffer_send,
    //             int count_send,
    //             MPI_Datatype datatype_send,
    //             void* buffer_recv,
    //             const int* counts_recv,
    //             const int* displacements,
    //             MPI_Datatype datatype_recv,
    //             int root,
    //             MPI_Comm communicator);
    // MPI_Barrier(MPI_COMM_WORLD);
    // if (rank == root) {
    //     cout << "Gatherv ";   
    // }
    // // Drawline(rank);
    // MPI_Barrier(MPI_COMM_WORLD);

    int *recevie_counts = new int[size];
    int *displacement = new int[size];

    for (int i=0; i<size; i++) 
        recevie_counts[i] = i+1; // receive size i from rank i

    displacement[0] = 0;
    for (int i=1; i<size; i++)
        displacement[i] = displacement[i-1] + recevie_counts[i-1];

    delete[] buffer;
    delete[] local_buffer;

    l_size = rank + 1;
    local_buffer = new DType[l_size];
    for (int i = 0; i < l_size; i++) local_buffer[i] = rank + 1;


    b_size = 0;
    for (int i=0; i<size; i++) 
        b_size += recevie_counts[i];
    buffer = new DType[b_size];
    memset(buffer, 0, b_size * sizeof(DType));

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(local_buffer, l_size, MPI_DTYPE, buffer, recevie_counts, displacement, MPI_DTYPE, root, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // Drawline(rank);
    Display(buffer, b_size, rank, "after gatherv, buffer");

    // Finalize -------------------------------------------------------------------

    delete[] buffer;
    delete[] local_buffer;

    delete[] recevie_counts;
    delete[] displacement;

    MPI_Finalize();
    return 0;
}