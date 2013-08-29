/*
 * SampleReorderShuffle.cpp
 *
 *  Created on: Jan 11, 2010
 *      Author: dpflug
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
        Ensemble<double> tmp = data->data[k];
        data->data[k] = data->data[n - 1];
        data->data[n - 1] = tmp;
    }

}

