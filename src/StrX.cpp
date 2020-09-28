/** @file StrX.cpp
 *  @brief Implementation of JEMRIS StrX
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
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

#include "StrX.h"
#include <iostream>


/***********************************************************/
StrX::StrX (const XMLCh* const toTranscode) {
    _xmlch = XMLString::replicate (toTranscode);
    _char = XMLString::transcode (toTranscode); 
    _string = std::string(_char);
}

/***********************************************************/
StrX::StrX (const char* const toTranscode ) : 
    _string (toTranscode) {
    _xmlch = XMLString::transcode (toTranscode);
    _char=NULL;
}

/***********************************************************/
StrX::StrX (const std::string& toTranscode ) :
    _string (toTranscode) {
    _xmlch = XMLString::transcode (toTranscode.c_str());
    _char=NULL;
}

StrX::StrX (const StrX& strx) {
    *this = strx;
}

StrX& StrX::operator= (const StrX& strx) {
    _string = strx._string;
    _xmlch = XMLString::replicate (strx._xmlch);
    return *this;
}


/***********************************************************/
StrX::~StrX() {
    if (_xmlch)
        XMLString::release (&_xmlch);
    if (_char)
        XMLString::release (&_char);
}

/***********************************************************/
const char* StrX::localForm() const {
    return _string.c_str();
}

/***********************************************************/
const std::string StrX::std_str() const {
    return _string;
}

/***********************************************************/
const XMLCh* StrX::XMLchar()  {
	return _xmlch;
}

