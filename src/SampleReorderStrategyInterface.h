/** @file SampleReorderStrategyInterface.h
 * @brief Implementation of JEMRIS of SampleReorderStrategyInterface
 *
 *  Created on: Jan 11, 2010
 *      Author: dpflug
 */

#ifndef SAMPLEREORDERSTRATEGYINTERFACE_H_
#define SAMPLEREORDERSTRATEGYINTERFACE_H_

struct Spin;

/**
 * @brief base class for different sample reorder strategies.
 */
class SampleReorderStrategyInterface {
public:
	SampleReorderStrategyInterface() {};
	virtual ~SampleReorderStrategyInterface() {};

	/**
	 * function which reorders the sample
	 */
	virtual void Execute(Spin* data) {};
};

#endif /* SAMPLEREORDERSTRATEGYINTERFACE_H_ */
