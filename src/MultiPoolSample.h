#ifndef MULTI_POOL_SAMPLE_H_
#define MULTI_POOL_SAMPLE_H_

#include "Sample.h"

class MultiPoolSample : public Sample {

public:

	MultiPoolSample() {};
	~MultiPoolSample() {};


	MultiPoolSample (string fname);
	MultiPoolSample (long   l);

    /**
     * init variables which are same for all constructors
     *
     */
    virtual void Prepare (std::string fname = "");

	
	int  GetNoOfPools ();
	void GetExchangeMatrix (long l, double** em);

	virtual IO::Status Populate (std::string fname);

	virtual void CropEnumerate ();
	
private:

	int      m_noofpools;

};

#endif 
