#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pizza.h"
#include <unistd.h>
#include <time.h>


int main(int argc, char *argv[])  {

    //Check if the correct number of command-line arguments is provided
    if (argc != 3) {
        printf("Usage: %s <Ncust> <seed>\n", argv[0]);
        return 1;
    }
    Ncust = atoi(argv[1]);
    seed = atoi(argv[2]);
    int id[Ncust];
    pthread_t threads[Ncust];

    //Initialize mutexes and conditions
    initAndDestroy(1);

    //For the first costumer the order is in time 0
    for (int i = 0; i < Ncust; i++) {
        id[i] = i+1;
        pthread_mutex_lock(&lock_output);
        printf("Customer %d is ordering.\n", i+1);
        pthread_mutex_unlock(&lock_output);

        //Create threads for each customer order/Error handling
        if(pthread_create(&threads[i], NULL, order, &id[i]) != 0){
            perror("Failed to create thread!");
            return 1;
        }

        //Sleep for a random time before the next customer places an order
        int nextCustomer = rand_r(&seed) % (Torderhigh - Torderlow + 1) + Torderlow;
        sleep(nextCustomer);
    }

    //Wait for all threads to finish/Error handling
    for (int i = 0; i < Ncust; i++) {
        if(pthread_join(threads[i], NULL) != 0){
            perror("Failed to join thread!");
            return 2;
        }
    }
    
    //Check for memory allocation error
    if (errorFlag) {
        perror("Error allocating memory for pizzas");
        return 3;
    }
    
    //Calculate average service time and average cooling time
    avgTime = sumTime / countSuccess;
    avgTimeCooling = coolingTime / countSuccess;

    //Print overall statistics
    printStatistics();

    //Clean up mutexes and conditions
    initAndDestroy(0);
    
    return 0;

}

void *order(void *x){
    
    struct timespec start_time,start_time_cooling, end_timeX, end_timeY;
    
    //X,Y time
    clock_gettime(CLOCK_REALTIME,&start_time);

    //Get the id
    int id = *(int *)x;
    pthread_mutex_lock(&lock_output);
    printf("Order %d started\n", id);
    pthread_mutex_unlock(&lock_output);
    
    //Variable to check if this is the last delivery
    pthread_mutex_lock(&lock_deliverer);
    counterOrder++;
    pthread_mutex_unlock(&lock_deliverer);
    
    //Determine the number of pizzas for the order
    int numberOfPizzas = rand_r(&seed) % (Norderhigh - Norderlow +1) + Norderlow;
    
    //Allocate memory for the array of pizzas
    int* pizzas = malloc(numberOfPizzas * sizeof(int));
    //If something is wrong, do your thing :*
    if (pizzas == NULL) {
        errorFlag = 1;
        pthread_exit(NULL);
    }
    
    //Determine the payment time and check if the card payment is accepted
    int paymentTime = rand_r(&seed) % (Tpaymenthigh - Tpaymentlow + 1) + Tpaymentlow;
    double acceptedCard = (double)rand_r(&seed) / RAND_MAX;
    
    //Sleep for the payment time
    sleep(paymentTime);
    
    //Decide whether to accept or decline the order based on the payment success probability
    if(acceptedCard > Pfail){
        acceptOrder(id,numberOfPizzas,pizzas);
    }else{
        free(pizzas);
        declineOrder(id);
    }

    //Wait for an available cook
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
    //Cook is occupied!
    Ncook--;
    pthread_mutex_unlock(&lock_cook);

    //Sleep for the preparation time of the all pizzas
    sleep(Tprep * numberOfPizzas);

    //Wait for available ovens to cook the pizzas
    pthread_mutex_lock(&lock_oven);
    while (Noven < numberOfPizzas) {
        pthread_mutex_lock(&lock_output);
        printf("Order %d did not find available ovens for all the pizzas at the same time. Blocked...\n", id);
        pthread_mutex_unlock(&lock_output);
        pthread_cond_wait(&cond_oven, &lock_oven);
    }
    //All pizzas go in the same time
    pthread_mutex_lock(&lock_output);
    printf("Order %d is cooking.\n", id);
    pthread_mutex_unlock(&lock_output);
    Noven -= numberOfPizzas;
    pthread_mutex_unlock(&lock_oven);
    
    
    //Increase the count of available cooks and signal waiting cooks
    pthread_mutex_lock(&lock_cook);
    Ncook++;
    pthread_cond_signal(&cond_cook);
    pthread_mutex_unlock(&lock_cook);

    //Sleep for the bake time
    sleep(Tbake);
    
    //Wait for an available packer
    pthread_mutex_lock(&lock_packer);
    while (Npacker == 0) {
        pthread_mutex_lock(&lock_output);
        printf("Order %d did not find available packer. Blocked...\n", id);
        pthread_mutex_unlock(&lock_output);
        pthread_cond_wait(&cond_packer, &lock_packer);
    }
    //Packer is occupied
    Npacker--;
    pthread_mutex_unlock(&lock_packer);

    //Sleep for the packing time of all the pizzas
    sleep(Tpack * numberOfPizzas);
    
    //Calculate the X time (preparation time) and print the information
    clock_gettime(CLOCK_REALTIME,&end_timeX);
    secondsX = (int)(end_timeX.tv_sec - start_time.tv_sec);
    pthread_mutex_lock(&lock_output);
    printf("Order %d has been prepared in %d minutes\n",id,secondsX);
    pthread_mutex_unlock(&lock_output);
    
    //Increase the count of available packers and signal waiting packers
    pthread_mutex_lock(&lock_packer);
    Npacker++;
    pthread_cond_signal(&cond_packer);
    pthread_mutex_unlock(&lock_packer);
    
    //Increase the count of available ovens and signal waiting ovens
    pthread_mutex_lock(&lock_oven);
    Noven += numberOfPizzas;
    pthread_cond_signal(&cond_oven);
    pthread_mutex_unlock(&lock_oven);
 
    //Determine the delivery time for the order
    int deliveryTime = rand_r(&seed) % (Tdelhigh - Tdellow + 1) + Tdellow;
    
    //Wait for an available deliverer
    pthread_mutex_lock(&lock_deliverer);
    while (Ndeliverer == 0) {
        pthread_mutex_lock(&lock_output);
        printf("Order %d did not find available deliverer. Blocked...\n", id);
        pthread_mutex_unlock(&lock_output);
        pthread_cond_wait(&cond_deliverer, &lock_deliverer);
    }
    //Deliverer is on the motorcycle now
    Ndeliverer--;
    pthread_mutex_unlock(&lock_deliverer);
    
    //Sleep for the delivery time
    sleep(deliveryTime);
    
    //Calculate the Y time (delivery time) and cooling time and print the information
    clock_gettime(CLOCK_REALTIME,&end_timeY);
    secondsY = (int)(end_timeY.tv_sec - start_time.tv_sec);
    secondsCooling = deliveryTime + (Tpack * numberOfPizzas);
    pthread_mutex_lock(&lock_output);
    printf("Order %d has been delivered in %d minutes\n",id, secondsY);
    pthread_mutex_unlock(&lock_output);
    
    //Deliverer gets available again when he returns back,
    //but in the last order we dont have to wait till he comes back
    //sleep(2) is for the looks! :)
    pthread_mutex_lock(&lock_deliverer);
    if (counterOrder < sizeof(id)) {
        sleep(deliveryTime);
        Ndeliverer++;
    } else {
        sleep(2);
        Ndeliverer++;
    }
    pthread_mutex_unlock(&lock_deliverer);

    //Update the total service time and cooling time in the statistics
    pthread_mutex_lock(&lock_statistics);
    sumTime += secondsY;
    coolingTime += secondsCooling;
    pthread_mutex_unlock(&lock_statistics);

    //Update the maximum service time and maximum cooling time if necessary
    findMax(secondsY, &maxTime);
    findMax(secondsCooling, &maxTimeCooling);
    
    //Deallocate the memory of the array!
    free(pizzas);
    pthread_exit(NULL);
}

