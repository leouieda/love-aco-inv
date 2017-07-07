#ifndef _ACOR_H_
#define _ACOR_H_

#define PI 3.141592653

#define MAX 5000


/* LIFO structure for quicksort */
/* ******************************************************************** */
typedef struct quick_exec_lifo {
    int *inicio, *fim;
    int top;
} LIFO;

LIFO quick_lifo;
/* ******************************************************************** */


/* PUSH - coloca 'inicio'  e 'fim' na pilha de execucao do quick */
/* ******************************************************************** */
void push (int inicio, int fim){
        
    quick_lifo.top++;    
    quick_lifo.inicio[quick_lifo.top]=inicio;
    quick_lifo.fim[quick_lifo.top]=fim;    
   
}
/* ******************************************************************** */


/* POP - tira 'inicio' e 'fim' da pilha de execucao do quick */
/* ******************************************************************** */
void pop (int *inicio, int *fim){
    
    *inicio = quick_lifo.inicio[quick_lifo.top];
    *fim = quick_lifo.fim[quick_lifo.top];
    quick_lifo.top--;
}
/* ******************************************************************** */
           

/* PARTICIONA - Divide 'a' de 'inicio' a 'fim' em duas. Retorna o elemendo do meio. */
/*              A esquerda do meio todos sao maiores que o mesmo e a direita todos sao menores. */
/* ******************************************************************** */
int particiona(double **a, int n, int inicio, int fim){
    
    register int i=inicio, j=fim;
    int direcao=1; /* Direcao na qual o pivo sera movido. 1=move i, -1=move j */
    double *aux;

    /* Ate que os pivos se choquem */
    while( i<j ){
        if( a[i][n] > a[j][n] ){
            /* Troca os elementos */
            aux=a[j];
            a[j]=a[i];
            a[i]=aux;
            /* Inverte a direcao */
            direcao*=-1;            
        }
        /* Move o pivo certo */
        if( direcao == 1 )
            i++;
        else
            j--;
    }

    /* i sera o elemento do meio, que deve ser retornado */
    return i;
}
/* ******************************************************************** */


/* Sort table a from begin to end using QUICK SORT. */
/* ******************************************************************** */
void quicksort(double **a, int n, int inicio, int fim){
    
    int i, f, meio;
    
    quick_lifo.inicio=(int *)malloc(sizeof(int)*2*fim);
    quick_lifo.fim=(int *)malloc(sizeof(int)*2*fim);
    quick_lifo.top=-1;

    /* Inicializa a pilha de execucao com a tabela de 'inicio' a 'fim' */
    push(inicio, fim);

    /* Faz o quick ate que nao haja mais elementos na pilha de execucao */
    while( quick_lifo.top > -1 ){

        /* Pega o inicio e o fim da parte a ser particionada */        
        pop(&i, &f);

        /* Particiona */
        meio=particiona(a,n,i,f);
       
        /* Poe na pilha de execucao as duas metades */
        if( f-meio > 1 ) /* Mas so se houver mais de 1 elemento na metade da direita */
            push(meio+1,f);
        if( meio-i > 1 ) /* ou se houver mais de 1 elemento na metade da esquerda */
            push(i,meio-1);
    }
    
    free(quick_lifo.inicio);
    free(quick_lifo.fim);
}
/* ******************************************************************** */


/* Return the module of x */
/* ******************************************************************** */ 
double mod(double x){
    if(x>=0)
        return x;
    else
        return -1*x;
}
/* ******************************************************************** */ 


/* Sample a gaussian PDF defined by its mean and variance. Uses the Box-Muller method */
/* ******************************************************************** */ 
double sampleprob(double mean, double variance){

    double u1=(double)rand()/RAND_MAX;
    double u2=(double)rand()/RAND_MAX;
    double x1 = mean + sqrt(variance)*sqrt(-2*log(u1))*cos(2*PI*u2);
    
    return x1;
}
/* ******************************************************************** */


