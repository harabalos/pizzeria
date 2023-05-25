#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pizza.h"
#include <unistd.h>
#include <time.h>


int main(int argc, char *argv[])  {

    if (argc != 3) {
        printf("Usage: %s <Ncust> <seed>\n", argv[0]);
        return 1;
    }
    Ncust = atoi(argv[1]);
    seed = atoi(argv[2]);
    int rc;
    int id[Ncust];
    pthread_t threads[Ncust];

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);


    for (int i = 0; i < Ncust; i++) {
        //X,Y time
        clock_gettime(CLOCK_REALTIME,&start_time);
        id[i] = i+1;
        printf("Customer %d is ordering.\n", i+1);
        rc = pthread_create(&threads[i], NULL, order, &id[i]);
        int nextCustomer = rand_r(&seed) % (Torderhigh - Torderlow + 1) + Torderlow;
        sleep(nextCustomer);
    }

    for (int i = 0; i < Ncust; i++) {
        pthread_join(threads[i], NULL);
    }
    avgTime = sumTime / countSuccess;
    avgTimeCooling = coolingTime / countSuccess;
    printStatistics();

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;

}

void *order(void *x){

    int id = *(int *)x;
    printf("Order %d started\n", id);
    
    counterOrder++;
    
    int isLastOrder = 0;
    if (id == Ncust){
        isLastOrder = 1;
    }
    
    int numberOfPizzas = rand_r(&seed) % (Norderhigh - Norderlow +1) + Norderlow;
    int pizzas[numberOfPizzas];
    int paymentTime = rand_r(&seed) % (Tpaymenthigh - Tpaymentlow + 1) + Tpaymentlow;
    double acceptedCard = (double)rand_r(&seed) / RAND_MAX;
    sleep(paymentTime);
    
    if(acceptedCard > Pfail){
        acceptOrder(id,numberOfPizzas,&pizzas[numberOfPizzas]);
    }else{
        declineOrder(id);
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
    secondsX = (int)(end_timeX.tv_sec - start_time.tv_sec);
    printf("Order %d has been prepared in %d minutes\n",id,secondsX);
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
    secondsY = (int)(end_timeY.tv_sec - start_time.tv_sec);
    secondsCooling = (int)(end_timeY.tv_sec - start_time_cooling.tv_sec);
    printf("Order %d has been delived in %d minutes\n",id, secondsY);
    
    if(counterOrder<sizeof(id)){
        sleep(deliveryTime);
        Ndeliverer++;
    }else{
        sleep(2);
        Ndeliverer++;
    }

    //total time
    sumTime+= secondsY;
    coolingTime+=secondsCooling;

    findMax(secondsY, &maxTime);
    findMax(secondsCooling, &maxTimeCooling);
    
    
    rc = pthread_cond_signal(&cond);
    pthread_exit(NULL);
}


void findMax(int sec, int* max){
    if(sec>*max){*max=sec;}
}

void printStatistics(void){
    printf("\n-- Overall Statistics --\n");
    printf("Total profit: %d\n", profit);
    printf("Number of plain pizzas sold: %d\n", countPlain);
    printf("Number of special pizzas sold: %d\n", countSpecial);
    printf("Successful orders: %d\n", countSuccess);
    printf("Failed orders: %d\n", countFail);
    printf("Average service time : %d\n",avgTime);
    printf("Max service time : %d\n",maxTime);
    printf("Average cooling time : %d\n",avgTimeCooling);
    printf("Max cooling time : %d\n",maxTimeCooling);
}


void acceptOrder(int id, int numberOfPizzas, int pizzas []) {
    printf("The order %d has been accepted!\n", id);
    countSuccess++;
    for (int i = 0; i < numberOfPizzas; i++) {
        double kindOfPizza = (double)rand_r(&seed) / RAND_MAX;
        if(kindOfPizza < Pplain){
            pizzas[i] = 0; // 0 for plain pizzas
            profit += Cplain;
            countPlain++;
        } else {
            pizzas[i] = 1; // 1 for special pizzas
            profit += Cspecial;
            countSpecial++;
        }
    }
}


void declineOrder(int id) {
    printf("The order %d has been declined!\n", id);
    countFail++;
    pthread_exit(NULL);
}
