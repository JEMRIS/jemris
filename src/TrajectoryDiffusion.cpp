/*
 * TrajectoryDiffusion.cpp
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

#include "TrajectoryDiffusion.h"
#include "Microstructure.h"
#include "MicrostructureSingleFiber.h"
#include "MicrostructureCrossingFiber.h"
#include "MicrostructureKissingFiber.h"
#include "MicrostructureBoxes.h"
#include "rng.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"

TrajectoryDiffusion::TrajectoryDiffusion() {
	m_seed = 42;
	m_microstruct = NULL;
	m_rng=NULL;
	m_max_timesteps=100000;
	m_timestep=0.05;//in ms
	m_mode=BOTH;
	m_diff_dimension=3;  //default : diffusion in 3 dimensions

	// init xml reader:
	m_domtree_error_rep = new DOMTreeErrorReporter;
	m_xio               = new XMLIO();
	m_dom_doc			= NULL;


}

TrajectoryDiffusion::~TrajectoryDiffusion() {
	if (m_microstruct 		!= NULL) delete m_microstruct;
	if (m_xio               != NULL) delete m_xio;
	if (m_domtree_error_rep != NULL) delete m_domtree_error_rep;

}

// xml access functions:
/**********************************************************/
string TrajectoryDiffusion::GetAttr         (DOMElement* element, string key) {
	return StrX( element->getAttribute( StrX(key).XMLchar()) ).std_str() ;
}

/**********************************************************/
DOMElement* TrajectoryDiffusion::GetElem    (string name) {
	return (DOMElement*)(m_dom_doc->getElementsByTagName(StrX(name).XMLchar()))->item(0);
}