/* Print the error message */
/* ******************************************************************** */
void printerr(int error){
    switch(error){
        case 1:
            fprintf(stderr, "\nError allocating memory for array 'w'!\nProgram terminated.\n\n");
        break;
        case 2:
            fprintf(stderr, "\nError allocating memory for array 'S'!\nProgram terminated.\n\n");
        break;
        case 3:
            fprintf(stderr, "\nError allocating memory for array 'wprob'!\nProgram terminated.\n\n");
        break;
        case 4:
            fprintf(stderr, "\nError allocating memory for array 'antzS'!\nProgram terminated.\n\n");
        break;
    }
}
/* ******************************************************************** */


/* Choose a PDF based on the probability array 'prob' */
/* ******************************************************************** */
int choosePDF(double *prob, int k){
    
    double sum=0, number=(double)rand()/(RAND_MAX+1);   
    int i;
    
    for(i=0; i<k; i++){
        if(number>=sum && number<(sum+prob[i])){
            return i;
        }
        else{
            sum+=prob[i];
        }
        if(number==1.0 && sum==1.0){
            return i;
        }
        
    }  
    
    fprintf(stderr,"\nAH! FUBANGO TUDO!! NAO ESCOLHEU UMA PDF!!!\n\n");
    
    return -1;
}
/* ******************************************************************** */


/* f(x,y)=x^2 + y^2 */
/* ******************************************************************** */
double f(double *args){

    return (args[0]*args[0])+(args[1]*args[1]);

}
/* ******************************************************************** */


double eggbox(double *args){

    return (100*sin(args[0])*cos(args[1]))+((args[0]-5)*(args[0]-5))+(args[1]*args[1]);

}


/*  ACOR: Continuous Ant Colony Optimization
    Finds the minimum of n-dimensional objective function f(args[])
    max = array of maximuns for the solution variables;    
    min = array of minimuns for the solution variables; 
    m = number of ants;
    k = size of solution archive;
    q = level of diversification;
    esp = rate of evaporation of trail (eps>0);
    RETURN:
        pointer to best solution: Successfull
        NULL: Error */        
