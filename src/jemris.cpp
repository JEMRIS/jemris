/** @file jemris.cpp
 *  @brief Implementation of JEMRIS main executable (sequential version)
 *
 *
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2022  Tony Stoecker
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

#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>

#include "Simulator.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"
#include "config.h"

#ifdef WIN32
 #define MKDIR(S,M)    mkdir(S)
#else
 #define MKDIR(S,M)    mkdir(S,M)
#endif

using namespace std;

/**
 * Print usage information
 */

void usage () {
  cout << endl << "  Usage:" << endl << endl;
	cout   << "  1. jemris <xml-file>        " << endl;
	cout   << "     The file could be either a a) Simulation, b) Sequence, or c) Coilarray file, " << endl;
	cout   << "     so that a) the simultion is performed, b) a sequence diagram is written, " << endl;
	cout   << "     or c) the sensitivity maps are dumped, respectively." << endl << endl;
	cout   << "  2. jemris modlist           " << endl;
	cout   << "     Writes the file mod.xml containing a list of all available modules." << endl << endl;
	cout   << "  Parameters:" << endl;
	cout   << "     -o <output_dir>: Output directory" << endl;
	cout   << "     -f <filename>:   Output filename (without extension)"  << endl;
	cout   << "     -x: Output Pulseq sequence file format (.seq)"  << endl;
	cout   << "     -r: Start reconstruction after simulation (running recon server is required). "  << endl;
	cout   << "     -d <def>=<val>:  Define custom sequence variable for Pulseq file"  << endl;
}

void do_simu (Simulator* sim) {
	World* pW = World::instance();
	cout << "Model    : " << sim->GetAttr(sim->GetElem("model"),  "name")<< "\t  , solver = "
			     	 	  << sim->GetAttr(sim->GetElem("model"), "type")  << endl;
	cout << "Sample   : " << sim->GetAttr(sim->GetElem("sample"), "name")<< "\t  , spins  = " << pW->TotalSpinNumber;
	if (pW->m_noofspincompartments > 1) cout << ", pools = " << pW->m_noofspincompartments;
	cout << endl;
	cout << "TxArray  : " << sim->GetAttr(sim->GetElem("TXcoilarray"), "uri") << endl;
	cout << "RxArray  : " << sim->GetAttr(sim->GetElem("RXcoilarray"), "uri") << endl;
	cout << "Sequence : " << sim->GetAttr(sim->GetElem("sequence"),"uri")<< endl;
	cout << endl;
	sim->Simulate();
	cout << endl;
	cout << endl;
}

void writeProt(string ismrmrd_file, string output_dir, string filename){

	// Writes new ISMRMRD protocol file containing only acquisitions with ADCs
	ISMRMRD::Dataset d_tmp((ismrmrd_file).c_str(), "dataset", false);
	std::string xml;
    d_tmp.readHeader(xml);

	std::remove((output_dir + filename + ".h5").c_str());
	ISMRMRD::Dataset d((output_dir + filename + ".h5").c_str(), "dataset", true);
	d.writeHeader(xml);

	ISMRMRD::Acquisition acq;
	for (int n = 0; n < d_tmp.getNumberOfAcquisitions(); ++n){
		d_tmp.readAcquisition(n, acq);
		if (!acq.isFlagSet(ISMRMRD::ISMRMRD_ACQ_USER1)) // acquistions with USER1 flag have no ADCs
			d.appendAcquisition(acq);
	}

	std::remove(ismrmrd_file.c_str());
}

