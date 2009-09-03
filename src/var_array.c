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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*************************************************************************/
/*                                                                       */
/* Name: Annika Schiller       Telefon: 3439         Institut: ZAM       */
/* Datum  : 13.12.2004                                                   */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* Modul  : array                                                        */
/* Aufgabe: enthält Funktionen zum Anlegen, Löschen und Abfragen der     */
/*          der Dimensionslängen von mehrdimensionalen Felder            */
/* Definierte Funktionen:                                                */
/*        - vaCreate_2d                                                  */
/*        - vaDelete                                                     */
/*        - vaSize                                                       */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LINKAGE static
#include "list.h"
#include "list_c.h"
#include "var_array.h"

typedef struct data_t {
  int dim_anz;        /* Anzahl der Dimensionen                              */
  int dimlen[10];     /* Größe der einzelnen Dimensionen                     */
  int is_allocated;   /* Flag zum merken, ob Speicherplatz allociert wurde   */
} data_t;

/* --------------------------------------------------------------------- */
/* Funktionsdefinitionen                                                 */
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* Funktion vaCreate_1d()                                                */
/*    erzeugt ein mehrdimensionales Feld mit 'dim1' Elementen, die Spei- */
/*    chergröße eines Elementes wird festgelegt durch die übergebene     */
/*    Größe 'element_size'                                               */
/* Parameter: Dimensionslänge, element_size, Anfangspointer              */
/* Rückgabe : generischer Zeiger auf die erste Zeile des erzeugten       */
/*            Feldes                                                     */
/* --------------------------------------------------------------------- */
#undef vaCreate_1d

void *vaCreate_1d ( int dim1, size_t element_size, void *begin_)
{

  char *begin = (char *) begin_;
  data_t element;

  if ( begin == NULL )
  {
    begin = (char *) malloc( dim1 * element_size );
   /* Wir müssen uns merken, dass der Speicherplatz von uns angelegt wurde,
       um ihn bei vaDelete() auch wieder freigeben zu können                 */
    element.is_allocated = 1;
  }
  else
    element.is_allocated = 0;
  

  /* merken der Dimensionen des Vetors */
  element.dim_anz = 1;
  element.dimlen[0] = dim1;

  /* Eintragen des Zeigers auf den Vektor in die Liste */
  insert(element,begin,begin, element_size);

  return (void *) begin;

}

/* --------------------------------------------------------------------- */
/* Funktion vaCreate_2d()                                                */
/*    erzeugt ein mehrdimensionales Feld mit 'dim1' Zeilen und           */
/*    'dim2' Spalten, die Speichergröße eines Elementes wird fest-       */
/*    gelegt durch die übergebene Größe 'element_size'                   */
/* Parameter: Dimensionslängen, element_size, Anfangspointer             */
/* Rückgabe : generischer Zeiger auf die erste Zeile des erzeugten       */
/*            Feldes                                                     */
/* --------------------------------------------------------------------- */
#undef vaCreate_2d

void *vaCreate_2d ( int dim1, int dim2, size_t element_size, 
                        const void *begin_)
{

  const char **help;
  const char *begin = (const char *) begin_;
  data_t element;
  int is_allocated, i;

  if ( begin == NULL )
  {
    begin = (const char *) malloc( dim1 * dim2 * element_size );
   /* Wir müssen uns merken, dass der Speicherplatz von uns angelegt wurde,
       um ihn bei vaDelete() auch wieder freigeben zu können                 */
    is_allocated = 1;
  }
  else
    is_allocated = 0;
  

  help = (const char **) malloc( dim1 * sizeof(char *) );
  for ( i=0; i<dim1; i++ ) 
    help[i] = begin + (dim2 * element_size ) * i;

  /* merken der Dimensionen der Zeilen */
  element.dim_anz = 1;
  element.dimlen[0] = dim2;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die Zeilen in die Liste */
  insert(element,(void *) help[0], (void *) help[dim1-1],dim2 * element_size);

  /* merken der Dimensionen der Matrix */
  element.dim_anz = 2;
  element.dimlen[0] = dim1;
  element.dimlen[1] = dim2;
  element.is_allocated = is_allocated;

  /* Eintragen des Zeigers auf die Matrix in die Liste */
  insert(element,help,help,sizeof(char *));

  return (void *) help;

}

/* --------------------------------------------------------------------- */
/* Funktion vaCreate_3d()                                                */
/*    erzeugt ein 3-dimensionales Feld mit den Dimensionen 'dim1', 'dim2'*/
/*    und 'dim3', die Speichergröße eines Elementes wird fest-           */
/*    gelegt durch die übergebene Größe 'element_size'                   */
/* Parameter: Dimensionslängen, element_size, Anfangspointer             */
/* Rückgabe : generischer Zeiger auf die erste Zeile des erzeugten       */
/*            Feldes                                                     */
/* --------------------------------------------------------------------- */
#undef vaCreate_3d