/* ******************************************************************** */
double *acor(double (*f)(double *), double *max, double *min, int n, int m, int k, double q, double eps){

    /* Define the seed to the random gen */
    srand(time(NULL));

    double *w,          /* Weights of the kernel PDFs */
           *wprob,      /* The probability array of choosing the lth PDF */
           **S,         /* The Solution Archive */
           **antzS,     /* The solutions the ants have found in current cicle */
           *best,       /* Pointer to the best solution found */
           mean,        /* The mean of a PDF */
           stddev,      /* The standard deviation of a PDF */
           tmp,         /* Temp variable */
           *ptmp1,       /* Temp pointer */
           *ptmp2;       /* Temp pointer */
           
    int cicle, i, l, e, ant;      /* Counter */

    
    /* Alloc the memory for w */
    w=(double *)malloc(sizeof(double)*k);
    if(w==NULL){
        printerr(1);
        return 0;
    }

    /* Alloc the memory for S */
    S=(double **)malloc(sizeof(double *)*k);
    if(S==NULL){
        printerr(2);
        return 0;
    }
    for(i=0; i<k; i++){
        S[i]=(double *)malloc(sizeof(double)*(n+1));
        if(S[i]==NULL){
            printerr(2);
            return 0;
        }
    }

    /* Alloc the memory for wprob */
    wprob=(double *)malloc(sizeof(double)*k);
    if(wprob==NULL){
        printerr(3);
        return 0;
    }

    /* Alloc the memory for antzS */
    antzS=(double **)malloc(sizeof(double *)*m);
    if(antzS==NULL){
        printerr(4);
        return 0;
    }
    for(i=0; i<m; i++){
        antzS[i]=(double *)malloc(sizeof(double)*(n+1));
        if(antzS[i]==NULL){
            printerr(2);
            return 0;
        }
    }

   

    /* Initialize S with normal random distribution */
    for(l=0; l<k; l++){
        for(i=0; i<n; i++){
            S[l][i]= ((double)rand()/RAND_MAX)*(max[i]-min[i]) + min[i]; /* Generate a random number */
                                                                         /* between max[i] and min[i] */
        }
        S[l][n]=f(S[l]);
    } 
    quicksort(S, n, 0, k-1); /* Quick Sort S */
    
        
    /* Print the initial positions */
   /* for(l=0; l<k; l++){
        for(i=0; i<=n; i++){
            printf("%lf ", S[l][i]);
        }
        printf("\n");
    }
    printf("\n\n");*/

    /* Compute Weights */
    for(l=0; l<k; l++){
        w[l]=(double)(1/(q*k*sqrt(2*PI)))*exp( (double)(-1*(l)*(l)) / (2*q*q*k*k) ) ;
    }
   
    /* Compute the wprob */
    for(l=0, tmp=0; l<k; l++){
        tmp+=w[l];
    }
    
    for(l=0; l<k; l++){
        wprob[l]=(double)w[l]/tmp;
        fprintf(stderr, "%.20lf\n", wprob[l]); /* Print the probability of each weight */
    }


    /* Run for MAX cicles */    
    for(cicle=0; cicle<MAX; cicle++){
    
        /* 1. Solution Construction */
        for(i=0; i<n; i++){ /* For each variable */
            for(ant=0; ant<m; ant++){ /* For each ant */

                /* a. Choose a PDF */
                l=choosePDF(wprob, k);

                /* b. Compute the mean */
                mean=S[l][i]; /* S^i_l */

                /* c. Compute the stddev */
                stddev=0;
                for(e=0; e<k; e++){
                    stddev+=mod(S[e][i]-S[l][i]);
                }              
                stddev=eps*((double)stddev/(k-1));

                /* d. Choose a new value */
                antzS[ant][i]=sampleprob(mean,stddev);
                while( antzS[ant][i]>max[i] || antzS[ant][i]<min[i] ){
                    antzS[ant][i]=sampleprob(mean,stddev);
                }
            }       
        }

        /* 2. Pheromone Update */
        for(ant=0; ant<m; ant++){ /* For each ant */
             
            /* a. Calculate f(S) */             
            antzS[ant][n]=f(antzS[ant]);
            
            /* Print the solution found by ant */
         /*   for(i=0; i<=n; i++){
                     printf("%lf ", antzS[ant][i]);
            }
            printf("\n");      */
              
            /* b. Place the solution found by ant in the proper place */            
            /* Find the right place for the solution */            
            for(l=0; l<k && S[l][n]<=antzS[ant][n]; l++){
            }
            if(l!=k){ /* If the solution has a place (if l=k, the solution is worst than the S[k-1] */
                ptmp1=S[l]; 
                S[l]=antzS[ant]; /* Put the solution in its place */                
                /* Move the solutions down */                
                for(i=l; i<(k-1); i++){ 
                    ptmp2=S[i+1];
                    S[i+1]=ptmp1;
                    ptmp1=ptmp2;
                }
                antzS[ant]=ptmp1; /* Since S[l] now points to antzS[ant], */
                                  /* antzS[ant] points to the solution that was discarted. */ 
                                  /* This way, the memory is recycled. */
            }
            
        }
        
            
    }
    /* Mark the best solution */
    best=S[0];
    
    /* Print the best solution found */
  /*  printf("\n\n");
    for(i=0; i<=n; i++){
        printf("%lf ", best[i]);
    }
    printf("\n");    
*/
    /* Free the memory used */
    free(w);
    free(wprob);
    for(i=0; i<m; i++){
        free(antzS[i]);
    }
    free(antzS);
    for(i=1; i<k; i++){
        free(S[i]);
    }
    free(S);

    return best;
}
/* ******************************************************************** */


#endif
