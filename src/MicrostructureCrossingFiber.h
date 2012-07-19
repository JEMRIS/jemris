/*
 * MicrostructureCrossingFiber.h
 *
 *  Created on: Jan 25, 2010
 *      Author: dpflug
 */

#ifndef MICROSTRUCTURECROSSINGFIBER_H_
#define MICROSTRUCTURECROSSINGFIBER_H_

#include "MicrostructureSingleFiber.h"

class MicrostructureCrossingFiber: public MicrostructureSingleFiber {
public:
	MicrostructureCrossingFiber();
	virtual ~MicrostructureCrossingFiber();

	void SetSecondFiberOrientation(double dx,double dy,double dz);

protected:
	virtual void SetCylOrientation(neuron &tmp);
	double m_2_fiber_orientation[3];
// uncomment next line to allow neurons to overlap spatially
//	virtual bool CylinderCollide(const neuron &test) {return false;}



};

#endif /* MICROSTRUCTURECROSSINGFIBER_H_ */
