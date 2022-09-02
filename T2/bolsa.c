#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "bolsa.h"

// Declare aca sus variables globales
int current_price = 0;
char *current_seller;
char *current_buyer;
int available_buyer = 0;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int vendo(int precio, char *vendedor, char *comprador)
{
  pthread_mutex_lock(&m);
  if (precio >= current_price && (current_price != 0))
  {
    # if 0
      printf("El precio es mayor al mejor precio\n");
    #endif
    pthread_mutex_unlock(&m);
    return 0;
  }
  else
  {
    #if 1
      printf("El mejor vendedor es %s y vende a %d\n", vendedor, precio);
    #endif
    current_price = precio;
    current_seller = vendedor;
    pthread_cond_broadcast(&c);
    while (precio <= current_price && !available_buyer)
    {
      pthread_cond_wait(&c, &m);
    }
    if (available_buyer)
    {
      #if 1
        printf("%s vende a %d\n", vendedor, precio);
      #endif
      strcpy(comprador, current_buyer);
      available_buyer = 0;
      current_buyer = NULL;
      current_price = 0;
      current_seller = NULL;
      pthread_mutex_unlock(&m);
      return 1;
    }
    else
    {
      #if 1
        printf("%s muere porque ahora %s vende a %d\n", vendedor, current_seller, current_price);
      #endif
      pthread_mutex_unlock(&m);
      return 0;
    }
  }
}

int compro(char *comprador, char *vendedor)
{
  pthread_mutex_lock(&m);
  if (current_price == 0)
  {
    pthread_mutex_unlock(&m);
    #if 1
      printf("Nadie vende\n");
    #endif
    return 0;
  }
  // else if (available_buyer == 1)
  // {
  //   return 0;
  // }
  else
  {
    available_buyer = 1;
    current_buyer = comprador;
    strcpy(vendedor, current_seller);
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&m);
    #if 1
      printf("Yo, %s le compro a %s a %d\n", comprador, vendedor, current_price);
    #endif
    return current_price;
  }
}
