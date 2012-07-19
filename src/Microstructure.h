/*
 * Microstructure.h
 *
 *  Created on: Dec 10, 2009
 *      Author: dpflug
 */

#ifndef MICROSTRUCTURE_H_
#define MICROSTRUCTURE_H_

#include "TrajectoryDiffusion.h"

struct neuron {
	double dx,dy,dz; // orientation of fiber
	double x,y,z;	 // position of fiber
	double r_a;		 // radius of axon without myelin sheet
	double r_m;		 // radius of axon including myelin sheet;
};

class RNG;

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
