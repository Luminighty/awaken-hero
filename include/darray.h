#ifndef DARRAY_H
#define DARRAY_H


#define DATypedef(Type, Item) \
	typedef struct { \
		Item *items; \
		size_t count; \
		size_t capacity; \
	} Type


#define da_push(vec, value)\
	do {\
	if (vec.count >= vec.capacity) {\
		if (vec.capacity == 0) vec.capacity = 256;\
		else vec.capacity *= 2;\
		vec.items = realloc(vec.items, vec.capacity * sizeof(*vec.items));\
	}\
	vec.items[vec.count++] = value;\
	} while (0)


#define da_pop(vec) vec.items[--vec.count]


#endif // DARRAY_H
