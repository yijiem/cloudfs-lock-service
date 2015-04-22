const MAXNAMELEN = 255;
typedef string keyname<MAXNAMELEN>;
enum status {OK, ERROR};
enum type {READ, WRITE};
struct lock_params {
	keyname key;
	type operation;
};
typedef struct lock_params *lock_params;

program LOCKSERVICEPROG {
	version LOCKSERVICEVERS {
		status ACQUIRE(lock_params) = 1;
		status RELEASE(lock_params) = 2;
	} = 1;
} = 0x20000002;
