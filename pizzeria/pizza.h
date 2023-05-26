#ifndef pizza_h
#define pizza_h

pthread_mutex_t lock_cook, lock_oven, lock_packer, lock_deliverer, lock_statistics, lock_output;
pthread_cond_t cond_cook, cond_oven, cond_packer, cond_deliverer;

void *order(void *x);
void printStatistics(void);
void findMax(int sec, int* max);
void acceptOrder(int id, int numberOfPizzas, int pizzas []);
void declineOrder(int id);
void initAndDestroy(int choice);

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
int coolingTime = 0;
int sumTime = 0;
int maxTime = 0;
int maxTimeCooling = 0;
int avgTime = 0;
int avgTimeCooling = 0;
int counterOrder = 0;
int errorFlag = 0;

int rc;
unsigned int seed;
int Ncust;
struct timespec start_time,start_time_cooling, end_timeX, end_timeY, end_time;
_Thread_local int secondsX;
_Thread_local int secondsY;
int secondsCooling;
double seconds;

#endif
