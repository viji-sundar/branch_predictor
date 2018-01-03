#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<stdlib.h>

#define ADDRESSWIDTH           32
#define TAKEN                  1
#define NOT_TAKEN              0
#define INIT_COUNT             2
#define PC_OFFSET              2
#define BASE_COUNT             2
#define SATURATE(A, MIN, MAX)  (A > MAX) ? MAX : (A < MIN ? MIN : A)   

typedef struct _predictT* predictPT;

typedef unsigned int uint32_t;

typedef struct _predictT {
   int       m;
   int       n;
   int       rows;
   int       lShiftBits;
   int       rShiftBits;
   int*      counters;
   int       predictions;
   int       missPredictions;
}predictT;

#include "build/branch_predictor_proto.h"
