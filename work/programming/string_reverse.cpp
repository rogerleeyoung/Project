#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void reverse(char* str)
{
    char temp;
    int i,j,begin,end;
    j=strlen(str)-1;
    printf("string=%s\n",str);
    //第一步是进行全盘反转，将单词变成“tneduts a ma I”
    while(j>i)
    {
        temp=str[i];
        str[i]=str[j];
        str[j]=temp;
        j--;
        i++;
    }
    printf(" string=%s\n",str);
    i=0;
    //第二步进行部分反转，如果不是空格则开始反转单词
    while(str[i]!='\0')
    {
        if(str[i]!=' ')
        {
            begin=i;
            while(str[i]&&str[i]!=' ')
            {
                i++;
            }
            end=i-1;
            while(end>begin)
            {
                temp=str[begin];
                str[begin]=str[end];
                str[end]=temp;
                end--;
                begin++;
            }
        }
        else
        {
            i++;
        }
    }
}
int main(void)
{
    char* str=(char*)malloc(100);
    gets(str);
    reverse(str);
    printf(" string=%s\n",str);
    return 0;
}