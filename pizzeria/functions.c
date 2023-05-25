#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pizza.h"
#include <unistd.h>
#include <time.h>

void *order(void *x){

    int id = *(int *)x;
    printf("Order %d started\n", id);
    
    int numberOfPizzas = rand_r(&seed) % (Norderlow + Norderhigh);
    int pizzas[numberOfPizzas];
    int paymentTime = rand_r(&seed) % (Tpaymenthigh - Tpaymentlow + 1) + Tpaymentlow;
    double acceptedCard = (double)rand_r(&seed) / RAND_MAX;
    
    if(acceptedCard > Pfail){
        printf("The order %d has been accepted!\n",id);
        countSuccess++;
        for (int i = 0; i < numberOfPizzas; i++) {
            double kindOfPizza = (double)rand_r(&seed) / RAND_MAX;
            if(kindOfPizza<Pplain){
                pizzas[i] = 0; // 0 for plain pizzas
                profit+=Cplain;
                countPlain++;
            }else{
                pizzas[i] = 1;//1 for special pizzas
                profit+=Cspecial;
                countSpecial++;
            }
        }
    }else{
        printf("The order %d has been declined!\n",id);
        countFail++;
        pthread_exit(NULL);
    }
    
    rc = pthread_mutex_lock(&lock);
    while (Ncook == 0) {
        printf("Order %d did not find available cook. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }
    printf("Order %d is being prepared.\n", id);
    Ncook--;
    rc = pthread_mutex_unlock(&lock);

    sleep(Tprep);

    rc = pthread_mutex_lock(&lock);
    while (Noven < numberOfPizzas) {
        printf("Order %d did not find available ovens for all the pizzas at the same time. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }
    printf("Order %d is cooking.\n", id);
    Noven-=numberOfPizzas;
    rc = pthread_mutex_unlock(&lock);
    
    Ncook++;
    sleep(Tbake);
    clock_gettime(CLOCK_REALTIME,&start_time_cooling);
    
    rc = pthread_mutex_lock(&lock);
    while (Npacker == 0) {
        printf("Order %d did not find available packer. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }
    Npacker--;
    rc = pthread_mutex_unlock(&lock);

    sleep(Tpack * numberOfPizzas);
    //X time
    clock_gettime(CLOCK_REALTIME,&end_timeX);
    secondsX = end_timeX.tv_sec - start_time.tv_sec;
    printf("Order %d has been prepared in %ld minutes\n",id,secondsX);
    Npacker++;
    Noven+=numberOfPizzas;

    
    int deliveryTime = rand_r(&seed) % (Tdelhigh - Tdellow + 1) + Tdellow;
    rc = pthread_mutex_lock(&lock);
    while (Ndeliverer == 0) {
        printf("Order %d did not find available deliverer. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }
    Ndeliverer--;
    rc = pthread_mutex_unlock(&lock);
    
    sleep(deliveryTime);
    //Y time
    clock_gettime(CLOCK_REALTIME,&end_timeY);
    secondsY = end_timeY.tv_sec - start_time.tv_sec;
    secondsCooling = end_timeY.tv_sec - start_time_cooling.tv_sec;
    printf("Order %d has been delived in %ld minutes\n",id, secondsY);
    sleep(deliveryTime);
    Ndeliverer++;
    
    //total time
    sumTime+= secondsY;
    coolingTime+=secondsCooling;

    findMax(secondsY, &maxTime);
    findMax(secondsCooling, &maxTimeCooling);
    
    
    rc = pthread_cond_signal(&cond); 
    pthread_exit(NULL);
}


void findMax(long sec, long* max){
    if(sec>*max){*max=sec;}
}



void printStatistics(void){
    printf("\n-- Overall Statistics --\n");
    printf("Total profit: %d\n", profit);
    printf("Number of plain pizzas sold: %d\n", countPlain);
    printf("Number of special pizzas sold: %d\n", countSpecial);
    printf("Successful orders: %d\n", countSuccess);
    printf("Failed orders: %d\n", countFail);
    printf("Average service time : %ld\n",avgTime);
    printf("Max service time : %ld\n",maxTime);
    printf("Average cooling time : %ld\n",avgTimeCooling);
    printf("Max cooling time : %ld\n",maxTimeCooling);
}
