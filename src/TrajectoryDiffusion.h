/*
 * TrajectoryDiffusion.h
 *
 *  Created on: Dec 10, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2018  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2018  Daniel Pflugfelder
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

#ifndef TRAJECTORYDIFFUSION_H_
#define TRAJECTORYDIFFUSION_H_

#include "TrajectoryInterface.h"
#include "rng.h"
#include "XMLIO.h"


enum modes{BOTH, INTERNAL, EXTERNAL};

using namespace std;

class Microstructure;

//! Cartesians positions for diffusion trajectory of a spin
struct triple {
	double x,y,z;
};

//! Diffusion trajectory of a spin
class TrajectoryDiffusion: public TrajectoryInterface {
public:
	TrajectoryDiffusion();
	virtual ~TrajectoryDiffusion();

//load diffusion definition file
	virtual void LoadFile(string filename) ;

	virtual void GetValueDerived(double time, double *pos);

	virtual void UpdateTrajectory(bool init);

	void DumpTrajectory(string filename);

private:

	// xml access functions
	string 			GetAttr         (DOMElement* element, string key);
	DOMElement* 	GetElem    		(string name);

	void GenerateDiffusionTrajectory();
	void CalcY();						// calc information for cubic spline interpolation.

	void SetSeed(long seed);

	Microstructure *m_microstruct;

	vector<triple> m_pos;
	// vector for cubic splines interpolation:
	vector<triple> m_y;

	unsigned long m_seed;
	RNG* m_rng;

	double m_timestep;
	long m_max_timesteps;

	// mode ==0 simulate internal and external, mode ==1 only internal, mode == 2 only external
	modes m_mode;

	vector<int> m_dump_index; //dump the trajectory of these spins

	int m_diff_dimension;	// flag for 3-dimensional, 2-dim or 1-dim diffusion; default: 3-dim
	void ProjectPosition(triple &pos, triple dir);

	// variables for xml reader:
	DOMTreeErrorReporter* 	m_domtree_error_rep;
	XMLIO*  		m_xio;
	DOMDocument*		m_dom_doc;


};

#endif /* TRAJECTORYDIFFUSION_H_ */