void *vaCreate_3d ( int dim1, int dim2, int dim3, size_t element_size,
			const void *begin_)
{
  const char ***help2;
  const char **help1;
  const char *begin = (const char *) begin_;
  data_t element;
  int is_allocated, i;


  if ( begin == NULL )
  {
    begin = (const char *) malloc( dim1 * dim2 * dim3 * element_size );
    /* Wir müssen uns merken, dass der Speicherplatz von uns angelegt wurde,
       um ihn bei vaDelete() auch wieder freigeben zu können                 */
    is_allocated = 1;
  }
  else
    is_allocated = 0; 

  help1 = (const char **) malloc( dim1 * dim2 * sizeof(char *) );
  help2 = (const char ***) malloc( dim1 * sizeof(char **) );

  help1[0] = begin;
  for ( i=1; i<dim1*dim2; i++ )
  {
    help1[i] = help1[i-1] + (dim3 * element_size);
  }

  help2[0] = help1;
  for ( i=1; i<dim1; i++ )
  {
    help2[i] = help2[i-1] + dim2;
  }

  /* merken der Dimensionen der Zeilen */
  element.dim_anz = 1;
  element.dimlen[0] = dim3;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die Zeilen in die Liste */
  insert(element,(void *) help1[0],(void *) help1[dim1*dim2-1],
         dim3 * element_size);

  /* merken der Dimensionen der 2-dimensionalen Felder */
  element.dim_anz = 2;
  element.dimlen[0] = dim2;
  element.dimlen[1] = dim3;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die 2- dimensionalen Felder in die Liste */
  insert(element,(void *) help2[0],(void *) help2[dim1-1],
         dim2 * sizeof(char *));

  /* merken der Dimensionen des 3-dimensionalen Feldes */
  element.dim_anz = 3;
  element.dimlen[0] = dim1;
  element.dimlen[1] = dim2;
  element.dimlen[2] = dim3;
  element.is_allocated = is_allocated;

  /* Eintragen des Zeigers auf das 3-dimensionale Feld in die Liste */
  insert(element,help2,help2,sizeof(char **));

  return (void *) help2;
}

/* --------------------------------------------------------------------- */
/* Funktion vaCreate_4d()                                                */
/*    erzeugt ein 4-dimensionales Feld mit den Dimensionen 'dim1',       */
/*    'dim2', 'dim3' und 'dim4', die Speichergröße eines Elementes wird  */
/*    festgelegt durch die übergebene Größe 'element_size'               */
/* Parameter: Dimensionslängen, element_size, Anfangspointer             */
/* Rückgabe : generischer Zeiger auf die erste Zeile des erzeugten       */
/*            Feldes                                                     */
/* --------------------------------------------------------------------- */
#undef vaCreate_4d

void *vaCreate_4d ( int dim1, int dim2, int dim3, int dim4, 
                        size_t element_size, const void *begin_)
{
  const char ****help3;
  const char ***help2;
  const char **help1;
  const char *begin = (const char *) begin_;
  data_t element;
  int is_allocated, i;


  if ( begin == NULL )
  {
    begin = (const char *) malloc( dim1 * dim2 * dim3 * dim4 * element_size );
    /* Wir müssen uns merken, dass der Speicherplatz von uns angelegt wurde,
       um ihn bei vaDelete() auch wieder freigeben zu können                 */
    is_allocated = 1;
  }
  else
    is_allocated = 0; 

  help1 = (const char **) malloc( dim1 * dim2 * dim3 * sizeof(char *) );
  help2 = (const char ***) malloc( dim1 * dim2 * sizeof(char **) );
  help3 = (const char ****) malloc( dim1 * sizeof(char ***) );

  help1[0] = begin;
  for ( i=1; i<dim1*dim2*dim3; i++ )
  {
    help1[i] = help1[i-1] + (dim4 * element_size);
  }

  help2[0] = help1;
  for ( i=1; i<dim1*dim2; i++ )
  {
    help2[i] = help2[i-1] + dim3;
  }

  help3[0] = help2;
  for ( i=1; i<dim1; i++ )
  {
    help3[i] = help3[i-1] + dim2;
  }

  /* merken der Dimensionen der Zeilen */
  element.dim_anz = 1;
  element.dimlen[0] = dim4;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die Zeilen in die Liste */
  insert(element,(void *) help1[0],(void *) help1[dim1*dim2*dim3-1],
         dim4 * element_size);

  /* merken der Dimensionen der 2-dimensionalen Felder */
  element.dim_anz = 2;
  element.dimlen[0] = dim3;
  element.dimlen[1] = dim4;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die Matrizen in die Liste */
  insert(element,(void *) help2[0],(void *) help2[dim1*dim2-1],
         dim3 * sizeof(char *));

 /* merken der Dimensionen der 3-dimensionalen Felder */
  element.dim_anz = 3;
  element.dimlen[0] = dim2;
  element.dimlen[1] = dim3;
  element.dimlen[2] = dim4;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die 3-dimensionalen Felder in die Liste */
  insert(element,(void *) help3[0],(void *) help3[dim1-1],
         dim2 * sizeof(char **));

  /* merken der Dimensionen des 4-dimesionalen Feldes */
  element.dim_anz = 4;
  element.dimlen[0] = dim1;
  element.dimlen[1] = dim2;
  element.dimlen[2] = dim3;
  element.dimlen[3] = dim4;
  element.is_allocated = is_allocated;

  /* Eintragen des Zeigers auf das 4-dimensionale Feld in die Liste */
  insert(element,help3,help3,sizeof(char ***));

  return (void *) help3;
}

