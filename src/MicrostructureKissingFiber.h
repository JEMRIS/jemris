/*
 * MicrostructureKissingFiber.h
 *
 *  Created on: Jun 2, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
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

#ifndef MICROSTRUCTUREKISSINGFIBER_H_
#define MICROSTRUCTUREKISSINGFIBER_H_

#include "Microstructure.h"
/**
 * Kissing fiber model: use two cylinders, placed on origin.
 * for crossing fibers: cylinder 1 is object 0, cylinder 2 is object 1.
 * for kissing fibers: cyl 1+2 with x<0 is object 0, cyl 1+2 with x>0 is object 1.
 *
 */

/**
 * @brief Microstructure consisting of kissing fibers
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
	vector<neuron> m_axons;		/* container holding the neurons. For now: use only two.	*/
	double m_D[2];			/* diffusion constants per layer	*/
	bool m_is_kissing;		/* defines if kissing or crossing fibers are used. */

	double GetPointDistance(const triple test, const neuron &tmp);

};

#endif /* MICROSTRUCTUREKISSINGFIBER_H_ */
