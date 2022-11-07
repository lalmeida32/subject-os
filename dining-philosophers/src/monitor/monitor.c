#include "monitor.h"
#include <stdlib.h>

struct monitor {
  int n_functions;
  void *shared_data;
  void *(**functions)(void *, void *);
  pthread_mutex_t *mutex;
};

typedef struct {
  monitor_t *monitor;
  int function_id;
  void *args;
} thread_args_t;

static void *_thread(void *args) {
  thread_args_t *thr_args = (thread_args_t *) args;
  void *result = monitor_exec(thr_args->monitor, thr_args->function_id, thr_args->args);
  free(thr_args);
  return result;
}

monitor_t *monitor_init(int n_functions, void *shared_data, void *(**functions)(void *, void *)) {
  monitor_t *m = malloc(sizeof(monitor_t));
  m->n_functions = n_functions;
  m->shared_data = shared_data;
  m->functions = functions;
  m->mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(m->mutex, NULL);
  return m;
}

void monitor_destroy(monitor_t **m, void (*shared_data_destroy)(void *)) {
  if (shared_data_destroy == NULL)
    free((*m)->shared_data);
  else
    shared_data_destroy((*m)->shared_data);

  free((*m)->functions);
  pthread_mutex_destroy((*m)->mutex);
  free((*m)->mutex);
  free(*m);
  *m = NULL;
}

void *monitor_exec(monitor_t *m, int function_id, void *args) {
  pthread_mutex_lock(m->mutex);
  void *r = m->functions[function_id](m->shared_data, args);
  pthread_mutex_unlock(m->mutex);
  return r;
}

pthread_t *monitor_exec_thread(monitor_t *m, int function_id, void *args) {
  pthread_t *thr = malloc(sizeof(pthread_t));
  thread_args_t *thr_args = malloc(sizeof(thread_args_t));
  thr_args->monitor = m;
  thr_args->function_id = function_id;
  thr_args->args = args;
  pthread_create(thr, NULL, &_thread, thr_args);
  return thr;
}

void *monitor_join_thread(pthread_t *thr) {
  void *result;
  pthread_join(*thr, &result);
  free(thr);
  return result;
}

pthread_mutex_t *monitor_get_mutex(monitor_t *m) {
  return m->mutex;
}