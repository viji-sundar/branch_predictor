#include "branch_predictor.h"


/*!proto*/
predictPT predictorAlloc (int m, int n) 
/*!endproto*/
{
   predictPT predictP          = (predictPT)calloc(1, sizeof(predictT));

   predictP->m                 = m;
   predictP->n                 = n;
   predictP->rows              = pow(2, m);
   predictP->lShiftBits        = ADDRESSWIDTH - predictP->m - PC_OFFSET;
   predictP->rShiftBits        = ADDRESSWIDTH - predictP->m;

   //make it into char if needed
   //FIXME: are they infact rows ?
   predictP->counters          = (int*)calloc(predictP->rows, sizeof(int));
   for(int i = 0; i < predictP->rows; i++) {
      predictP->counters[i]    = INIT_COUNT;
   }
   return predictP;
}

/*!proto*/
int getIndex (predictPT predictP, uint32_t address)
/*!endproto*/
{
   return (address << predictP->lShiftBits) >> predictP->rShiftBits;  
}

/*!proto*/
// predict at a particular index. Make a prediction.
int makePrediction (predictPT predictP, int index)
/*!endproto*/
{
   return (predictP->counters[index] >= BASE_COUNT) ? TAKEN : NOT_TAKEN;
}

/*!proto*/
void updateCounters (predictPT predictP, int index, int prediction, int actual)
/*!endproto*/
{
   if(actual == TAKEN) 
      predictP->counters[index]++;
   else
      predictP->counters[index]--;

   predictP->counters[index] = SATURATE ( predictP->counters[index], 0, 3 );
}

/*!proto*/
void bpProcess (predictPT predictP, uint32_t address, int actual)
/*!endproto*/
{
   int index      = getIndex (predictP, address);
   int prediction = makePrediction (predictP, index); 
   updateCounters (predictP, index, prediction, actual);
}

/*!proto*/
void printCounters (predictPT predictP)
/*!endproto*/
{
   for(int i = 0; i < predictP->rows; i++)
      printf("%d %d\n", i, predictP->counters[i]);
}
