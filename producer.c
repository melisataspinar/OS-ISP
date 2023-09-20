#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <string.h>



int main(int argc, char *argv[])
{
    char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    srand(getpid());

    if (argc < 2) {
        printf("Must specify M\n");
        return -1;
    }
    int M = atoi(argv[1]);

    char res[M+1];
    int m = 0;
    while (m < M) {
        res[m++] = chars[rand() % strlen(chars)];
    }
    res[M] = '\0';

    printf("%s", res);
}
