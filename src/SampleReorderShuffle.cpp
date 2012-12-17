/*
 * SampleReorderShuffle.cpp
 *
 *  Created on: Jan 11, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

#include "SampleReorderShuffle.h"
#include "rng.h"
#include "Sample.h"

SampleReorderShuffle::SampleReorderShuffle() {
}

SampleReorderShuffle::~SampleReorderShuffle() {
}


void SampleReorderShuffle::Execute(Spin* data) {
	// fisher-yates shuffle algorithm as seen on wikipedia:

	// seed always with same number. must be consistent for restart!
	RNG rng(42);

    // n is the number of items left to shuffle
    for (long n = data->size; n > 1; n--)
    {
        // Pick a random element to move to the end
        long k =  floor(rng.rand_halfclosed01()*n) ;  // 0 <= k <= n - 1.
        // Simple swap of variables
        Spin_data tmp = data->data[k];
        data->data[k] = data->data[n - 1];
        data->data[n - 1] = tmp;
    }

}

