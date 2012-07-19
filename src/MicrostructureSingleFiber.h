/*
 * MicrostructureSingleFiber.h
 *
 *  Created on: Jan 22, 2010
 *      Author: dpflug
 */

#ifndef MICROSTRUCTURESINGLEFIBER_H_
#define MICROSTRUCTURESINGLEFIBER_H_

#include "Microstructure.h"
#include <vector>

/**
 * Microstructure consisting of parallel cylinders.
 */
class MicrostructureSingleFiber: public Microstructure {
public:
	MicrostructureSingleFiber();
	virtual ~MicrostructureSingleFiber();

	/**
	 * set cylinder orientation
	 */
	void SetFiberOrientation(double dx,double dy,double dz);

	/**
	 * Set Cylinder Density;
	 */
	void SetCylinderDensity(double density){if((density>0) && (density<1)) m_cyl_dens = density; else cout <<"density must be between 0 and 1!"<<endl;};

	/**
	 * Set Cylinder Radius;
	 */
	void SetCylinderRadius(double inner, double outer=0) {m_cyl_radius_inner = inner; if (outer > inner) m_cyl_radius_outer=outer; else m_cyl_radius_outer=inner;};

	/**
	 * Set axon radius distribution, string: -> filename, three doubles: distribution from AxCaliber (Assaf 2008 MagResMed.); alpha, beta, cutoffradius.
	 */
	void SetRadiusDistribution(string filename);
	void SetRadiusDistribution(double alpha, double beta, double rmax);

	/**
	 * Set seed for random number generator
	 */
	void SetSeed(int val) {m_seed = val;}

	/**
	 * Set Diffusion constants
	 */
	void SetD(double D,int index) {m_D[index]=D;};

	/**
	 * builds Structure
	 */
	void GenerateStructure();

	virtual void IndexObject(triple pos,  int &LastId ,int &shellid);

	virtual double GetD(int LastId,int shellid);

	/**
	 * sets ratio axon/(axion + myelin sheet). Needed if radius distribution of neurons is used
	 */
	void SetAxonToNeuronRadiusRatio(double ratio) {m_myelin_axon_ratio=ratio;};

	/*
	 * Dump microstructure to file
	 */
	void DumpStructure();

protected:

	virtual bool	CylinderCollide(const neuron &test);		/* test if this neuron is overlapping with any other in structure. */
	double 	GetCylinderVolume(const neuron &test);	/* calculate volume of cylinder in box.  */
	/**
	 * Get distance between point pos and cylinder tmp.
	 */
	double GetPointDistance(const triple pos, const neuron &tmp);

	/**
	 * Get distance between two cylinders.
	 */
	double GetCylinderDistance(const neuron&n1, const neuron&n2);
	/**
	 * sets orientation of cyl in GenerateStructure.
	 */
	virtual void SetCylOrientation(neuron &tmp) {tmp.dx = m_fiber_orientation[0];tmp.dy = m_fiber_orientation[1];tmp.dz = m_fiber_orientation[2];};

	/**
	 * if radius distribution is used: calculate compartments (begin with largest radii, then fill up with smaller ones.)
	 */
	void CalcPercentiles();

	/**
	 * Get a trial axon radius
	 */
	void GetRadius(neuron &tmp,const double complete_vol, const double used_vol);


	double m_fiber_orientation[3];			/* orientation of cylinders 		*/
	double m_cyl_dens;						/* density of cylinders				*/
	double m_cyl_radius_inner;				/* inner radius of cylinders		*/
	double m_cyl_radius_outer;				/* outer radius of cylinders		*/
	double m_D[2];							/* diffusion constants per layer	*/

	vector<neuron> m_axons;					/* container holding the neurons. 	*/

	int m_seed;								/* random number seed				*/


	/* variables for axon radius distribution */
	bool m_use_var_rad;
	vector<int> m_percentiles;				/* compartment indices (first place axons from large compartment) */
	vector<double> m_radius_axis;			/* axis for radius distribution 	*/
	vector<double> m_radius_hist;			/* values for radius distribution 	*/
	double m_myelin_axon_ratio;				/* ratio axon to (axon+myelin). Between 0 and 1. Used for variable radii only; else myelin radius is set explicitly. */
	double m_extracellular_spacing;			/* minimal space between neurons		*/
};

#endif /* MICROSTRUCTURESINGLEFIBER_H_ */
