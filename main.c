#include "branch_predictor.h"

void tracify (predictPT predictP, char* fileName) {
   FILE *trace;
   int  address;
   char  actual; 

   trace = fopen(fileName, "r");
   
   do
   {
      fscanf(trace,"%x %c\n", &address, &actual);

      bpProcess(predictP, address, (actual == 't' || actual == 'T') ? TAKEN : NOT_TAKEN);
   }
   while(!feof(trace));
   fclose(trace);
}

void printHeader (predictPT predictP)
{
   int predictions, missPredictions;
   getResults(predictP, &predictions, &missPredictions);

   float missRate = (float)missPredictions / (float)predictions;

   printf("OUTPUT\n");
   printf("number of predictions:	%d\n", predictions);
   printf("number of mispredictions: %d\n", missPredictions);
   printf("misprediction rate:	%.2f%%\n", missRate*100.0);

}

int main (int argc, char** argv) {
   char* cMode    = argv[1];
   int   mode     = strcmp(cMode, "gshare") == 0 ? GSHARE : BIMODAL;
   int   m        = atoi(argv[2]);
   int   n        = 0;
   int   btbSize  = atoi(argv[3]);
   int   btbAssoc = atoi(argv[4]);
   int   traceNo  = 5;

   if(mode == GSHARE) {
      n        = atoi(argv[3]);
      btbSize  = atoi(argv[4]);
      btbAssoc = atoi(argv[5]);
      traceNo  = 6;
   }

   predictPT predictP = predictorAlloc (mode, m, n);

   tracify(predictP, argv[traceNo]);

   printf("COMMAND\n");
   for(int i = 0; i < argc; i++ ){
      printf("%s ", argv[i]);
   }
   printf("\n");

   printHeader(predictP);
   printf("FINAL %s CONTENTS\n", mode == GSHARE ? "GSHARE" : "BIMODAL");
   printCounters(predictP);
}
