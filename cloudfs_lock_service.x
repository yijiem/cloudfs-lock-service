const MAXNAMELEN = 255;
typedef string keyname<MAXNAMELEN>;
enum status {ERROR, OK, UNALLOC};
enum type {UNDEFINED, READ, WRITE, RELEASE};
struct identity {
	int id_arr[2];
};
struct lock_params {
	keyname key;
	type operation;
	identity id;
};

program LOCKSERVICEPROG {
	version LOCKSERVICEVERS {
		status ACQUIRE(lock_params) = 1;
		status RELEASE(lock_params) = 2;
	} = 1;
} = 0x20000002;
