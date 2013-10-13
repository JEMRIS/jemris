/** @file jemris.cpp
 *  @brief Implementation of JEMRIS main executable (sequential version)
 *
 *
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
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

#include "Simulator.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"
#include "config.h"

#ifndef SVN_REVISION
	#define SVN_REVISION "unkown"
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
}

void do_simu (Simulator* sim) {
	World* pW = World::instance();
	cout << "Model    : " << sim->GetAttr(sim->GetElem("model"),  "name")<< "\t  , solver = "
			      << sim->GetAttr(sim->GetElem("model"), "type")  << endl;
	cout << "Sample   : " << sim->GetAttr(sim->GetElem("sample"), "name")<< "\t  , spins  = " << pW->TotalSpinNumber  << endl;
	cout << "TxArray  : " << sim->GetAttr(sim->GetElem("TXcoilarray"), "uri") << endl;
	cout << "RxArray  : " << sim->GetAttr(sim->GetElem("RXcoilarray"), "uri") << endl;
	cout << "Sequence : " << sim->GetAttr(sim->GetElem("sequence"),"uri")<< endl;
	cout << endl;
	sim->Simulate();
	cout << endl;
	cout << endl;
}

int main (int argc, char *argv[]) {
	//print usage
		cout << "\njemris "  << VERSION;
#ifdef GIT_COMMIT
		cout << " (" << GIT_COMMIT << ")";
#endif
        cout << endl;

	if (argc==1) {
		usage();
		return 0;
	}

	string input (argv[1]);

	//CASE 1: Dump list of modules in xml file
	if (input == "modlist")  {
		SequenceTree* seqTree = SequenceTree::instance();
		seqTree->SerializeModules("mod.xml");
		//delete seqTree;
		return 0;
	}

	//CASE 2: try Dump of seq-diagram from Sequence xml-file
	SequenceTree* seqTree = SequenceTree::instance();
	seqTree->Initialize(input);
	if (seqTree->GetStatus()) {
		seqTree->Populate();
		ConcatSequence* seq = seqTree->GetRootConcatSequence();
		seq->SeqDiag("seq.h5");
		seq->DumpTree();
		if (argc==3) seq->WriteStaticXML("jemris_seq.xml");
		//delete seqTree;
		return 0;
	}

	//CASE 3: try simulation from Simulator xml-file
	Simulator sim (input);
	if (sim.GetStatus()) {
		static clock_t runtime = clock();
		do_simu(&sim);
		runtime = clock() - runtime;
		printf ("Actual simulation took %.2f seconds.\n", runtime / 1000000.0);
		return 0;
	}

	//CASE 4: try Dump of sensitivities from CoilArray xml-file
	CoilArray* coils = new CoilArray();
	cout << "dumping sensitivity maps to sensmaps.h5 ...\n";
	coils->Initialize(input);
	if (coils->Populate() == OK) {
		coils->DumpSensMaps(true);
		cout << "done!\n";
		return 0;
	}
	
	//OTHERWISE: not a valid input
	cout << input << " is not a valid input.\n";
	return 0;
}


