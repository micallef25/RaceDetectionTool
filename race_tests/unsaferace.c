#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;
int c = 0;
int a = 0;
int racevar;
int var2;

void *fnC()
{
    int i;
    //char* var = malloc(sizeof(char));
    for(i=0;i<10;i++)
    {   
        c++;
        a++;
        racevar++;
        var2++;
        printf(" %d", c); 
    }   
   // free(var);
}


int main()
{
    int rt1, rt2;
    pthread_t t1, t2; 

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    /* Create two threads */
    if( (rt1=pthread_create( &t1, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt1);
    if( (rt2=pthread_create( &t2, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt2);
    /* Wait for both threads to finish */
    pthread_join( t1, NULL);
    pthread_join( t2, NULL);
    pthread_mutex_destroy(&lock);
    printf ("\n");
    return 0;
}