//Finds the maximum
void findMax(int sec, int* max){
    if(sec > *max){
        *max = sec;
    }
}

//Prints the statistics
void printStatistics(void){
    pthread_mutex_lock(&lock_output);
    printf("\n-- Overall Statistics --\n");
    printf("Total profit: %d\n", profit);
    printf("Number of plain pizzas sold: %d\n", countPlain);
    printf("Number of special pizzas sold: %d\n", countSpecial);
    printf("Successful orders: %d\n", countSuccess);
    printf("Failed orders: %d\n", countFail);
    printf("Average service time : %d\n", avgTime);
    printf("Max service time : %d\n", maxTime);
    printf("Average cooling time : %d\n", avgTimeCooling);
    printf("Max cooling time : %d\n", maxTimeCooling);
    pthread_mutex_unlock(&lock_output);
}


//Aceepted orders!
void acceptOrder(int id, int numberOfPizzas, int pizzas []) {
    pthread_mutex_lock(&lock_output);
    printf("The order %d has been accepted!\n", id);
    pthread_mutex_unlock(&lock_output);
    pthread_mutex_lock(&lock_statistics);
    countSuccess++;

    //Determine the type of each pizza (plain or special) and update statistics and profit accordingly
    for (int i = 0; i < numberOfPizzas; i++) {
        double kindOfPizza = (double)rand_r(&seed) / RAND_MAX;
        if(kindOfPizza < Pplain){
            // 0 for plain pizzas
            pizzas[i] = 0;
            profit += Cplain;
            countPlain++;
        } else {
            // 1 for special pizzas
            pizzas[i] = 1;
            profit += Cspecial;
            countSpecial++;
        }
    }
    pthread_mutex_unlock(&lock_statistics);
}

//Declined Orders!!
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
        // Initialize mutexes and conditions
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
        // Destroy mutexes and conditions
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
