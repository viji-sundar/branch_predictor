#include "branch_predictor.h"

/*!proto*/
predictPT predictorAlloc (int mode, int m, int n, int initCount, int btbSize, int btbAssoc) 
/*!endproto*/
{
   predictPT predictP          = (predictPT)calloc(1, sizeof(predictT));
   predictP->btbP              = cacheAllocate(btbSize, 4, btbAssoc, WBWA, LRU); 

   predictP->mode              = mode;
   predictP->m                 = m;
   predictP->n                 = n;
   predictP->rows              = pow(2, m);
   predictP->lShiftBits        = ADDRESSWIDTH - predictP->m - PC_OFFSET;
   predictP->rShiftBits        = ADDRESSWIDTH - predictP->m;

   //make it into char if needed
   //FIXME: are they infact rows ?
   predictP->counters          = (int*)calloc(predictP->rows, sizeof(int));
   for(int i = 0; i < predictP->rows; i++) {
      predictP->counters[i]    = initCount;
   }
   return predictP;
}

/*!proto*/
int getIndex (predictPT predictP, uint32_t address)
/*!endproto*/
{
   int index = (address << predictP->lShiftBits) >> predictP->rShiftBits; 
   if(predictP->mode == BIMODAL)
      return index; 
   else
      return ((predictP->gHistoryReg) << (predictP->m - predictP->n)) ^ index;
}


/*!proto*/
// predict at a particular index. Make a prediction.
int makePrediction (predictPT predictP, int index)
/*!endproto*/
{
   return (predictP->counters[index] >= BASE_COUNT) ? TAKEN : NOT_TAKEN;
}

/*!proto*/
void bpUpdateCounters (predictPT predictP, int index, int actual)
/*!endproto*/
{
   if(actual == TAKEN) 
      predictP->counters[index]++;
   else 
      predictP->counters[index]--;

   predictP->counters[index] = SATURATE ( predictP->counters[index], 0, 3 );
}

/*!proto*/
void updateHCounter (predictPT predictP, int index, int actual, int predictG, int predictB)
/*!endproto*/
{
   if(predictG == actual && predictB != actual)
      predictP->counters[index]++;
   else if(predictG != actual && predictB == actual)
      predictP->counters[index]--;

   predictP->counters[index] = SATURATE ( predictP->counters[index], 0, 3 );
}


/*!proto*/
void updateGHR (predictPT predictP, int actual) 
/*!endproto*/
{
   predictP->gHistoryReg     = (predictP->gHistoryReg >> 1) | (actual << (predictP->n-1));
}

/*!proto*/
void bpProcess (predictPT predictP, uint32_t address, int actual)
/*!endproto*/
{
   
   predictP->predictions++;
   int prediction = NOT_TAKEN;

   if(!(predictP->btbP != NULL && !read (predictP->btbP, address))) {
      if(predictP->mode != HYBRID){
         int index           = getIndex (predictP, address);
         prediction          = makePrediction (predictP, index); 
         bpUpdateCounters (predictP, index, actual);
         updateGHR (predictP, actual); 
      }
      else {
         int indexH          = getIndex (predictP, address);
         int indexG          = getIndex (predictP->gShareP, address);
         int indexB          = getIndex (predictP->biModalP, address);

         int predictionG     = makePrediction(predictP->gShareP, indexG);
         int predictionB     = makePrediction(predictP->biModalP, indexB);

         if(predictP->counters[indexH] >= BASE_COUNT) {
            prediction       = predictionG;
            bpUpdateCounters(predictP->gShareP, indexG, actual);
         }
         else {
            prediction       = predictionB;
            bpUpdateCounters(predictP->biModalP, indexB, actual);
         }
         updateGHR (predictP->gShareP, actual); 
         updateHCounter(predictP, indexH, actual, predictionG, predictionB); 
      }
   }

   if(prediction != actual)
      predictP->missPredictions++;
}

/*!proto*/
void makeHybridPredictor (predictPT predictP, predictPT gShareP, predictPT biModalP)
/*!endproto*/
{
   predictP->gShareP    = gShareP;
   predictP->biModalP   = biModalP;
}

/*!proto*/
void printCounters (predictPT predictP, char* name)
/*!endproto*/
{
   if( predictP == NULL ) return;

   printf("FINAL %s CONTENTS\n", name);
   for(int i = 0; i < predictP->rows; i++)
      printf("%d %d\n", i, predictP->counters[i]);
}

/*!proto*/
void getResults (predictPT predictP, int* predictions, int* missPredictions)
/*!endproto*/
{
   *predictions     = predictP->predictions;
   *missPredictions = predictP->missPredictions;
}

