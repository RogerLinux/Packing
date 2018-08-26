#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926

float dist(float x1, float y1, float x2, float y2);
float dou(float x);
int go(float* des_x, float* des_y, float int_x, float int_y, float agl, float stp, float R, float L, float W);
float objective_func(float* ctr_x_ref, float* ctr_y_ref, float* ctr_r_ref, int self_num, float agl, float stp, float R_th, float L, float W, int NUM);
float cov(float* ctr_x_ref, float* ctr_y_ref, float* ctr_r_ref, float L, float W, float dmin, int NUM);

float dist(float x1, float y1, float x2, float y2){
      float d;

      d = dou(x1-x2)+dou(y1-y2);
      d = sqrt(d);

      return d;
                }

float dou(float x){
    return x*x;
                }

int go(float* des_x, float* des_y, float int_x, float int_y, float agl, float stp, float R, float L, float W){
    *des_x = int_x + stp * cos(agl*PI/180);
    *des_y = int_y + stp * sin(agl*PI/180);

    if(*des_x < R)*des_x = R;
      else if(*des_x > L-R)*des_x = L - R;

    if(*des_y < R)*des_y=R;
       else if(*des_y > W-R)*des_y = W - R;

    return 0;
                }

float objective_func(float* ctr_x_ref, float* ctr_y_ref, float* ctr_r_ref, int self_num, float agl, float stp, float R_th, float L, float W, int NUM){

   int i, j;
   float value = 0, N = 0, x_temp, y_temp;

   go(&x_temp, &y_temp, ctr_x_ref[self_num], ctr_y_ref[self_num], agl, stp, ctr_r_ref[self_num], L, W);

   for(i = 0;i < NUM; i++){
      if(dist(x_temp, y_temp, ctr_x_ref[i], ctr_y_ref[i]) < R_th*(ctr_r_ref[i] + ctr_r_ref[self_num]) && i != self_num)
            N += ctr_r_ref[i];
                            }

   if(N == 0){
      for(i = 0;i < NUM; i++)
         if(i != self_num){
             value += ctr_r_ref[i] * dist(x_temp, y_temp, ctr_x_ref[i], ctr_y_ref[i]);
             N += ctr_r_ref[i];
                              }
      value/=N;
      //value/=(NUM-1); //maybe value/=(NUM)
                          }
   else {
      for(i = 0;i < NUM; i++)
         if(dist(x_temp, y_temp, ctr_x_ref[i], ctr_y_ref[i]) <= R_th * (ctr_r_ref[i] + ctr_r_ref[self_num]) && i != self_num)
         value += ctr_r_ref[i] * dist(x_temp, y_temp, ctr_x_ref[i], ctr_y_ref[i]);

         value /= N;
                          }
     return value;
                                 }

float cov(float* ctr_x_ref, float* ctr_y_ref, float* ctr_r_ref, float L, float W, float dmin, int NUM){
    int total = 0, inrange = 0, n;
    float i, j, value;

    for(j = 0;j < L;j += dmin){
       for(i = 0;i < W;i += dmin){
           for(n = 0;n < NUM; n++)
		if(dist(j, i, ctr_x_ref[n], ctr_y_ref[n]) <= ctr_r_ref[n]){
              			inrange++;
              			break;
                                       }
           total++;
                                 }
                              }
    value = (float)inrange / (float)total;
    return value;
                         }

float MineDeploy(float* ctr_x_opt, float* ctr_y_opt, float* ctr_x, float* ctr_y, float* R, float L, float W, int NUM, float dmin, float R_th_min, float R_th_MAX, float d_R_th, int iter){
    int i, j, k, n, m;
    float R_th, agl_opt, dist_temp, dist_opt, cov_now, cov_opt;
    float* ctr_x_temp = (float* )malloc(NUM * sizeof(float)), *ctr_y_temp = (float* )malloc(NUM * sizeof(float));

    printf("Loading original location data...\n");

    for(k = 0;k < NUM; k++)ctr_x_temp[k] = ctr_x[k];
    for(k = 0;k < NUM; k++)ctr_y_temp[k] = ctr_y[k]; //load original data to temporary buffer

    printf("Calculating original coverage...\n");

    cov_opt = cov(ctr_x, ctr_y, R, L, W, dmin, NUM);

    printf("Coverage= %f\n", cov_opt);

    printf("Optimizing...\n");

    R_th = R_th_MAX;

    //Algorithm starts

    do{
      for(i = 0;i < iter; i++){
        for(n = 0;n < NUM; n++){
           dist_opt = objective_func(ctr_x_temp, ctr_y_temp, R, n, 0, R[n]*0.1, R_th, L, W, NUM);
           agl_opt = 0;

           for(j = 1;j < 360;j++){

             dist_temp = objective_func(ctr_x_temp, ctr_y_temp, R, n, j, R[n]*0.1, R_th, L, W, NUM);//step size depends on radii
              if(dist_temp>dist_opt){
                   dist_opt = dist_temp;
                   agl_opt = (float)j; //better deployment found, update optimal distance and angle
                                           }
                                  }
              go((ctr_x_temp+n), (ctr_y_temp+n), ctr_x_temp[n], ctr_y_temp[n], agl_opt, R[n]*0.1, R[n], L, W);//go along optimal angle
                        }
                      }

           cov_now = cov(ctr_x_temp, ctr_y_temp, R, L, W, dmin, NUM);

           if(cov_now > cov_opt){
                cov_opt = cov_now;

                for(k = 0;k < NUM; k++){

                   ctr_x_opt[k] = ctr_x_temp[k];
                   ctr_y_opt[k] = ctr_y_temp[k]; //Update the best deployment and coverage if better deployment(Coverage) is found
                   }
                                   }

           if(R_th <= R_th_min)d_R_th *= -1; //Rebound

           R_th -= d_R_th; //Radii shrink
                               }while(R_th < R_th_MAX && R_th >= R_th_min - fabsf(d_R_th));
        free(ctr_x_temp);
        free(ctr_y_temp);

        return cov_opt;
                     }
