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


int main (int argc, char** argv) {

   char* mode     = argv[1];
   int   m        = atoi(argv[2]);
   int   btbSize  = atoi(argv[3]);
   int   btbAssoc = atoi(argv[4]);
   int   traceNo  = 5;
   int   n        = 0;

   predictPT predictP = predictorAlloc (m, n);

   tracify(predictP, argv[traceNo]);

   printCounters(predictP);
   
}