/***********************************************************/
void TrajectoryDiffusion::LoadFile(string filename) {

	cout <<"\n--------------------------- Diffusion simulation variables -----------------------"<<endl;
	cout << "Diffusion file: "<<filename<<endl;
	// read xml-file:
	m_dom_doc           = m_xio->Parse(filename);
	DOMElement* element = GetElem("Diffusion");

    World*pw = World::instance();

    /***************** Read values valid for any diffusion type: *************/
    // Diffusion type:
	string type = GetAttr(element, "DiffusionType");
	if (type.empty()) {
		cout << "'DiffusionType' not defined in XML file. Currently available: free, single_fiber, crossing_fiber, kissing_fiber, boxes. \nUsing default.  ";
		type="free";
	}
	cout <<"'DiffusionType' = "<<type<<endl;


	// Microstructure box size.
	double boxsize=0.05; // default value.
    string s_boxsize = GetAttr(element, "Boxsize");
    if (s_boxsize.empty()) {cout << "'Boxsize' not set in XML file. Using default. ";} else {boxsize = atof(s_boxsize.c_str());}
    if (boxsize <= 0) { cout <<"   Error reading 'Boxsize'. using default. ";boxsize=0.05;}
    cout << "'Boxsize' = 2 * "<<boxsize <<"mm."<<endl;

    // Diffusion projection. Default: 3D diffusion, e.g. no projection:
    string projection = GetAttr(element, "Projection");
	if (projection.compare("1D")==0) {m_diff_dimension=1; cout <<"'Projection' = 1D: each spin diffuses along a randomly oriented line."<<endl;}
	if (projection.compare("2D")==0) {m_diff_dimension=2; cout <<"'Projection' = 2D: each spin diffuses in a randomly oriented plane."<<endl;}

	// dump microstructure
	string Dump = GetAttr(element,"DumpStructure");
	bool dumpStruct = false;
	if (type.compare("free")==0) {/* do nothing */} else {
		if (!Dump.empty()) {
			if (Dump.compare("1") || (Dump.compare("true")) ) dumpStruct=true;
		} else {cout <<"'DumpStructure' not set in XML file. Using default. ";}
		cout << "'DumpStructure' = "; if (dumpStruct) cout <<"true"; else cout<<"false"; cout <<". Microstructure is "; if (!dumpStruct) cout<<"not "; cout <<"dumped to file."<<endl;
	}

    // Diffusion pools: simulate internal spins only, external spins only, both
	m_mode = BOTH;  // default.
    string pool = GetAttr(element, "Pool");
    if (pool.empty()) {cout << "'Pool' not set in XML file. Using default. ";}else
    {
    	int d = atoi(pool.c_str());
    	switch (d) {
			case(0):
				m_mode = BOTH;
			break;
			case(1):
				m_mode = INTERNAL;
			break;
			case(2):
				m_mode = EXTERNAL;
			break;
			default:
				cout << "'Pool' = " << d<< " is no valid mode. Using default.  ";
			break;
    	}
    }
	cout<< "'Pool' = "<<m_mode<<" (0 == simulate internal+external spins, 1 == only internal spins, 2 == only external spins)" <<endl;

    // read seed for diffusion trajectory; if parallel jemris is used, even same seed values do not guarantee identical simulation results since the distribution of spins to each parallel process is non-deterministic.
    string Seed = GetAttr(element, "Seed");
    if (Seed.empty()) {cout << "'Seed' not set in XML file. using default diffusion trajectory seed: " << m_seed <<endl;} else {SetSeed((long) atoi(Seed.c_str())); cout <<"Diffusion 'Seed': = " <<m_seed<<endl;};

    // read the spin numbers for which the trajectory should be dumped for debug/visualization purposes. Can specify as many as wanted, read until end of file.
    string DumpTrajectory = GetAttr(element, "DumpTrajectory");
    if (DumpTrajectory.empty()) {
    	cout <<"'DumpTrajectory' not set in XML file. No spin trajectories are dump to file." <<endl;
    } else {
		int index;
		stringstream ssT(DumpTrajectory); // Insert the string into a stream
		while (ssT >> index) {
			cout << "dumping Trajectory of spin " << index<<endl;
			m_dump_index.push_back(index);
		}
    }

    if (m_microstruct != NULL) {
    	delete m_microstruct;
    	m_microstruct = NULL;
    }


    // free diffusion:
    if (type.compare("free")==0)  {
    	m_microstruct = new Microstructure();
    	m_microstruct->SetBoxSize(boxsize);
    }

    // "bunch of boxes"
    if((type.compare("boxes"))==0) {
    	MicrostructureBoxes* m_struct = new MicrostructureBoxes();
    	m_struct->SetBoxSize(boxsize);

    	// read boxes to be placed in microstructure.
    	// each box needs 7 entries:
    	// 		middlepoint [x/y/z] 				unit mm
    	//		boxlength in [size_x/size_y/size_z	unit mm
    	//		Diffusion constant in side box.		unit mm^2/ms
    	// boxes will extend from: x-size_x to x+size_x (in each direction)
    	// boxes currently non-rotateable.
    	// as many boxes as needed possible.
    	string boxes = GetAttr(element, "Boxes");
    	// read boxes:
    	if (boxes.empty()) {cout <<"'Boxes' not defined in XML file. No microstructure available. Exit."<<endl;exit(-1);} else {
    		stringstream ss(boxes); // Insert the string into a stream
    		int i=0;
	    	box cur_box;
    	    while (ss >> cur_box.x) {
        		i++;
        		cout<< "Box "<<i+1<<":\n"<<"Position=["<<cur_box.x;
        		if (!(ss >> cur_box.y)) {cout << "y position of "<<i+1<<"th box is missing. exit!" << endl; exit(-1);} else {cout <<", "<<cur_box.y;}
        		if (!(ss >> cur_box.z)) {cout << "z position of "<<i+1<<"th box is missing. exit!" << endl; exit(-1);} else {cout <<", "<<cur_box.z<<"]\n";}
        		if (!(ss >> cur_box.size_x)) {cout << "size_x of "<<i+1<<"th box is missing. exit!" << endl; exit(-1);} else {cout <<"sizes=[ "<<cur_box.size_x;}
        		if (!(ss >> cur_box.size_y)) {cout << "size_y of "<<i+1<<"th box is missing. exit!" << endl; exit(-1);} else {cout <<", "<<cur_box.size_y;}
        		if (!(ss >> cur_box.size_z)) {cout << "size_z of "<<i+1<<"th box is missing. exit!" << endl; exit(-1);} else {cout <<", "<<cur_box.size_z<<"]\n";}
        		if (!(ss >> cur_box.D)) {cout << "Diffusion constant of "<<i+1<<"th box is missing. exit!" << endl; exit(-1);} else {cout<<"Diffusion constant = "<<cur_box.D<<"mm^2/ms."<<endl;}
        		m_struct->AddBox(cur_box);
        	}
    		m_microstruct=m_struct;
    	}
    } // end boxes model

    	// now fiber models:
    	if ((type.compare("single_fiber")==0) || (type.compare("crossing_fiber")==0) || (type.compare("kissing_fiber")==0)){
    		// values needed for all fiber models
			// first fiber direction:
			double dir_f1_x,dir_f1_y,dir_f1_z;
			string dirf1 = GetAttr(element, "FiberDirection");
			if (dirf1.empty()) {cout << "'FiberDirection' not defined in XML file. Using default. "; dir_f1_x=1;dir_f1_y=1;dir_f1_z=0;} else {
				stringstream ss(dirf1);
				if (!(ss >> dir_f1_x)) {cout <<"3 values for FiberDirection needed! exit!"<<endl;exit(-1);}
				if (!(ss >> dir_f1_y)) {cout <<"3 values for FiberDirection needed! exit!"<<endl;exit(-1);}
				if (!(ss >> dir_f1_z)) {cout <<"3 values for FiberDirection needed! exit!"<<endl;exit(-1);}
			}
			cout << "'FiberDirection' = ["<<dir_f1_x<<" "<<dir_f1_y <<" "<<dir_f1_z<<"]"<<endl;

			// second fiber direction:
			double dir_f2_x,dir_f2_y,dir_f2_z;
			string dirf2 = GetAttr(element, "SecondFiberDirection");
			if (dirf2.empty()) {//do nothing
				} else {
				stringstream ss(dirf2);
				if (!(ss >> dir_f2_x)) {cout <<"3 values for SecondFiberDirection needed! exit!"<<endl;exit(-1);}
				if (!(ss >> dir_f2_y)) {cout <<"3 values for SecondFiberDirection needed! exit!"<<endl;exit(-1);}
				if (!(ss >> dir_f2_z)) {cout <<"3 values for SecondFiberDirection needed! exit!"<<endl;exit(-1);}
			}

			// fiber density:
			double fiber_dens=0.2;
			string FiberDens = GetAttr(element, "FiberDensity");
    		if (FiberDens.empty()) { cout << "'FiberDensity' not set in XML file. Using default. ";} else {
    			double dummy=atof(FiberDens.c_str());
    			if ((dummy > 1) || (dummy<0)) {cout <<"'FiberDensity' must be between 0 and 1. Using default! ";} else {fiber_dens=dummy;}
    		}
			cout << "'FiberDensity' = "<<fiber_dens<<endl;

    		// Fiber radius:
    		string FiberRadius = GetAttr(element, "FiberRadius");
    		double inner_radius=-1.0;
			double outer_radius=-1.0;
    		if (!FiberRadius.empty()) {
					// read fiber radius
					stringstream ss(FiberRadius);
					if (!(ss >> inner_radius)) {cout <<"Invalid value for: 'FiberRadius' = "<<FiberRadius<<".Exit!"<<endl; exit(-1);}
					if (!(ss >> outer_radius)) {/*do nothing. outer shell ignored.*/};
					cout <<"'FiberRadius': r_axon = " <<inner_radius << "mm "; if (outer_radius > inner_radius) cout <<", r_axon_myelin= "<<outer_radius<<"mm."; cout<<endl;
    		}

    		// Diffusion constants
    		string FiberD = GetAttr(element,"FiberD");
    		double Daxon=1.4e-6,Dmyelin=0.3e-6;
    		if (!(FiberD.empty())) {
				// read Diffusion constants
				stringstream ss(FiberRadius);
				double dummy;
				if (!(ss >> dummy)) {cout <<"Invalid value: 'FiberD' = "<<FiberD<<".Exit!"<<endl; exit(-1);}
				Daxon=fabs(dummy);
				if (!(ss >> dummy)) {/* no myelin used. */if (outer_radius>inner_radius) {cout << "No myelin diffusion constant specified in 'FiberD'. Using default D_m = "<<Dmyelin << " mm^2/ms."<<endl;}} else {Dmyelin=fabs(dummy);}
    		} else {
    			cout << "Diffusion constants 'FiberD' not set in XML file. Using default. ";
    		}
    		cout << "Diffusion constant(s) 'FiberD': Daxon = "<<Daxon<<"mm^2/ms"; if (outer_radius>inner_radius) cout <<", Dmyelin="<<Dmyelin<<"mm^2/ms";cout<<endl;


    		// create microstructure:
        	if ((type.compare("single_fiber")==0) || (type.compare("crossing_fiber")==0)) {
        		MicrostructureSingleFiber* mstruct;
        		if (type.compare("crossing_fiber")==0){
        			mstruct = new MicrostructureCrossingFiber();
        		} else {
        			mstruct = new MicrostructureSingleFiber();
        		}
            	// set boxsize:
            	mstruct->SetBoxSize(boxsize);

        		mstruct->SetFiberOrientation(dir_f1_x,dir_f1_y,dir_f1_z);
        		if (type.compare("crossing_fiber")==0) {
        			if (dirf2.empty()) {cout << "'SecondFiberDirection' is missing in file XML file. Exit!"<<endl; exit(-1);}
        			cout << "'SecondFiberDirection' = ["<<dir_f2_x<< " "<<dir_f2_y<<" "<<dir_f2_z<<"]"<<endl;
            		((MicrostructureCrossingFiber*) (mstruct))->SetSecondFiberOrientation(dir_f2_x,dir_f2_y,dir_f2_z);
        		}

        		mstruct->SetCylinderDensity(fiber_dens);
        		mstruct->SetD(Daxon,0);mstruct->SetD(Dmyelin,1);

        		if (!FiberRadius.empty()) {
    					mstruct->SetCylinderRadius(inner_radius,outer_radius);
        			} else {
        				bool haveDist=true;
        				string FiberRadiusDistribution = GetAttr(element, "FiberRadiusDistribution");
        				if (!FiberRadiusDistribution.empty()) {
        				// read fiber distribution
						double alpha,beta,rmax;
        				stringstream ss(FiberRadiusDistribution);
        				// values e.g. alpha=2.9, beta=0.0007, r_max=.01
            			if (!(ss >> alpha)) {cout <<"Invalid value: 'FiberRadiusDistribution' = "<<FiberRadiusDistribution<<". Three values needed (alpha, beta, r_max). Exit!"<<endl; exit(-1);}
            			if (!(ss >> beta)) {cout <<"Invalid value: 'FiberRadiusDistribution' = "<<FiberRadiusDistribution<<". Three values needed (alpha, beta, r_max). Exit!"<<endl; exit(-1);}
            			if (!(ss >> rmax)) {cout <<"Invalid value: 'FiberRadiusDistribution' = "<<FiberRadiusDistribution<<". Three values needed (alpha, beta, r_max). Exit!"<<endl; exit(-1);}
    					cout <<"'FiberRadiusDistribution': Using alpha="<<alpha<<", beta="<<beta<<", r_max="<<rmax<<"mm"<<endl;
            			mstruct->SetRadiusDistribution(alpha,beta,rmax);
        			} else {
            			string FiberRadiusDistributionFile = GetAttr(element, "FiberRadiusDistributionFile");
            			if (!FiberRadiusDistributionFile.empty()) {
            				// read fiber distribution from file later
            				cout <<"Reading 'FiberRadiusDistributionFile' = "<<FiberRadiusDistributionFile<<endl;
        					mstruct->SetRadiusDistribution(FiberRadiusDistributionFile);
            			} else {
            				// nothing there...
            				haveDist=false;
            				inner_radius=0.01;
            				cout << "No fiber radius specified in XML file. Available options are:\n   1.) 'FiberRadius' = dx dy, where dx is axon radius, dy radius including axon and myelin sheet (dy is optional, if neglected, no myelin is assumed.)\n   2.) 'FiberRadiusDistribution' = alpha, beta, r_max (Distribution used e.g. in AXCaliber (Assaf et al., MRM, 2008); Radius for each neuron is randomly drawn from this distribution.)\n   3.) 'FiberRadiusDistributionFile' = filename (a textfile specifying an arbitrary disribution.) "<<endl;
            				cout << "Using the default neuron radius (r="<<inner_radius<<"mm)."<<endl;
        					mstruct->SetCylinderRadius(inner_radius,inner_radius);
            			}
        			}
					if (haveDist) {
						string Ratio = GetAttr(element,"RadiusRatioAxonToNeuron");
						double ratio=1;
						if (Ratio.empty()) {
							cout <<"'SetAxonToNeuronRadiusRatio' not set in XML file. Using default. ";
						} else {
							double dummy=atof(Ratio.c_str());
							if ((dummy<=0) || (dummy>1)) { cout <<"Invalid value for 'SetAxonToNeuronRadiusRatio' = " << Ratio <<  " (needs to be between 0 and 1). Using default. ";} else {ratio=dummy;}
						}
						cout <<"'SetAxonToNeuronRadiusRatio' = " <<ratio<<endl;
						mstruct->SetAxonToNeuronRadiusRatio(ratio);
        			}
        		}  // end FiberRadius

        		// Microstructure seed:
        		string MicroSeed = GetAttr(element,"MicrostructSeed");
        		long microSeed=42;
        		if (!(MicroSeed.empty())) {
        			long dummy=atol(MicroSeed.c_str());
        			if (dummy==0) {cout <<"Invalid 'MicrostructSeed' = "<<MicroSeed<<". Using default. ";} else {microSeed=dummy;}
        		} else {
        			cout <<"'MicrostructSeed' for random generation of Microstructure not specified in XML file. Using default. ";
        		}
        		cout <<"'MicrostructSeed' = "<<microSeed<<"."<<endl;
        		// Microstructure Seed: needs to be the same for all parallel JEMRIS jobs! (otherwise, different microstructures are generated...)
        		mstruct->SetSeed(microSeed);

                mstruct->GenerateStructure();
                if (dumpStruct) mstruct->DumpStructure();
            	m_microstruct = mstruct;
        	} // end single/crossing fiber

            // kissing fibers:
            if (type.compare("kissing_fiber")==0) {
            	MicrostructureKissingFiber* mstruct = new MicrostructureKissingFiber();

            	// set boxsize:
            	mstruct->SetBoxSize(boxsize);
        		mstruct->SetD(Daxon,0);mstruct->SetD(Dmyelin,1);

            	triple dir1,dir2;
            	dir1.x=dir_f1_x;dir1.y=dir_f1_y;dir1.z=dir_f1_z;
            	dir2.x=dir_f2_x;dir2.y=dir_f2_y;dir2.z=dir_f2_z;
            	mstruct->SetFiberDirections(dir1,dir2);
            	mstruct->SetRadius(inner_radius,outer_radius);

            	string IsKiss = GetAttr(element, "IsKissing");
            	int isKiss;
            	if (IsKiss.empty()){
            		cout << "'IsKissing' not set in XML file. Use default. ";
            	} else {
            		int dummy = atoi(IsKiss.c_str());
            		if (!((dummy==1)||(dummy==0))){cout << "Invalid value for 'IsKissing' in XML file. (Options: 1 for kissing or 0 for crossing.) Using default. ";dummy=1;}
            		isKiss=dummy;
            	}
            	cout << "'IsKissing' = "<<isKiss <<". Fibers treated as ";
            	if (isKiss==1) {cout << "kissing";} else {cout <<"crossing";}cout<<"."<<endl;
            	mstruct->SetKissing(isKiss);

                if (dumpStruct) mstruct->DumpStructure();
                m_microstruct = mstruct;
            }

    	} // end is fiber model

    if (m_microstruct == NULL ) {cout << "No valid microstructure defined. exit!"<<endl; exit (-1); }


    // Read Diffusion constant for extracellular space; only possible after Microstructure was created...:
    string ExtraD = GetAttr(element, "ExtracellularD");
    if (ExtraD.empty()) {cout <<"'ExtracellularD' not set in XML file. Using default. ";} else {
    	double dummy=atof(ExtraD.c_str());
    	if (dummy <= 0 ) { cout <<ExtraD<<" not a valid value for 'ExtracellularD' in XML file. using default. ";} else {m_microstruct->SetExternalDiffusionConstant(dummy);}
    }
    cout << "'ExtravascularD': diffusion constant D= "<<m_microstruct->GetD(-1,-1) <<" mm^2/s"<<endl;



    if (m_rng!=NULL) delete m_rng;
    // multiply diffusion seed value with parallel jemris rank, so that each job as a different seed. (+2 because serial jemris has m_myRank=-1.)
    m_rng=new RNG(m_seed*(pw->m_myRank+2));

    cout <<"------------------------ End Diffusion simulation variables ----------------------\n"<<endl;

}
/***********************************************************/
void TrajectoryDiffusion::SetSeed(long seed) {
	World* pw=World::instance();
	if (pw->m_myRank>0) m_seed=seed*pw->m_myRank; else m_seed=seed;
	return;
}
/***********************************************************/

