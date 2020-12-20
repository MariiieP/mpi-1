#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#define INITIATOR 0
#define SIZE 4

//   № 1
// Два вектора А и В размерности N распределены между процессам: значения векторов проыессов с четными номерами составляют вектор А,
// значения векторов с нечетными номерами составляют вектор В. Вычислить скалярное произведение (А, В).
// Два процесса, содержащие соответствуюшие компоненты векторов, обмениваются сообщениями, один из них выполняет сложение и отправляет результат нулевому вектору,
// который получает и выводит результат.
int main(int argc, char * argv[]){

    int myrank,slot;
    int sizePartOfVector;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &slot);
    if (slot<3) MPI_Abort(MPI_COMM_WORLD,0);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    if(myrank==0) {
        int cntSlotBySum = ((slot-1)/2);
        sizePartOfVector = cntSlotBySum;
        int cntSend = slot%2 == 0 ? slot-2 : slot-1;
        for (int i=1;i<=cntSend;i++) {
            MPI_Send(&sizePartOfVector,1,MPI_INT,i,0,MPI_COMM_WORLD);
        }

        int tmpRes,result=0;
        for (int i=1;i<=cntSlotBySum;i++) {
            MPI_Recv(&tmpRes,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result += tmpRes;
        }
        printf("Result: %d\n",result);
    }
    else {
        MPI_Recv(&sizePartOfVector,1,MPI_INT,0,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *x;
        x=new int[sizePartOfVector];
        for (int i=0; i<sizePartOfVector; i++) {
            x[i]=(rand()*myrank%10);
        }

        if (myrank%2==0) {
            MPI_Send(&x[0],sizePartOfVector,MPI_INT,myrank-1,0,MPI_COMM_WORLD);
        } else {
            int y[sizePartOfVector];
            MPI_Recv(&y,sizePartOfVector,MPI_INT,myrank+1,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("y: %d, %d\n",y[0], y[1]);
            printf("x: %d, %d\n",x[0], x[1]);
            int result=0;
            for (int i=0;i<sizePartOfVector;i++)
            result += x[i]*y[i];
//            printf("                   result: %d \n",result);
            MPI_Ssend(&result,1,MPI_INT,0,0,MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;

}
