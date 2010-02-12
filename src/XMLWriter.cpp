#include "XMLWriter.h"
#include "config.h"
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

bool XMLWriter::Write (DOMImplementation* impl, DOMNode* node, string filename) {


	XMLFormatTarget* mft;

	if (filename.empty())
		mft = new StdOutFormatTarget();
	else
		mft = new LocalFileFormatTarget (StrX(filename).XMLchar());

	// Xerces 2
    #ifdef XSEC_XERCES_HAS_SETIDATTRIBUTE
	DOMWriter* serializer = ((DOMImplementationLS*)impl)->createDOMWriter();

    if (serializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
		serializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
	
	serializer->writeNode(mft, node);

	serializer->release();
    #endif 

    #ifdef XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE
	DOMLSSerializer*   serializer    = ((DOMImplementationLS*) impl)->createLSSerializer();
	DOMLSOutput*       output        = ((DOMImplementationLS*)impl)->createLSOutput(); 
    DOMConfiguration*  configuration = serializer->getDomConfig(); 

	if (configuration->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
		configuration->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true); 

	output->setByteStream (mft);
	serializer->write(node, output);

	output->release();
	serializer->release(); 
    #endif


	return true;

}

