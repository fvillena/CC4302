#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "bolsa.h"

// Declare aca sus variables globales
int current_price = 0;
char *current_seller;
char *current_buyer;
int processing_transaction = 0;
int sellers = 0;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int vendo(int precio, char *vendedor, char *comprador)
{
  pthread_mutex_lock(&m);
  while (processing_transaction)
  {
    pthread_cond_wait(&c, &m);
  }
  if (precio >= current_price && current_price != 0)
  {
#if 0
      printf("El precio es mayor al mejor precio\n");
#endif
    pthread_mutex_unlock(&m);
    return 0;
  }
  else
  {

#if 1
    printf("BEST: %s = %d\n", vendedor, precio);
#endif
    current_price = precio;
    current_seller = vendedor;
    pthread_cond_broadcast(&c);
    sellers++;
    while (precio <= current_price && !processing_transaction)
    {
      pthread_cond_wait(&c, &m);
    }
    if (precio > current_price)
    {
#if 1
      printf("REJECTED: %s -> %s = %d\n", vendedor, current_seller, current_price);
#endif
      sellers--;
#if 0
    if (sellers > 1)
    {
      printf("-> SELLERS = %d\n", sellers);
    }
#endif
      pthread_mutex_unlock(&m);
      return 0;
    }
    else
    {
#if 1
      printf("SELL: %s -> %s = %d\n", vendedor, current_buyer, precio);
#endif
      strcpy(comprador, current_buyer);
      processing_transaction = 0;
      current_buyer = NULL;
      current_price = 0;
      current_seller = NULL;
      pthread_cond_broadcast(&c);
      pthread_mutex_unlock(&m);
      return 1;
    }
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
    pthread_mutex_unlock(&m);
#if 0
    printf("Nadie vende\n");
#endif
    return 0;
  }
  else
  {
    processing_transaction = 1;
    current_buyer = comprador;
    strcpy(vendedor, current_seller);
    int buying_price = current_price;
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&m);
#if 1
    printf("BUY: %s -> %s = %d\n", comprador, vendedor, buying_price);
#endif
    return buying_price;
  }
}
