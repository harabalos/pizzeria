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
    int id[Ncust];
    pthread_t threads[Ncust];

    initAndDestroy(1);

    for (int i = 0; i < Ncust; i++) {
        id[i] = i+1;
        pthread_mutex_lock(&lock_output);
        printf("Customer %d is ordering.\n", i+1);
        pthread_mutex_unlock(&lock_output);
        if(pthread_create(&threads[i], NULL, order, &id[i]) != 0){
            perror("Failed to create thread!");
            return 1;
        }
        int nextCustomer = rand_r(&seed) % (Torderhigh - Torderlow + 1) + Torderlow;
        sleep(nextCustomer);
    }

    for (int i = 0; i < Ncust; i++) {
        if(pthread_join(threads[i], NULL) != 0){
            perror("Failed to join thread!");
            return 2;
        }
    }
    
    if (errorFlag) {
        perror("Error allocating memory for pizzas");
        return 3;
    }
    
    avgTime = sumTime / countSuccess;
    avgTimeCooling = coolingTime / countSuccess;
    printStatistics();

    initAndDestroy(0);
    
    return 0;

}

void *order(void *x){
    
    //X,Y time
    clock_gettime(CLOCK_REALTIME,&start_time);

    int id = *(int *)x;
    pthread_mutex_lock(&lock_output);
    printf("Order %d started\n", id);
    pthread_mutex_unlock(&lock_output);
    
    pthread_mutex_lock(&lock_deliverer);
    counterOrder++;
    pthread_mutex_unlock(&lock_deliverer);
    
    int numberOfPizzas = rand_r(&seed) % (Norderhigh - Norderlow +1) + Norderlow;
    
    int* pizzas = malloc(numberOfPizzas * sizeof(int));
    if (pizzas == NULL) {
        errorFlag = 1;
        pthread_exit(NULL);
    }
    
    int paymentTime = rand_r(&seed) % (Tpaymenthigh - Tpaymentlow + 1) + Tpaymentlow;
    
    double acceptedCard = (double)rand_r(&seed) / RAND_MAX;
    
    sleep(paymentTime);
    
    if(acceptedCard > Pfail){
        acceptOrder(id,numberOfPizzas,&pizzas[numberOfPizzas]);
    }else{
        free(pizzas);
        declineOrder(id);
    }

    pthread_mutex_lock(&lock_cook);
    while (Ncook == 0) {
        pthread_mutex_lock(&lock_output);
        printf("Order %d did not find available cook. Blocked...\n", id);
        pthread_mutex_unlock(&lock_output);
        pthread_cond_wait(&cond_cook, &lock_cook);
    }
    pthread_mutex_lock(&lock_output);
    printf("Order %d is being prepared.\n", id);
    pthread_mutex_unlock(&lock_output);
    Ncook--;
    pthread_mutex_unlock(&lock_cook);

    sleep(Tprep * numberOfPizzas);

    pthread_mutex_lock(&lock_oven);
    while (Noven < numberOfPizzas) {
        pthread_mutex_lock(&lock_output);
        printf("Order %d did not find available ovens for all the pizzas at the same time. Blocked...\n", id);
        pthread_mutex_unlock(&lock_output);
        pthread_cond_wait(&cond_oven, &lock_oven);
    }
    pthread_mutex_lock(&lock_output);
    printf("Order %d is cooking.\n", id);
    pthread_mutex_unlock(&lock_output);
    Noven-=numberOfPizzas;
    pthread_mutex_unlock(&lock_oven);
    
    
    pthread_mutex_lock(&lock_cook);
    Ncook++;
    pthread_cond_signal(&cond_cook);
    pthread_mutex_unlock(&lock_cook);

    
    sleep(Tbake);
    
    pthread_mutex_lock(&lock_packer);
    while (Npacker == 0) {
        pthread_mutex_lock(&lock_output);
        printf("Order %d did not find available packer. Blocked...\n", id);
        pthread_mutex_unlock(&lock_output);
        pthread_cond_wait(&cond_packer, &lock_packer);
    }
    Npacker--;
    pthread_mutex_unlock(&lock_packer);

    sleep(Tpack * numberOfPizzas);
    //X time
    clock_gettime(CLOCK_REALTIME,&end_timeX);
    secondsX = (int)(end_timeX.tv_sec - start_time.tv_sec);
    pthread_mutex_lock(&lock_output);
    printf("Order %d has been prepared in %d minutes\n",id,secondsX);
    pthread_mutex_unlock(&lock_output);
    
    pthread_mutex_lock(&lock_packer);
    Npacker++;
    pthread_cond_signal(&cond_packer);
    pthread_mutex_unlock(&lock_packer);
    
    pthread_mutex_lock(&lock_oven);
    Noven+=numberOfPizzas;
    pthread_cond_signal(&cond_oven);
    pthread_mutex_unlock(&lock_oven);
 

    
    int deliveryTime = rand_r(&seed) % (Tdelhigh - Tdellow + 1) + Tdellow;
    pthread_mutex_lock(&lock_deliverer);
    while (Ndeliverer == 0) {
        pthread_mutex_lock(&lock_output);
        printf("Order %d did not find available deliverer. Blocked...\n", id);
        pthread_mutex_unlock(&lock_output);
        pthread_cond_wait(&cond_deliverer, &lock_deliverer);
    }
    Ndeliverer--;
    pthread_mutex_unlock(&lock_deliverer);
    
    sleep(deliveryTime);
    //Y time
    clock_gettime(CLOCK_REALTIME,&end_timeY);
    secondsY = (int)(end_timeY.tv_sec - start_time.tv_sec);
    secondsCooling = deliveryTime + (Tpack * numberOfPizzas);
    pthread_mutex_lock(&lock_output);
    printf("Order %d has been delivered in %d minutes\n",id, secondsY);
    pthread_mutex_unlock(&lock_output);
    
    pthread_mutex_lock(&lock_deliverer);
    if (counterOrder < sizeof(id)) {
        sleep(deliveryTime);
        Ndeliverer++;
    } else {
        sleep(2);
        Ndeliverer++;
    }
    pthread_mutex_unlock(&lock_deliverer);

    pthread_mutex_lock(&lock_statistics);
    sumTime += secondsY;
    coolingTime += secondsCooling;
    pthread_mutex_unlock(&lock_statistics);


    findMax(secondsY, &maxTime);
    findMax(secondsCooling, &maxTimeCooling);
    
    free(pizzas);
    pthread_exit(NULL);
}


