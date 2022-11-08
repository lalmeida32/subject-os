#include "monitor/monitor.h"
#include "dinphil/dinphil.h"
#include "util/util.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Constants

#define N_REQUESTS 20
#define N_FUNCTIONS 2

// Data

typedef enum {
  PICKUP,
  PUTDOWN
} function_id_t;

typedef struct {
  monitor_t *monitor;
  int philosopher;
} thread_args_t;

// Code to run in each thread.
void *thread(void *args) {
  thread_args_t *thr_args = (thread_args_t *) args;

  int *mon_args = (int *) malloc(sizeof(int));
  *mon_args = thr_args->philosopher;
 
  for (int i = 0; i < N_REQUESTS; i++) {
    monitor_exec(thr_args->monitor, PICKUP, mon_args);
    delay(random() % 2 + 1);
    monitor_exec(thr_args->monitor, PUTDOWN, mon_args);
  }

  free(mon_args);
  free(thr_args);

  return NULL;
}

int main(void) {
  // Initialization
  void *(**functions)(void *, void *) = malloc(N_FUNCTIONS * sizeof(void *));
  functions[PICKUP] = &dinphil_pickup;
  functions[PUTDOWN] = &dinphil_putdown;

  dinphil_shared_data_t *shared_data = dinphil_shared_data_init();
  monitor_t *monitor = monitor_init(N_FUNCTIONS, shared_data, functions);
  dinphil_set_mutex(shared_data, monitor_get_mutex(monitor));

  // Each philosopher request resources N_REQUESTS times for 1 or 2 seconds each
  pthread_t *threads[N_THREADS];

  for (int i = 0; i < N_THREADS; i++) {
    
    threads[i] = malloc(sizeof(pthread_t));
    thread_args_t *thread_args = malloc(sizeof(thread_args_t));
    thread_args->monitor = monitor;
    thread_args->philosopher = i;
    pthread_create(threads[i], NULL, &thread, thread_args);
  }

  // Joining threads and freeing memory
  for (int i = 0; i < N_THREADS; i++) {
    pthread_join(*threads[i], NULL);
    free(threads[i]);
  }

  monitor_destroy(&monitor, &dinphil_shared_data_destroy);
  
  return 0;
}