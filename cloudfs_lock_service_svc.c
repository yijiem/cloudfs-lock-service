/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "cloudfs_lock_service_svc.h"
#include "cloudfs_lock_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <libintl.h>
#include <sys/poll.h>
#include <rpc/rpc.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

#define DEBUG 0

void stop_handler();
void init();
void destroy();

struct data_str
{
        struct svc_req *rqstp;
        SVCXPRT *transp;
};

// key: char *, value: pthread_rwlock_t
hashmap *hash_map;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t pthread;
pthread_attr_t attr;
status acquire_result_cache[5][5000];
status release_result_cache[5][5000];
int acquire_repeat_connect[5][5000];
int release_repeat_connect[5][5000];
int tid_cache[5][5000];

/*
void
svc_run (void)
{
  int i;
  struct pollfd *my_pollfd = NULL;
  int last_max_pollfd = 0;
  int free_flag = 0;

  for (;;)
    {
      int max_pollfd = svc_max_pollfd;
      if (max_pollfd == 0 && svc_pollfd == NULL)
	break;

      if (last_max_pollfd != max_pollfd)
	{
	  struct pollfd *new_pollfd
	    = realloc (my_pollfd, sizeof (struct pollfd) * max_pollfd);
	  free_flag = 1;

	  if (new_pollfd == NULL)
	    {
	      perror ("svc_run: - out of memory");
	      break;
	    }

	  my_pollfd = new_pollfd;
	  last_max_pollfd = max_pollfd;
	}

      for (i = 0; i < max_pollfd; ++i)
	{
	  my_pollfd[i].fd = svc_pollfd[i].fd;
	  my_pollfd[i].events = svc_pollfd[i].events;
	  my_pollfd[i].revents = 0;
	}

      switch (i = __poll (my_pollfd, max_pollfd, -1))
	{
	case -1:
	  if (errno == EINTR)
	    continue;
	  perror ("svc_run: - poll failed");
	  break;
	case 0:
	  continue;
	default:
	  // INTUSE(svc_getreq_poll) (my_pollfd, i);
	  svc_getreq_poll(my_pollfd, i);
	  continue;
	}
      break;
    }

  printf("free? free_flag=%d\n", free_flag);
  if (free_flag) free(my_pollfd);
}
*/

void stop_handler() {
        printf("rpc server will shutdown....\n");
        destroy();
        exit(1);
}

void init() {
        int i,j;

        signal(SIGINT, stop_handler);
        hash_map = hashmapCreate(0); // use default start size
        for (i = 0; i < 5; i++) {
                for (j = 0; j < 5000; j++) {
                        acquire_result_cache[i][j] = UNALLOC;
                        release_result_cache[i][j] = UNALLOC;
                }
        }
}

void destroy() {
        int i;

        for (i = 0; i < hash_map->size; i++) {
                if (hash_map->table[i].flags & ACTIVE) {
                        lock_item *lm = (lock_item *) hash_map->table[i].data;
                        pthread_mutex_destroy(&lm->mutex);
                        pthread_cond_destroy(&lm->cond);
                        free(lm);
                }
        }

        hashmapDelete(hash_map);
}



void *
serv_request(void *data)
{
	if (DEBUG) printf("enter serv_request\n");
        long tid;
        tid = syscall(SYS_gettid);

        struct data_str *ptr_data = (struct data_str *) data;
        struct svc_req *rqstp = ptr_data->rqstp;
        SVCXPRT *transp = ptr_data->transp;

	if (DEBUG) printf("1\n");

	union {
		lock_params acquire_1_arg;
		lock_params release_1_arg;
	} argument;
	union {
		status acquire_1_res;
		status release_1_res;
	} result;
	bool_t retval;
	xdrproc_t _xdr_argument, _xdr_result;
	bool_t (*local)(char *, void *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		free(data);
		return;

	case ACQUIRE:
		_xdr_argument = (xdrproc_t) xdr_lock_params;
		_xdr_result = (xdrproc_t) xdr_status;
		local = (bool_t (*) (char *, void *,  struct svc_req *))acquire_1_svc;
		break;

	case RELEASE:
		_xdr_argument = (xdrproc_t) xdr_lock_params;
		_xdr_result = (xdrproc_t) xdr_status;
		local = (bool_t (*) (char *, void *,  struct svc_req *))release_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		free(data);
		return;
	}
	if (DEBUG) printf("2\n");
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		free(data);
		return;
	}
	if (DEBUG) printf("3\n");
	retval = (bool_t) (*local)((char *)&argument, (void *)&result, rqstp);
	if (DEBUG) printf("4\n");
	if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
		if (DEBUG) printf("you shall not pass\n");
		svcerr_systemerr (transp);
	}
	if (DEBUG) printf("5\n");
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		free(data);
		return;
		// exit (1);
	}
	if (DEBUG) printf("6\n");
	if (!lockserviceprog_1_freeresult (transp, _xdr_result, (caddr_t) &result))
		fprintf (stderr, "%s", "unable to free results");
	if (DEBUG) printf("7\n");
	free(data);
	if (DEBUG) printf("exit serv_request\n");
	return;
}

static void
lockserviceprog_1(struct svc_req *rqstp, SVCXPRT *transp)
{
        struct data_str *data_ptr = (struct data_str*) malloc(sizeof(struct data_str));
        data_ptr->rqstp = rqstp;
        data_ptr->transp = transp;
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
        pthread_create(&pthread,&attr,serv_request,(void *)data_ptr);
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (LOCKSERVICEPROG, LOCKSERVICEVERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, LOCKSERVICEPROG, LOCKSERVICEVERS, lockserviceprog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (LOCKSERVICEPROG, LOCKSERVICEVERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, LOCKSERVICEPROG, LOCKSERVICEVERS, lockserviceprog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (LOCKSERVICEPROG, LOCKSERVICEVERS, tcp).");
		exit(1);
	}

	init();

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