void TrajectoryDiffusion::UpdateTrajectory(bool init) {
	if (m_microstruct==NULL) return;

	if (init) {
		m_time.clear();
		m_pos.clear();
		m_y.clear();
		GenerateDiffusionTrajectory();
	}

}
/***********************************************************/
void TrajectoryDiffusion::GenerateDiffusionTrajectory() {

	triple trialpos;

	int ObjectIndex=-1,ShellIndex=-1;
	int TrialObjID=-1, TrialShellID=-1;

	do {
		trialpos.x=m_rng->uniform(-m_microstruct->GetBoxSize(),m_microstruct->GetBoxSize());
		trialpos.y=m_rng->uniform(-m_microstruct->GetBoxSize(),m_microstruct->GetBoxSize());
		trialpos.z=m_rng->uniform(-m_microstruct->GetBoxSize(),m_microstruct->GetBoxSize());
		m_microstruct->IndexObject(trialpos,ObjectIndex,ShellIndex);
	} while (!((m_mode==BOTH) | ((ObjectIndex==-1)& (m_mode==EXTERNAL) ) | ((ObjectIndex>-1)& (m_mode==INTERNAL) )  ));

	m_time.push_back(0.0);
	m_pos.push_back(trialpos);


	SequenceTree* pSeqTree = World::instance()->pSeqTree;
	double seqDuration = pSeqTree->GetRootConcatSequence()->GetDuration();

	long steps = (int) ((seqDuration+0.01)/m_timestep)+2;
	if (steps > m_max_timesteps) {
		m_timestep = seqDuration/m_max_timesteps;
		cout << "Sequence too long; increasing Delta t to: " <<m_timestep << "ms." << endl;
	}

	// diffusion direction for 1-d and plane normal for 2-d diffusion:
	triple diff_dir;
	double norm;
	do{
		diff_dir.x=m_rng->uniform(-1.0,1.0);
		diff_dir.y=m_rng->uniform(-1.0,1.0);
		diff_dir.z=m_rng->uniform(-1.0,1.0);
		norm = sqrt(diff_dir.x*diff_dir.x + diff_dir.y*diff_dir.y + diff_dir.z*diff_dir.z);
	} while (norm > 1.0); //sample from sphere, not from box.
	diff_dir.x/=norm;
	diff_dir.y/=norm;
	diff_dir.z/=norm;
	// end diff_dir


	double sigma;
	sigma = sqrt(2*m_microstruct->GetD(ObjectIndex,ShellIndex)*m_timestep);
	int i=1;
	while (m_time.back()<=seqDuration) {
		bool accept;
		do {
			trialpos.x=m_pos[i-1].x + m_rng->normal(0,sigma);
			trialpos.y=m_pos[i-1].y + m_rng->normal(0,sigma);
			trialpos.z=m_pos[i-1].z + m_rng->normal(0,sigma);
			if (m_diff_dimension!=3) {ProjectPosition(trialpos,diff_dir);}
			accept = m_microstruct->isInsideBox(trialpos);

			// compartment exchange probability could be implemented here:
			if (accept) {
				TrialObjID = ObjectIndex;TrialShellID = ShellIndex;
				m_microstruct->IndexObject(trialpos,TrialObjID,TrialShellID);
// Currently, spins are not allowed to leave their object: if you want an exchange between objects, sigma needs to be recalculated for the new Diffusion Constant
			if (!((TrialObjID == ObjectIndex) && (TrialShellID == ShellIndex )))
					accept = false; //*/
			}
// with exchange comment out the "if" statement and uncomment next line:
//				sigma = sqrt(2*m_microstruct->GetD(TrialObjID,TrialShellID)*m_timestep);

		} while (!accept);
		m_time.push_back(m_time.back()+m_timestep);
		m_pos.push_back(trialpos);
		i++;
	}
	CalcY();
	static int counter=0;
	for (unsigned int i=0; i<m_dump_index.size();i++) {
		if (counter == m_dump_index[i]) {
	        stringstream sstr;
	        sstr << "trajectory_spin" << setw(5) << setfill('0') << counter << ".dat";
	      	DumpTrajectory(sstr.str());
		}
	}
	counter++;

}
/***********************************************************/
void TrajectoryDiffusion::CalcY() {
	double p,sig;
	vector<double> store;
	// init store to length of pos:
	store.resize(m_pos.size());
	m_y.resize(m_pos.size());
	store.back()= 0;
	store[0] 	= 0;
	m_y[0].x	= 0;
	m_y.back().x= 0;

	for (int i = 1; i<=m_y.size()-2;i++) {
		sig = (m_time[i]-m_time[i-1])/(m_time[i+1]-m_time[i-1]);
		p 	= sig*m_y[i-1].x+2.0;
		m_y[i].x = (sig-1.0)/p;
		store[i] = (m_pos[i+1].x - m_pos[i].x)/(m_time[i+1]-m_time[i]) - (m_pos[i].x - m_pos[i-1].x)/(m_time[i]-m_time[i-1]);
		store[i] = (6.0*store[i]/(m_time[i+1]-m_time[i-1]) - sig*store[i-1])/p;
	}

	for (int k=m_y.size()-2;k>=0;k--) {
		m_y[k].x = m_y[k].x*m_y[k+1].x+store[k];
	}

	store.back()= 0;
	store[0] 	= 0;
	m_y[0].y	= 0;
	m_y.back().y= 0;

	for (int i = 1; i<=m_y.size()-2;i++) {
		sig = (m_time[i]-m_time[i-1])/(m_time[i+1]-m_time[i-1]);
		p 	= sig*m_y[i-1].y+2.0;
		m_y[i].y = (sig-1.0)/p;
		store[i] = (m_pos[i+1].y - m_pos[i].y)/(m_time[i+1]-m_time[i]) - (m_pos[i].y - m_pos[i-1].y)/(m_time[i]-m_time[i-1]);
		store[i] = (6.0*store[i]/(m_time[i+1]-m_time[i-1]) - sig*store[i-1])/p;
	}

	for (int k=m_y.size()-2;k>=0;k--) {
		m_y[k].y = m_y[k].y*m_y[k+1].y+store[k];
	}


	store.back()= 0;
	store[0] 	= 0;
	m_y[0].z	= 0;
	m_y.back().z= 0;

	for (unsigned int i = 1; i<=m_y.size()-2;i++) {
		sig = (m_time[i]-m_time[i-1])/(m_time[i+1]-m_time[i-1]);
		p 	= sig*m_y[i-1].z+2.0;
		m_y[i].z = (sig-1.0)/p;
		store[i] = (m_pos[i+1].z - m_pos[i].z)/(m_time[i+1]-m_time[i]) - (m_pos[i].z - m_pos[i-1].z)/(m_time[i]-m_time[i-1]);
		store[i] = (6.0*store[i]/(m_time[i+1]-m_time[i-1]) - sig*store[i-1])/p;
	}

	for (int k=m_y.size()-2;k>=0;k--) {
		m_y[k].z = m_y[k].z*m_y[k+1].z+store[k];
	}


}

