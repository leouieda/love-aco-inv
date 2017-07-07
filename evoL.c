#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include "acor.h"

#define BETA1 0
#define BETA2 1
#define H     2
#define EQM   3

#define LPI 3.1415926535897932384626433832795

/* the number of data in the file */
int N;
double v[20], T[20], rho1=2700, rho2=3300;
 
double difusion(double c, double b1, double b2, double h){
    double b1_2 = b1*b1, c_2=c*c, b2_2 = b2*b2, ro2ro1  = (double)rho2/rho1;
    double amp = (double)(2*LPI*h*sqrt( ((double)c_2/b1_2)-1 ) )/c;           
    double theta = ro2ro1*sqrt( (double)(b2_2*b2_2 - c_2*b2_2)/(b1_2*c_2 - b1_2*b1_2) );
    
    return ((double)amp/atan(theta));    
}

double erro(double *p){
    register int i;
    double res=0, aux;
    for(i=0; i < N; i++){
        aux = difusion(v[i],p[BETA1],p[BETA2],p[H]);
        res += (T[i]-aux)*(T[i]-aux);
    }
    res = (double) res/N;
    return res;
}



int main(){
    
   
    double *res;
    double max[5], min[5];
    double q=0.5, eps=0.85;
    int n=3, m=5, k=50, i;

    for(N=0; scanf("%lf %lf", &T[N], &v[N])==2; N++){
        fprintf(stderr,"%g %g\n", T[N], v[N]);       
    }
    
    max[BETA1] = 3.5;
    min[BETA1] = 1.0;
    
    max[BETA2] = 5.0;
    min[BETA2] = 3.8;
    
    max[H] = 40.0; 
    min[H] = 1.0;
    /*
    max[RHO1] = 2000;
    min[RHO1] = 1000;
    
    max[RHO2] = 3000;
    min[RHO2] = 2500;*/
    
    res=acor(&erro, max, min, n, m, k, q, eps);

    fprintf(stderr, "\nBeta1: %g\nBeta2: %g\nh: %g\nrho1: %g\nrho2: %g\nErro Quad medio: %g\n\n",
        res[BETA1],res[BETA2],res[H],rho1,rho2,res[EQM]);
        
        
    for(i=0; i<N; i++){
        printf("%g %g\n", difusion(v[i],res[BETA1],res[BETA2],res[H]), v[i]);
    }
    printf("Beta1: %g\nBeta2: %g\nh: %g\nrho1: %g\nrho2: %g\nErro: %g",
        res[BETA1],res[BETA2],res[H],rho1,rho2,res[EQM]);
    return 0;
}
