/*
 * MicrostructureKissingFiber.h
 *
 *  Created on: Jun 2, 2010
 *      Author: dpflug
 */

#ifndef MICROSTRUCTUREKISSINGFIBER_H_
#define MICROSTRUCTUREKISSINGFIBER_H_

#include "Microstructure.h"
/**
 * Kissing fiber model: use two cylinders, placed on origin.
 * for crossing fibers: cylinder 1 is object 0, cylinder 2 is object 1.
 * for kissing fibers: cyl 1+2 with x<0 is object 0, cyl 1+2 with x>0 is object 1.
 *
 */

class MicrostructureKissingFiber: public Microstructure {
public:
	MicrostructureKissingFiber();
	virtual ~MicrostructureKissingFiber();

	void SetFiberDirections(triple dir1, triple dir2);
	void SetRadius(double r_a, double r_m=0);
	void SetKissing(bool flag){m_is_kissing=flag;};
	void SetD(double D,int index) {m_D[index]=D;};
	virtual double GetD(int LastId=-1,int shellid=-1);
	virtual void IndexObject(triple pos,  int &LastId ,int &shellid);

	void DumpStructure();
private:
	vector<neuron> m_axons;					/* container holding the neurons. For now: use only two.	*/
	double m_D[2];							/* diffusion constants per layer	*/

	bool m_is_kissing;						/* defines if kissing or crossing fibers are used. */

	double GetPointDistance(const triple test, const neuron &tmp);

};

#endif /* MICROSTRUCTUREKISSINGFIBER_H_ */
