#include "disk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "priqueue.h"
#include "spinlocks.h"

// Le sera de ayuda la clase sobre semáforos:
// https://www.u-cursos.cl/ingenieria/2022/2/CC4302/1/novedades/detalle?id=431689
// Le serviran la solucion del productor/consumidor resuelto con el patron
// request y la solucion de los lectores/escritores, tambien resuelto con
// el patron request.  Puede substituir los semaforos de esas soluciones
// por spin-locks, porque esos semaforos almacenan a lo mas una sola ficha.

// Declare los tipos que necesite
// ...
typedef struct {
  int track;
  int w;
} Request;

// Declare aca las variables globales que necesite
// ...
int sl;
int last_track;
int busy;
PriQueue *meq_last_track;
PriQueue *lt_last_track;

// Agregue aca las funciones requestDisk y releaseDisk

void iniDisk(void) {
  // ...
  sl = OPEN;
  last_track = 0;
  busy = 0;
  meq_last_track = makePriQueue();
  lt_last_track = makePriQueue();
}

void requestDisk(int track) {
  // ...
  spinLock(&sl);
  if (!busy) {
    last_track = track;
    busy = 1;
  } else {
    Request req = {track, CLOSED};
    if (track >= last_track) {
      priPut(meq_last_track, &req, track);
    } else {
      priPut(lt_last_track, &req, track);
    }
    spinUnlock(&sl);
    spinLock(&req.w);
    return;
  }
  spinUnlock(&sl);
  return;
}

void move(PriQueue *a, PriQueue *b) {
  while (!emptyPriQueue(a)) {
    Request *req = priGet(a);
    priPut(b, req, req->track);
  }
}

void releaseDisk() {
  // ...
  spinLock(&sl);
  if (!emptyPriQueue(meq_last_track)) {
    Request *req = priGet(meq_last_track);
    last_track = req->track;
    spinUnlock(&req->w);
  } else if (!emptyPriQueue(lt_last_track)) {
    Request *req = priGet(lt_last_track);
    last_track = req->track;
    move(lt_last_track, meq_last_track);
    spinUnlock(&req->w);
  } else {
    busy = 0;
  }
  spinUnlock(&sl);
}