/* --------------------------------------------------------------------- */
/* Funktion vaCreate_5d()                                                */
/*    erzeugt ein 5-dimensionales Feld mit den Dimensionen 'dim1',       */
/*    'dim2', 'dim3', 'dim4' und 'dim5', die Speichergröße eines         */
/*    Elementes wird festgelegt durch die übergebene Größe 'element_size'*/
/* Parameter: Dimensionslängen, element_size, Anfangspointer             */
/* Rückgabe : generischer Zeiger auf die erste Zeile des erzeugten       */
/*            Feldes                                                     */
/* --------------------------------------------------------------------- */
#undef vaCreate_5d

void *vaCreate_5d ( int dim1, int dim2, int dim3, int dim4, int dim5, 
                        size_t element_size, const void *begin_)
{
  const char *****help4;
  const char ****help3;
  const char ***help2;
  const char **help1;
  const char *begin = (const char *) begin_;
  data_t element;
  int is_allocated, i;


  if ( begin == NULL )
  {
    begin = (const char *) malloc( dim1 * dim2 * dim3 * dim4 * dim5 * element_size );
    /* Wir müssen uns merken, dass der Speicherplatz von uns angelegt wurde,
       um ihn bei vaDelete() auch wieder freigeben zu können                 */
    is_allocated = 1;
  }
  else
    is_allocated = 0; 

  help1 = (const char **) malloc( dim1 * dim2 * dim3 *  dim4 * sizeof(char *) );
  help2 = (const char ***) malloc( dim1 * dim2 *  dim3 * sizeof(char **) );
  help3 = (const char ****) malloc( dim1 * dim2 * sizeof(char ***) );
  help4 = (const char *****) malloc( dim1 * sizeof(char ****) );

  help1[0] = begin;
  for ( i=1; i<dim1*dim2*dim3*dim4; i++ )
  {
    help1[i] = help1[i-1] + (dim5 * element_size);
  }

  help2[0] = help1;
  for ( i=1; i<dim1*dim2*dim3; i++ )
  {
    help2[i] = help2[i-1] + dim4;
  }

  help3[0] = help2;
  for ( i=1; i<dim1*dim2; i++ )
  {
    help3[i] = help3[i-1] + dim3;
  }

  help4[0] = help3;
  for ( i=1; i<dim1; i++ )
  {
    help4[i] = help4[i-1] + dim2;
  }

  /* merken der Dimensionen der Zeilen */
  element.dim_anz = 1;
  element.dimlen[0] = dim5;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die Zeilen in die Liste */
  insert(element,(void *) help1[0],(void *) help1[dim1*dim2*dim3*dim4-1],
         dim5 * element_size);

  /* merken der Dimensionen der 2-dimensionalen Felder */
  element.dim_anz = 2;
  element.dimlen[0] = dim4;
  element.dimlen[1] = dim5;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die Matrizen in die Liste */
  insert(element,(void *) help2[0],(void *) help2[dim1*dim2*dim3-1],
         dim4 * sizeof(char *));

  /* merken der Dimensionen der 3-dimensionalen Felder */
  element.dim_anz = 3;
  element.dimlen[0] = dim3;
  element.dimlen[1] = dim4;
  element.dimlen[2] = dim5;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die Matrizen in die Liste */
  insert(element,(void *) help3[0],(void *) help3[dim1*dim2-1],
         dim3 * sizeof(char **));

  /* merken der Dimensionen der 4-dimensionalen Felder */
  element.dim_anz = 4;
  element.dimlen[0] = dim2;
  element.dimlen[1] = dim3;
  element.dimlen[2] = dim4;
  element.dimlen[3] = dim5;
  element.is_allocated = 1;

  /* Eintragen der Zeiger auf die 4-dimensionalen Felder in die Liste */
  insert(element,(void *) help4[0],(void *) help4[dim1-1],
         dim2 * sizeof(char ***));

  /* merken der Dimensionen des 5-dimensionalen Feldes */
  element.dim_anz = 5;
  element.dimlen[0] = dim1;
  element.dimlen[1] = dim2;
  element.dimlen[2] = dim3;
  element.dimlen[3] = dim4;
  element.dimlen[4] = dim5;
  element.is_allocated = is_allocated;

  /* Eintragen des Zeigers auf das 5-dimensionale Feld in die Liste */
  insert(element,help4,help4,sizeof(char ****));

  return (void *) help4;
}


