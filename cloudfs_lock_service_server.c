/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "hashmap.h"
#include "cloudfs_lock_service.h"
#include "cloudfs_lock_service_svc.h"
#include <sys/syscall.h>
#include <sys/time.h>
#include <errno.h>


void print_log(long tid, type operation, long key, const char *msg) {
        long sec, ms;
        struct timeval time;
        char timestamp[50];

        memset(timestamp, 0, 50);
        gettimeofday(&time, NULL);
        sec = time.tv_sec;
        ms = time.tv_usec / 1000;
        sprintf(timestamp, "sec:%ld ms:%ld ", sec, ms);

        printf("%s tid:%ld----type:%s----key:%ld %s\n",
                        timestamp, tid,
                        operation == 0 ? "undefined" : (operation == 3 ? "release" : (operation == 1 ? "read" : "write")),
                        key, msg);
}



bool_t
acquire_1_svc(lock_params *argp, status *result, struct svc_req *rqstp)
{
        bool_t retval;
        keyname key_str;
        long key, tid;
        type operation;
        lock_item *lock;
        char start_msg[100];
        int machine, version;

        /*
         * insert server code here
         */
        memset(start_msg, 0, sizeof(start_msg));
        operation = argp->operation;
        tid = syscall(SYS_gettid);
        key_str = argp->key;
        machine = argp->id.id_arr[0];
        version = argp->id.id_arr[1];
        sprintf(start_msg, "Receive key:%s machine:%d version:%d in acquire", key_str, machine, version);
        print_log(tid, operation, 0L, start_msg);

        if (machine < 0 || machine >= 2 || version >= 5000 || version < 0) {
                // arg1.key = (char *) malloc(5); // give you sth to free
                *result = 0;
                return TRUE;
        }

        if (operation == UNDEFINED) {
                // arg1.key = (char *) malloc(5); // give you sth to free
                *result = 0;
                return TRUE;
        }

        if (!(operation == READ || operation == WRITE)) { // RPC goes to wrong function
                *result = 0;
                return TRUE;
        }

        if (key_str == NULL) {
                // arg1.key = (char *) malloc(5); // give you sth to free
                *result = 0;
                return TRUE;
        }

        if (acquire_result_cache[machine][version] != UNALLOC) {
                printf("hit!\n");
                *result = acquire_result_cache[machine][version];
                return TRUE;
        }

        acquire_result_cache[machine][version] = ERROR;

        key = hashFunction(key_str, hash_map);

        pthread_mutex_lock(&mutex); // lock here
        lock = (lock_item *) hashmapGet(hash_map, key);

        if (lock == NULL) {
                print_log(tid, operation, key, "lock is new, create a new one..");
                lock = malloc(sizeof(lock_item));
                lock->i = 0;
                pthread_mutex_init(&lock->mutex, NULL);
                pthread_cond_init(&lock->cond, NULL);
                hashmapInsert(hash_map, lock, key);
                pthread_mutex_unlock(&mutex); // unlock here

                if (operation == READ) {
                        // acquire_read(lock);
                        pthread_mutex_lock(&lock->mutex);

                        while (lock->i < 0) {
                                pthread_cond_wait(&lock->cond, &lock->mutex);
                        }
                        lock->i = lock->i + 1;
                        print_log(tid, operation, key, "grab read lock!");
                        *result = OK;
                        acquire_result_cache[machine][version] = OK;
                        lock->operation = operation; // this is the next free operation for this key

                        pthread_mutex_unlock(&lock->mutex);
                        return TRUE;
                } else if (operation == WRITE) {
                        // acquire_write(lock);
                        pthread_mutex_lock(&lock->mutex);

                        while (lock->i != 0) {
                                pthread_cond_wait(&lock->cond, &lock->mutex);
                        }
                        lock->i = lock->i - 1;
                        print_log(tid, operation, key, "grab write lock!");
                        *result = OK;
                        acquire_result_cache[machine][version] = OK;
                        lock->operation = operation; // this is the next free operation for this key

                        pthread_mutex_unlock(&lock->mutex);
                        return TRUE;
                }
        } else {
                print_log(tid, operation, key, "lock exist..");
                printf("lock->i==%d\n", lock->i);
                pthread_mutex_unlock(&mutex); // or unlock here
                if (operation == READ) {
                        // acquire_read(lock);
                        pthread_mutex_lock(&lock->mutex);

                        while (lock->i < 0) {
                                pthread_cond_wait(&lock->cond, &lock->mutex);
                        }
                        lock->i = lock->i + 1;
                        print_log(tid, operation, key, "grab read lock!");
                        *result = OK;
                        acquire_result_cache[machine][version] = OK; // i see live people
                        lock->operation = operation; // this is the next free operation for this key

                        pthread_mutex_unlock(&lock->mutex);
                        return TRUE;
                } else if (operation == WRITE) {
                        // acquire_write(lock);
                        pthread_mutex_lock(&lock->mutex);

                        while (lock->i != 0) {
                                pthread_cond_wait(&lock->cond, &lock->mutex);
                        }
                        lock->i = lock->i - 1;
                        print_log(tid, operation, key, "grab write lock!");
                        *result = OK;
                        acquire_result_cache[machine][version] = OK; // i see live people
                        lock->operation = operation; // this is the next free operation for this key

                        pthread_mutex_unlock(&lock->mutex);
                        return TRUE;

                }
                *result = OK;
        }

        return TRUE;
}


