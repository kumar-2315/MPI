#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv){

    MPI_Init( &argc , &argv);

    int rank, size;
    MPI_Comm_rank( MPI_COMM_WORLD , &rank);
    MPI_Comm_size( MPI_COMM_WORLD , &size);

    if (argc < 2){
        if( rank == 0){
        printf("Please provide the file name in the terminal arrgument! \n");
        }
        MPI_Finalize();
        return -1; 
    }

    const char* file_name  = argv[1];

    MPI_File file;
    MPI_Status status;

    int mpi_file_status = MPI_File_open( MPI_COMM_WORLD , file_name , MPI_MODE_RDONLY , MPI_INFO_NULL , &file);

    if (mpi_file_status != MPI_SUCCESS){

        if (rank == 0){
            printf("There was a problem in opening the file %s.\nPlease, check the file name and run again", file_name);
        }
        MPI_Finalize();
        return -1;
    }

MPI_Offset file_size;
MPI_File_get_size( file , &file_size);

MPI_Offset sub_part_size = file_size / size;
MPI_Offset starting_address = sub_part_size * rank;

if (rank == size-1){
    sub_part_size += file_size % size;
}

unsigned char* buffer = (unsigned char*)malloc(sub_part_size);
MPI_File_read_at( file , starting_address , buffer , sub_part_size , MPI_BYTE , &status);

MPI_File_close(&file);
free(buffer);

MPI_Finalize();
return 0;

}


