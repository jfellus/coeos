#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*------------------------------------------------------------------------------------*/
/* affichage des message de debug / desactive si pas de DEBUG demande a la compilation*/

static inline void debug_printf(const char *fmt, ...)
{
#ifdef DEBUG  /* comme c'est un inline il ne devrait rien rester dans le code si on a pas DEBUG */
  va_list ap;
  int ret;

  va_start(ap, fmt);
  ret = vprintf(fmt, ap); 
  va_end(ap);
#endif
}

#endif
