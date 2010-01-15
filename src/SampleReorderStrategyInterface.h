/*
 * SampleReorderStrategyInterface.h
 *
 *  Created on: Jan 11, 2010
 *      Author: dpflug
 */

#ifndef SAMPLEREORDERSTRATEGYINTERFACE_H_
#define SAMPLEREORDERSTRATEGYINTERFACE_H_

//#include "Sample.h"
struct Spin;

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
