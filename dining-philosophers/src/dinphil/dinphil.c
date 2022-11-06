#include <pthread.h>

typedef enum {
  THINKING,
  HUNGRY,
  EATING
} dinphil_state_t;

typedef struct {
  dinphil_state_t state[5];
  pthread_cond_t self[5];
} dinphil_shared_data_t;

/*

void pickup(int i) {
  state[i] = HUNGRY;
  test(i);
  if (state[i] != EATING)
  self[i].wait();
  }
void putdown(int i) {
  state[i] = THINKING;
  test((i + 4) % 5);
  test((i + 1) % 5);
  }
void test(int i) {
  if ((state[(i + 4) % 5] != EATING) &&
  (state[i] == HUNGRY) &&
  (state[(i + 1) % 5] != EATING)) {
      state[i] = EATING;
      self[i].signal();
      }
  }

void initialization_code() {
  for (int i = 0; i < 5; i++)
  state[i] = THINKING;
}
*/