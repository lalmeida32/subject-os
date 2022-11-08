#include "dinphil.h"
#include "../util/util.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_COUNTER_MAX 4

typedef enum {
  THINKING,
  HUNGRY,
  EATING
} dinphil_state_t;

struct dinphil_shared_data {
  dinphil_state_t state[N_THREADS];
  pthread_cond_t self[N_THREADS];
  bool_t priority[N_THREADS];
  int counter[N_THREADS];
  int counter_max;
  pthread_mutex_t *mutex;
};

static int _left(int i) {
  return (i + N_THREADS-1) % N_THREADS;
}

static int _right(int i) {
  return (i + 1) % N_THREADS;
}

static void _test(dinphil_shared_data_t *shared_data, int i) {
  if ((shared_data->state[_left(i)] != EATING) &&
      (shared_data->state[i] == HUNGRY) &&
      (shared_data->state[_right(i)] != EATING)) {
    
    shared_data->state[i] = EATING;
    printf("Philosopher %d eating\n", i);
    shared_data->counter[i] = 0;
    shared_data->priority[i] = FALSE;

    pthread_cond_signal(&shared_data->self[i]);
  }
  else if (shared_data->state[i] == HUNGRY) {
    int counter = ++shared_data->counter[i];
    printf("Philosopher %d tried to eat %d times\n", i, counter);
    if (counter == shared_data->counter_max) {
      shared_data->priority[i] = TRUE;
      printf("Philosopher %d is priority\n", i);
    }
  }
}

void *dinphil_pickup(void *sd, void *args) {
  dinphil_shared_data_t *shared_data = (dinphil_shared_data_t *) sd;
  int i = *(int *) args;
  pthread_mutex_t *mutex = shared_data->mutex;

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
  _test(shared_data, _left(i));
  _test(shared_data, _right(i));
  
  return NULL;
}

dinphil_shared_data_t *dinphil_shared_data_init() {
  dinphil_shared_data_t *shared_data = malloc(sizeof(dinphil_shared_data_t));
  shared_data->mutex = NULL;
  shared_data->counter_max = DEFAULT_COUNTER_MAX;

  for (int i = 0; i < N_THREADS; i++) {
    shared_data->state[i] = THINKING;
    printf("Philosopher %d thinking\n", i);
    pthread_cond_init(&shared_data->self[i], NULL);
  }

  return shared_data;
}

void dinphil_shared_data_destroy(void *sd) {
  dinphil_shared_data_t *shared_data = (dinphil_shared_data_t *) sd;
  free(shared_data);
}

void dinphil_set_mutex(dinphil_shared_data_t *shared_data, pthread_mutex_t *mutex) {
  shared_data->mutex = mutex;
}

void dinphil_set_counter_max(dinphil_shared_data_t *shared_data, int counter_max) {
  shared_data->counter_max = counter_max;
}