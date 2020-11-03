#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <cstring>
#include <cstdlib>
#define INITIATOR 0
#define SIZE 4

void findmax ( int * invec, int * inoutvec, int * len, MPI_Datatype *dtype)
{
    for (int i=0;i<*len;i++)
    {
        int res;
        if (invec[i]<1) res = 1;
        else res = pow(2,ceil(log10((double)invec[i])/log10((double)2)));
        if (res>=inoutvec[i]) inoutvec[i] = res;
    }
}


int main(int argc, char * argv[]){

//      MPI_Init(&argc, &argv);
//      MPI_Comm_size(MPI_COMM_WORLD, &p);
//      MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
//
//      printf("size: %d rank: %d\n", p, my_rank);
//
//      MPI_Finalize();


//   № 1
    char message[24];
    int myrank,slot;
    int TAG=0;
    int sizePartOfVector;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &slot);
    if (slot<3) MPI_Abort(MPI_COMM_WORLD,0); // Нужно мининмум 3 процесса
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    if(myrank==0)
    {
        sizePartOfVector = ((slot-1)/2); // размерность кусков вектора

//        int numprocs2 = 5;
//        int m2;
//        m2 = (6-1)%2;
//        printf("m, numprocs, myrank: %d, %d, %d\n",m2, numprocs2, sizePartOfVector);
        printf("m, numprocs, myrank: %d, %d, %d\n",sizePartOfVector, slot, myrank);

        int cntSend = slot%2 == 0 ? slot-2 : slot-1;
        for (int i=1;i<=cntSend;i++) {
            MPI_Send(&sizePartOfVector,1,MPI_INT,i,TAG,MPI_COMM_WORLD);
        }

        int tmp_res,res=0;
        for (int i=1;i<=(slot-1)/2;i++)
        {
            MPI_Recv(&tmp_res,1,MPI_INT,MPI_ANY_SOURCE,TAG,MPI_COMM_WORLD,&status);
            res += tmp_res;
        }
        printf("Result: %d\n",res);
    }
    else if ((slot-1)%2==0 || myrank!=slot-1) // если я - не последний и нечетный, которому нет работы
    {
        printf("_______________slot, myrank:  %d,  %d\n",slot, myrank);
        MPI_Recv(&sizePartOfVector,1,MPI_INT,0,TAG,MPI_COMM_WORLD,&status);
        bool not_full = sizePartOfVector<0 &&
            (myrank==slot-1-(slot-1)%2 // я - последний процесс
            || myrank==slot-2-(slot-1)%2); // или я - предпоследний процесс
        sizePartOfVector=abs(sizePartOfVector);
        int *x;
        x=new int[sizePartOfVector];
        srand(time(NULL));
        char str[100]="Process: ";
        char buf[5];
        snprintf(buf, sizeof(buf), "%d", myrank);
//        itoa(myrank,buf,10);
        strncat(str,buf,strlen(buf));
        strncat(str," array: ",8);
        for (int i=0; i<sizePartOfVector; i++)
        {
            if (i==sizePartOfVector-1 && not_full) x[i]=0; // если N нечетное, то m увеличена на 1
            else {

                x[i]=(myrank%2 == 0 ? myrank + 1 : myrank + 2);
//                x[i]=rand()*myrank%100;
            }

            snprintf(buf, sizeof(buf), "%d", myrank);
//            itoa(x[i],buf,10);
            strncat(str,buf,strlen(buf));
            strncat(str,", ",2);

        }
//        printf("%s\n",str);
        if (myrank%2==0) {// если я - А, отправляю свой вектор процессу, у которого кусок В
//            printf("_________________________str2: %d, %d, %d, %d\n",x[0], x[1], x[2], x[3]);
            MPI_Send(&x,sizePartOfVector,MPI_INT,myrank-1,TAG,MPI_COMM_WORLD);
        }
        else // если я - В, перемножаю и отправляю мастеру
        {
            int *y;
//            printf("_________________________ x: %d, %d, %d, %d\n",x[0], x[1], x[2], x[3]);
            MPI_Recv(&y,sizePartOfVector,MPI_INT,myrank+1,TAG,MPI_COMM_WORLD,&status);
//            printf("_________________________y: %d, %d, %d, %d\n",y[0], y[1], y[2], y[3]);
            int res=0;
            for (int i=0;i<sizePartOfVector;i++)
                res += x[i]*y[i];
//            printf("_________________________res: %d, %d, %d, %d\n",x[0], x[1], x[2], x[3]);
            MPI_Ssend(&res,1,MPI_INT,0,TAG,MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;

//    const int n = 2;
//    int rank,size,data[n],result[n];
//    MPI_Op op1;
//    MPI_Init(&argc,&argv);
//    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//    srand(time(NULL));
//    char str[100]="Process: ";
//    char buf[5];
//    snprintf(buf, sizeof(buf), "%d", rank);
////        itoa(rank,buf,10);
//    strncat(str,buf,strlen(buf));
//    strncat(str," array: ",8);
//    for (int i=0;i<n;i++)
//    {
//        data[i] = (rand()*(rank+1)-rand()*2)%129; //диапазон от -128 до 128
//        snprintf(buf, sizeof(buf), "%d", data[i]);
////            itoa(data[i],buf,10);
//        strncat(str,buf,strlen(buf));
//        strncat(str,", ",2);
//        result[i]=0;
//    }
//    printf("%s\n",str);
//    MPI_Op_create((MPI_User_function *)findmax,1,&op1);
//    MPI_Reduce (&data,&result, n, MPI_INT, op1, 0, MPI_COMM_WORLD);
//    MPI_Op_free(&op1);
//    if (rank==0)
//    {
//        printf("Result: ");
//        for (int i=0;i<n;i++)
//            printf("%d ",result[i]);
//    }
//    MPI_Finalize();
//    return 0;

//    int sizeVector = 4;
//    int vectorOdd[sizeVector], vectorEven[sizeVector];
//    MPI_Init(&argc, &argv);
//
//    // Generate a vectorOdd
//    for( int i=0; i<sizeVector; i++){
//        vectorOdd[i] = (i%2 == 0 ? i + 1 : i + 6);
//    }
//
//    // Generate a vectorEven
//    for( int i=0; i<sizeVector; i++){
//        vectorEven[i] = (i%2 == 0 ? i + 2 : i + 7);
//    }
//
//    printf("Received vector: \n");
//    for (int i = 0; i < SIZE; i++) {
//        printf("%d ", vectorEven[i]);
//    }
//    printf("\n");
//    for (int i = 0; i < SIZE; i++) {
//        printf("%d ", vectorOdd[i]);
//    }
//    printf("\n");
//
//
//    // Get the number of processes and check only 2 processes are used
//    int size;
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//    if(size != 2)
//    {
//        printf("This application is meant to be run with 2 processes.\n");
//        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
//    }
//
//    // Get my rank and do the corresponding job
//    enum rank_roles { SENDER, RECEIVER };
//    int rank;
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    switch(rank)
//    {
//        case SENDER:
//        {
//            // Create the datatype
//            MPI_Datatype column_type;
//
//            int buffer[4] = { vectorEven[0] * vectorOdd[0], vectorEven[1] * vectorOdd[1], vectorEven[2] * vectorOdd[2], vectorEven[3] * vectorOdd[3]};
//
//            MPI_Request request;
//            printf("MPI process %d sends values %d, %d, %d, %d.\n", rank, buffer[0], buffer[1], buffer[2], buffer[3]);
//            MPI_Send(&buffer[0], 4, MPI_INT, RECEIVER, 0, MPI_COMM_WORLD);
//            break;
//        }
//        case RECEIVER:
//        {
//            // Receive the message
//            int received[4];
//            MPI_Recv(&received, 4, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//            int scalarProduct = received[0] + received[1] + received[2] + received[3];
//            printf("MPI process %d received values: %d, %d, %d, %d.\n", rank, received[0], received[1], received[2], received[3]);
//            printf("scalar product: %d\n", scalarProduct);
//            break;
//        }
//    }
//
//   MPI_Finalize();
    
}
