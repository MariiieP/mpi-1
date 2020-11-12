#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <cstring>
#include <cstdlib>
#define INITIATOR 0
#define SIZE 4

//   № 1
//int main(int argc, char * argv[]){
//
//    int myrank,slot;
//    int sizePartOfVector;
//    MPI_Init(&argc,&argv);
//    MPI_Comm_size(MPI_COMM_WORLD, &slot);
//    if (slot<3) MPI_Abort(MPI_COMM_WORLD,0);
//    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
//    if(myrank==0) {
//        sizePartOfVector = ((slot-1)/2);
//        int cntSend = slot%2 == 0 ? slot-2 : slot-1;
//        for (int i=1;i<=cntSend;i++) {
//            MPI_Send(&sizePartOfVector,1,MPI_INT,i,0,MPI_COMM_WORLD);
//        }
//
//        int tmpRes,result=0;
//        for (int i=1;i<=(slot-1)/2;i++) {
//            MPI_Recv(&tmpRes,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//            result += tmpRes;
//        }
//        printf("Result: %d\n",result);
//    }
//    else {
//        MPI_Recv(&sizePartOfVector,1,MPI_INT,0,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//        int *x;
//        x=new int[sizePartOfVector];
//        for (int i=0; i<sizePartOfVector; i++) {
//            x[i]=(myrank%2 == 0 ? myrank : myrank + 9);
//        }
//
//        if (myrank%2==0) {
//            MPI_Send(&x[0],sizePartOfVector,MPI_INT,myrank-1,0,MPI_COMM_WORLD);
//        } else {
//            int y[sizePartOfVector];
//            MPI_Recv(&y,sizePartOfVector,MPI_INT,myrank+1,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
////            printf("y: %d, %d\n",y[0], y[1]);
////            printf("x: %d, %d\n",x[0], x[1]);
//            int result=0;
//            for (int i=0;i<sizePartOfVector;i++)
//            result += x[i]*y[i];
////            printf("                   result: %d \n",result);
//            MPI_Ssend(&result,1,MPI_INT,0,0,MPI_COMM_WORLD);
//        }
//    }
//    MPI_Finalize();
//    return 0;
//
//}





//   № 2
//float Sum_series(float x,float eps)
//{
//    double powX = -x, previous = 1;
//    double sum = previous - 3*x ;
//
//    for (int pos = 3; abs(previous - sum) > eps ; pos ++  ) {
//        powX *= -x;
//        previous = sum;
//        sum +=  (powX * pos * (pos+1))/2 ;
//    }
//    return  sum;
//}
//
//int main(int argc, char * argv[]){
//
//    int myrank,slot,n;
//    float e;
//    float *sendX, *readX, *sendRes, *readRes;
//    MPI_Init(&argc,&argv);
//    MPI_Comm_size(MPI_COMM_WORLD, &slot);
//    if (slot<2) MPI_Abort(MPI_COMM_WORLD,0);
//    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
//
//    if(myrank==0)
//    {
//        float A=0,B=0;
//        A = 0;
//        B = 0.99;
//        e = 0.01;
//        n = 6;
////            double A1,B1;
////            std::cout<<"ENTER A and B\n";
////            std::cin>>A1>>B1;
//
////            printf("Enter A: ");
////            scanf("%f",&A);
////
////            printf("Enter B: ");
////            scanf("%f",&B);
////
////            printf("Enter epsilon: ");
////            do
////            scanf("%f",&e);
////            while (e<=0);
////
////            printf("Enter N: ");
////            do
////            scanf("%d",&n);
////            while (n<2);
//
//        sendX=new float[n];
//        float step = (float)(B-A)/(n-1);
//        for (int i=0; i<n; i++)
//            sendX[i]=A+i*step;
//    }
//    else sendX=NULL;
//
//    MPI_Bcast(&e,1,MPI_FLOAT,0,MPI_COMM_WORLD);
//    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
//
//    int add = (int)(n % slot != 0);
//    readX=new float[n/slot+add];
//    sendRes=new float[2*(n/slot+add)];
//    readRes=new float[2*n];
//
//    MPI_Scatter(sendX,n/slot+add,MPI_FLOAT,readX,n/slot+add,MPI_FLOAT,0,MPI_COMM_WORLD);
//
//    float x;
//    for (int i=0;i<n/slot+add;i++)
//    {
//        x=readX[i];
//        sendRes[2*i]=Sum_series(x,e);
//        sendRes[2*i+1]=1/pow(1+x, 3);
//    }
//
//    MPI_Gather(sendRes,2*(n/slot+add),MPI_FLOAT,readRes,2*(n/slot+add),MPI_FLOAT,0,MPI_COMM_WORLD);
//
//    if (myrank==0)
//    {
//        printf("x\t\t\tSumma\t\tExact value\n");
//        for (int i=0; i<n;i++)
//            printf("%f\t%f\t%f\n",sendX[i],readRes[2*i],readRes[2*i+1]);
//    }
//
//    MPI_Finalize();
//    return 0;
//}



//   № 3
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

int main(int argc, char* argv[])
{
    const int n=5;
    int rank,size,data[n],result[n];
    MPI_Op op1;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand(time(NULL));
    char str[100]="Process: ";
    char buf[5];
    snprintf(buf, sizeof(buf), "%d", rank);
//    itoa(rank,buf,10);
    strncat(str,buf,strlen(buf));
    strncat(str," array: ",8);
    for (int i=0;i<n;i++)
    {
        data[i] = (rand()*(rank+1)-rand()*2)%129; //диапазон от -128 до 128
        snprintf(buf, sizeof(buf), "%d", rank);
//        itoa(data[i],buf,10);
        strncat(str,buf,strlen(buf));
        strncat(str,", ",2);
        result[i]=0;
    }
    printf("момп %s\n",str);
    MPI_Op_create((MPI_User_function *)findmax,1,&op1);
    MPI_Reduce (&data,&result, n, MPI_INT, op1, 0, MPI_COMM_WORLD);
    MPI_Op_free(&op1);
    if (rank==0)
    {
        printf("Result: ");
        for (int i=0;i<n;i++)
            printf("%d ",result[i]);
    }
    MPI_Finalize();
    return 0;
}

