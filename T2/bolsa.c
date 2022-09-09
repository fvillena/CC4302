#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "bolsa.h"

int current_price = 0;
char *current_seller;
int *state = NULL;
char *buyer;
int processing_transaction = 0;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int vendo(int precio, char *vendedor, char *comprador)
{
    pthread_mutex_lock(&m);
    if (precio < current_price || current_price == 0)
    {
        if (state != NULL)
        {
            *state = -1;
        }
        int local_state = 0;
#if 0
        printf("BEST: %s = %d (state %d)\n", vendedor, precio, local_state);
#endif
        buyer = comprador;
        current_price = precio;
        current_seller = vendedor;
        state = &local_state;
        pthread_cond_broadcast(&c);
        while (local_state == 0)
        {
            pthread_cond_wait(&c, &m);
        }
        if (local_state == -1)
        {
#if 0
            printf("REJECTED: %s -> %s = %d (state %d)\n", vendedor, current_seller, current_price, local_state);
#endif
            pthread_mutex_unlock(&m);
            return 0;
        }
        else
        {
#if 0
            printf("SELL: %s -> %s = %d (state %d)\n", vendedor, comprador, precio, local_state);
#endif
            pthread_cond_broadcast(&c);
            pthread_mutex_unlock(&m);
            return 1;
        }
    }
    else
    {
#if 0
        printf("WRONG: %s -> %d > %d\n", vendedor, precio, current_price);
#endif
        pthread_mutex_unlock(&m);
        return 0;
    }
}

int compro(char *comprador, char *vendedor)
{
    pthread_mutex_lock(&m);
    if (current_price == 0)
    {
#if 0
        printf("WRONG: %s -> Nadie vende\n", comprador);
#endif
        pthread_mutex_unlock(&m);
        return 0;
    }
    else
    {
        int buying_price = current_price;
        strcpy(vendedor, current_seller);
        strcpy(buyer, comprador);
#if 0
        printf("BUY: %s -> %s = %d (pt %d)\n", comprador, vendedor, buying_price, processing_transaction);
#endif
        *state = 1;
        state = NULL;
        current_price = 0;
        current_seller = NULL;
        pthread_cond_broadcast(&c);
        pthread_mutex_unlock(&m);
        
        return buying_price;
    }
}