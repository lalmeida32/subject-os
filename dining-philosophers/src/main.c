#include "monitor/monitor.h"
#include "dinphil/dinphil.h"
#include "util/util.h"
#include <stdio.h>
#include <stdlib.h>

void *pickup_delay_and_putdown(void *sd, void *args) {
  dinphil_pickup(sd, args);
  delay(1);
  dinphil_putdown(sd, args);
  free(args);

  return NULL;
}

int main(void) {
  void *(**functions)(void *, void *) = malloc(sizeof(void *));
  functions[0] = &pickup_delay_and_putdown;

  dinphil_shared_data_t *shared_data = dinphil_shared_data_init();
  monitor_t *monitor = monitor_init(1, shared_data, functions);

  dinphil_set_monitor(shared_data, monitor);

  // Each philosopher request resources 1 time for 1 second
  pthread_t *threads[5];

  for (int i = 0; i < 5; i++) {
    int *phil = malloc(sizeof(int));
    *phil = i;
    threads[i] = monitor_exec_thread(monitor, 0, phil);
  }

  for (int i = 0; i < 5; i++) {
    monitor_join_thread(threads[i]);
  }

  monitor_destroy(&monitor, NULL);
  
  return 0;
}