#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pizza.h"
#include <unistd.h>
#include <time.h>


int Ncook = 10;
int Noven = 15;
int Npacker = 2;
int Ndeliverer = 10;
int Torderlow = 1;
int Torderhigh = 3;
int Norderlow = 1;
int Norderhigh = 5;
double Pplain = 0.6;
int Tpaymentlow = 1;
int Tpaymenthigh = 3;
double Pfail = 0.1;
int Cplain = 10;
int Cspecial = 12;
int Tprep = 1;
int Tbake = 10;
int Tpack = 1;
int Tdellow = 5;
int Tdelhigh = 15;
int profit = 0;
int countPlain = 0;
int countSpecial = 0;
int countSuccess = 0;
int countFail = 0;
long coolingTime = 0;
long sumTime = 0;
long maxTime = 0;
long maxTimeCooling = 0;
long avgTime = 0;
long avgTimeCooling = 0;


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
        int nextCustomer = rand_r(&seed) % Torderhigh + Torderlow;
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
