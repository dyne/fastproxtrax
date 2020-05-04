// Written by @exebook

// This is free and unencumbered software released into the public domain.

#ifndef HASHDICTC
#define HASHDICTC
#include <stdlib.h> /* malloc/calloc */
#include <stdint.h> /* uint32_t */
#include <string.h> /* memcpy/memcmp */

typedef int (*enumFunc)(void *key, int count, uint32_t value, void *user);

#define HASHDICT_VALUE_TYPE uint32_t
#define KEY_LENGTH_TYPE uint8_t

struct keynode {
	struct keynode *next;
	char *key;
	KEY_LENGTH_TYPE len;
	HASHDICT_VALUE_TYPE value;
};
		
struct dictionary {
	struct keynode **table;
	int length, count;
	double growth_treshold;
	double growth_factor;
	HASHDICT_VALUE_TYPE *value;
};

/* See hashdict.md */

struct dictionary* dic_new(int initial_size);
void dic_delete(struct dictionary* dic);
int dic_add(struct dictionary* dic, void *key, int keyn);
int dic_find(struct dictionary* dic, void *key, int keyn);
void dic_forEach(struct dictionary* dic, enumFunc f, void *user);
#endif
