/* Copyright (c) 2003-2005
   Forschungszentrum Juelich GmbH, Federal Republic of Germany.

   All rights reserved. Redistribution and use in source and binary forms,
   with or without modification, are permitted provided that the following
   conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
 
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

   * Neither the name of Forschungszentrum Juelich GmbH nor the names of
     its contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY FORSCHUNGSZENTRUM JUELICH GMBH "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL FORSCHUNGSZENTRUM JUELICH GMBH BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  */

/*
 * This file is part of the VarArray library, version 1.0.
 *
 * Authors:
 *  Guenter Egerer, Annika Schiller, Forschungszentrum Juelich
 * Bugs:
 *  Please report any bugs to `G.Egerer@fz-juelich.de'.
 */


/*************************************************************************/
/*                                                                       */
/* Name: Annika Schiller       Telefon: 3439         Institut: ZAM       */
/* Datum  : 13.12.2004                                                   */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* Modul  : list                                                         */
/* Aufgabe: enthält die Funktionen zur Verwaltung der Liste              */
/* Definierte Funktionen:                                                */
/*        - search()                                                     */
/*        - insert()                                                     */
/*        - delete()                                                     */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

/*** #undef insert ***/

typedef struct List List;

struct List {
  void *key_start;
  void *key_end;
  size_t offset;
  datapointer dates;
  List *next;
};


static List *first=NULL;

#ifndef LINKAGE
#define LINKAGE
#endif
/* --------------------------------------------------------------------- */
/* Funktionsdefinitionen                                                 */
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* Funktion search()                                                     */
/*    sucht nach dem Listenelement, das den Schlüssel key enthält        */
/* Parameter: Zeiger auf ein Feld                                        */
/* Rückgabe : Zeiger auf die gespeicherten Daten des Listenelementes     */
/*            NULL, falls key nicht gefunden wurde                       */
/* --------------------------------------------------------------------- */
LINKAGE datapointer search( const void *key )
{
  List *current;

  for (current = first; current; current = current->next)
  {
    if ( (key >= current->key_start) && (key <= current->key_end) )
    {
      if ( ! (((char *)key - (char *)current->key_start) % current->offset) )
      {
        return current->dates;
      }
      else
        break;
    }
  }

  return NULL;
}

/* --------------------------------------------------------------------- */
/* Funktion insert()                                                     */
/*    fügt ein Element sortiert in die Liste ein                         */
/* Parameter: Zeiger auf Element, Anfangszeiger, Endzeiger, Offset       */
/* Rückgabe : keine                                                      */
/* --------------------------------------------------------------------- */
LINKAGE void (insert)(datapointer element, void *start, void *end, size_t offs)
{
  List *add, *current, *last;
 
#ifdef DEBUG_LIST
  printf("start: %p\n ",start);
  printf("ende: %p\n ",end);
  printf("offs: %lu \n",(unsigned long) offs);
#endif   /* DEBUG_LIST */
  /* überprüfen, ob der Schlüssel bereits existiert */
  assert( search(start) == NULL );
  assert( search(end) == NULL );

  /* anlegen eines neuen Listenelementes */
  add = (List *) malloc(sizeof(List));
  assert( add != NULL );

  /* Initialisierung mit den übergebenen Werten */
  add->key_start = start;
  add->key_end = end;
  add->offset = offs;
  add->dates = element;

  /* sortiertes Einfügen in die Liste */
  last = NULL;
  for (current = first; current; current = current->next)
  {
    if ( start < current->key_start )
      break;

    last = current;
  }

  /* an der richtigen Stelle einfügen */
  add->next = current;
  if ( last ) 
    last->next = add;
  else
    first = add;

  return;
}

/* --------------------------------------------------------------------- */
/* Funktion delete()                                                     */
/*    löscht ein Element aus der Liste                                   */
/* Parameter: Zeiger                                                     */
/* Rückgabe : keine                                                      */
/* --------------------------------------------------------------------- */
LINKAGE void delete( void *key )
{
  List *current, *p;

  for (current = first; current; current = current->next)
  {
    if ( key == current->key_start)
    {
      if ( current == first ) 
	first = current->next;
      else
	p->next = current->next;
      free((void *) current->dates);
      free(current);
      break;
    }
   
    p = current;
  }

  assert (current != NULL);  /* Annahme: zu löschendes Element wurde gefunden */
}


#ifdef DEBUG_LIST
LINKAGE void print ( void )
{
  List* current;

  for (current = first; current; current = current->next)
  {
    printf("start: %p  \n", current->key_start);
  }

  return;
}
#endif   /* DEBUG_LIST */
