/*
 * Microstructure.h
 *
 *  Created on: Dec 10, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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

#ifndef MICROSTRUCTURE_H_
#define MICROSTRUCTURE_H_

#include "TrajectoryDiffusion.h"

/**
 *@brief Definition of a neuron/axon for the Microstructure
 */
struct neuron {
	double dx;	/**<@brief x-component of fiber orientation */
	double dy;	/**<@brief y-component of fiber orientation */
	double dz;	/**<@brief z-component of fiber orientation */
	double x;	/**<@brief x position of fiber */
	double y;	/**<@brief y position of fiber */
	double z;	/**<@brief z position of fiber */
	double r_a;	/**<@brief radius of axon without myelin sheet */
	double r_m;	/**<@brief radius of axon including myelin sheet */
};

class RNG;

/**
 *@brief Microsructure base class
 */
class Microstructure {
public:
	Microstructure();
	virtual ~Microstructure();

	/**
	 *  return index of Microstructure object; -1 -> outside.
	 *  shellid: object may have multiple layer (myelinated axon). id of layer; shellid = -1 -> object has no layers.
	 *  LastId:  during tracking of spin: object where last position was in.
	 */
	virtual void IndexObject(triple pos,  int &LastId ,int &shellid) {LastId=-1;shellid=-1;};

	/**
	 * returns Diffusion constant. unit: mm^2/ms
	 */
	virtual double GetD(int LastId=-1,int shellid=-1) {return m_D_external;};

	/**
	 * unit: mm^2/ms
	 */
	void SetExternalDiffusionConstant(double d) {m_D_external=d;};

	bool isInsideBox(triple pos) {return ((fabs(pos.x)<m_boxsize) & (fabs(pos.y)<m_boxsize) & (fabs(pos.z)<m_boxsize));};

	/*
	 * maps a position outside of the box into it
	 */
	triple PeriodicBoundary(triple trialpos);

	void SetBoxSize(double size) {m_boxsize=size;}
	double GetBoxSize() {return m_boxsize;}

protected:
	// microstructure is defined in box with [-m_boxsize..m_boxsize] in each direction
	double m_boxsize;
	double m_D_external;

	RNG* m_rng;

};

#endif /* MICROSTRUCTURE_H_ */
