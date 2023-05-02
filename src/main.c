#include <stdio.h>
#include "menu.c"
#include <unistd.h>
#include <pthread.h>

int main( int argc, char *argv[] )  {
   pthread_t th1;
   printf("Welcome to IP sniffer\n");
 
   if( argc == 1 ) {
      StartMenu(th1);
   }
}