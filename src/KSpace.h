#ifndef __K_SPACE_H__
#define __K_SPACE_H__

#include <cstddef>
#include <vector>


/**
 * @brief K-space
 */
template <class T, unsigned D = 3>
class KSpace {

	typedef T KPoint[D];

public:

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
    	_k = k;
    	return *this;
    }

    /**
     * @brief Default destructor
     */
    ~KSpace () {};


private:

    std::vector<KPoint> _k;

};

#endif
