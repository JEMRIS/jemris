/** @file StrX.h
 *  @brief Implementation of JEMRIS StrX
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2025  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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

#ifndef STRX_H_
#define STRX_H_

#include <string>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

//! Simple class for transcoding sax errors to the machines locale

class StrX {

 public :

    /**
     * @brief Contructor.
     */
    StrX (const XMLCh* const);

    /**
     * @brief Construct with char array.
     */
    StrX (const char* const);

    /**
     * @brief Construct with string.
     */
    StrX (const std::string&);

    /**
     * @brief Construct with string.
     */
    StrX (const StrX&);

    /**
     * @brief Destructor
     */
    ~StrX ();

    /**
     * @brief Construct with string.
     */
    StrX& operator= (const StrX&);

    /**
     * @brief Transcode to local form char array
     *
     * @return Transcoded char array.
     */
    const char* localForm () const;

    /**
     * @brief Transcode to string.
     *
     * @return Transcoded string.
     */
    const std::string std_str () const;

    /**
     * @brief Transcode to XMLCh*.
     *
     * @return Transcoded XMLCh*.
     */
    const XMLCh* XMLchar () ;

private :
    
    XMLCh*  _xmlch;
    char*	_char;
    std::string  _string;
    
};

#endif /*STRX_H_*/
