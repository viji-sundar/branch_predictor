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
   int   mode     = !strcmp(cMode, "gshare") ? GSHARE : ( !strcmp(cMode, "hybrid") ? HYBRID : BIMODAL);
   int   m1       = mode == HYBRID ? atoi(argv[3]) : atoi(argv[2]);
   int   m2       = mode == HYBRID ? atoi(argv[5]) : atoi(argv[2]);

   int   btbSize  = 0;
   int   btbAssoc = 0;
   int   n        = (mode == GSHARE) ? atoi(argv[3]) : (mode == HYBRID ? atoi(argv[4]) : 0);
   int   k        = atoi(argv[2]);  
   int   traceNo  = (mode == GSHARE) ? 6 : (mode == HYBRID ? 8 : 5);

   predictPT predictP;

   if( mode == HYBRID ){
      predictP           = predictorAlloc (HYBRID , k, n, 1);
      predictPT gShareP  = predictorAlloc (GSHARE , m1, n, 2);
      predictPT biModalP = predictorAlloc (BIMODAL, m2, n, 2);

      makeHybridPredictor( predictP, gShareP, biModalP );
   } else{
      predictP = predictorAlloc (mode, m1, n, 2);
   }

   tracify(predictP, argv[traceNo]);

   printf("COMMAND\n");
   for(int i = 0; i < argc; i++ ){
      printf("%s ", argv[i]);
   }
   printf("\n");

   printHeader(predictP);
   printCounters(predictP, mode == GSHARE ? "GSHARE" : (mode == HYBRID ? "CHOOSER" : "BIMODAL"));
   printCounters(predictP->gShareP, "GSHARE");
   printCounters(predictP->biModalP, "BIMODAL");
}
