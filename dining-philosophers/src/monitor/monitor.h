#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <pthread.h>

typedef struct monitor monitor_t;

// This function initializes a monitor structure.
// It receives a function array and data to be shared between this functions.
// The shared data must be in the heap memory, in order to work with threads.
monitor_t *monitor_init(int n_functions, void *shared_data, void *(**functions)(void *, void *));

// This function free all memory allocated by the monitor structure.
// Optionally receives a function to free shared_data memory. Otherwise uses
//  stdlib's free function.
void monitor_destroy(monitor_t **m, void (*shared_data_destroy)(void *));

// This function properly executes a function stored in the monitor structure.
// The monitor structure guarantees the mutual exclusion of these functions by
//  using an internal mutex.
void *monitor_exec(monitor_t *m, int function_id, void *arg);

// This function just return the internal mutex from a monitor. This is useful
//  to wait pthread conditions properly.
pthread_mutex_t *monitor_get_mutex(monitor_t *m);

#endif