void findMax(int sec, int* max){
    if(sec>*max){*max=sec;}
}

void printStatistics(void){
    pthread_mutex_lock(&lock_output);
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
    pthread_mutex_unlock(&lock_output);
}


void acceptOrder(int id, int numberOfPizzas, int pizzas []) {
    pthread_mutex_lock(&lock_output);
    printf("The order %d has been accepted!\n", id);
    pthread_mutex_unlock(&lock_output);
    pthread_mutex_lock(&lock_statistics);
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
    pthread_mutex_unlock(&lock_statistics);
}


void declineOrder(int id) {
    pthread_mutex_lock(&lock_output);
    printf("The order %d has been declined!\n", id);
    pthread_mutex_unlock(&lock_output);
    pthread_mutex_lock(&lock_statistics);
    countFail++;
    pthread_mutex_unlock(&lock_statistics);
    pthread_exit(NULL);
}


void initAndDestroy(int choice){
    if(choice){
        pthread_mutex_init(&lock_statistics, NULL);
        pthread_mutex_init(&lock_cook, NULL);
        pthread_mutex_init(&lock_oven, NULL);
        pthread_mutex_init(&lock_packer, NULL);
        pthread_mutex_init(&lock_deliverer, NULL);
        pthread_mutex_init(&lock_output, NULL);
        pthread_cond_init(&cond_cook, NULL);
        pthread_cond_init(&cond_oven, NULL);
        pthread_cond_init(&cond_packer, NULL);
        pthread_cond_init(&cond_deliverer, NULL);
    }else{
        pthread_mutex_destroy(&lock_statistics);
        pthread_mutex_destroy(&lock_cook);
        pthread_mutex_destroy(&lock_oven);
        pthread_mutex_destroy(&lock_packer);
        pthread_mutex_destroy(&lock_deliverer);
        pthread_mutex_destroy(&lock_output);
        pthread_cond_destroy(&cond_cook);
        pthread_cond_destroy(&cond_oven);
        pthread_cond_destroy(&cond_packer);
        pthread_cond_destroy(&cond_deliverer);
    }
}
