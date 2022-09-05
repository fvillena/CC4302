#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "bolsa.h"

// Declare aca sus variables globales
int current_price = 0;
char *current_seller;
char *current_buyer;
int *state;
int processing_transaction = 0;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int vendo(int precio, char *vendedor, char *comprador)
{
    pthread_mutex_lock(&m);
    int local_state = 0;
    state = &local_state;
    if (precio < current_price || current_price == 0)
    {
#if 0
        printf("BEST: %s = %d (state %d)\n", vendedor, precio, local_state);
#endif
        *state = -1;
        local_state = 0;
        current_price = precio;
        current_seller = vendedor;
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
#if 1
            printf("SELL: %s -> %s = %d (state %d)\n", vendedor, current_buyer, precio, local_state);
#endif
            strcpy(comprador, current_buyer);
            current_buyer = NULL;
            current_price = 0;
            current_seller = NULL;
            processing_transaction = 0;
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
    while (processing_transaction)
    {
        pthread_cond_wait(&c, &m);
    }
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
#if 0
        printf("BUY: %s -> %s = %d (pt %d)\n", comprador, vendedor, buying_price, processing_transaction);
#endif
        processing_transaction = 1;
        *state = 1;
        current_buyer = comprador;
        pthread_cond_broadcast(&c);
        pthread_mutex_unlock(&m);
        return buying_price;
    }
}