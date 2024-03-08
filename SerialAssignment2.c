/****************************************************************************** 
*  Name: Nicholas Bornkamp                                                    *
*  ID: 916320670                                                              *
*  Homework #2                                                                *
*   I vow that my work is my own, unless otherwise explained or outlined.     *
*  To Compile: gcc -o a2.out Assignment2.c -fopenmp                           *
*  To run: ./a2.out <size> <iterations> <threads>                             *
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//#include <omp.h>

double gettime(void) {
  struct timeval tval;

  gettimeofday(&tval, NULL);

  return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

int **allocarray(int P, int Q) {
  int i;
  int *p, **a;
  
  p = (int *)malloc(P*Q*sizeof(double));
  a = (int **)malloc(P*sizeof(double*));

  if (p == NULL || a == NULL) 
    printf("Error allocating memory\n");

  /* for row major storage */
  for (i = 0; i < P; i++)
    a[i] = &p[i*Q];
  
  return a;
}

int **initarray(int **a, int mrows, int ncols) {
  int i,j;
    
  for (i=1; i<mrows-1; i++)
    for (j=1; j<ncols-1; j++)
      // a[i][j] = drand48()*value;
      //a[i][j] = value;
      
      a[i][j] = (rand() % 10 < 7) ?  0 : 1;
  
  return a;
}

/* Simple method for testing array methods */
int **testtarray(int **a, int mrows, int ncols) {
  int i,j;
    int val = 1;
  for (i=1; i<mrows-1; i++)
    for (j=1; j<ncols-1; j++){
      // a[i][j] = drand48()*value;
      //a[i][j] = value;
      a[i][j] = 0;
      //val++;
    }
  a[3][2] = 1;
  a[3][3] = 1;
  a[3][4] = 1;
  return a;
}

void printarray(int **a, int mrows, int ncols) {
  int i,j;
  
  for (i=0; i<mrows; i++) {
    for (j=0; j<ncols; j++)
      printf("%d ", a[i][j]);
    printf("\n");
  }
}

void printgame(int **a, int mrows, int ncols) {
  int i,j;
  
  for (i=1; i<mrows-1; i++) {
    for (j=1; j<ncols-1; j++)
      printf("%d ", a[i][j]);
    printf("\n");
  }
}


/* N-2 dimmensions, four step process, going
i <= N top and bottom, i < 5 left and right
top (a[0][i] = a[N][i]), 
bottom (a[N+1][i] = a[1][i], 
left (a[i][0] = a[i][N]
right (a[i][N-1]) a[i][1]*/
int **calcghosts(int **a, int N)
{
  int **output = NULL;
  output = allocarray(N, N);
  
  //Top, Bottom
  for(int i = 1; i < N-1; i++){
    a[0][i] = a[N-2][i];
    a[N-1][i] = a[1][i];
  }
  //Left, Right
  for(int i = 0; i < N; i++){
    a[i][0] = a[i][N-2];
    a[i][N-1] = a[i][1];
  }
  
  return a;
  
  
    
}

int calcNeighbors(int **a, int r, int c)
{
    
   
    int neighbors = 0;
    int i, j;
    for (i = r-1; i <= r+1; i++)
    	for (j = c-1; j <= c+1; j++)
    		if( !(i == r && j == c))
        neighbors += a[i][j];
    		
    return neighbors; //this accounts for current cell, whether it is 1 or 0

}

int **gameoflife(int **a, int N, int maxgen)
{

 
  int g, r, c;
  int **next, **curr = NULL;
  next = allocarray(N, N);
  curr = allocarray(N, N);
  
  
  for (r = 0; r < N; r++) {
        for (c = 0; c < N; c++){ 
            curr[r][c] = a[r][c];
        }
    }
    
  
  
  
  //#pragma omp parallel num_threads(threads) private(g, r, c) // private (r) shared(next, curr)
  {
  
  for(g=0; g<maxgen; g++){
  
  //int tid = omp_get_thread_num();
    if(g % 100 == 0)  //&& tid == 0)
        printf("Gen: %d \n", g);
        
  //  #pragma omp barrier    
        
   // #pragma omp for private(c)

     for(r = 1; r < N - 1; r++){
        
        
        for(c = 1; c < N - 1; c++){
           
           // printf("Thread %d is working on row %dcol %d\n", tid, r, c); 
            int neighbors = calcNeighbors(curr, r, c);
            
              //printf("Col %d has %d neighbors in thread%d\n", c, neighbors, tid);
            if(curr[r][c] == 1){
                if(neighbors <= 1 || neighbors >= 4 )
                    next[r][c] = 0;
                else
                    next[r][c] = 1;
            } else{
                if(neighbors == 3)
                    next[r][c] = 1;
                else
                  next[r][c] = 0;
            }      
        
    }
    
    }
    //End of Gen
   
   
    //#pragma omp barrier
// Copy next generation to current generation
            //#pragma omp for private(c)
   
    for (r = 0; r < N; r++) {
        for (c = 0; c < N; c++){ 
            curr[r][c] = next[r][c];
        }
    }
    calcghosts(curr, N);    
  }
  }
  
  
  printf("All done\n");
  return curr;
}

int main(int argc, char **argv) 
{
   
    int N, maxgen, threads, i, j, k;
    int **prev=NULL, **curr=NULL, **test=NULL;
    double starttime, endtime;

    if (argc != 3) {
      printf("Two arguments needed. Usage: %s <N>; Generations;\n", argv[0]);
      exit(-1);
    }
    
    N = atoi(argv[1]) + 2;
    
    maxgen = atoi(argv[2]);
    //threads = atoi(argv[3]);
    printf("Generations: %d\n", maxgen);
    /* Allocate memory for all three matrices and temporary arrays */
    prev = allocarray(N, N);
    
    test = allocarray(N, N);
    

    /* Initialize the matrices */
    srand48(123456);
    prev = initarray(prev, N, N);
    
    test = testtarray(test, N, N);
    
    starttime = gettime();
    
    prev = calcghosts(prev, N);
    
    //# pragma omp parallel num_threads ( threads )
    
    gameoflife(prev, N, maxgen);
    
#ifdef DEBUGP
    //printf("Initial:\n");
    printgame(prev, N, N);
    //printarray(prev, N, N);
    
    
    test = gameoflife(prev, N, maxgen);
    printf("Final\n");
    printgame(test, N, N);
    //printarray(test, N, N);
    endtime = gettime();
    printf("Time taken = %lf seconds\n", endtime-starttime);
    exit(0);
    
#endif
    
    
    endtime = gettime();
    printf("Time taken = %lf seconds\n", endtime-starttime);

    return 0;
}
