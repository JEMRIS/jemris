/*
 * TrajectoryDiffusion.h
 *
 *  Created on: Dec 10, 2009
 *      Author: dpflug
 */

#ifndef TRAJECTORYDIFFUSION_H_
#define TRAJECTORYDIFFUSION_H_

#include "TrajectoryInterface.h"
#include "rng.h"
#include "XMLIO.h"


enum modes{BOTH, INTERNAL, EXTERNAL};

using namespace std;

class Microstructure;
struct triple {
	double x,y,z;
};

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
	XMLIO*  				m_xio;
	DOMDocument*			m_dom_doc;


};

#endif /* TRAJECTORYDIFFUSION_H_ */
