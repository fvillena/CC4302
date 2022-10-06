#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...

static int readers;
static NthQueue *waiting_readers;
static nThread sharer;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  // ...
  readers = 0;
  sharer = NULL;
  waiting_readers = nth_makeQueue();
}

void nCompartir(void *ptr) {
  // ...
  START_CRITICAL
  nThread thisTh = nSelf();
  thisTh->ptr = ptr;
  sharer = thisTh;
  int l = nth_queueLength(waiting_readers);
  if (l > 0) {
    for (int i = 0; i < l; i++) {
      setReady(nth_getFront(waiting_readers));
    }
  }
  suspend(WAIT_ACCEDER);
  schedule();
  END_CRITICAL
}

void *nAcceder(int max_millis) {
  // ...
  START_CRITICAL
  nThread thisTh = nSelf();
  readers++;
  if (sharer == NULL) {
    nth_putBack(waiting_readers, thisTh);
    suspend(WAIT_COMPARTIR);
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
