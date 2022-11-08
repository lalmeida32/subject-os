#include "dinphil.h"
#include "../util/util.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_COUNTER_MAX 3

typedef enum {
  THINKING,
  HUNGRY,
  EATING
} dinphil_state_t;

struct dinphil_shared_data {
  dinphil_state_t state[N_THREADS];
  pthread_cond_t self[N_THREADS];
  bool_t priority[N_THREADS]; // Criteria: counter is equal to counter_max
  int counter[N_THREADS];     // Philosopher tried to eat but failed
  int counter_eat[N_THREADS]; // How many times philosopher ate
  int counter_max;
  pthread_mutex_t *mutex;
};

static int _left(int i) {
  return (i + N_THREADS-1) % N_THREADS;
}

static int _right(int i) {
  return (i + 1) % N_THREADS;
}

static void _print_state(dinphil_shared_data_t *shared_data) {
  printf("\n");

  for (int i = 0; i < N_THREADS; i++) {
    dinphil_state_t state = shared_data->state[i];

    char *priority_str = shared_data->priority[i] ? "*" : " ";
    char *state_str = state == THINKING ? "THINKING" : state == HUNGRY ? "HUNGRY" : "EATING";

    printf("%s[%d] - %s\t%d\n", priority_str, i, state_str, shared_data->counter_eat[i]);
  }

}

static void _test(dinphil_shared_data_t *shared_data, int i) {
  // If at least one of the adjacent philosophers is priority, then the current
  //  philosopher cannot eat or try to eat. Must wait that adjacent philosopher
  //  eat.
  bool_t is_blocked = shared_data->priority[_left(i)] || shared_data->priority[_right(i)];

  // If can eat
  if ((shared_data->state[_left(i)] != EATING) &&
      (shared_data->state[i] == HUNGRY) &&
      (shared_data->state[_right(i)] != EATING) &&
      !is_blocked) {
    
    // Then eat
    shared_data->state[i] = EATING;
    shared_data->counter[i] = 0;
    shared_data->priority[i] = FALSE;
    shared_data->counter_eat[i]++;
    _print_state(shared_data);

    pthread_cond_signal(&shared_data->self[i]);
  }

  // If tried to eat
  else if (shared_data->state[i] == HUNGRY && !is_blocked) {
    int counter = ++shared_data->counter[i];
    printf("\nPhilosopher %d tried to eat %d times\n", i, counter);

    // If get priority criteria
    if (counter == shared_data->counter_max) {
      shared_data->priority[i] = TRUE;
      _print_state(shared_data);
    }
  }
}

void *dinphil_pickup(void *sd, void *args) {
  dinphil_shared_data_t *shared_data = (dinphil_shared_data_t *) sd;
  int i = *(int *) args;
  pthread_mutex_t *mutex = shared_data->mutex;

  shared_data->state[i] = HUNGRY;
  _print_state(shared_data);

  // Try to eat
  _test(shared_data, i);

  // Could not eat
  if (shared_data->state[i] != EATING)
    pthread_cond_wait(&shared_data->self[i], mutex);
  
  return NULL;
}

void *dinphil_putdown(void *sd, void *args) {
  dinphil_shared_data_t *shared_data = (dinphil_shared_data_t *) sd;
  int i = *(int *) args;


  shared_data->state[i] = THINKING;
  _print_state(shared_data);
  
  // Notify adjacent philosophers
  _test(shared_data, _left(i));
  _test(shared_data, _right(i));
  
  return NULL;
}

dinphil_shared_data_t *dinphil_shared_data_init() {
  dinphil_shared_data_t *shared_data = malloc(sizeof(dinphil_shared_data_t));
  shared_data->mutex = NULL;
  shared_data->counter_max = DEFAULT_COUNTER_MAX;

  for (int i = 0; i < N_THREADS; i++) {
    // Initialization of each philosopher.
    shared_data->state[i] = THINKING;
    shared_data->counter[i] = 0;
    shared_data->priority[i] = FALSE;
    shared_data->counter_eat[i] = 0;
    pthread_cond_init(&shared_data->self[i], NULL);
  }
  _print_state(shared_data);

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