int main (int argc, char *argv[]) {

	//print usage
		cout << "\njemris "  << VERSION;
#ifdef GIT_COMMIT
		cout << " (" << GIT_COMMIT << ")";
#endif
        cout << "\n" << endl;

	if (argc==1) {
		usage();
		return 0;
	}

	string output_dir("");
	string filename("");
	string definition, def_name, def_val;
	std::size_t pos;
	map<string,string> scan_defs;
	bool export_seq=false;
	bool recon=false;
	opterr = 0;
	int status;

	int c;
	while((c = getopt (argc, argv, "f:o:d:xr")) != -1)
	{
		switch (c)
		{
		case 'o':
			output_dir = optarg;
			output_dir += "/";
			status = MKDIR(output_dir.c_str(), 0777);
			if(status && errno != EEXIST)
			{
				cerr << "mkdir failed: Could not create output directory: "
						<< output_dir << endl;
				return 1;
			}
			break;
		case 'f':
			filename = optarg;
			break;
		case 'x':
			export_seq=true;
			break;
		case 'r':
			recon=true;
			break;
		case 'd':
			definition = optarg;
			pos = definition.find("=");
			if (pos==std::string::npos) {
				cerr << "error: Custom scan definitions must be in format: -d <name>=<val>" << endl;
				return 1;
			}
			def_name = definition.substr(0,pos);
			def_val = definition.substr(pos+1);
			scan_defs[def_name] = def_val;
			break;
		case '?':
			if (optopt == 'o')
				cerr << "Option '-o' requires an argument." << endl;
			if (optopt == 'f')
				cerr << "Option '-f' requires an argument." << endl;
			if (optopt == 'd')
				cerr << "Option '-d' requires an argument." << endl;
			else if (isprint(optopt))
				cerr << "Unknown option '-" << (char)optopt << "'." << endl;
			else
				cerr << "Unknown option character '-" << (char)optopt << "'." << endl;
			return 1;
		default:
			abort();
		}
	}

	string input (argv[optind]);

	//CASE 1: Dump list of modules in xml file
	if (input == "modlist")  {
		if(filename == "")
			filename = "mod.xml";
		else
			filename += ".xml";
		SequenceTree seqTree;
		seqTree.SerializeModules("mod.xml");
		return 0;
	}

	//CASE 2: try Dump of seq-diagram from Sequence xml-file or write Pulseq sequence & protocol
	try {
		SequenceTree seqTree;
		seqTree.Initialize(input);
		if (seqTree.GetStatus()) {
			seqTree.Populate();
			ConcatSequence* seq = seqTree.GetRootConcatSequence();
			seq->DumpTree();

			if (!export_seq){
				if(filename == "")
					filename = "seq";
				seq->SeqDiag(output_dir + filename + ".h5");
			}
			else{
				if (filename == "")
					filename = "external";
				// Pulseq sequence file
				seq->OutputSeqData(scan_defs, output_dir, filename + ".seq");

				// ISMRMRD file
				string ismrmrd_tmp = output_dir + filename + "_ismrmrd_tmp.h5";
				World* pW = World::instance();
				pW->pSeqTree = &seqTree;
				bool img_adcs = seq->SeqISMRMRD(ismrmrd_tmp);
				if (!img_adcs)
					cout << "Warning: No Imaging ADCs in sequence - export ISMRMRD file anyways." << endl;			
				writeProt(ismrmrd_tmp, output_dir, filename);
			}
			return 0;
		}
	} catch (...) {

	}

	//CASE 3: try Dump of sensitivities from CoilArray xml-file
	try {
		CoilArray ca;
		if(filename != "")
			ca.SetSenMaplPrefix(filename);
		ca.SetSenMapOutputDir(output_dir);

		ca.Initialize(input);
		if (ca.Populate() == OK) {
			ca.DumpSensMaps(true);
			return 0;
		}
	} catch (...) {

	}

	//CASE 4: try simulation from Simulator xml-file
	try {
		Simulator sim (input);
		if (sim.GetStatus()) {
			sim.SetOutputDir(output_dir);
			if(filename != "")
				sim.SetSignalPrefix(filename);
			static clock_t runtime = clock();
			do_simu(&sim);
			runtime = clock() - runtime;
			printf ("Actual simulation took %.2f seconds.\n", runtime / 1000000.0);

			// Recon, if available
			if (recon){
				try{
					printf ("Starting reconstruction.\n");
					struct timeval begin, end;
					gettimeofday(&begin, 0);
					string infile = sim.GetRxCoilArray()->GetSignalOutputDir() + sim.GetRxCoilArray()->GetSignalPrefix() + "_ismrmrd.h5";
					string outfile = sim.GetRxCoilArray()->GetSignalOutputDir() + sim.GetRxCoilArray()->GetSignalPrefix() + "_ismrmrd_recon.h5";
					remove(outfile.c_str());
					string cmd = "client.py -c bart_jemris " + infile + " -o " + outfile + " -G images";
					string conda_cmd = "conda run -n ismrmrd_client";
					string docker_cmd = "docker run -d --user $(id -u):$(id -g) -p 9002:9002 mavel101/bart-reco-server";
					int err;

					// Executing reconstruction
					printf ("Try to automatically start reconstruction server.\n");
					err = system(docker_cmd.c_str());
					if (err){
						printf ("Automatic start of reconstruction server failed. Try to start reconstruction anyways.\n");
						printf ("Try to detect conda environment with ISMRMRD client.\n");
						err = system(conda_cmd.c_str());
						if (err){
							std::cout << "Conda environment not detected. Try to start reconstruction anyways." << std::endl;
							err = system(cmd.c_str());
							if (err)
								std::cout << "Reconstruction failed. Check if ISMRMRD client is installed and if reconstruction server is running." << std::endl;
						}
						else{
							std::cout << "Conda environment detected." << std::endl;
							err = system((conda_cmd+" "+cmd).c_str());
							if (err)
								std::cout << "Reconstruction failed. Check if ISMRMRD client is installed and if reconstruction server is running." << std::endl;
						}
					}
					else{
						printf ("Reconstruction server started.\n");
						printf ("Try to detect conda environment with ISMRMRD client.\n");
						string conda_cmd = "conda run -n ismrmrd_client";
						err = system(conda_cmd.c_str());
						if (err){
							std::cout << "Conda environment not detected. Try to start reconstruction anyways." << std::endl;
							err = system(cmd.c_str());
							if (err)
								std::cout << "Reconstruction failed. Check if ISMRMRD client is installed and if reconstruction server is running." << std::endl;
						}
						else{
							std::cout << "Conda environment detected." << std::endl;
							err = system((conda_cmd+" "+cmd).c_str());
							if (err)
								std::cout << "Reconstruction failed. Check if ISMRMRD client is installed and if reconstruction server is running." << std::endl;
						}
						string docker_kill_cmd = "docker kill `docker ps -qf \"ancestor=mavel101/bart-reco-server\"`";
						err = system(docker_kill_cmd.c_str());
						if (err)
							std::cout << "Reconstruction server could not be killed." << std::endl;
						else
							std::cout << "Reconstruction server killed." << std::endl;
					}

					gettimeofday(&end, 0);
					long sec = end.tv_sec - begin.tv_sec;
					long usec = end.tv_usec - begin.tv_usec;
					double elapsed = sec + usec*1e-6;
					printf ("Reconstruction took %.2f seconds.\n", elapsed);
				}
				catch (...) {

				}
			}
			return 0;
		}
	} catch (...) {

	}


	//OTHERWISE: not a valid input
	cout << input << " is not a valid input.\n";

        return 0;
}


