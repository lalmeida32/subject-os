#include "monitor/monitor.h"
#include "dinphil/dinphil.h"
#include "util/util.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  PICKUP,
  PUTDOWN
} function_id_t;

typedef struct {
  monitor_t *monitor;
  int philosopher;
} thread_args_t;

void *thread(void *args) {
  thread_args_t *thr_args = (thread_args_t *) args;

  int *mon_args = (int *) malloc(sizeof(int));
  *mon_args = thr_args->philosopher;
 
  for (int i = 0; i < 2; i++) {
    monitor_exec(thr_args->monitor, PICKUP, mon_args); 
    delay(1);
    monitor_exec(thr_args->monitor, PUTDOWN, mon_args);
  }

  free(mon_args);
  free(thr_args);

  return NULL;
}

int main(void) {
  void *(**functions)(void *, void *) = malloc(2 * sizeof(void *));
  functions[0] = &dinphil_pickup;
  functions[1] = &dinphil_putdown;

  dinphil_shared_data_t *shared_data = dinphil_shared_data_init();
  monitor_t *monitor = monitor_init(2, shared_data, functions);
  dinphil_set_monitor(shared_data, monitor);

  // Each philosopher request resources 2 times for 1 second each
  pthread_t *threads[5];

  for (int i = 0; i < 5; i++) {
    
    threads[i] = malloc(sizeof(pthread_t));
    thread_args_t *thread_args = malloc(sizeof(thread_args_t));
    thread_args->monitor = monitor;
    thread_args->philosopher = i;
    pthread_create(threads[i], NULL, &thread, thread_args);
  }

  for (int i = 0; i < 5; i++) {
    pthread_join(*threads[i], NULL);
    free(threads[i]);
  }

  monitor_destroy(&monitor, NULL);
  
  return 0;
}