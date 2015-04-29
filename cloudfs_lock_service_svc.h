#include "cloudfs_lock_service.h"
#include "hashmap.h"
#include <pthread.h>

extern hashmap *hash_map;
extern pthread_mutex_t mutex;
extern status acquire_result_cache[5][5000]; // TODO: change to more general way, probably an array with list or 2D list
extern status release_result_cache[5][5000];
extern int acquire_repeat_connect[5][5000];
extern int release_repeat_connect[5][5000];
