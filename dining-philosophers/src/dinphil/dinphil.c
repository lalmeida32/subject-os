#include "dinphil.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
  THINKING,
  HUNGRY,
  EATING
} dinphil_state_t;

struct dinphil_shared_data {
  dinphil_state_t state[5];
  pthread_cond_t self[5];
  monitor_t *monitor;
};

static void _test(dinphil_shared_data_t *shared_data, int i) {
  if ((shared_data->state[(i + 4) % 5] != EATING) &&
      (shared_data->state[i] == HUNGRY) &&
      (shared_data->state[(i + 1) % 5] != EATING)) {
    
    shared_data->state[i] = EATING;
    printf("Philosopher %d eating\n", i);
    pthread_cond_signal(&shared_data->self[i]);
  }
}

void *dinphil_pickup(void *sd, void *args) {
  dinphil_shared_data_t *shared_data = (dinphil_shared_data_t *) sd;
  int i = *(int *) args;
  pthread_mutex_t *mutex = monitor_get_mutex(shared_data->monitor);

  shared_data->state[i] = HUNGRY;
  printf("Philosopher %d hungry\n", i);
  _test(shared_data, i);
  if (shared_data->state[i] != EATING)
    pthread_cond_wait(&shared_data->self[i], mutex);
  
  return NULL;
}

void *dinphil_putdown(void *sd, void *args) {
  dinphil_shared_data_t *shared_data = (dinphil_shared_data_t *) sd;
  int i = *(int *) args;

  shared_data->state[i] = THINKING;
  printf("Philosopher %d thinking\n", i);
  _test(shared_data, (i + 4) % 5);
  _test(shared_data, (i + 1) % 5);
  
  return NULL;
}

dinphil_shared_data_t *dinphil_shared_data_init() {
  dinphil_shared_data_t *shared_data = malloc(sizeof(dinphil_shared_data_t));
  shared_data->monitor = NULL;

  for (int i = 0; i < 5; i++) {
    shared_data->state[i] = THINKING;
    printf("Philosopher %d thinking\n", i);
    pthread_cond_init(&shared_data->self[i], NULL);
  }

  return shared_data;
}

void dinphil_set_monitor(dinphil_shared_data_t *shared_data, monitor_t *monitor) {
  shared_data->monitor = monitor;
}
