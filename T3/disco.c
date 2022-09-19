#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "pss.h"
#include "disco.h"

typedef struct {
    char *woman;
    char *man;
    int ready;
    pthread_cond_t w;
} RequestToDance;

// Defina aca sus variables globales

#define TRUE 1
#define FALSE 0
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
Queue *waiting_men;
Queue *waiting_women;

void DiscoInit(void) {
  // ... inicialice aca las variables globales ...

  waiting_men = makeQueue();
  waiting_women = makeQueue();

}

void DiscoDestroy(void) {
  // ... destruya las colas para liberar la memoria requerida ...

  destroyQueue(waiting_men);
  destroyQueue(waiting_women);

}

char *dama(char *nom) {
  // ...

  pthread_mutex_lock(&m);
  if (queueLength(waiting_men) == 0)
  {
    RequestToDance req = {nom, NULL, FALSE, PTHREAD_COND_INITIALIZER};
    put(waiting_women, &req);
    while (!req.ready)
    {
      pthread_cond_wait(&req.w, &m);
    }
    pthread_mutex_unlock(&m);
    return req.man;
  }
  else
  {
    RequestToDance *req = get(waiting_men);
    req->woman = nom;
    req->ready = TRUE;
    char *varon = req->man;
    pthread_cond_signal(&req->w);
    pthread_mutex_unlock(&m);
    return varon;
  }

}

char *varon(char *nom) {
  // ...

  pthread_mutex_lock(&m);
  if (queueLength(waiting_women) == 0)
  {
    RequestToDance req = {NULL, nom, FALSE, PTHREAD_COND_INITIALIZER};
    put(waiting_men, &req);
    while (!req.ready)
    {
      pthread_cond_wait(&req.w, &m);
    }
    pthread_mutex_unlock(&m);
    return req.woman;
  }
  else
  {
    RequestToDance *req = get(waiting_women);
    req->man = nom;
    req->ready = TRUE;
    char *dama = req->woman;
    pthread_cond_signal(&req->w);
    pthread_mutex_unlock(&m);
    return dama;
  }

}
