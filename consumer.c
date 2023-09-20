#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int index = 0;
    char character;
    
    if ( argc < 2 ) 
    {
        printf("No value for M was given.\n");
        return 0;
    }
    
    int M = atoi( argv[1] );
    //printf("Enter %d values one by one (press Enter after putting in each value).\n",M);
    
    while( index < M ) 
    {
    	++index;
        character = getchar();
        //printf("%c\n", character);
 
        if ( character == '\n' )
        {
            --index;
        } 
        else if ( character == EOF ) 
        {
            return 0;
        }
    }

}

