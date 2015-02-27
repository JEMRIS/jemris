/** @file DOMTreeErrorReporter.h
 *  @brief Implementation of JEMRIS DOMTreeErrorReporter
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

#ifndef DOMTREEERRORREPORTER_H_
#define DOMTREEERRORREPORTER_H_

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include "StrX.h"

#if defined(XERCES_NEW_IOSTREAMS)
    #include <iostream>
#else
    #include <iostream.h>
#endif

XERCES_CPP_NAMESPACE_USE

//! XML exception handling

class DOMTreeErrorReporter : public ErrorHandler {

 public:
    /**
     * Contructor
     */
    DOMTreeErrorReporter() : fSawErrors(false) {
    }

    
    /**
     * Destructor
     */
    ~DOMTreeErrorReporter() {
    }

    /**
     * Handling of warnings
     * 
     * @param toCatch the sax parser exception to catch
     */
    void warning(const SAXParseException& toCatch);
    
    /**
     * Handling of errors
     * 
     * @param toCatch the sax parser exception to catch
     */
    void error(const SAXParseException& toCatch);
    
    /**
     * Handling of fatal errors
     * 
     * @param toCatch the sax parser exception to catch
     */
    void fatalError(const SAXParseException& toCatch);
    
    /**
     * Reset all errors
     */
    void resetErrors();

    /**
     * Getter method
     */
    bool GetSawErrors() const;

    /**
     * This is set if we get any errors, and is queryable via a getter                                                                        
     * method. Its used by the main code to suppress output if there are
     * errors.                                                                                                                                
     */ 
    bool    fSawErrors;
    
};


inline XERCES_STD_QUALIFIER ostream& operator <<  
    (XERCES_STD_QUALIFIER ostream& target, const StrX& toDump) {
    target << toDump.localForm();
    return target;
}

#endif /*DOMTREEERRORREPORTER_H_*/
