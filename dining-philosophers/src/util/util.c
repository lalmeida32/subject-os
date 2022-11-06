#include <stdlib.h>
#include <time.h>

void delay(int seconds) {	
	int start = clock();
  while (((clock() - start) / CLOCKS_PER_SEC) < seconds);
}