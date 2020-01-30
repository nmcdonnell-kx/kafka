#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <librdkafka/rdkafka.h>
#include "k.h"

// Required macro definitions for C scripts
#define K3(f) K f(K x,K y,K z)
#define K4(f) K f(K x,K y,K z,K r)
#define KR -128
#define KNL (K) 0

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "librdkafka.lib")
#define EXP __declspec(dllexport)
static SOCKET spair[2];
#else
#include <unistd.h>
#define EXP
#define SOCKET_ERROR -1
static int spair[2];
#endif

#define KFK_OK RD_KAFKA_RESP_ERR_NO_ERROR
#ifdef __GNUC__
#  define UNUSED(x) x __attribute__((__unused__))
#else
#  define UNUSED(x) x
#endif

typedef unsigned int UI;
static K S0;
static K clients, topics;
static int validinit;

I indexClient(const rd_kafka_t *rk);

// C scripts 
#include "src/socketpair.c"
#include "src/q_utils.c"
#include "src/decode.c"
#include "src/clients.c"
#include "src/topic.c"
#include "src/kfk_utils.c"
#include "src/consumer.c"
#include "src/producer.c"
#include "src/offsets.c"

// Define the actions to be taken on closing of the process
// Deleting topics & clients, closing of socket pairs
static V detach(V){
  I sp,i;
  if(topics){
    for(i= 0; i < topics->n; i++)
      kfkTopicDel(ki(i));
    r0(topics);
  }
  if(clients){
    for(i= 0; i < clients->n; i++)
      kfkClientDel(ki(i));
    rd_kafka_wait_destroyed(1000); /* wait for cleanup*/
    r0(clients);
  }
  if(sp=spair[0]){
    sd0x(sp,0);
    close(sp);
  }
  if(sp=spair[1])
    close(sp); 
  spair[0]= 0;
  spair[1]= 0;
  validinit = 0;
}


// Initialization function
// Initialize client/topic arrays, open socketpair to kafka main thread
EXP K kfkInit(K UNUSED(x)){
  if(!(0==validinit))
   return 0; 
  clients=ktn(KS,0);
  topics=ktn(KS,0);
  S0=ks("");
  if(dumb_socketpair(spair, 1) == SOCKET_ERROR)
    fprintf(stderr, "Init failed, creating socketpair: %s\n", strerror(errno));
  K r=sd1(-spair[0], &kfkCallback);
  if(r==0){
    fprintf(stderr, "Init failed, adding callback\n");
    spair[0]=0;
    spair[1]=0;
    return 0;
  }
  r0(r);
  validinit=1;
  atexit(detach);
  return 0;
}
