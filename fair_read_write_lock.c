#include <stdio.h>
#include <pthread.h>
#include "fair_read_write_lock.h"

void atomic_array_init(struct atomic_array *aa) {
	pthread_mutex_init(&aa->mutex, NULL);
}

int atomic_array_get(struct atomic_array *aa, int id) {
	int res;

	pthread_mutex_lock(&aa->mutex);
	res = aa->arr[id];
	pthread_mutex_unlock(&aa->mutex);
	return res;
}

void atomic_array_set(struct atomic_array *aa, int id, int state) {
	aa->arr[id] = state;
}

void atomic_int_init(struct atomic_int *ai) {
	pthread_mutex_init(&ai->mutex, NULL);
}

int atomic_int_compare_and_set(struct atomic_int *ai, int prev_state, int new_state) {
	int res;

	pthread_mutex_lock(&ai->mutex);
	if (ai->value == prev_state) {
		ai->value = new_state;
		res = 1;
	} else {
		res = 0;
	}
	pthread_mutex_unlock(&ai->mutex);

	return res;
}

extern int tid_cache[5][5000]; // this is extern, store tid ring

void read_lock(struct atomic_array *readers_states, struct atomic_int *writer_state, int version, int machine, int tid_ring) {
	int local_tid;

	local_tid = tid_ring;
	tid_cache[machine][version] = local_tid;
	atomic_array_set(readers_states, local_tid, RSTATE_PREP);
	if (writer_state->value > WSTATE_UNUSED) {
		while (writer_state->value > WSTATE_UNUSED) {
			atomic_array_set(readers_states, local_tid, RSTATE_WAITING);
			while (writer_state->value > WSTATE_UNUSED) pthread_yield();
			atomic_array_set(readers_states, local_tid, RSTATE_PREP);
		}
	}
	atomic_array_set(readers_states, local_tid, RSTATE_READING);
}

void read_unlock(struct atomic_array *readers_states, int version, int machine) {
	int local_tid;

	local_tid = tid_cache[machine][version];
	if (atomic_array_get(readers_states, local_tid) != RSTATE_READING) {
		// ignore or error
	}
	atomic_array_set(readers_states, local_tid, RSTATE_UNUSED);
}

void write_lock(struct atomic_array *readers_states, struct atomic_int *writer_state, int version, int machine, int tid_ring) {
	int local_tid;
	int wait_for_readers[MAX_NUM_THREADS];
	int num_wait_readers;
	int i;
	int has_enter = 0;

	num_wait_readers = 0;
	local_tid = tid_ring;
	tid_cache[machine][version] = local_tid;
	while (!atomic_int_compare_and_set(writer_state, WSTATE_UNUSED, WSTATE_WRITEORWAIT)) {
		if (!has_enter) {
			printf("machine:%d version:%d waiting...\n", machine, version);
			has_enter = 1;
		}
		pthread_yield();
	}
	printf("machine:%d version:%d hostage has been rescued...\n", machine, version);
/*
	for (i = 0; i < MAX_NUM_THREADS; i++) {
		if (atomic_array_get(readers_states, i) > RSTATE_WAITING) {
			pthread_yield();
		}
	}
*/
}

void write_unlock(struct atomic_int *writer_state) {
	if (writer_state->value == WSTATE_UNUSED) {
		// ignore or error
	}

	writer_state->value = WSTATE_UNUSED;
}
