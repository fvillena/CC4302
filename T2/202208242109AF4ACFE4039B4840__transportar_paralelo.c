#define P 8
#define R 100 
#define TRUE 1
#define FALSE 0
//R es el número máximo de solicitudes pendientes

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
Camion* camiones[P];        // arreglo que contiene todos los camiones
Ciudad* ubic[P];    //ubicación actual de los camiones. Inicialmente todos en Stgo
int ocupados[P];    //pos k es TRUE si el camion k-esimo está ocupado

typedef struct {
    int ready;
    int c_idx;
    Ciudad *orig;
    pthread_cond_t w;
} Request;

Requests *requests[R]; //inicialmente todos los valores son NULL

int buscar(Ciudad *orig) {
    int c_idx = -1;
    // hay camiones desocupados, elegimos el más cercano
    for (int k=0; i <P; i++) {
        if (!ocupados[k])
            if (c_idx == -1 || distancia(ubic[k], orig) < distancia(ubic[c_idx], orig))
                c_idx = k;            
    }  
    // no hay camiones libres, esperar (patrón requests)
    if (c_idx < 0) {
        Request req = {FALSE, -1, orig, PTHREAD_COND_INITIALIZER};
        
        for (int i=0; i<R; i++) {
            if (requests[i] == NULL) {
                requests[i] = &req;
                break;
            }
        }
        
        while (!req.ready)
            pthread_cond_wait(&req.w, &m);
            
        c_idx = req.c_idx;         
    }
    
    return c_idx;
}

void desocupar(int c_idx) {
    //si hay solicitudes pendientes, elegir el contenedor más cercano
    int r_idx = -1;
    for (int i=0; i<R; i++) {
        if (requests[i] != NULL) 
            if (r_idx == -1 || 
                distancia(ubic[c_idx], requests[i]->orig) < distancia(ubic[c_idx], requests[r_idx]->orig))
                r_idx = i;
    }
    
    if (r_idx < 0) 
        ocupados[c_idx] = FALSE;
    else {
        request[r_idx]->ready = TRUE;
        request[r_idx]->c_idx = c_idx;
        pthread_cond_signal(&request[r_idx]->w);
        requests[r_idx] = NULL;
    } 
}

void transportar(Contenedor *cont, Ciudad *orig, Ciudad *dest) {
    pthread_mutex_lock(&m);
    
    int c_idx = buscar(orig); //el índice del camión seleccionado
    
    Camion *c = camiones[c_idx];
    ocupados[c_idx] = TRUE;
    pthread_mutex_unlock(&m);
    
    conducir(c, ubic[c_idx], orig);
    cargar(c, cont);
    conducir(c, orig, dest);
    descargar(c, cont);
    
    pthread_mutex_lock(&m);
    ubic[c_idx] = dest;
    desocupar(c_idx);
    pthread_mutex_unlock(&m);
}