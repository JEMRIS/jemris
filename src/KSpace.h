#ifndef __K_SPACE_H__
#define __K_SPACE_H__

#include "NDData.h"
#include "BinaryContext.h"

#include <cstddef>
#include <vector>


/**
 * @brief K-space
 */
template <class T, unsigned D = 3>
class KSpace {

public:

	struct KPoint {

		T _data[D];

		inline KPoint () {};

		inline
		KPoint (const KPoint& kp) {
			*this = kp;
		}

		inline KPoint&
		operator= (const KPoint& kp) {
			std::copy (kp.CPtr(), kp.CPtr()+D-1, this->Ptr());
			return *this;
		}

		inline T*
		Ptr (const size_t n = 0) {
			return &_data[n];
		}

		inline const T*
		CPtr (const size_t n = 0) const {
			return &_data[n];
		}

		inline const T
		operator[] (const size_t n) const {
			return _data[n];
		}

		inline T
		operator[] (const size_t n) {
			return _data[n];
		}


	};

	/**
	 * @brief Default constructor
	 */
	KSpace ()  {}

	/**
	 * @brief Copy constructor
	 */
	KSpace (const KSpace& ks) {
    	*this = ks;
    }

	/**
	 * @brief Assignement operator
	 */
    KSpace& operator= (const KSpace& k) {
    	_k = k._k;
    	return *this;
    }

    /**
     * @brief Default destructor
     */
    ~KSpace () {};

    /**
     * @brief Append element to end
     */
    void PushBack (const KPoint& p) {
    	_k.push_back(p);
    }

    /**
     * @brief Write k-space to file
     */
    void Write (const std::string& fname, const std::string& urn, const std::string& url = "") {

    	if (_k.empty())
    		return;

    	NDData<T> data (D,_k.size());
    	for (size_t i = 0; i < _k.size(); ++i)
    		std::copy (_k[i].CPtr(), _k[i].CPtr()+end, &data[i*D]);
    	BinaryContext bc (fname, IO::OUT);
    	bc.Write(data, urn, url);
    }


private:

    std::vector<KPoint> _k;
    const static size_t end = D-1;

};

#endif
