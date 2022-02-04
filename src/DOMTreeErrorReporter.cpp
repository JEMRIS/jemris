/** @file DOMTreeErrorReporter.cpp
 *  @brief Implementation of JEMRIS DOMTreeErrorReporter
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2022  Tony Stoecker
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

#include <xercesc/sax/SAXParseException.hpp>
#include "DOMTreeErrorReporter.h"

/***********************************************************/
inline bool DOMTreeErrorReporter::GetSawErrors() const {
    return fSawErrors;
}

/***********************************************************/
void DOMTreeErrorReporter::warning(const SAXParseException&) {
    // Ignore all warnings
}

/***********************************************************/
void DOMTreeErrorReporter::error(const SAXParseException& toCatch) {
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "Error at file \"" << StrX(toCatch.getSystemId())
         << "\", line " << toCatch.getLineNumber()
         << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
}

/***********************************************************/
void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch) {
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "Fatal Error at file \"" << StrX(toCatch.getSystemId())
         << "\", line "      << toCatch.getLineNumber()
         << ", column "      << toCatch.getColumnNumber()
         << "\n   Message: " << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
}

/***********************************************************/
void DOMTreeErrorReporter::resetErrors() {
    fSawErrors = false;
}
