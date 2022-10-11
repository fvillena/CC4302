#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER, WAIT_ACCEDER_TIMEOUT y
// WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

// Defina aca sus variables globales.
// Para la cola de esperade nCompartir prefiera el tipo Queue.

static int readers;
static Queue *waiting_readers;
static nThread sharer;

// nth_compartirInit se invoca al partir nThreads para que Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  // ...
  readers = 0;
  sharer = NULL;
  waiting_readers = makeQueue();
}

void nCompartir(void *ptr) {
  // ...
  START_CRITICAL
  nThread thisTh = nSelf();
  thisTh->ptr = ptr;
  sharer = thisTh;
  int l = queueLength(waiting_readers);
  if (l > 0) {
    while (!emptyQueue(waiting_readers)) {
      nThread th = get(waiting_readers);
      if (th->status == WAIT_ACCEDER_TIMEOUT) {
        nth_cancelThread(th);
      }
      setReady(th);
    }
  }
  suspend(WAIT_COMPARTIR);
  schedule();
  END_CRITICAL
}

static void f(nThread th) {
  // programe aca la funcion que usa nth_queryThread para consultar si
  // th esta en la cola de espera de nCompartir.  Si esta presente
  // eliminela con nth_delQueue.
  // Ver funciones en nKernel/nthread-impl.h y nKernel/pss.h
  if (nth_queryThread((NthQueue *)waiting_readers, th)) {
    nth_delQueue((NthQueue *)waiting_readers, th);
  }
}

void *nAcceder(int max_millis) {
  // ...
  START_CRITICAL
  nThread thisTh = nSelf();
  readers++;
  if (sharer == NULL) {
    put(waiting_readers, thisTh);
    if (max_millis < 0) {
      suspend(WAIT_ACCEDER);
    } else {
      suspend(WAIT_ACCEDER_TIMEOUT);
      nth_programTimer(max_millis * 1000000LL, &f);
    }
    schedule();
  }
  void *data = sharer->ptr;
  END_CRITICAL
  return data;
}

void nDevolver(void) {
  // ...
  START_CRITICAL
  readers--;
  if (readers == 0) {
    setReady(sharer);
    sharer = NULL;
  }
  END_CRITICAL
}
