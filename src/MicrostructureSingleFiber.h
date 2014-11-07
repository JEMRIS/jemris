/*
 * MicrostructureSingleFiber.h
 *
 *  Created on: Jan 22, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#ifndef MICROSTRUCTURESINGLEFIBER_H_
#define MICROSTRUCTURESINGLEFIBER_H_

#include "Microstructure.h"
#include <vector>

/**
 * @brief Microstructure consisting of parallel cylinders.
 */
class MicrostructureSingleFiber: public Microstructure {
public:
	MicrostructureSingleFiber();
	virtual ~MicrostructureSingleFiber();

	/**
	 * @brief set cylinder orientation
	 */
	void SetFiberOrientation(double dx,double dy,double dz);

	/**
	 * @brief Set Cylinder Density;
	 */
	void SetCylinderDensity(double density){if((density>0) && (density<1)) m_cyl_dens = density; else cout <<"density must be between 0 and 1!"<<endl;};

	/**
	 * @brief Set Cylinder Radius;
	 */
	void SetCylinderRadius(double inner, double outer=0) {m_cyl_radius_inner = inner; if (outer > inner) m_cyl_radius_outer=outer; else m_cyl_radius_outer=inner;};

	/**
	 * @brief Set axon radius distribution, string: -> filename, three doubles: distribution from AxCaliber (Assaf 2008 MagResMed.); alpha, beta, cutoffradius.
	 */
	void SetRadiusDistribution(string filename);
	void SetRadiusDistribution(double alpha, double beta, double rmax);

	/**
	 * @brief Set seed for random number generator
	 */
	void SetSeed(int val) {m_seed = val;}

	/**
	 * @brief Set Diffusion constants
	 */
	void SetD(double D,int index) {m_D[index]=D;};

	/**
	 * @brief builds Structure
	 */
	void GenerateStructure();

	virtual void IndexObject(triple pos,  int &LastId ,int &shellid);

	virtual double GetD(int LastId,int shellid);

	/**
	 * @brief sets ratio axon/(axion + myelin sheet). Needed if radius distribution of neurons is used
	 */
	void SetAxonToNeuronRadiusRatio(double ratio) {m_myelin_axon_ratio=ratio;};

	/*
	 * @brief Dump microstructure to file
	 */
	void DumpStructure();

protected:

	virtual bool	CylinderCollide(const neuron &test);		/* test if this neuron is overlapping with any other in structure. */
	double 	GetCylinderVolume(const neuron &test);	/* calculate volume of cylinder in box.  */
	/**
	 * @brief Get distance between point pos and cylinder tmp.
	 */
	double GetPointDistance(const triple pos, const neuron &tmp);

	/**
	 * @brief Get distance between two cylinders.
	 */
	double GetCylinderDistance(const neuron&n1, const neuron&n2);
	/**
	 * @brief sets orientation of cyl in GenerateStructure.
	 */
	virtual void SetCylOrientation(neuron &tmp) {tmp.dx = m_fiber_orientation[0];tmp.dy = m_fiber_orientation[1];tmp.dz = m_fiber_orientation[2];};

	/**
	 * @brief if radius distribution is used: calculate compartments (begin with largest radii, then fill up with smaller ones.)
	 */
	void CalcPercentiles();

	/**
	 * @brief Get a trial axon radius
	 */
	void GetRadius(neuron &tmp,const double complete_vol, const double used_vol);


	double m_fiber_orientation[3];		/**<@brief orientation of cylinders 		*/
	double m_cyl_dens;			/**<@brief density of cylinders			*/
	double m_cyl_radius_inner;		/**<@brief inner radius of cylinders		*/
	double m_cyl_radius_outer;		/**<@brief outer radius of cylinders		*/
	double m_D[2];				/**<@brief diffusion constants per layer	*/

	vector<neuron> m_axons;			/**<@brief container holding the neurons. 	*/

	int m_seed;				/**<@brief random number seed			*/


	/* variables for axon radius distribution */
	bool m_use_var_rad;		/**<@brief true for variable radius distribution*/
	vector<int> m_percentiles;	/**<@brief compartment indices (first place axons from large compartment) */
	vector<double> m_radius_axis;	/**<@brief axis for radius distribution 	*/
	vector<double> m_radius_hist;	/**<@brief values for radius distribution 	*/
	double m_myelin_axon_ratio;	/**<@brief ratio axon to (axon+myelin). Between 0 and 1. Used for variable radii only; else myelin radius is set explicitly. */
	double m_extracellular_spacing;	/**<@brief minimal space between neurons		*/
};

#endif /* MICROSTRUCTURESINGLEFIBER_H_ */
