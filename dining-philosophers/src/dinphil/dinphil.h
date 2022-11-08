#ifndef _DINPHIL_H_
#define _DINPHIL_H_

#include <pthread.h>
#define N_THREADS 5

typedef struct dinphil_shared_data dinphil_shared_data_t;

// This function is used inside a monitor and represents the PICKUP of the
//  sticks to eat.
void *dinphil_pickup(void *sd, void *args);

// This function is used inside a monitor and represents the PUTDOWN of the
//  sticks to think.
void *dinphil_putdown(void *sd, void *args);

// Initiliazation of the shared data (states and conditions)
dinphil_shared_data_t *dinphil_shared_data_init();

// This is useful to get access to the mutex of the monitor
void dinphil_set_mutex(dinphil_shared_data_t *shared_data, pthread_mutex_t *mutex);

// This function just destroy the shared data and is useful to properly freeing
//  memory inside a monitor.
void dinphil_shared_data_destroy(void *sd);

// This is useful to set how many times a philosopher must try to eat to get
//  priority.
void dinphil_set_counter_max(dinphil_shared_data_t *shared_data, int counter_max);

#endif