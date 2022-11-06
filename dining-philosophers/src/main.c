#include "monitor/monitor.h"
#include "util/util.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *increment(void *shared_data, void *arg) {
  (*(int *)shared_data) += (*(int *) arg);
  delay(1);
  printf("%d\n", (*(int *)shared_data));
  return NULL;
}

int main(void) {
  int *shared_data = malloc(sizeof(int));
  (*shared_data) = 0;
  void *(**functions)(void *, void *) = malloc(sizeof(void *(*)(void *, void *)));
  functions[0] = &increment;
  monitor_t *m = monitor_init(1, shared_data, functions);

  int i1 = 1;
  pthread_t *thr1 = monitor_exec_thread(m, 0, &i1);

  int i2 = 2;
  pthread_t *thr2 = monitor_exec_thread(m, 0, &i2);

  int i3 = 3;
  pthread_t *thr3 = monitor_exec_thread(m, 0, &i3);


  monitor_join_thread(thr1);
  monitor_join_thread(thr2);
  monitor_join_thread(thr3);

  monitor_destroy(&m, NULL);
  
  return 0;
}