#define MAX_NUM_THREADS 256

#define RSTATE_UNUSED 0
#define RSTATE_WAITING 1
#define RSTATE_PREP 2
#define RSTATE_READING 3

#define WSTATE_UNUSED 0
#define WSTATE_WRITEORWAIT 1

struct atomic_array {
        pthread_mutex_t mutex;
        int arr[MAX_NUM_THREADS];
};

extern void atomic_array_init(struct atomic_array *aa);
extern int atomic_array_get(struct atomic_array *aa, int id);
extern void atomic_array_set(struct atomic_array *aa, int id, int state);

struct atomic_int {
        pthread_mutex_t mutex;
        int value;
};

extern void atomic_int_init(struct atomic_int *ai);
extern int atomic_int_compare_and_set(struct atomic_int *ai, int prev_state, int new_state);

extern void read_lock(struct atomic_array *readers_states, struct atomic_int *writer_state, int version, int machine, int tid_ring);
extern void read_unlock(struct atomic_array *readers_states, int version, int machine);
extern void write_lock(struct atomic_array *readers_states, struct atomic_int *writer_state, int version, int machine, int tid_ring);
extern void write_unlock(struct atomic_int *writer_state);
