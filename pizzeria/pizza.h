#ifndef pizza_h
#define pizza_h

int Ncook = 10;
int Noven = 15;
int Npacker = 2;
int Ndeliverer = 10;
#define Torderlow 1
#define Torderhigh 3
#define Norderlow 1
#define Norderhigh 5
#define Pplain 0.6
#define Tpaymentlow 1
#define Tpaymenthigh 3
#define Pfail 0.1
#define Cplain 10
#define Cspecial 12
#define Tprep 1
#define Tbake 10
#define Tpack 1
#define Tdellow 5
#define Tdelhigh 15

int rc;
unsigned int seed;
int Ncust;
int profit = 0;
int countPlain = 0;
int countSpecial = 0;
int countSuccess = 0;
int countFail = 0;

struct timespec start_time,start_time_cooling, end_timeX, end_timeY;
long secondsX;
long secondsY;
long secondsCooling;
long coolingTime = 0;
long sumTime = 0;
long maxTime = 0;
long maxTimeCooling = 0;
long avgTime = 0;
long avgTimeCooling = 0;
#endif