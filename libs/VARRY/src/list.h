/**************************************************************************
** Copyright (c) 2003-2005                                               **
** Forschungszentrum Juelich GmbH, Federal Republic of Germany.          **
**                                                                       **
** See the file `var_array.c',                                           **
**              `list_c.h', or                                           **
**              `var_array++_cpp.h' for details                          **
**************************************************************************/

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
/* Header : list                                                         */
/* Aufgabe: enthält die Funktionen zur Verwaltung der Liste              */
/* Definierte Funktionen:                                                */
/*        - search()                                                     */
/*        - insert()                                                     */
/*        - delete()                                                     */
/*                                                                       */
/*************************************************************************/

#ifndef LIST_H_INCLUDE
#define LIST_H_INCLUDE

typedef const struct data_t *datapointer;

#ifndef LINKAGE
#define LINKAGE
#endif
/* --------------------------------------------------------------------- */
/* Funktionsdeklarationen                                                */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* Funktion search()                                                     */
/*    sucht nach dem Listenelement, das den Schlüssel key enthält        */
/* Parameter: Zeiger auf ein Feld                                        */
/* Rückgabe : Zeiger auf die gespeicherten Daten des Listenelementes     */
/*            NULL, falls key nicht gefunden wurde                       */
/* --------------------------------------------------------------------- */
LINKAGE datapointer search(const void *key );


/* --------------------------------------------------------------------- */
/* Funktion insert()                                                     */
/*    fügt ein Element sortiert in die Liste ein                         */
/* Parameter: Zeiger auf Element, Anfangszeiger, Endzeiger, Offset       */
/* Rückgabe : keine                                                      */
/* --------------------------------------------------------------------- */
LINKAGE void insert(datapointer element, void *start, void *end, size_t offs); 
/*
 * wegen des folgenden Makros ist die Funktion insert so aufzurufen,
 * als wäre sie wie folgt deklariert:
 * 
 * extern int insert ( struct data_t element,void *start, void *end, size_t offs);
 */ 
#define insert(d,s,e,o) insert (memcpy(malloc(sizeof(d)), &(d), sizeof(d)),(s), (e), (o))


/* --------------------------------------------------------------------- */
/* Funktion delete()                                                     */
/*    löscht ein Element aus der Liste                                   */
/* Parameter: Zeiger                                                     */
/* Rückgabe : keine                                                      */
/* --------------------------------------------------------------------- */
LINKAGE void delete( void *key );


/* --------------------------------------------------------------------- */
/* nur zum Testen                                                        */
/* --------------------------------------------------------------------- */

#ifdef DEBUG_LIST
LINKAGE void print ( void );
#endif   /* DEBUG_LIST */

#endif   /* LIST_H_INCLUDE */
