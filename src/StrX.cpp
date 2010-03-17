/** @file StrX.cpp
 *  @brief Implementation of JEMRIS StrX
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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

#include "StrX.h"
#include <iostream>


/***********************************************************/
StrX::StrX(const XMLCh* const toTranscode):brelease2(false),tmp(NULL) {
	// Call the private transcoding method
	fLocalForm = XMLString::transcode(toTranscode);
	if (fLocalForm != NULL)
		mString = string(fLocalForm) ;
	brelease = true;
}

/***********************************************************/
StrX::StrX(const char* toTranscode ):brelease2(false),tmp(NULL) {
	if (toTranscode != NULL)
		mString = string(toTranscode) ;
	else
		mString ="";

    brelease = false;
}

/***********************************************************/
StrX::StrX(const string toTranscode ):brelease2(false),tmp(NULL) {
    mString = toTranscode;
    brelease = false;
}
/***********************************************************/
StrX::~StrX() {
    if (brelease) XMLString::release(&fLocalForm);
    if (brelease2) XMLString::release(&tmp);
}

/***********************************************************/
const char* StrX::localForm() const {
    return fLocalForm;
}

/***********************************************************/
const string StrX::std_str() const {
    return mString;
}

/***********************************************************/
const XMLCh* StrX::XMLchar()  {
	if (tmp!=NULL) XMLString::release(&tmp);
	tmp = XMLString::transcode(mString.c_str());
    brelease2 = true;
	return tmp;
}

