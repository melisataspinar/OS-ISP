#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h> 

// constants
const int MAX_SIZE_STRINGS = 32;

// global variables
bool legal;
bool printed_counts = true;
int buffer_size;
bool pipe_exists;
char *statement_2[10];
char *statement_1[10];
char entry[256];

void run( bool isFork, char *statement[] ) 
{
    if ( !isFork )
    {
    	execvp( statement[0], statement );
    }
    else
    {
		if ( fork() == 0 )
		{
			execvp( statement[0], statement );
		}
		wait(NULL);
    }
}

void run_version( bool normal_version ) 
{
    if ( normal_version ) 
    {
	    int pipe_fd[2];
	    if ( pipe( pipe_fd ) < 0 ) 
	    {
			printf("Pipe could not be opened.\n");
			exit(-1); 
	    }

	    if ( fork() == 0 ) 
	    {
			dup2( pipe_fd[1], 1);
			close( pipe_fd[1] );
			close( pipe_fd[0] );
			run( false, statement_1 );
	    }

	    if ( fork() == 0 ) 
	    {
			dup2( pipe_fd[0], 0) ;
			close( pipe_fd[1] );
			close( pipe_fd[0] );
			run( false, statement_2 );
	    }
	    
	    close( pipe_fd[1] );
	    close( pipe_fd[0] );
	    wait(NULL);
	    wait(NULL);
    }
    else // tapped version
    {
	    int index2 = 1;
	    int character_count = 0;
	    int read_call_count = 0;
	    int write_call_count = 0;
	    char *str;
	    char *reading_buffer = malloc( buffer_size * sizeof(char) );

	    // pipe 1
	    int pipe_fd1[2];    
	    if ( pipe( pipe_fd1 ) < 0 )  
	    {
			printf("Pipe 1 could not be opened.\n");
			exit(-1); 
	    }

	    if (fork() == 0) 
	    {
			dup2( pipe_fd1[1], 1 );
			close( pipe_fd1[1] );
			close( pipe_fd1[0] );
			run( false, statement_1 );
	    }

	    // pipe 2
	    int pipe_fd2[2];
	    if ( pipe( pipe_fd2 ) < 0 )  
	    {
			printf("Pipe 2 could not be opened.\n");
			exit(-1); 
	    }

	    if ( fork() == 0 ) 
	    {
			dup2( pipe_fd2[0], 0 );
			close( pipe_fd1[1] );
			close( pipe_fd1[0] );
			close( pipe_fd2[1] );
			close( pipe_fd2[0] );
			run( false, statement_2 );
	    }
	    
	    close( pipe_fd1[1] );
	    close( pipe_fd2[0] );




	    int number_of_bytes = read( pipe_fd1[0], reading_buffer, buffer_size );
	    character_count += number_of_bytes;
	    
	    while ( number_of_bytes > 0 )
	    {
	    		write(pipe_fd2[1], reading_buffer, number_of_bytes);
			number_of_bytes = read( pipe_fd1[0], reading_buffer, buffer_size);
			write_call_count++;
			read_call_count++;
			character_count += number_of_bytes;
	    }
	    	    
	    if ( printed_counts )
	    {
		    printf( "\ncharacter-count: %d", character_count );
		    printf( "\nread-call-count: %d", read_call_count );
		    printf( "\nwrite-call-count: %d\n", write_call_count );
	    }

	    close( pipe_fd1[0] );
	    close( pipe_fd2[1] );

	    wait(NULL);
	    wait(NULL);

	}
}

void process_entry() 
{
    char *substr = NULL;
    int substr_index = 0;

	substr = strtok(entry, " ");
	substr_index = 0;

	while ( true )
	{
		statement_1[substr_index] = substr;
		++substr_index;
		substr = strtok(NULL, " ");
		if ( (!pipe_exists) && substr == NULL )
			break;
		if ( pipe_exists && strcmp(substr, "|") == 0 )
			break;
	}
	statement_1[substr_index] = NULL;

	if ( pipe_exists )
	{
		substr_index = 0;
		while (substr != NULL)
		{
			substr = strtok(NULL, " ");
			statement_2[substr_index] = substr;
			++substr_index;
		}
	}
}

void statement() 
{
    legal = true;
	entry[0] = '\0';

    printf("Enter a statement (CTRL C to exit): ");
    scanf("%[^\n]", entry); 
    
	int c_clean;
    while ( true )
    {
        c_clean = getchar();
        if ( !(c_clean != '\n' && c_clean != EOF) ) 
        	break;
    } 

    if ( strlen(entry) == 0 ) 
	{
        printf("Illegal Input. Empty statement was entered.\n");
        legal = false;
        return;
    }

	// TODO
	if ( entry[strlen(entry) - 1] == '|' ) 
	{
        printf("Illegal input. Pipe is at last index.\n");
        legal = false;
        return;
    }

    pipe_exists = false;
    int i;
    for (i = 0; i < strlen(entry); i++) 
	{
        if (entry[i] == '|') {
            if (pipe_exists) {
                printf("Illegal Input. Multiple pipes entered.\n");
                legal = false;
                return;
            }
            pipe_exists = true;
        }
    }

    process_entry();
}


int main(int argc, char* argv[]) 
{
    int version; // 1: normal, 2: tapping
    
    // for timing:
    struct timeval start;
    struct timeval finish;
    time_t t;
    srand( (unsigned) time(&t) );
    int n;
        
    if (argc > 1) 
    {
        buffer_size = atoi( argv[1] );
        version = atoi( argv[2] );

        if ( version != 1 && version != 2 ) 
        {
            printf("Illegal Version. Enter 1 for normal, 2 for tapped mode.\n" );
            return -1;
        }
        if ( buffer_size < 0 || buffer_size > 4096 ) 
        {
            printf("Illegal Buffer Size. Enter 1 <= buffer size <= 4096.\n" );
            return -1;
        }
    } 
    else 
    {
        printf("ERROR: No buffer size or mode selection entered.\n");
		return -1;
    }
    
    while (true) 
    {
        statement();

	if (!legal) 
        {
            continue;
        }
	
	n = 0;
	suseconds_t run_time = 0;
	printed_counts = true;
	while ( n < 1 )
	{
		gettimeofday( &start, NULL );
		
		if ( !pipe_exists ) 
		{
		    run( true, statement_1 );
		}
		else 
		{
			if ( version == 1 )
				run_version( true );
			if ( version == 2 )
				run_version( false );
		}
		gettimeofday( &finish, NULL );
            	run_time += finish.tv_sec * 1000000 + finish.tv_usec - start.tv_sec * 1000000 - start.tv_usec;
            	n++;
            	printed_counts = false;
	}
	run_time /= n;
	//printf("Runtime: %ld us\n", run_time);
	
    }
}


