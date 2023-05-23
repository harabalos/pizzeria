#include <stdio.h>
#include <stdlib.h>
#include "pizza.h"
#include <pthread.h>
#include <unistd.h>


pthread_mutex_t lock;
pthread_cond_t cond;


// Thread function to process an order
void *order(void *x){

    int id = *(int *)x; // Extracting the ID of the order
    int rc;
    printf("Order %d started\n", id);
    
    int numberOfPizzas = rand_r(&seed) % (Norderlow + Norderhigh);
    int pizzas[numberOfPizzas];
    int paymentTime = rand_r(&seed) % (Tpaymenthigh - Tpaymentlow + 1) + Tpaymentlow;
    double acceptedCard = (double)rand_r(&seed) / RAND_MAX;
    
    if(acceptedCard > Pfail){
        countSuccess++;
        for (int i = 0; i < numberOfPizzas; i++) {
            double kindOfPizza = (double)rand_r(&seed) / RAND_MAX; // tells the type of pizzas of each costumer
            if(kindOfPizza<Pplain){
                pizzas[i] = 0; // 0 for plain pizzas
                profit+=10;
                countPlain++;
            }else{
                pizzas[i] = 1;//1 for special pizzas
                profit+=12;
                countSpecial++;
            }
        }
    }else{
        countFail++;
        pthread_exit(NULL);
    }
    

    rc = pthread_mutex_lock(&lock); // Acquiring the lock
    while (Ncook == 0) { // While there are no available resources
        printf("Order %d did not find available cook. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock); // Waiting on the condition variable
    }
    printf("Order %d is being prepared.\n", id);
    Ncook--; // Decreasing the number of available resources
    rc = pthread_mutex_unlock(&lock); // Releasing the lock

    sleep(Tprep); // Simulating some work being done with the resource

    rc = pthread_mutex_lock(&lock); // Acquiring the lock
    while (Noven < numberOfPizzas) { // While there are no available resources
        printf("Order %d did not find available ovens for all the pizzas at the same time. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock); // Waiting on the condition variable
    }
    printf("Order %d is cooking.\n", id);
    Noven-=numberOfPizzas; // Decreasing the number of available resources
    rc = pthread_mutex_unlock(&lock); // Releasing the lock
    
    sleep(Tbake);
    
    rc = pthread_mutex_lock(&lock); // Acquiring the lock
    while (Npacker == 0) { // While there are no available resources
        printf("Order %d did not find available packer. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock); // Waiting on the condition variable
    }
    printf("Order %d getting packeted.\n", id);
    Npacker--; // Decreasing the number of available resources
    rc = pthread_mutex_unlock(&lock); // Releasing the lock
    
    sleep(Tpack * numberOfPizzas);
    Noven+=numberOfPizzas;
    
    int deliveryTime = rand_r(&seed) % (Tdelhigh - Tdellow + 1) + Tdellow;
    rc = pthread_mutex_lock(&lock); // Acquiring the lock
    while (Ndeliverer == 0) { // While there are no available resources
        printf("Order %d did not find available deliverer. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock); // Waiting on the condition variable
    }
    printf("Order %d getting delivered.\n", id);
    Ndeliverer++; // Decreasing the number of available resources
    rc = pthread_mutex_unlock(&lock); // Releasing the lock
    Npacker++;
    
    sleep(deliveryTime * 2);
    
    
    rc = pthread_mutex_lock(&lock); // Acquiring the lock again
    printf("Order %d has been successfully served! \n", id);
    Ncook++; // Increasing the number of available resources
    rc = pthread_cond_signal(&cond); // Signaling a waiting thread
    rc = pthread_mutex_unlock(&lock); // Releasing the lock

    pthread_exit(NULL);
}

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

    pthread_mutex_init(&lock, NULL); // Initializing the mutex
    pthread_cond_init(&cond, NULL); // Initializing the condition variable


    for (int i = 0; i < Ncust; i++) {
        id[i] = i+1;
        printf("Customer %d is ordering.\n", i+1);
        rc = pthread_create(&threads[i], NULL, order, &id[i]); // Creating a thread for each order
        int nextCustomer = rand_r(&seed) % Torderhigh + Torderlow;
        sleep(nextCustomer);
    }

    for (int i = 0; i < Ncust; i++) {
        pthread_join(threads[i], NULL); // Waiting for all threads to finish
    }

    pthread_mutex_destroy(&lock); // Destroying the mutex
    pthread_cond_destroy(&cond); // Destroying the condition variable

    return 0;

}