bool_t
release_1_svc(lock_params *argp, status *result, struct svc_req *rqstp)
{
        bool_t retval;
        keyname key_str;
        long key, tid;
        type operation;
        lock_item *lock;
        char start_msg[100];
        int machine, version;

        /*
         * insert server code here
         */
        memset(start_msg, 0, sizeof(start_msg));
        operation = argp->operation;
        tid = syscall(SYS_gettid);
        key_str = argp->key;
        machine = argp->id.id_arr[0];
        version = argp->id.id_arr[1];
        sprintf(start_msg, "Receive key:%s machine:%d version:%d in release", key_str, machine, version);
        print_log(tid, operation, 0L, start_msg);

        if (machine < 0 || machine >= 2 || version >= 5000 || version < 0) {
                // arg1.key = (char *) malloc(5); // give you sth to free
                *result = 0;
                return TRUE;
        }

        if (operation == UNDEFINED) {
                // arg1.key = (char *) malloc(5); // give you sth to free
                *result = 0;
                return TRUE;
        }

        if (key_str == NULL) {
                // arg1.key = (char *) malloc(5); // give you sth to free
                *result = 0;
                return TRUE;
        }

        if (release_result_cache[machine][version] != UNALLOC) {
                printf("hit!\n");
                *result = release_result_cache[machine][version];
                return TRUE;
        }

        if (acquire_result_cache[machine][version] == UNALLOC) { // nothing but skip
                printf("No acquire when release...May lose packet...skip!\n");
                *result = OK;
                return TRUE;
        }

        key = hashFunction(key_str, hash_map);
        tid = syscall(SYS_gettid);

        // pthread_mutex_lock(&mutex); // lock here
        lock = (lock_item *) hashmapGet(hash_map, key);
        // pthread_mutex_unlock(&mutex);

        if (lock == NULL) {
                // arg1.key = (char *) malloc(5); // give you sth to free
                *result = 0;
                return TRUE;
        }

        release_result_cache[machine][version] = ERROR;

        memset(start_msg, 0, sizeof(start_msg));
        if (lock->operation == READ && acquire_result_cache[machine][version] == ERROR) { // indicate acquire has been passed
                pthread_mutex_lock(&lock->mutex);

                pthread_cond_signal(&lock->cond);

                sprintf(start_msg, "<-----skip this version, release read key:%ld lock->i=%d----->", key, lock->i);
                print_log(tid, operation, key, start_msg);
                *result = OK;
                release_result_cache[machine][version] = OK; //  pass this version

                pthread_mutex_unlock(&lock->mutex);
                return TRUE;
        }

        if (lock->operation == READ) {
                // release_read(lock);
                pthread_mutex_lock(&lock->mutex);

                lock->i = lock->i - 1;
                if (lock->i == 0) {
                        pthread_cond_signal(&lock->cond);
                }
                sprintf(start_msg, "<-----release read key:%ld lock->i=%d----->", key, lock->i);
                print_log(tid, operation, key, start_msg);
                *result = OK;
                release_result_cache[machine][version] = OK;

                pthread_mutex_unlock(&lock->mutex);
                return TRUE;
        } else if (lock->operation == WRITE) {
                // release_write(lock);
                pthread_mutex_lock(&lock->mutex);

                lock->i = 0;
                pthread_cond_broadcast(&lock->cond);
                sprintf(start_msg, "<-----release write key:%ld lock->i=%d----->", key, lock->i);
                print_log(tid, operation, key, start_msg);
                *result = OK;
                release_result_cache[machine][version] = OK;

                pthread_mutex_unlock(&lock->mutex);
                return TRUE;
        }

        return TRUE;
}



int
lockserviceprog_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}
