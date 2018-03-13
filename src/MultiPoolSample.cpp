#include "MultiPoolSample.h"
#include "BinaryContext.h"

MultiPoolSample::MultiPoolSample (const std::string& fname) {

	Prepare(fname);
	CropEnumerate();

} 


MultiPoolSample::MultiPoolSample (const long size): m_noofpools (1) { Prepare(); }


IO::Status MultiPoolSample::Populate (const std::string& fname) {

	// Doing 
	Sample::Populate (fname);

	BinaryContext bc (fname, IO::IN);
	if (bc.Status() != IO::OK)
		return bc.Status();

	NDData<double>      di;
	IO::Status    ios;

	bc.Read(di, "exchange", "/");
	if (bc.Status() != IO::OK)
		return bc.Status();

	m_helper = di.Data();

	if (GetNoOfPools() != sqrt(di.Size())) {
		cout << "Error in MultiPoolSample::Populate() - exchange matrix does not fit number of pools" << endl;
		return IO::UNMATCHED_DIMENSIONS;
	}

	cout << "MPsample: #Pools= " << GetNoOfPools() << " , SampleDims=" << GetSampleDims().size() << " , SampleProps= " << GetNProps() << " , SampleSize=" << GetSize()
		 << " , ExMatDim=" << di.NDim() << " , ExMat[0]=" << m_helper[0] << endl << endl;

	m_noofpools = GetNoOfPools(); /*  this number is not used anywhere! (?) */

	return IO::OK;

}



void MultiPoolSample::Prepare (const std::string& fname) {

	Sample::Prepare(fname);

}





void MultiPoolSample::CropEnumerate () {
	
	Sample::CropEnumerate ();
	return;
/*
	int  nsize = 0;
	long osize = m_ensemble.NSpins();
 	int nprops = m_ensemble.NProps();

	double* tmp = (double*) malloc (osize * nprops * sizeof(double));
	memcpy (tmp, m_ensemble.Data(), osize * nprops * sizeof(double));

	for (int i = 0; i < osize; i++) {

		int nonzero = 0;
		for (int j = 0; j < m_no_spin_compartments; j++)
			if (tmp[i* nprops + j*m_no_spin_compartments + M0] > 0)
				nonzero++;
		
		if (nonzero > 0)
			nsize++;

	}

	m_ensemble.ClearData();
	m_ensemble.Init(nsize);

	int n = 0;
	for (int i = 0; i < osize; i++) {

		int nonzero = 0;
		for (int j = 0; j < m_no_spin_compartments; j++)
			if (tmp[i* nprops + j*m_no_spin_compartments + M0] > 0)
				nonzero++;


		if (nonzero > 0) {

			long npos = n * nprops;

			memcpy (&m_ensemble[npos], &tmp[i * nprops] , nprops * sizeof(double));
			m_ensemble[npos + nprops - 1] = n;
			m_spin_state.push_back(0);
			n++;

		}

	}

	free (tmp);
*/
}

