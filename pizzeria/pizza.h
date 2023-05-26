#ifndef pizza_h
#define pizza_h

#include <pthread.h>

//Mutexes and conditions
pthread_mutex_t lock_cook;         //Mutex for cook
pthread_mutex_t lock_oven;         //Mutex for oven
pthread_mutex_t lock_packer;       //Mutex for packer
pthread_mutex_t lock_deliverer;    //Mutex for deliverer
pthread_mutex_t lock_statistics;   //Mutex for statistics
pthread_mutex_t lock_output;       //Mutex for output
pthread_cond_t cond_cook;          //Condition variable for cook
pthread_cond_t cond_oven;          //Condition variable for oven
pthread_cond_t cond_packer;        //Condition variable for packer
pthread_cond_t cond_deliverer;     //Condition variable for deliverer

//Function prototypes
void *order(void *x);
void printStatistics(void);
void findMax(int sec, int* max);
void acceptOrder(int id, int numberOfPizzas, int pizzas []);
void declineOrder(int id);
void initAndDestroy(int choice);

//Global variables
int Ncook = 10;                   //Number of available cooks
int Noven = 15;                   //Number of available ovens
int Npacker = 2;                  //Number of available packers
int Ndeliverer = 10;              //Number of available deliverers
int Torderlow = 1;                //Minimum time between orders
int Torderhigh = 3;               //Maximum time between orders
int Norderlow = 1;                //Minimum number of pizzas in an order
int Norderhigh = 5;               //Maximum number of pizzas in an order
double Pplain = 0.6;              //Probability of a plain pizza
int Tpaymentlow = 1;              //Minimum time for payment
int Tpaymenthigh = 3;             //Maximum time for payment
double Pfail = 0.1;               //Probability of a payment failure
int Cplain = 10;                  //Cost of a plain pizza
int Cspecial = 12;                //Cost of a special pizza
int Tprep = 1;                    //Time to prepare a pizza
int Tbake = 10;                   //Time to bake a pizza
int Tpack = 1;                    //Time to pack a pizza
int Tdellow = 5;                  //Minimum time for delivery
int Tdelhigh = 15;                //Maximum time for delivery
int profit = 0;                   //Total profit
int countPlain = 0;               //Number of plain pizzas sold
int countSpecial = 0;             //Number of special pizzas sold
int countSuccess = 0;             //Number of successful orders
int countFail = 0;                //Number of failed orders
int coolingTime = 0;              //Total cooling time
int sumTime = 0;                  //Total service time
int maxTime = 0;                  //Maximum service time
int maxTimeCooling = 0;           //Maximum cooling time
int avgTime = 0;                  //Average service time
int avgTimeCooling = 0;           //Average cooling time
int counterOrder = 0;             //Counter for tracking number of orders
int errorFlag = 0;                //Flag to indicate memory allocation error

unsigned int seed;                //Seed for random number generation
int Ncust;                        //Number of customers
int secondsX;                     //Time for preparation
int secondsY;                     //Time for delivery
int secondsCooling;               //Cooling time

#endif
