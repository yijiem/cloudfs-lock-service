/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "cloudfs_lock_service.h"


void
lockserviceprog_1(char *host)
{
	CLIENT *clnt;
	enum clnt_stat retval_1;
	status result_1;
	lock_params lock_arg;
	enum clnt_stat retval_2;
	status result_2;
	lock_params release_1_arg;

	lock_arg.key = calloc(1, 20);

#ifndef	DEBUG
	clnt = clnt_create (host, LOCKSERVICEPROG, LOCKSERVICEVERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	strcpy(lock_arg.key, "goliah online");
        lock_arg.operation = READ;

	retval_1 = acquire_1(&lock_arg, &result_1, clnt);
	if (retval_1 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_2 = release_1(&lock_arg, &result_2, clnt);
	if (retval_2 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
	free(lock_arg.key);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	lockserviceprog_1 (host);
exit (0);
}