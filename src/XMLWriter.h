/** @file XMLWriter.h
 *  @brief Implementation of JEMRIS XMLWriter
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony StÃ¶cker, Kaveh Vahedipour
 *                                  Forschungszentrum JÃ¼lich, Germany
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

#ifndef XMLWRITER_H_
#define XMLWRITER_H_

#include <string.h>
#include <stdlib.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

XERCES_CPP_NAMESPACE_USE

using namespace std;

#include "DOMTreeErrorReporter.h"

/**
 * @brief Return codes 
 */

class XMLWriter {

 public:

	/**
	 * @brief Constructor
	 */
    XMLWriter() {};

    /**
     * @brief Destructor
     */
	virtual ~XMLWriter() {};

	bool Write (DOMImplementation* impl, DOMNode* node, string filename) ;


 private:

    XercesDOMParser*            m_parser;
    DOMTreeErrorReporter*       m_err_reporter;

};

#endif /*XMLWRITER_H_*/
