#include "monitor/monitor.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  monitor_t *monitor;
  void *arg;
} thr_args_t;

void delay(float seconds){	
	int start = clock();
	// wait n seconds
    	while (((clock()-start)/(float)CLOCKS_PER_SEC)<seconds);         
}


void *increment(void *shared_data, void *arg) {
  (*(int *)shared_data) += (*(int *) arg);
  delay(0.2);
  printf("%d\n", (*(int *)shared_data));
  return NULL;
}

void *thread(void *args) {
  thr_args_t *thr_args = (thr_args_t *) args;
  return monitor_exec(thr_args->monitor, 0, thr_args->arg);
}

int main(void) {
  int *shared_data = malloc(sizeof(int));
  (*shared_data) = 0;
  void *(**functions)(void *, void *) = malloc(sizeof(void *(*)(void *, void *)));
  functions[0] = &increment;
  monitor_t *m = monitor_init(1, shared_data, functions);
  

  pthread_t pth1;
  thr_args_t pth1_args;
  pth1_args.monitor = m;
  int i1 = 1;
  pth1_args.arg = &i1; 
  pthread_create(&pth1, NULL, &thread, &pth1_args);

  pthread_t pth2;
  thr_args_t pth2_args;
  pth2_args.monitor = m;
  int i2 = 2;
  pth2_args.arg = &i2; 
  pthread_create(&pth2, NULL, &thread, &pth2_args);

  pthread_t pth3;
  thr_args_t pth3_args;
  pth3_args.monitor = m;
  int i3 = 3;
  pth3_args.arg = &i3; 
  pthread_create(&pth3, NULL, &thread, &pth3_args);

  pthread_join(pth1, NULL);
  pthread_join(pth2, NULL);
  pthread_join(pth3, NULL);

  monitor_destroy(&m, NULL);
  
  return 0;
}