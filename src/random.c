#include "random.h"

#include <stdlib.h>


int random_index(int size) {
	return rand() % size;
}
