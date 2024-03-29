#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include "cache.h"

#define ADDRESSWIDTH           32
#define TAKEN                  1
#define NOT_TAKEN              0
#define PC_OFFSET              2
#define BASE_COUNT             2
#define BIMODAL                0
#define GSHARE                 1
#define HYBRID                 2

#define SATURATE(A, MIN, MAX)  (A > MAX) ? MAX : (A < MIN ? MIN : A)   

typedef struct _predictT* predictPT;

typedef unsigned int uint32_t;

typedef struct _predictT {
   int         m;
   int         n;
   int         rows;
   int         lShiftBits;
   int         rShiftBits;
   int*        counters;
   int         predictions;
   int         missPredictions;
   int         gHistoryReg;
   int         mode;
   predictPT   biModalP;
   predictPT   gShareP;
   cachePT     btbP;   
}predictT;

#include "build/branch_predictor_proto.h"
