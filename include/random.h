#ifndef RANDOM_H
#define RANDOM_H


int random_index(int size);
#define random_array(array) array[random_index(sizeof(array) / sizeof(*array))]


#endif // RANDOM_H
