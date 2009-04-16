/** @file XMLRead.cpp
 *  @brief Implementation of JEMRIS XMLRead
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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

#include "XMLRead.h"

/**********************************************************/
XMLRead::~XMLRead() {
	delete m_parser;
	delete m_err_reporter;
}

/**********************************************************/
XMLRead::XMLRead () {

	try {
        XMLPlatformUtils::Initialize();
    } catch (const  XMLException& e) {
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
    }

	m_parser      = new XercesDOMParser;
    m_err_reporter = new DOMTreeErrorReporter;

	XercesDOMParser::ValSchemes valScheme    = XercesDOMParser::Val_Auto;
	doNamespaces                             = false;
	doSchema                                 = false;
	schemaFullChecking                       = false;
	doCreate                                 = false;

	m_parser->setValidationScheme             (valScheme);
	m_parser->setDoNamespaces                 (doNamespaces);
	m_parser->setDoSchema                     (doSchema);
	m_parser->setValidationSchemaFullChecking (schemaFullChecking);
	m_parser->setCreateEntityReferenceNodes   (doCreate);
	m_parser->setErrorHandler                 (m_err_reporter);

}

/**********************************************************/
DOMDocument* XMLRead::ParseFile (string uri) {

	DOMDocument* doc = NULL;

	try {
	    LocalFileInputSource LocFile( StrX(uri).XMLchar() );
	    m_parser->parse( LocFile );
	
	} catch (const OutOfMemoryException&) {

	    XERCES_STD_QUALIFIER cerr
	        << "OutOfMemoryException"
	        << XERCES_STD_QUALIFIER endl;
	    return NULL;

	} catch (const XMLException& e) {

	    XERCES_STD_QUALIFIER cerr
	        << "An error occurred during parsing\n   Message: "
	        << StrX(e.getMessage())
	        << XERCES_STD_QUALIFIER endl;
	    return NULL;

	} catch (const DOMException& e) {

	    const unsigned int maxChars = 2047;

	    XMLCh errText[maxChars + 1];

	    XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '"
	        << uri << "'\n"
	        << "DOMException code is:  "
	        << e.code << XERCES_STD_QUALIFIER endl;

	    if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
	         XERCES_STD_QUALIFIER cerr
	         << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

	    return NULL;

	} catch (...) {

	    XERCES_STD_QUALIFIER cerr
	        << "An error occurred during parsing\n "
	        << XERCES_STD_QUALIFIER endl;
	    return NULL;

	}

	if (m_parser) {

	    doc  = m_parser->getDocument();
	    if (doc) {

	        try {

	            doc->normalizeDocument();

	        } catch (const DOMException& e) {
	            const unsigned int maxChars = 2047;

	            XMLCh errText[maxChars + 1];

	            XERCES_STD_QUALIFIER cerr << "\nDOM Error during normalizing: '"
	                << uri << "'\n"
	                << "DOMException code is:  "
	                << e.code << XERCES_STD_QUALIFIER endl;

	            if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
	                 XERCES_STD_QUALIFIER cerr
	                 << "Message is: " << StrX(errText)
	                 << XERCES_STD_QUALIFIER endl;

	             return NULL;
	        }

	      } else
	        return NULL;

	}


	return doc;
}

/**********************************************************/
DOMNode* XMLRead::RunTree (DOMNode* node, void* ptr, unsigned int (*fun) (void*, DOMNode*) ) {

    DOMNode* child;
    DOMNode* rnode=NULL;

    if (node) {
        if (node->getNodeType() == DOMNode::ELEMENT_NODE) {

            unsigned int code = fun(ptr,node);
            if (code>0) return node;

            for (child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
                rnode = RunTree(child,ptr,fun);
                if (rnode!=NULL) break;
            }
        }
    }
    return rnode;
}

