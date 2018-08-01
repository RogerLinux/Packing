#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "./Roger_Packing.c"

#define item 10
#define rand_mode 1
#define opt_max 64
#define PI 3.1415926

int main(int argc, char** argv){
    int i, num, iter, dmin;
    float *ctr_x, *ctr_y, *R, *ctr_x_opt, *ctr_y_opt;
    float L, W, M_r, m_r, target_area=0;
    float cov_opt, R_th_Max, R_th_min, R_th_d;

    if(argc!=item && argc!=(item+1)){
      printf("Usage: Packing num Max_Radius min_radius length width R_th_Max R_th_min R_th_decreasing iterations [written_file]\n");
      printf("Return values:\n0 is OK\n1 is wrong argument\n2 is cannot write the output file\n");
      return 1;
                           }
    num=atoi(argv[1]);
    iter=atoi(argv[9]);
    L=atof(argv[4]);
    W=atof(argv[5]);

    dmin=(int)(L+W)/2000;

    R_th_Max=atof(argv[6]);
    R_th_min=atof(argv[7]);
    R_th_d=atof(argv[8]);

    ctr_x=malloc(num*sizeof(float));
    ctr_y=malloc(num*sizeof(float));
    R=malloc(num*sizeof(float));

    M_r=atof(argv[2]);
    m_r=atof(argv[3]);
    if(M_r<=m_r)M_r=m_r+1;

    if(rand_mode==1)srand(time(NULL));

    for(i=0;i<num;i++)R[i]=rand()%(int)(M_r-m_r)+m_r;
    for(i=0;i<num;i++)target_area+=PI*dou(R[i]);
    target_area/=L;
    target_area/=W;

    for(i=0;i<num;i++)ctr_x[i]=rand()%(int)(L-2*R[i])+R[i];
    for(i=0;i<num;i++)ctr_y[i]=rand()%(int)(W-2*R[i])+R[i];

    printf("Quantity: %d\n", num);
    printf("Radii: %.2f - %.2f\n", m_r, M_r);
    printf("%.2f * %.2f\n", L, W);
    printf("Iterations: %d\n", iter);
    printf("Theshold: %.2f - %.2f\n", R_th_min, R_th_Max);

    if(argc==(item+1))printf("Output file: %s\n", argv[item]);

    ctr_x_opt=malloc(num*sizeof(float));
    ctr_y_opt=malloc(num*sizeof(float));

    cov_opt=MineDeploy(ctr_x_opt, ctr_y_opt, ctr_x, ctr_y, R, L, W, num, dmin, R_th_min, R_th_Max, R_th_d, iter);
                     
    printf("Optimal Coverage= %.4f\nPerformance: %.4f\n", cov_opt, cov_opt/target_area);

    if(argc==(item+1)){

       FILE* p;
       p=fopen(argv[item], "w");
       if(p==NULL)return 2;

       fprintf(p, "#Quantity: %d\n", num);
       fprintf(p, "#Range: %f - %f\n", m_r, M_r);
       fprintf(p, "#Optimized Coverage: %f\n#Performance: %f\n", cov_opt, cov_opt/target_area);

       fprintf(p, "from math import *\nfrom matplotlib.pyplot import *\n");
       fprintf(p, "pi=3.1415926\nnum= %d\n", num);

       fprintf(p, "x=[");
       for(i=0;i<num-1;i++)fprintf(p, "%f, ", ctr_x_opt[i]);
       fprintf(p, "%f]\n", ctr_x_opt[num-1]);

       fprintf(p, "y=[");
       for(i=0;i<num-1;i++)fprintf(p, "%f, ", ctr_y_opt[i]);
       fprintf(p, "%f]\n", ctr_y_opt[num-1]);

       fprintf(p, "r=[");
       for(i=0;i<num-1;i++)fprintf(p, "%f, ", R[i]);
       fprintf(p, "%f]\n", R[num-1]);

       fprintf(p, "\n\nplot(x, y, \"ro\")\n");
       fprintf(p, "for i in range(num):\n");
       fprintf(p, "   for agl in range(72):\n");
       fprintf(p, "      plot(x[i]+r[i]*cos(5*agl*pi/180), y[i]+r[i]*sin(5*agl*pi/180), \"b.\");\n");
       fprintf(p, "show();\n");

       fclose(p);
                                               }

    free(ctr_x);
    free(ctr_y);
    free(ctr_x_opt);
    free(ctr_y_opt);
    free(R);

        return 0;
                               }
