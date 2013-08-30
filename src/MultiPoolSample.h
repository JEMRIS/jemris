#ifndef MULTI_POOL_SAMPLE_H_
#define MULTI_POOL_SAMPLE_H_

#include "Sample.h"

class MultiPoolSample : public Sample {

public:

	MultiPoolSample() : m_noofpools(1) {};
	~MultiPoolSample() {};


	MultiPoolSample (const string& fname);
	MultiPoolSample (const long   l);

    /**
     * init variables which are same for all constructors
     *
     */
    virtual void Prepare (const std::string& fname = "");

	
	int  GetNoOfPools ();
	void GetExchangeMatrix (long l, double** em);

	virtual IO::Status Populate (const std::string& fname);

	virtual void CropEnumerate ();
	
private:

	int      m_noofpools;

};

#endif 
