/*
 * MicrostructureKissingFiber.cpp
 *
 *  Created on: Jun 2, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
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

#include "MicrostructureKissingFiber.h"

MicrostructureKissingFiber::MicrostructureKissingFiber() {
	// init microstructure; changeable: radius, direction.
	neuron start;
	start.dx=1;
	start.dy=1;
	double norm;
	norm = sqrt(start.dx*start.dx+ start.dy*start.dy);
	start.dx/=norm;
	start.dy/=norm;
	start.dz=0;
	start.r_a=0.002;
	start.r_m=0.002;
	start.x=0;	start.y=0;	start.z=0;
	m_axons.push_back(start);
	start.dy*=-1.0;
	m_axons.push_back(start);

	m_is_kissing=true;


}
/***********************************************************/
MicrostructureKissingFiber::~MicrostructureKissingFiber() {

}

/***********************************************************/
void MicrostructureKissingFiber::IndexObject(triple pos,  int &LastId ,int &shellid) {
	if (m_is_kissing) {
		double dis1, dis2;
		dis1 = GetPointDistance(pos,m_axons[0]);
		dis2 = GetPointDistance(pos,m_axons[1]);
		if ((dis1<m_axons[0].r_a) ||  (dis2<m_axons[1].r_a) ) {
			shellid=0;
			if (pos.x<0) LastId = 0; else LastId = 1;
			return;
		}
		if ((dis1<m_axons[0].r_m) ||  (dis2<m_axons[1].r_m) ) {
			shellid=1;
			if (pos.x<0) LastId = 0; else LastId = 1;
			return;
		}
	} else {
		//crossing fibers:
		double dis;

		if (LastId >=0) {
			dis = GetPointDistance(pos,m_axons[LastId]);
			if (dis < m_axons[LastId].r_a) {shellid = 0; return;}
			if (dis < m_axons[LastId].r_m) {shellid = 1; return;}
		}
		for (int i=0; i<m_axons.size(); i++) {
			dis = GetPointDistance(pos,m_axons[i]);
			if (dis < m_axons[i].r_a) {shellid = 0; LastId = i; return;}
			if (dis < m_axons[i].r_m) {shellid = 1; LastId = i; return;}
		}
	}

	shellid = -1;
	LastId = -1;
	return;
};


/***********************************************************/
double MicrostructureKissingFiber::GetD(int LastId,int shellid) {
	if (shellid == -1)
		return (m_D_external);
	else
		return (m_D[shellid]);
};

/***********************************************************/
double MicrostructureKissingFiber::GetPointDistance(const triple test, const neuron &tmp) {

	double proj, dx,dy,dz,px,py,pz;
	px = (test.x-tmp.x); py = (test.y-tmp.y); pz = (test.z-tmp.z);
	proj = px*tmp.dx + py*tmp.dy + pz*tmp.dz;

	dx = proj*tmp.dx - px;
	dy = proj*tmp.dy - py;
	dz = proj*tmp.dz - pz;

	return sqrt(dx*dx+dy*dy+dz*dz);

}
/***********************************************************/
void MicrostructureKissingFiber::SetFiberDirections(triple dir1, triple dir2) {
	double norm;

	norm=sqrt(dir1.x*dir1.x+dir1.y*dir1.y+dir1.z*dir1.z);
	if (norm >1e-6) {
		m_axons[0].dx=dir1.x/norm; m_axons[0].dy=dir1.y/norm;	m_axons[0].dz=dir1.z/norm;
	} else {
		cout << "bad Direction 1! use default."<<endl;
	}

	norm=sqrt(dir2.x*dir2.x+dir2.y*dir2.y+dir2.z*dir2.z);
	if (norm >1e-6) {
		m_axons[1].dx=dir2.x/norm; m_axons[1].dy=dir2.y/norm;	m_axons[1].dz=dir2.z/norm;
	} else {
		cout << "bad Direction 2! use default."<<endl;
	}

	return;
}

/***********************************************************/
void MicrostructureKissingFiber::SetRadius(double r_a, double r_m) {
	if (r_m<r_a) r_m=r_a;
	m_axons[0].r_a=r_a;m_axons[0].r_m=r_m;
	m_axons[1].r_a=r_a;m_axons[1].r_m=r_m;
	return;
}
/***********************************************************/
void MicrostructureKissingFiber::DumpStructure(){
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
