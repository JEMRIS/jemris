/** @file RepIter.h
 *  @brief Implementation of JEMRIS RepIter
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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

#ifndef REPCOUNTER_H_
#define REPCOUNTER_H_

class ConcatSequence;

/**
 *  @brief A super-simple iterator for counting the repetitions of a concat sequence.
 *
 * usage:
 * ConcatSequence C;
 * for (RepIter r=C.begin();r<C.end(); ++r)
 * { ... }
 */

class RepIter {

	inline friend bool operator<(const RepIter &rc1,const RepIter &rc2) {return rc1.m_RC < rc2.m_RC;};

 public:

	/**
	 * @brief Default Constructor
	 */
    RepIter(){};

	/**
	 * @brief Construct with concat sequence and counter
	 *
	 * @param cs Concat sequence on which to iterate
	 * @param rc Repitition counter
     * @return myself
	 */
	RepIter(ConcatSequence* cs, unsigned int rc);

	/**
	 * @brief Default destructor
	 */
	~RepIter(){};

	/**
	 * @brief Operator =
	 *
	 * @param rhs Assignement.
	 * @return myself
	 */
	RepIter operator=(const RepIter &rhs);

	/**
	 * @brief Operator ++
	 *
	 * Iterate i further
	 *
	 * @param i Amount to iterate further
	 * @return myself
	 */
	RepIter operator++(int i);

	/**
	 * @brief Operator ++
	 *
	 * Iterate 1 further
	 * @return myself
	 */
	RepIter operator++();


 private:

	ConcatSequence* m_cs; /**< @brief Concat sequence */
	unsigned int    m_RC; /**< @brief Counter         */

};

#endif
