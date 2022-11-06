#ifndef _DINPHIL_H_
#define _DINPHIL_H_

#include "../monitor/monitor.h"

typedef struct dinphil_shared_data dinphil_shared_data_t;

void *dinphil_pickup(void *sd, void *args);

void *dinphil_putdown(void *sd, void *args);

dinphil_shared_data_t *dinphil_shared_data_init();

void dinphil_set_monitor(dinphil_shared_data_t *shared_data, monitor_t *monitor);

#endif