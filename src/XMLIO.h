/** @file XMLIO.h
 *  @brief Implementation of JEMRIS XMLIO
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

#ifndef XMLIO_H_
#define XMLIO_H_

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

enum errorCodes {
    OK,                             /**< OK */
    XML_PLATFORM_INIT,              /**< Error during XML plattform initialisation */
    PARSER_OUT_OF_MEMORY_EXCEPTION, /**< Parser ran out of memory */
    PARSER_XML_EXCEPTION,           /**< Parser initialisation failed */
    PARSER_DOM_EXCEPTION,           /**< XML doesn't meet DOM specs */
    PARSER_GENERAL_EXCEPTION,       /**< General parser error */
    NORMALIZE_DOM_EXCEPTION,        /**< Normalisation of the DOM tree failed */
    EMPTY_DOCUMENT                  /**< Empty document: Nothing to be done! */
};

//! XML file-IO class

class XMLIO {

 public:

	/**
	 * @brief Constructor
	 */
    XMLIO();

    /**
     * @brief Destructor
     */
	virtual ~XMLIO();

	/**
	 * @brief Read in the XML file and return a DOM object.
	 *
	 * @param  uri URI to file.
	 * @return     Contained DOM document.
	 */
	DOMDocument* 
	Parse (string uri);

    /**
     * @brief Recursive run through XML tree and do something.
     *
     * @param node   The node to start the travel.
     * @param ptr    Pointer to an object to be passed to the function.
     * @param fun    Pointer to static function which is executed for the node.
     * @return       DOMNode at which the recursion is aborted by the static function.
     */
	DOMNode*     
	RunTree (DOMNode* node, void* ptr, unsigned int (*fun) (void*, DOMNode*) );

	/**
	 * @brief Write DOM node to file
	 *
	 * @param  impl      DOM implementation
	 * @param  node      Node for output
	 * @param  filename  File name
	 * @return           Success
	 */
	bool 
	Write (DOMImplementation* impl, DOMNode* node, string filename) ;

 private:

	XercesDOMParser::ValSchemes valScheme;
	bool                        doNamespaces;
	bool                        doSchema;
	bool                        schemaFullChecking;
    bool                        doCreate;

    XercesDOMParser*            m_parser;
    DOMTreeErrorReporter*       m_err_reporter;
};

#endif /*XMLIO_H_*/
