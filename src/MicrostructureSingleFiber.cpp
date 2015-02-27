/*
 * MicrostructureSingleFiber.cpp
 *
 *  Created on: Jan 22, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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

#include "MicrostructureSingleFiber.h"
#include "rng.h"
#define SIGN(x) ((x>0) - (x<0))

MicrostructureSingleFiber::MicrostructureSingleFiber() {
	m_use_var_rad=false;
	m_myelin_axon_ratio=0.6;
	m_extracellular_spacing=.0005;
	m_fiber_orientation[0]=1;m_fiber_orientation[0]=0;m_fiber_orientation[0]=0;
	m_cyl_dens = .1;

// default values will be set during ReadDiffusionFile
	m_cyl_radius_inner 	= 0.004;	/* values used by Oleg Posnansky, ISMRM 2009 abstract*/
	m_cyl_radius_outer 	= 0.00657;	/* values used by Oleg Posnansky, ISMRM 2009 abstract*/
// unit: [D]=mm^2/ms
	m_D[0]				= 0.75e-6;	/* values used by Oleg Posnansky, ISMRM 2009 abstract*/
	m_D[1]				= 0.3e-6;	/* values used by Oleg Posnansky, ISMRM 2009 abstract*/

//	m_D[0]=1.61e-6;  // squid axon diffusivity (parallel to axis) (Beaulieu et al., MRM 1994)
	m_D[0]=1.4e-6;   // value used by AXCaliber (Barazany et al., Brain, 2009)
	m_myelin_axon_ratio=0.0;
//	m_D[1]=0.3e-12;

	m_seed = 42;
}
/***********************************************************/
MicrostructureSingleFiber::~MicrostructureSingleFiber() {
}

/***********************************************************/
void MicrostructureSingleFiber::SetFiberOrientation(double dx,double dy,double dz) {
	double norm;
	norm = sqrt(dx*dx + dy*dy + dz*dz);
	if (norm == 0) {
		cout << "invalid fiber orientation! (dx=" << dx <<", dy="<<dy<<", dz="<<dz<<"). Exit!" << endl;
		exit(-1);
	}
	m_fiber_orientation[0]=dx/norm;
	m_fiber_orientation[1]=dy/norm;
	m_fiber_orientation[2]=dz/norm;
};

/***********************************************************/
void MicrostructureSingleFiber::GenerateStructure(){
	double vol_box = (2*m_boxsize)*(2*m_boxsize)*(2*m_boxsize);
	double vol_cylinders = 0;
	m_rng = new RNG(m_seed);
	int counter=0;
	while (vol_cylinders < m_cyl_dens * vol_box){
		neuron tmp;
		if(counter==0) SetCylOrientation(tmp);
		GetRadius(tmp,m_cyl_dens * vol_box,vol_cylinders);
		tmp.x = m_rng->uniform(-m_boxsize,m_boxsize);
		tmp.y = m_rng->uniform(-m_boxsize,m_boxsize);
		tmp.z = m_rng->uniform(-m_boxsize,m_boxsize);
		if ((CylinderCollide(tmp)) && (counter<1000)) {
			counter++;
		} else {
			vol_cylinders += GetCylinderVolume(tmp);
			m_axons.push_back(tmp);
			if ((counter ==1000) ) cout << "density too high; cylinders overlap."<<endl;
			counter=0;
		}
	}
	cout << "build "<<m_axons.size() << " neurons."<<endl;
	delete m_rng;
}
/***********************************************************/
bool MicrostructureSingleFiber::CylinderCollide(const neuron &test) {

//	triple tmp;
//	tmp.x=test.x;tmp.y=test.y;tmp.z=test.z;
	for (int i=0; i<m_axons.size(); i++) {
		if (GetCylinderDistance(test, m_axons[i]) < (test.r_m + m_axons[i].r_m + m_extracellular_spacing)) {return true;}
	}
	return false;
}
/***********************************************************/
double MicrostructureSingleFiber::GetCylinderVolume(const neuron &tmp) {
	double lamdapos, lamdaneg, lamda;

	lamdapos = 1e100;
	lamdaneg = -1e100;

	// x-dimension:
	if (tmp.dx !=0)
	{
		lamda = (m_boxsize - tmp.x)/tmp.dx;
		if (lamda > 0) lamdapos = lamda; if (lamda < 0) lamdaneg = lamda; if (lamda == 0) {if(SIGN(m_boxsize) == SIGN(tmp.dx)) lamdapos=0; else lamdaneg = 0; };
		lamda = (-m_boxsize - tmp.x)/tmp.dx;
		if (lamda > 0) lamdapos = lamda; if (lamda < 0) lamdaneg = lamda; if (lamda == 0) {if(SIGN(-m_boxsize) == SIGN(tmp.dx)) lamdapos=0; else lamdaneg = 0; };
	}

	// y-dimension:
	if (tmp.dy !=0)
	{
		lamda = (m_boxsize - tmp.y)/tmp.dy;
		if ((lamda > 0) && (lamda < lamdapos)) lamdapos = lamda; if ((lamda < 0) && (lamda > lamdaneg)) lamdaneg = lamda; if (lamda == 0) {if(SIGN(m_boxsize) == SIGN(tmp.dy)) lamdapos=0; else lamdaneg = 0; };
		lamda = (-m_boxsize - tmp.y)/tmp.dy;
		if ((lamda > 0) && (lamda < lamdapos)) lamdapos = lamda; if ((lamda < 0) && (lamda > lamdaneg)) lamdaneg = lamda; if (lamda == 0) {if(SIGN(-m_boxsize) == SIGN(tmp.dy)) lamdapos=0; else lamdaneg = 0; };
	}

	// z-dimension:
	if (tmp.dz !=0)
	{
		lamda = (m_boxsize - tmp.z)/tmp.dz;
		if ((lamda > 0) && (lamda < lamdapos)) lamdapos = lamda; if ((lamda < 0) && (lamda > lamdaneg)) lamdaneg = lamda; if (lamda == 0) {if(SIGN(m_boxsize) == SIGN(tmp.dz)) lamdapos=0; else lamdaneg = 0; };
		lamda = (-m_boxsize - tmp.z)/tmp.dz;
		if ((lamda > 0) && (lamda < lamdapos)) lamdapos = lamda; if ((lamda < 0) && (lamda > lamdaneg)) lamdaneg = lamda; if (lamda == 0) {if(SIGN(-m_boxsize) == SIGN(tmp.dz)) lamdapos=0; else lamdaneg = 0; };
	}

	return ((lamdapos - lamdaneg)*PI*tmp.r_m*tmp.r_m);


}
/***********************************************************/
void MicrostructureSingleFiber::IndexObject(triple pos,  int &LastID ,int &shellid) {
	double dis;

	if (LastID >=0) {
		dis = GetPointDistance(pos,m_axons[LastID]);
		if (dis < m_axons[LastID].r_a) {shellid = 0; return;}
		if (dis < m_axons[LastID].r_m) {shellid = 1; return;}
	}
	for (int i=0; i<m_axons.size(); i++) {
		dis = GetPointDistance(pos,m_axons[i]);
		if (dis < m_axons[i].r_a) {shellid = 0; LastID = i; return;}
		if (dis < m_axons[i].r_m) {shellid = 1; LastID = i; return;}
	}

	shellid = -1;
	LastID = -1;
	return;
}

