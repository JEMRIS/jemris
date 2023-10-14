/** @file sanityck.cpp
 *  @brief Implementation of JEMRIS unit test (santity-check)
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

#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <vector>

#include "Simulator.h"
#include "BinaryContext.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"

using namespace std;

/****************************************************/
void usage () {
	cout << endl << "sanityck usage " << endl << endl;
	cout   << "  sanityck <path_to_example_data> 1 : creates tree-dumps and seq-diagrams for some sequences" << endl;
	cout   << "  sanityck <path_to_example_data> 2 : performs simulation on a small sample for all these sequences" << endl;
	cout   << "  sanityck <path_to_example_data> 3 : creates sensitivity maps" << endl;
	cout   << "  sanityck <path_to_example_data> 4 : exports some sequences in pulseq format for scanner execution" << endl << endl;
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
double compare_hdf5_fields(string file1, string file2, string field){

	double dif = 0.0;
	double sum = 0.0;
	NDData<double> data;
	BinaryContext bc1 (file1, IO::IN);
	BinaryContext bc2 (file2, IO::IN);
    vector<double> v1;
    vector<double> v2;

	if (!file1.length() || !file2.length() ) {
	       cout	<< "\n sanityck warning : can not read binary file  with empty name" << endl;
		return -1.0;
	}


	if (bc1.Status() != IO::OK || bc2.Status() != IO::OK ) {
	       cout	<< "\n sanityck warning : can not read binary files" << endl;
		return -1.0;
	}

	if (bc1.Read(data, field, "/") != IO::OK) {
		cout << "Couldn't read data field " << field  << " in file " << file1 << endl;
		return -1.0;
	}
	v1 = data.Data();

	if (bc2.Read(data, field, "/") != IO::OK) {
		cout << "Couldn't read data field " << field  << " in file " << file1 << endl;
		return -1.0;
	}
	v2 = data.Data();


	if (v1.size() != v2.size()) return -1.0;

	for (int i=0; i<v1.size(); i++) {
		dif += sqrt(pow(v1[i]-v2[i],2));
		sum += 0.5*(fabs(v1[i])+fabs(v2[i]));
	}

	if (sum < 1e-15) return 0.0;
	return 100.0*dif/sum;

}

/****************************************************/
bool CheckSeqs(string path, vector<string> seq){

	bool status=true;

	cout << endl << "Test directory: " << path << endl;
	cout << endl << "Test Case 1: producing tree-dumps and sequence diagrams" << endl;
	cout << "======================================================="<< endl << endl;


	for (unsigned int i=0;i<seq.size();i++) {

		SequenceTree seqTree;
		seqTree.Initialize(path+seq[i]);

		if (seqTree.GetStatus()) {

			seqTree.Populate();
			ConcatSequence* CS = seqTree.GetRootConcatSequence();
			printf("%02d. %18s | ",i+1,seq[i].c_str());

			//sequence-diagram
			string binfile = seq[i];
			binfile.replace(binfile.find("xml",0),3,"h5");
			CS->SeqDiag(path+binfile);

			printf("%18s (seq-diag) ",binfile.c_str());
			double d = compare_hdf5_fields(path+binfile,path+"approved/"+binfile,"/seqdiag/T") ;

			if ( d < 0.0 ) {
				status = false;
				cout << "is NOT ok (#TPOIS differs!) | ";
			} else {
				d += compare_hdf5_fields(path+binfile,path+"approved/"+binfile,"/seqdiag/RXP") ;
				d += compare_hdf5_fields(path+binfile,path+"approved/"+binfile,"/seqdiag/TXM") ;
				d += compare_hdf5_fields(path+binfile,path+"approved/"+binfile,"/seqdiag/TXP") ;
				d += compare_hdf5_fields(path+binfile,path+"approved/"+binfile,"/seqdiag/GX")  ;
				d += compare_hdf5_fields(path+binfile,path+"approved/"+binfile,"/seqdiag/GY")  ;
				d += compare_hdf5_fields(path+binfile,path+"approved/"+binfile,"/seqdiag/GZ")  ;
			if (d > 0.1 ) {
				status = false;
				printf("is NOT ok (e=%7.3f %%)       | ",d);
			} else
				printf("is ok (e=%7.3f %%)           | ",d);
			}

			//sequence-tree
			string outfile = seq[i];
			outfile.replace(outfile.find("xml",0),3,"out");
			CS->DumpTree(path+outfile);
			printf("%18s (tree-dump) ",outfile.c_str());

			if (compare_text_files(path+outfile,path+"approved/"+outfile))
				cout << "is ok " << endl;
			else {
				status = false;
				cout << "is NOT ok " << endl;
			}
		}
	}

	return status;

}

/****************************************************/
bool CheckSigs(string path, vector<string> seq){

	bool status=true;

	cout << endl << "Test directory: " << path << endl;
	cout << endl << "Test Case 2: simulating MR signals" << endl;
	cout << "========================================"<< endl << endl;


	for (unsigned int i=0;i<seq.size();i++) {

		Simulator sim(  path+"/approved/simu.xml",path+"/approved/sample.h5",
			            path+"/approved/uniform.xml",path+"/approved/uniform.xml",
			            path+seq[0],"CVODE");

		if (!sim.GetStatus()) {
			cout << "can not initialize Simulator. exit.\n";
			return false;
		}
		printf("%02d. %18s | ",i+1,seq[i].c_str());

		sim.SetSequence(path+seq[i]);
		string binfile = seq[i];
		binfile.replace(binfile.find(".xml",0),4,"");
		binfile +="_signal";
		sim.GetRxCoilArray()->SetSignalPrefix(path+binfile);
		sim.GetModel()->SetDumpProgress(false);
		sim.Simulate();

		printf("%20s (sig-simu) ",binfile.c_str());

		double d = compare_hdf5_fields(path+binfile+".h5",path+"approved/"+binfile+".h5","/signal/times") ;

		if ( d < 0.0 ) {
			status = false;
			cout << "is NOT ok (#ADCs differs!)" << endl;
		} else {
			d += compare_hdf5_fields(path+binfile+".h5",path+"approved/"+binfile+".h5","/signal/channels/00") ;
			if (d > 0.1 ) {
				status = false;
				printf("is NOT ok (e=%7.4f %%) \n",d);
			} else
				printf("is ok (e=%7.4f %%) \n",d);
		}
	}
	return status;

}

/****************************************************/
bool CheckSens(string path, vector<string> coils){

	bool          status = true;

	cout << endl << "Test directory: " << path << endl;
	cout << endl << "Test Case 3:  dump coil sensitivities" << endl;
	cout << "========================================"<< endl << endl;


	for (unsigned int i=0;i<coils.size();i++) {

		CoilArray ca;
		ca.Initialize(path+coils[i]);
		string binfile = coils[i];
		binfile.replace(binfile.find(".xml",0),4,"");
		ca.SetSenMaplPrefix(path+binfile);
		int out = ca.Populate();
		status = ( (out==0) && status );
		out = ca.DumpSensMaps(false);
		status = ( (out==0) && status) ;

		printf("%02d. %15s | %18s (sens-dump) ",i+1,coils[i].c_str(),binfile.c_str());

		double d = 0.0;
		for (unsigned int j=0; j<ca.GetSize(); j++) {
			stringstream sstr;
			sstr << setw(2) << setfill('0') << j;
			double m = compare_hdf5_fields(	path+binfile+".h5",
										path+"approved/"+binfile+".h5",
										"/maps/magnitude/"+sstr.str()) ;
			double p = compare_hdf5_fields(	path+binfile+".h5",
										path+"approved/"+binfile+".h5",
										"/maps/phase/"+sstr.str()) ;
			if ( m < 0.0 || p < 0.0) {
				status = false;
				cout << "is NOT ok (# grid points differs!)" << endl;
				break;
			}
			d += m+p;
		}

		if (d > 0.1 ) {
			status = false;
			printf("is NOT ok (e=%7.4f %%) \n",d);
		} else
			printf("is ok (e=%7.4f %%) \n",d);
	}

	return status;

}

/****************************************************/
bool CheckOutput(string path, vector<string> seq){

	bool status=true;

	cout << endl << "Test directory: " << path << endl;
	cout << endl << "Test Case 4: output sequence for hardware execution" << endl;
	cout << "======================================================="<< endl << endl;


	for (unsigned int i=0;i<seq.size();i++) {

		SequenceTree seqTree;
		seqTree.Initialize(path+seq[i]);

		if (seqTree.GetStatus()) {

			seqTree.Populate();
			ConcatSequence* CS = seqTree.GetRootConcatSequence();
			printf("%02d. %15s | ",i+1,seq[i].c_str());

			//sequence-diagram
			string seqfile = seq[i];
			seqfile.replace(seqfile.find("xml",0),3,"seq");
			map<string,string> defs;
			CS->OutputSeqData (defs, path, seqfile);

			printf("%15s (seq-output) ",seqfile.c_str());

			if (compare_text_files(path+seqfile,path+"approved/"+seqfile))
				cout << "is ok " << endl;
			else {
				status = false;
				cout << "is NOT ok " << endl;
			}
		}
	}

	return status;

}

/****************************************************/
int main (int argc, char *argv[]) {

	if (argc<3) { usage();  return 0; };

	string path (argv[1]);
	string input(argv[2]);
	path += '/';

	//sequences to test
	vector<string> seq;
	seq.push_back("ThreePulses.xml");
	seq.push_back("epi.xml");
	seq.push_back("gre.xml");
	seq.push_back("tse.xml");
	seq.push_back("analytic.xml");
	seq.push_back("radial.xml");
	seq.push_back("radial2.xml");
	seq.push_back("spiral.xml");
	seq.push_back("sli_sel.xml");
	seq.push_back("var_dur.xml");
	seq.push_back("extpulses.xml");
	seq.push_back("epi_modular.xml");
	seq.push_back("trapezoid.xml");
	seq.push_back("eddycurrents.xml");

	//sequences to output for scanner execution
	vector<string> outseq;
	outseq.push_back("gre.xml");
	outseq.push_back("epi.xml");
	outseq.push_back("trapezoid.xml");
	outseq.push_back("sli_sel.xml");
	outseq.push_back("radial.xml");

	//coils to test
	vector<string> coils;
	coils.push_back("8chheadcyl.xml");
	coils.push_back("1chext.xml");

	bool          status = true;

	switch (atoi(input.c_str())) {
	case(1): status = CheckSeqs(path,seq); break;	//test sequence diagrams for all sequences
	case(2): status = CheckSigs(path,seq); break;	//test signal simulations for all sequences
	case(3): status = CheckSens(path,coils); break;	//test sensitivity maps for all coils
	case(4): status = CheckOutput(path,outseq); break;  //test sequence output for execution
	default: cout << "\nsanityck: unknown input\n\n"; break;
	}

	//return exit code
	if ( status )	return 0;

	cout << "Warning: ERROR(S) occurred!" << endl;
	return 1;

}