/* --------------------------------------------------------------------- */
/* Funktion vaDelete()                                                   */
/*    gibt den Speicherplatz des dynamisch angelegten Feldes, auf den    */
/*    key zeigt, wieder frei                                             */
/* Parameter: Anfangszeiger des zu löschenden Feldes                     */
/* Rückgabe : keine                                                      */
/* --------------------------------------------------------------------- */
void vaDelete ( const void *start )
{
  datapointer d;

  assert( (d = search(start)) != NULL );

  switch ( d->dim_anz )
  {
    case 1: {
              void *help = (void *) start;

              if ( d->is_allocated == 1) /* Freigabe des Speicherplatzes, nur wenn */
                free(help);           /* er von uns angelegt wurde              */

            }
            break;
    case 2: {
              void **help = (void **) start;
              /* Löschen der Zeiger auf die Zeilen aus der Liste */
              delete(help[0]);

              if ( d->is_allocated == 1) /* Freigabe des Speicherplatzes, nur wenn */
                free(help[0]);           /* er von uns angelegt wurde              */

              free(help);
            }
            break;
    case 3: {
              void ***help = (void ***) start;
              /* Löschen der Zeiger auf die Zeilen aus der Liste */
              delete(help[0][0]);
              /* Löschen der Zeiger auf die Matrizen aus der Liste */
              delete(help[0]);

              if ( d->is_allocated == 1) /* Freigabe des Speicherplatzes, nur wenn */
                free(help[0][0]);        /* er von uns angelegt wurde              */

              free(help[0]);
              free(help);
            }
            break;
    case 4: {
              void ****help = (void ****) start;
              /* Löschen der Zeiger auf die Zeilen aus der Liste */
              delete(help[0][0][0]);
              /* Löschen der Zeiger auf die Matrizen aus der Liste */
              delete(help[0][0]);
              /* Löschen der Zeiger auf die 3-dimensionalen Felder aus der Liste */
              delete(help[0]);

              if ( d->is_allocated == 1) /* Freigabe des Speicherplatzes, nur wenn */
                free(help[0][0][0]);     /* er von uns angelegt wurde              */

              free(help[0][0]);
              free(help[0]);
              free(help);
            }
            break;
    case 5: {
              void *****help = (void *****) start;
              /* Löschen der Zeiger auf die Zeilen aus der Liste */
              delete(help[0][0][0][0]);
              /* Löschen der Zeiger auf die Matrizen aus der Liste */
              delete(help[0][0][0]);
              /* Löschen der Zeiger auf die 3-dimensionalen Felder aus der Liste */
              delete(help[0][0]);
              /* Löschen der Zeiger auf die 4-dimensionalen Felder aus der Liste */
              delete(help[0]);

              if ( d->is_allocated == 1) /* Freigabe des Speicherplatzes, nur wenn */
                free(help[0][0][0][0]);  /* er von uns angelegt wurde              */

              free(help[0][0][0]);
              free(help[0][0]);
              free(help[0]);
              free(help);
            }
            break;
    default:
            fprintf(stderr, "Zur Zeit ist nur das Löschen von 2-, 3-, 4- und 5-"
              "dimensionalen Feldern implementiert.\n");
            break;
  }

  /* Löschen des ersten Zeigers aus der Liste */
  delete((void *) start);
}

/* --------------------------------------------------------------------- */
/* Funktion vaSize()                                                     */
/*    liefert zu einem Zeiger auf ein Feld die Dimensionslänge der       */
/*    Dimension 'dim'                                                    */
/* Parameter: Zeiger auf ein Feld, Dimension                             */
/* Rückgabe : Größe der Dimension, falls der Schlüssel gefunden wurde    */
/*            -1, falls der Schlüssel nicht gefunden wurde               */
/* --------------------------------------------------------------------- */
int vaSize ( const void *key, int dim )
{
  datapointer d;
  int res = -1;

  if ( (d = search(key)) != NULL )
  {
    if ( ((dim-1) < d->dim_anz) && ((dim-1) >= 0) )
      res = d->dimlen[dim-1];
  }

  return  res;
}
