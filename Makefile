
# This is a template Makefile generated by rpcgen

# Parameters

CLIENT = cloudfs_lock_service_client
SERVER = cloudfs_lock_service_server

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = cloudfs_lock_service_svc.h
SOURCES.x = cloudfs_lock_service.x

TARGETS_SVC.c = cloudfs_lock_service_svc.c cloudfs_lock_service_server.c cloudfs_lock_service_xdr.c hashmap.c
TARGETS_CLNT.c = cloudfs_lock_service_clnt.c cloudfs_lock_service_client.c cloudfs_lock_service_xdr.c 
TARGETS = cloudfs_lock_service.h cloudfs_lock_service_xdr.c cloudfs_lock_service_clnt.c cloudfs_lock_service_svc.c cloudfs_lock_service_client.c cloudfs_lock_service_server.c hashmap.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)
# Compiler flags 

CPPFLAGS += -D_REENTRANT
CFLAGS += -g 
LDLIBS += -lnsl -lpthread -lm
RPCGENFLAGS = 

# Targets 

all : $(CLIENT) $(SERVER)

$(TARGETS) : $(SOURCES.x) 
	rpcgen $(RPCGENFLAGS) $(SOURCES.x)

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c) 

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c) 

$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.c) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS) 

$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.c) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

 clean:
	 $(RM) *.o cloudfs_lock_service_client cloudfs_lock_service_server