/***********************************************************/
double MicrostructureSingleFiber::GetPointDistance(const triple test, const neuron &tmp) {

	double proj, dx,dy,dz,px,py,pz;
	px = (test.x-tmp.x); py = (test.y-tmp.y); pz = (test.z-tmp.z);
	proj = px*tmp.dx + py*tmp.dy + pz*tmp.dz;

	dx = proj*tmp.dx - px;
	dy = proj*tmp.dy - py;
	dz = proj*tmp.dz - pz;

	return sqrt(dx*dx+dy*dy+dz*dz);

}
/***********************************************************/
double MicrostructureSingleFiber::GetD(int LastId,int shellid) {
	if (shellid == -1) return m_D_external;
	return m_D[shellid];
}

/***********************************************************/
void MicrostructureSingleFiber::DumpStructure(){
	std::ostringstream name2;
	name2 << "slice_0_vessels.txt";
	ofstream fout(name2.str().c_str(),ios::out);
	fout << "% position [x y z], direction [x y z], radius, OxygenExtrationFraction \n";
	fout << "% Dimension of unit cell: m_dim_x = " << m_boxsize << endl;
	for (int i=0; i< m_axons.size(); i++) {
		fout << m_axons[i].x << " " << m_axons[i].y << " " << m_axons[i].z << " " <<  m_axons[i].dx << " " << m_axons[i].dy << " " << m_axons[i].dz << " " << m_axons[i].r_m << " 1"<<  endl;
		if (m_axons[i].r_a != m_axons[i].r_m) {
			fout << m_axons[i].x << " " << m_axons[i].y << " " << m_axons[i].z << " " <<  m_axons[i].dx << " " << m_axons[i].dy << " " << m_axons[i].dz << " " << m_axons[i].r_a << " 0.8"<<  endl;
		}
	}
	fout.close();

}
/***********************************************************/
double MicrostructureSingleFiber::GetCylinderDistance(const neuron&n1, const neuron&n2){
	double nx, ny, nz; // cross product of the two directions
	double px,py,pz,dis,norm;
	// actvessel:n1, m_vessel[i]=n2
	nx = n1.dy * n2.dz - n1.dz * n2.dy;
	ny = n1.dz * n2.dx - n1.dx * n2.dz;
	nz = n1.dx * n2.dy - n1.dy * n2.dx;

	px = n1.x - n2.x;
	py = n1.y - n2.y;
	pz = n1.z - n2.z;

	norm = sqrt(nx*nx +ny*ny + nz*nz);
	// lines parallel?
	if (norm > 1e-6) {
		// skew lines
		nx/=norm; ny/=norm; nz/=norm;
		dis = abs(px*nx + py*ny + pz*nz);
	} else {
		//parallel lines
		double dummy;
		dummy = px*n1.dx + py*n1.dy + pz*n1.dz;
		dis = sqrt(px*px + py*py + pz*pz - dummy*dummy);
	}
	return dis;
}


