/** @file RepIter.cpp
 *  @brief Implementation of JEMRIS RepIter
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2018  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2018  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "RepIter.h"
#include "ConcatSequence.h"

/***********************************************************/
RepIter::RepIter (ConcatSequence* cs, unsigned int rc) {

  m_cs=cs;
  m_RC=rc;

}

/***********************************************************/
RepIter RepIter::operator= (const RepIter &rhs) {

  m_cs = rhs.m_cs;
  m_RC = rhs.m_RC;
  return *this;

}

/***********************************************************/
RepIter RepIter::operator++ (int) {

  m_cs->SetRepCounter(++m_RC);
  return *this;

}

/***********************************************************/
RepIter RepIter::operator++(){

  m_cs->SetRepCounter(++m_RC);
  return *this;

}
