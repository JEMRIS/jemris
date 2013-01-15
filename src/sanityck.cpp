/** @file sanityck.cpp
 *  @brief Implementation of JEMRIS unit test (santity-check)
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
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

#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <vector>

#include "Simulator.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"

using namespace std;

/****************************************************/
void usage () {
  cout << endl << "sanityck usage " << endl << endl;
	cout   << "  sanityck <path_to_example_data> 1 : creates tree-dumps and seq-diagrams for some sequences" << endl;
	cout   << "  sanityck <path_to_example_data> 2 : performs simulation on a small sample for all these sequences " << endl;
	cout   << "  sanityck <path_to_example_data> 3 : same as 3, but parallel simulation (should be started wth mpirun)" << endl;
	cout   << "  sanityck <path_to_example_data> 4 : creates sensitivity maps" << endl << endl;
}

/****************************************************/
bool compare_text_files (string file1, string file2) {

	//read file1 in string
	string    s1("");
	ifstream IF1(file1.c_str());

	istream_iterator<string> DataBegin1( IF1 );
	istream_iterator<string> DataEnd1;

	while( DataBegin1 != DataEnd1 ) {
		s1 += *DataBegin1;
		DataBegin1++;
	}

	//read file2 in string

	string    s2("");
	ifstream IF2(file2.c_str());

	istream_iterator<string> DataBegin2( IF2 );
	istream_iterator<string> DataEnd2;

	while( DataBegin2 != DataEnd2 ) {
		s2 += *DataBegin2;
		DataBegin2++;
	}

	return (s1 == s2);
}

/****************************************************/
double compare_binary_files(string file1, string file2){

	double dif = 0.0;
	double sum = 0.0;
	double d1,d2;

	ifstream IF1(file1.c_str(),ios::binary);
	ifstream IF2(file2.c_str(),ios::binary);

	while(!IF1.eof() && !IF2.eof()) {

		IF1.read((char*)(&(d1)), sizeof(double));
		IF2.read((char*)(&(d2)), sizeof(double));
		dif += sqrt(pow(d1-d2,2));
		sum += (fabs(d1)+fabs(d2));
	}

	double error = 100.0*dif/sum;

	if (!IF1.eof() || !IF2.eof()) error = -1.0;

	IF1.close();
	IF1.close();

	return error;

}

