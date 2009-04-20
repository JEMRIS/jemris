/** @file pjemris.cpp
 *  @brief Implementation of JEMRIS main executable (parallel version)
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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
#include <string>
#include <iomanip>

using namespace std;

#include "Simulator.h"
#include "mpi_Model.h"
#include "config.h"

#ifndef SVN_REVISION
	#define SVN_REVISION "unkown"
#endif


int main (int argc, char *argv[]) {

	//init MPI
	MPI::Init(argc, argv);
	int my_rank = MPI::COMM_WORLD.Get_rank();
	World* pW = World::instance();
	pW->m_myRank = my_rank;
	int master=0, second=1, tag=42;
	double t1 = MPI::Wtime();

	// read simulator settings. Slaves do not read the sample !!!
	string input="simu.xml";
	if (argc>1) {
		input = string(argv[1]);
		// erase all " characters in string (ASCII code 34). xcerces sometimes has problems with it...
		for (int i=input.size(); i>=0; i--) {
			if (input[i] == 34 ) input.erase(i,1);
		}
	}
	Simulator* psim;
	if ( my_rank == master)	psim = new Simulator(input);
	else			psim = new Simulator(input,"NoSample");

	if ( !psim->GetStatus() ) {
		delete psim;
		MPI::COMM_WORLD.Barrier();
		cout << "Input '" << input << "' is not a valid Simulation xml-file." << endl;
		MPI::Finalize();
		return 0;
	}

	//MASTER: writes seq-file, Dump seq-report, and sends the sample
	if ( my_rank == master) {
		cout << "\nPARALLEL JEMRIS " << VERSION << " r"<< SVN_REVISION <<"\n\n";
		cout << "Model    : " << psim->GetAttr(psim->GetElem("model"),  "name")<< "\t  , solver = "
		     << psim->GetAttr(psim->GetElem("model"), "type")  << endl;
		cout << "Sample   : " << psim->GetAttr(psim->GetElem("sample"), "name")<< "\t  , spins  = " << World::instance()->TotalSpinNumber  << endl;
		cout << "TxArray  : " << psim->GetAttr(psim->GetElem("TXcoilarray"), "uri") << endl;
		cout << "RxArray  : " << psim->GetAttr(psim->GetElem("RXcoilarray"), "uri") << endl;
		cout << "Sequence : " << psim->GetAttr(psim->GetElem("sequence"),"uri")<< endl;
		mpi_devide_and_send_sample( psim->GetSample() , tag );

	}

	mpiEvolution Evolution;
	//SLAVES: receives the (sub)sample, Simulate model, then sends (sub)signal(s) of each coil
	if ( my_rank != master) {

		psim->SetSample( mpi_receive_sample(master, tag) );
		psim->GetSample()->InitRandGenerator( my_rank );
		mpiEvolution SlaveEvolutionSender();
		Evolution.send();
		psim->Simulate(false); //false = do not Dump signal to binary file !
		if (pW->m_useLoadBalancing) {
			bool SpinsLeft = true;
			while (SpinsLeft) {
				SpinsLeft = mpi_recieve_sample_paket(psim->GetSample());
				psim->Simulate(false,false); // false,false = neither dump nor init signal
			}
		}// end use load balancing

		//send signal of all RxCoils
		CoilArray* RxCA = psim->GetRxCoilArray();
		for (int i=0; i < RxCA->GetSize(); i++)
			mpi_send_signal(RxCA->GetCoil(i)->GetSignal(), master, tag);

	}

	//MASTER: receives (and sums) signals of each coil, then writes results to files
	if ( my_rank == master) {
		CoilArray* RxCA = psim->GetRxCoilArray();
		RxCA->InitializeSignals( psim->GetSequence()->GetNumOfADCs() );
		Evolution.Receive();
		for (int i=0; i < RxCA->GetSize(); i++)
			mpi_receive_and_sum_signal(tag, RxCA->GetCoil(i)->GetSignal() );
		RxCA->DumpSignals();
	}

	delete psim;

	//finished
	MPI::COMM_WORLD.Barrier();
	double t2 = MPI::Wtime();
	if ( my_rank == master)
		cout << endl << "MPI Process Wtime = " << t2-t1 << " seconds" << endl << "Finished" << endl << flush;
	MPI::Finalize();

	return 0;
}


