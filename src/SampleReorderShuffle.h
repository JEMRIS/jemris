/** @file SampleReorderShuffle.h
 * @brief Implementation of JEMRIS SampleReorderShuffle
 *
 *  Created on: Jan 11, 2010
 *      Author: dpflug
 */

#ifndef SAMPLEREORDERSHUFFLE_H_
#define SAMPLEREORDERSHUFFLE_H_

#include "SampleReorderStrategyInterface.h"

/**
 * @brief shuffles the order in which spins are simulated.
 */
class SampleReorderShuffle: public SampleReorderStrategyInterface {
public:
	SampleReorderShuffle();
	virtual ~SampleReorderShuffle();

	/**
	 * Shuffles the sample. uses always the same seed (-> reproducible)
	 */
	virtual void Execute(Spin* data);
};

#endif /* SAMPLEREORDERSHUFFLE_H_ */
