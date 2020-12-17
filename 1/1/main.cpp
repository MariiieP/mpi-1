#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//№1
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


//   № 2
//Требуется вычислить значения суммы ряда в n точках заданного интервала [A,B] с точностью e. Процесс-мастер вводит с клавиатуры A,B,e,n,
//вычисляет аргументы и рассылает каждому процессу n/k значений. Каждый процесс вычисляет значения функции в полученных точках и отправляет процессу-мастеру.
// Процесс мастер выводит полученные результаты и точные значения функции в соответствущих точках.
float Sum_series(float x,float eps)
{
    double powX = -x, previous = 1;
    double sum = previous - 3*x ;

    for (int pos = 3; abs(previous - sum) > eps ; pos ++  ) {
        powX *= -x;
        previous = sum;
        sum +=  (powX * pos * (pos+1))/2 ;
    }
    return  sum;
}

int main(int argc, char * argv[]){

    int myrank,slot,n;
    float e;
    float *sendX, *readX, *sendRes, *readRes;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &slot);
    if (slot<2) MPI_Abort(MPI_COMM_WORLD,0);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    if(myrank==0)
    {
        float A=0,B=0;
        std::string line;
        std::ifstream in("/Users/maria_urevna/desctopTop/лабы/1к мага/mpi-1/point.txt");
       if (in.is_open())
       {
          while (getline(in, line))
          {
              std::stringstream stream(line);
              stream >> A >> B >> e >> n;
              std::cout << "A:"<< A << " B:" << B << " e:" << e << " n:" << n << std::endl;
          }
       }
       in.close();

        sendX=new float[n];
        float step = (float)(B-A)/(n-1);
        for (int i=0; i<n; i++)
            sendX[i]=A+i*step;
    }
    else sendX=NULL;

    MPI_Bcast(&e,1,MPI_FLOAT,0,MPI_COMM_WORLD);
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

    int addValue = (int)(n % slot != 0);
    readX=new float[n/slot+addValue];
    sendRes=new float[2*(n/slot+addValue)];
    readRes=new float[2*n];

    MPI_Scatter(sendX,n/slot+addValue,MPI_FLOAT,readX,n/slot+addValue,MPI_FLOAT,0,MPI_COMM_WORLD);

    float x;
    for (int i=0;i<n/slot+addValue;i++)
    {
        x=readX[i];
        sendRes[2*i]=Sum_series(x,e);
        sendRes[2*i+1]=1/pow(1+x, 3);
    }

    MPI_Gather(sendRes,2*(n/slot+addValue),MPI_FLOAT,readRes,2*(n/slot+addValue),MPI_FLOAT,0,MPI_COMM_WORLD);

    if (myrank==0)
    {
        printf("x\t\t\tSum_series\t\tSumma\n");
        for (int i=0; i<n;i++)
            printf("%f\t%f\t%f\n",sendX[i],readRes[2*i],readRes[2*i+1]);
    }

    MPI_Finalize();
    return 0;
}

// Каждый процесс заполяет свой массив размером n случайными числами. Для решения задачи использовать
// операции приведения с собственной функцией для решения задачи. Результат - вектор размером n,
// каждый элемент которого получен по правилу определенной в задае функции.
// Найти максимальную степень двойки, превосходящую максимальное значение.
//   № 3
void GetMinPowerOfTwoLargerThan ( int * invec, int * inoutvec, int * len, MPI_Datatype *dtype)
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
    const int n=6;
    int rank,size,data[n],result[n];
    MPI_Op myOp;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int i=0;i<n;i++)
    {
        data[i] = rand()*(rank)%15;
        result[i]=0;
    }
    printf("%d, %d, %d, %d\n",data[0],data[1],data[2],data[3]);
    MPI_Op_create((MPI_User_function *)GetMinPowerOfTwoLargerThan,true,&myOp);
    MPI_Reduce (&data,&result, n, MPI_INT, myOp, 0, MPI_COMM_WORLD);
    MPI_Op_free(&myOp);

    if (rank==0)
    {
        printf("\nResult: \n");
        for (int i=0;i<n;i++)
            printf("%d ",result[i]);
    }
    MPI_Finalize();
    return 0;
}

//   № 4
// Создать описатель типа и испльзовать его при передаче данных в качестве шаблона для следующего преобразования
// mmmmmm
// m    m
// mmmmmm
#define cntRow 7
#define cntCol 7
void printMatrix(int (*arr)[cntCol], int num)
{
    char message[200] = "Matrix ";
    strncat(message,":\n",3);
    char buf[5];
    for (int i=0;i<cntRow;i++)
    {
        for (int j=0;j<cntCol;j++)
        {
            snprintf(buf, sizeof(buf), "%d", arr[i][j]);
            strncat(message,buf,strlen(buf));
            strncat(message," ",1);
        }
        strncat(message,"\n",2);
    }
    printf("%s\n",message);
}

int main (int argc, char *argv[])
{
    int myrank,slot;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &slot);

    int blocklength[cntRow-1];
    int displacement[cntRow-1];

    blocklength[0]=cntCol+1;
    for (int i=1;i<cntRow-2;i++)
        blocklength[i]=2;
    blocklength[cntRow-2]=cntCol+1;
    displacement[0]=0;
    for (int i=1;i<cntRow-1;i++)
        displacement[i]=(i+1)*cntCol-1;

    MPI_Datatype My_Type;
    MPI_Type_indexed(cntRow-1,blocklength,displacement,MPI_INT,&My_Type);
    MPI_Type_commit(&My_Type);

    int matr[cntRow][cntCol];
    for(int i = 0; i < cntRow; i ++)
        for(int j = 0; j < cntCol; j ++)
            matr[i][j]=myrank;


    if (myrank%2==0) {
        printMatrix(matr, 0);
        if ((slot%2!=0 && myrank != slot-1) || slot%2==0) {
            MPI_Send(&matr[0][0],1,My_Type,myrank+1,0,MPI_COMM_WORLD);
        }
    } else
    {
        if ((slot%2!=0 && myrank != slot-1) || slot%2==0) {
            MPI_Recv(&matr[0][0],1,My_Type,myrank-1,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        printMatrix(matr,1);
    }
    MPI_Type_free(&My_Type);
    MPI_Finalize();

    return 0;
}