/***********************************************************/
void TrajectoryDiffusion::GetValueDerived(double time, double *pos) {

	int ilo;
	double a,b,step;
	ilo= GetLowerIndex(time);
	step = m_time[ilo+1] - m_time[ilo];
	b = (time - m_time[ilo])/step;

/*		// linear interpolation:
	*(xpos) = m_x_ne[ilo] + ((m_x_ne[ilo + 1] - m_x_ne[ilo]) * b);
	*(ypos) = m_y_ne[ilo] + ((m_y_ne[ilo + 1] - m_y_ne[ilo]) * b);
	*(zpos) = m_z_ne[ilo] + ((m_z_ne[ilo + 1] - m_z_ne[ilo]) * b);
*/
	a = 1.0 - b;
	// cubic splines interpolation
	step *= step;
/*		*(xpos) = a*m_x_ne[ilo] + b*m_x_ne[ilo + 1] + ((a*a*a-a)*m_x_ne_y2[ilo] + (b*b*b-b)*m_x_ne_y2[ilo + 1])*step/6.0;
	*(ypos) = a*m_y_ne[ilo] + b*m_y_ne[ilo + 1] + ((a*a*a-a)*m_y_ne_y2[ilo] + (b*b*b-b)*m_y_ne_y2[ilo + 1])*step/6.0;
	*(zpos) = a*m_z_ne[ilo] + b*m_z_ne[ilo + 1] + ((a*a*a-a)*m_z_ne_y2[ilo] + (b*b*b-b)*m_z_ne_y2[ilo + 1])*step/6.0;
*/
	double ta=(a*a*a-a);
	double tb=(b*b*b-b);
	step/=6.0;
	pos[0] += a*m_pos[ilo].x + b*m_pos[ilo + 1].x + (ta*m_y[ilo].x + tb*m_y[ilo + 1].x)*step;
	pos[1] += a*m_pos[ilo].y + b*m_pos[ilo + 1].y + (ta*m_y[ilo].y + tb*m_y[ilo + 1].y)*step;
	pos[2] += a*m_pos[ilo].z + b*m_pos[ilo + 1].z + (ta*m_y[ilo].z + tb*m_y[ilo + 1].z)*step;



}
/***********************************************************/
void TrajectoryDiffusion::DumpTrajectory(string filename) {
	ofstream outFile;
    outFile.open(filename.c_str(), ofstream::out);
    outFile << "% time [ms];  x[mm] y[mm] z[mm]"<<endl;
    for (unsigned int i=0; i<m_time.size(); i++) {
    	outFile << m_time[i] << " "<<m_pos[i].x<< " "<<m_pos[i].y << " "<<m_pos[i].z << endl;
    }
	outFile.close();
	cout << "dump done."<<endl;

}
/***********************************************************/
void TrajectoryDiffusion::ProjectPosition(triple &pos, triple dir){
	// dir needs to be normed!!
	if (m_diff_dimension==1) {
		// (pos - m_pos[0])
		triple D;
		D.x=pos.x-m_pos[0].x;D.y=pos.y-m_pos[0].y;D.z=pos.z-m_pos[0].z;
		double proj;
		proj= D.x*dir.x + D.y*dir.y + D.z*dir.z;
		pos.x=m_pos[0].x + proj*dir.x;
		pos.y=m_pos[0].y + proj*dir.y;
		pos.z=m_pos[0].z + proj*dir.z;

		return;
	}
	if (m_diff_dimension==2) {
		triple D;
		D.x=pos.x-m_pos[0].x;D.y=pos.y-m_pos[0].y;D.z=pos.z-m_pos[0].z;
		double proj;
		proj= D.x*dir.x + D.y*dir.y + D.z*dir.z;

		pos.x=pos.x - proj*dir.x;
		pos.y=pos.y - proj*dir.y;
		pos.z=pos.z - proj*dir.z;


		return;
	}
	cout <<"m_diff_dimension="<<m_diff_dimension<<" is not defined!! Exit!"<<endl;
	exit(-1);

}