/***********************************************************/
void MicrostructureSingleFiber::SetRadiusDistribution(string filename) {

	ifstream inFile;
    inFile.open(filename.c_str(), ifstream::in);
    if (!inFile.is_open()) {
        cout << "Unable to open axon radius distribution file: " << filename<< "; exit!"<<endl;
        exit(-1); // terminate with error
    }

	double r,w;
	while ((inFile >> r)) {
		if (!(inFile >> w)) {cout << "error in axon radius distribution file. exit. "<<endl; exit(-1);}
		m_radius_axis.push_back(r);
		m_radius_hist.push_back(w);
	}
	CalcPercentiles();
}
/***********************************************************/
void MicrostructureSingleFiber::SetRadiusDistribution(double alpha,double beta, double r_max) {
	double r_min=.00025;
	double step=(r_max-r_min)/500;
	double r=r_min,w;

	double dummy = pow(beta,alpha)*tgamma(alpha);
	while (r<=r_max) {
		w=pow(r,alpha-1)*exp(-r/beta)/dummy;
		m_radius_axis.push_back(r);
		m_radius_hist.push_back(w);
		r+=step;
	}
	CalcPercentiles();
}
/***********************************************************/
void MicrostructureSingleFiber::CalcPercentiles() {
	//set use of var radius:
	m_use_var_rad=true;
	// norm distribution
	double summe=0.0;
	double summe_vol=0.0;
	vector<double> vol_hist;

	for (int i=0;i<m_radius_axis.size();i++) {
		summe+=m_radius_hist[i];
		vol_hist.push_back(m_radius_hist[i]*m_radius_axis[i]*m_radius_axis[i]);
		summe_vol+=vol_hist[i];
	}
	for (int i=0;i<m_radius_axis.size();i++) {
		m_radius_hist[i]/=summe;
		vol_hist[i]/=summe_vol;
	}
	// calc indices for percentiles; use noComp compartments;
	int noComp=16;
	double step = 1.000001/((double) noComp);
	double border = step;
	summe_vol=0.0;

	for (int i=m_radius_axis.size()-1;i>=0;i--) {
		summe_vol+=vol_hist[i];
		if (summe_vol>border) {
			m_percentiles.push_back(i+1);
			border+=step;
		}
	}
}

/***********************************************************/
double Interpolate(const vector<double> &axis,const vector<double> &value,double r,int ibeg=0,int iend=-1){
	if (iend < 0) iend = axis.size()-1;

	// bisection phase:
	int middle;
	while ( (ibeg-iend) > 1) {
		middle=(ibeg+iend) >> 1;
		if (axis[middle] > r) iend = middle; else ibeg = middle;
	}
	//interpolate:
	return ((value[iend] - value[ibeg])/(axis[iend] - axis[ibeg])*(r-axis[ibeg])+value[ibeg]);
}

void MicrostructureSingleFiber::GetRadius(neuron &tmp,const double complete_vol, const double used_vol) {
	if (m_use_var_rad) {
		// get interval from which to draw the radius
		int ibeg, iend;
		double step = complete_vol/(m_percentiles.size()+1);
		int comp=floor(used_vol/step);
		if (comp==0) {
			ibeg = m_percentiles[0]; iend=m_radius_axis.size()-1;
		} else {
			if (comp == m_percentiles.size()) {
				ibeg=0;iend=m_percentiles.back();
			} else {
				ibeg=m_percentiles[comp]; iend=m_percentiles[comp-1];
			}
		}
		// get maximal value in interval:
		double ma=m_radius_hist[ibeg];
		for (int i=ibeg+1; i<=iend;i++) {
			if (m_radius_hist[i]>ma) ma=m_radius_hist[i];
		}
		//now draw from distribution:
		bool valid_radius=false;
		double r,y, trial_y;
		while (!valid_radius) {
			r=m_rng->uniform(m_radius_axis[ibeg],m_radius_axis[iend]);
			trial_y=m_rng->uniform(0.0,ma);
			y=Interpolate(m_radius_axis,m_radius_hist,r,ibeg,iend);
			if (trial_y< y) valid_radius = true;
		}
		tmp.r_m=r;
		tmp.r_a=r*m_myelin_axon_ratio;
	} else {
		tmp.r_a = m_cyl_radius_inner;tmp.r_m = m_cyl_radius_outer;
	}

}