/****************************************************/
int main (int argc, char *argv[]) {

	if (argc<3) { usage();  return 0; };

	string path (argv[1]);
	string input(argv[2]);
	path += '/';

	vector<string> seq;

	seq.push_back("ThreePulses.xml");
	seq.push_back("epi.xml");
	seq.push_back("gre.xml");
	seq.push_back("tse.xml");
	seq.push_back("analytic.xml");
	seq.push_back("radial.xml");
	seq.push_back("sli_sel.xml");

	vector<string> coils;

	coils.push_back("8chheadcyl.xml");
	coils.push_back("1chext.xml");

	//CASE 1: test sequence diagrams
	if (input == "1") {

		bool          status = true;
		SequenceTree* seqTree;

		cout << endl << "Test directory: " << path << endl;
		cout << endl << "Test Case 1: producing tree-dumps and sequence diagrams" << endl;
		cout << "======================================================="<< endl << endl;

		for (unsigned int i=0;i<seq.size();i++) {
			seqTree = SequenceTree::instance();
			seqTree->Initialize(path+seq[i]);

			if (seqTree->GetStatus()) {

				seqTree->Populate();
				ConcatSequence* CS = seqTree->GetRootConcatSequence();
				printf("%02d. %15s | ",i+1,seq[i].c_str());

				//sequence-diagram
				string binfile = seq[i];
				binfile.replace(binfile.find("xml",0),3,"bin");
				CS->SeqDiag(path+binfile);

				printf("%15s (seq-diag) ",binfile.c_str());
				double d = compare_binary_files(path+binfile,path+"approved/"+binfile) ;

				if ( d < 0.0 ) {
					status = false;
					cout << "is NOT ok (#TPOIS differs!) | ";
				} else if (d > 0.1 ) {
					status = false;
					printf("is NOT ok (e=%7.4f ppm)       | ",d*10);
				} else
					printf("is ok (e=%7.4f ppm)           | ",d*10);
					//cout << "is ok ";

				//sequence-tree
				string outfile = seq[i];
				outfile.replace(outfile.find("xml",0),3,"out");
				CS->DumpTree(path+outfile);
				printf("%15s (tree-dump) ",outfile.c_str());

				if (compare_text_files(path+outfile,path+"approved/"+outfile))
					cout << "is ok " << endl;
				else {
					status = false;
					cout << "is NOT ok " << endl;
				}

			}
			delete seqTree;
			delete World::instance();
		}

		if ( status )
			return 0;
		else
			return 1;

	}

	//CASE 2: perform simulations with all sequences
	if (input == "2") {

		bool          status = true;

		cout << endl << "Test directory: " << path << endl;
		cout << endl << "Test Case 2: producing simulated signals" << endl;
		cout << "========================================"<< endl << endl;

		Simulator sim(	path+"/approved/simu.xml",path+"/approved/sample.h5",
				path+"/approved/uniform.xml",path+"/approved/uniform.xml",
				path+seq[0],"CVODE");

		delete SequenceTree::instance();

		if (!sim.GetStatus()) {
			cout << "can not initialize Simulator. exit.\n";
			return 0;
		}

		for (unsigned int i=0;i<seq.size();i++) {

			sim.SetSequence(path+seq[i]);
			printf("%02d. %15s | ",i+1,seq[i].c_str());

			string binfile = seq[i];
			binfile.replace(binfile.find(".xml",0),4,"");
			sim.GetRxCoilArray()->SetSignalPrefix(path+binfile);
			sim.Simulate();

			delete SequenceTree::instance();

			binfile +="01.bin";
			printf("%18s (sig-simu) ",binfile.c_str());

			double d = compare_binary_files(path+binfile,path+"approved/"+binfile) ;

			if ( d < 0.0 ) {
				status = false;
				cout << "is NOT ok (#ADCs differs!)" << endl;
			} else if (d > 1.0 ) {
				status = false;
				printf("is NOT ok (e=%7.4f ppm) \n",d*10);
			} else
				printf("is ok (e=%7.4f ppm) \n",d*10);

		}

		if ( status )
			return 0;
		else
			return 1;

	}

	if (input == "3") {

		bool          status = true;

		cout << endl << "Test directory: " << path << endl;
		cout << endl << "Test Case 3: producing simulated signals" << endl;
		cout << "========================================"<< endl << endl;

		Simulator sim(	path+"/approved/simu.xml",path+"/approved/sample.h5",
				path+coils[0],path+coils[0],
				path+"epi.xml","CVODE");

		delete SequenceTree::instance();

		if (!sim.GetStatus()) {
			cout << "can not initialize Simulator. exit.\n";
			return 0;
		}

		for (unsigned int i=0;i<coils.size();i++) {

			sim.SetSequence(path+"epi.xml");
			printf("%02d. %15s | ",i+1,coils[i].c_str());

			string binfile = coils[i];
			binfile.replace(binfile.find(".xml",0),4,"");
			sim.GetRxCoilArray()->SetSignalPrefix(path+binfile);
			sim.Simulate();

			delete SequenceTree::instance();

			binfile +="01.bin";
			printf("%18s (sig-simu) ",binfile.c_str());

			double d = compare_binary_files(path+binfile,path+"approved/"+binfile) ;

			if ( d < 0.0 ) {
				status = false;
				cout << "is NOT ok (#ADCs differs!)" << endl;
			} else if (d > 1.0 ) {
				status = false;
				printf("is NOT ok (e=%7.4f ppm) \n",d*10);
			} else
			printf("is ok (e=%7.4f ppm) \n",d*10);

			}

		if ( status )
			return 0;
		else
			return 1;

	}

	return 0;

}


