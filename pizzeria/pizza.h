#ifndef pizza_h
#define pizza_h

pthread_mutex_t lock;
pthread_cond_t cond;

void *order(void *x);
void printStatistics(void);
void findMax(long sec, long* max);
void acceptOrder(int id, int numberOfPizzas, int pizzas []);
void declineOrder(int id);
extern int Ncook;
extern int Noven;
extern int Npacker;
extern int Ndeliverer;
extern int Torderlow;
extern int Torderhigh;
extern int Norderlow;
extern int Norderhigh;
extern double Pplain;
extern int Tpaymentlow;
extern int Tpaymenthigh;
extern double Pfail;
extern int Cplain;
extern int Cspecial;
extern int Tprep;
extern int Tbake;
extern int Tpack;
extern int Tdellow;
extern int Tdelhigh;
extern int profit;
extern int countPlain;
extern int countSpecial;
extern int countSuccess;
extern int countFail;
extern long coolingTime;
extern long sumTime;
extern long maxTime;
extern long maxTimeCooling;
extern long avgTime;
extern long avgTimeCooling;

int rc;
unsigned int seed;
int Ncust;
struct timespec start_time,start_time_cooling, end_timeX, end_timeY;
long secondsX;
long secondsY;
long secondsCooling;

#endif
