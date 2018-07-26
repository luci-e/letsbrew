



#include "globals.h"
#include <math.h>
#include <stdlib.h>

int unOptimize(void * what){
int *x = (int*) (what);
int ran = rand();
	if(ran * ran<0){
		if(*x == 1){
			return 1;
		}
		else{
			return 0;
		}
	}
	return 3;
}
