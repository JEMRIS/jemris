/** @file pjemris.cpp
 *  @brief Implementation of JEMRIS main executable (parallel version)
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

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

#include "Simulator.h"
#include "mpi_Model.h"
#include "config.h"
#include "Mpi2Evolution.h"

#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>

#ifdef WIN32
 #define MKDIR(S,M)    mkdir(S)
#else
 #define MKDIR(S,M)    mkdir(S,M)
#endif


int main (int argc, char *argv[]) {
  
	//init MPI
#ifdef HAVE_MPI_THREADS
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
#else
	MPI_Init(&argc, &argv);
#endif
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	World* pW = World::instance();
	pW->m_myRank = my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &pW->m_no_processes);
	int master=0, tag=42;
	double t1 = MPI_Wtime();

  string output_dir("");
  string filename("");

  opterr = 0;
  int status;

  int c;
  while((c = getopt (argc, argv, "f:o:")) != -1)
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
      case '?':
        if (optopt == 'o')
          cerr << "Option '-o' requires an argument." << endl;
        if (optopt == 'f')
          cerr << "Option '-f' requires an argument." << endl;
        else if (isprint(optopt))
          cerr << "Unkown option '-" << (char)optopt << "'." << endl;
        else
          cerr << "Unkown option character '-" << (char)optopt << "'." << endl;
        return 1;
      default:
        abort();
    }
  }

	// read simulator settings. Slaves do not read the sample !!!
	string input="simu.xml";
	if (argc>1) {
		input = string(argv[optind]);
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
		MPI_Barrier(MPI_COMM_WORLD);
		cout << "Input '" << input << "' is not a valid Simulation xml-file." << endl;
		MPI_Finalize();
		return 0;
	}

	//MASTER: writes seq-file, Dump seq-report, and sends the sample
	if ( my_rank == master) {
		cout << "\nParallel jemris " << VERSION << " "
#ifdef GIT_COMMIT
             << "(" << GIT_COMMIT << ")" 
#endif
             <<"\n\n";
		cout << "Model    : " << psim->GetAttr(psim->GetElem("model"),  "name")<< "\t  , solver = "
		     << psim->GetAttr(psim->GetElem("model"), "type")  << endl;
		cout << "Sample   : " << psim->GetAttr(psim->GetElem("sample"), "name")<< "\t  , spins  = " << World::instance()->TotalSpinNumber  << endl;
		cout << "TxArray  : " << psim->GetAttr(psim->GetElem("TXcoilarray"), "uri") << endl;
		cout << "RxArray  : " << psim->GetAttr(psim->GetElem("RXcoilarray"), "uri") << endl;
		cout << "Sequence : " << psim->GetAttr(psim->GetElem("sequence"),"uri")<< endl;
		CoilArray* RxCA = psim->GetRxCoilArray();
		RxCA->InitializeSignals( psim->GetSequence()->GetNumOfADCs() );
		psim->CheckRestart();
		Mpi2Evolution::OpenFiles((int) psim->GetSample()->IsRestart());
		// returns when last spin is simulated; collects signals:
		mpi_devide_and_send_sample( psim->GetSample(), psim->GetRxCoilArray() );
		// set output directory
		RxCA->SetSignalOutputDir(output_dir);
		if (filename != "")
			// set output name
			RxCA->SetSignalPrefix(filename);
		RxCA->DumpSignals();
		psim->DeleteTmpFiles();
	}

	//SLAVES: receives the (sub)sample, Simulate model, then sends (sub)signal(s) of each coil
	if ( my_rank != master) {
		Sample* dummy = new Sample(0);
		psim->SetSample(dummy);
		Mpi2Evolution::OpenFiles ((int) psim->GetSample()->IsRestart());
		pW->saveEvolFunPtr = &Mpi2Evolution::saveEvolution;
		psim->SetSample( mpi_receive_sample(master, tag) );
		psim->GetSample()->InitRandGenerator( my_rank );
		psim->Simulate(false); //false = do not Dump signal to binary file !
		bool SpinsLeft = true;

		while (true) {

			SpinsLeft = mpi_recieve_sample_paket(psim->GetSample(),	psim->GetRxCoilArray());

			if (!SpinsLeft)
				break;

			psim->Simulate(false); // false = do not Dump signal to binary file !

		}
	}

	Mpi2Evolution::CloseFiles();
	delete psim;

	//finished
	MPI_Barrier(MPI_COMM_WORLD);
	double t2 = MPI_Wtime();
	if ( my_rank == master)
		printf ("\n\nActual simulation took %.2f seconds.\n", t2-t1);
	MPI_Finalize();

	return 0;
}


