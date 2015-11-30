#include"inverse_matrix.h"
//高斯消元法求矩阵逆
void inverseMatrix(double input[5][5],double output[5][5],int dimension)
{
    int i,j,k;
    //将输出矩阵初始化为单位矩阵
    for(i=0;i<dimension;i++)
    {
        memset(output[i],0,sizeof(double)*dimension);
        output[i][i]=1;
    }
    for(i=0;i<dimension;++i)  //依次处理每一列
    {
        for(j=0;i<dimension;++j)  //如果当前行当前列值为0，做行变换
        {
            if(input[j][i]!=0)
            {
                swap(input,output,dimension,0,j);
                break;
            }
        }
        for(j=0;j<dimension;++j)  //依次处理每一行
        {
            if(j==0)  //如果是第一行，将input[j][i]设置为1，其他元素均除以input[i][i]
            {
                for(k=dimension-1;k>=0;--k)
                    output[j][k]/=input[j][i];
                for(k=dimension-1;k>=i;--k)
                    input[j][k]/=input[j][i];
            }
            else  //如果不是第一行，将每一行的input[j][i]设置为0，该行其他元素都要倍数的减去第一行对应位置上的值
            {
                for(k=dimension-1;k>=0;--k)
                    output[j][k]=output[j][k]-input[j][i]/input[0][i]*output[0][k];
                for(k=dimension-1;k>=i;--k)
                    input[j][k]=input[j][k]-input[j][i]/input[0][i]*input[0][k];
            }
        }
        swap(input,output,dimension,0,(i+1)%dimension);  //每次都将下一次需要处理的行和当前的第一行交换
    }
    reorderOutput(output,dimension); //因为之前的交换操作，行顺序乱了，需要重新排列一下，即把第一行的数据放到最后一行后面
}

void swap(double input[5][5], double output[5][5], int dimension, int first_row, int second_row)
{
    double* temp_row1=new double[dimension];
    double* temp_row2=new double[dimension];
    int i;
    for(i=0;i<dimension;i++)
    {
        temp_row1[i]=input[first_row][i];
        temp_row2[i]=output[first_row][i];
    }
    for(i=0;i<dimension;i++)
    {
        input[first_row][i]=input[second_row][i];
        output[first_row][i]=output[second_row][i];
        input[second_row][i]=temp_row1[i];
        output[second_row][i]=temp_row2[i];
    }

    delete[] temp_row1;
    delete[] temp_row2;
}

void reorderOutput(double output[5][5], int dimension)
{
    double**temp=new double*[dimension];
    for(int i=0;i<dimension;++i)
        temp[i]=new double[dimension];

    for(int i=1;i<dimension;++i)
        memcpy(temp[i-1],output[i],sizeof(double)*dimension);
    memcpy(temp[dimension-1],output[0],sizeof(double)*dimension);

    for(int i=0;i<dimension;++i)
        memcpy(output[i],temp[i],sizeof(double)*dimension);

    for(int i=0;i<dimension;++i)
        delete[] temp[i];
    delete[] temp;
}
