#ifndef __ND_DATA_H__
#define __ND_DATA_H__

#include <H5public.h>

#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

template<class T>
struct T_minmax {
	T min;
	T max;
};
template<class T> inline std::ostream&
operator<< (std::ostream& os, const T_minmax<T>& mm) {
	return os << "[" << mm.min << ", " << mm.max << "]";
}

template<class T> inline static T
_prod (const T a, const T b) {return (a*b);}
template<class T> inline static T
prod (const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), (T)1, _prod<T>);
}
template<class T> inline static T
sum (const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), (T)0);
}
template<class T> inline const T_minmax<T>
minmax (const std::vector<T>& v) {
	T_minmax<T> ret;
	for (size_t i = 0; i < v.size(); ++i){
		ret.min = std::min(ret.min,v[i]);
		ret.max = std::max(ret.max,v[i]);
	}
	return ret;
}

const static std::string SLASH ("/");
const static std::string DSLASH ("//");

/**
 * @brief  Simple nd-data structure.
 */
template<class T>
class NDData {


protected:

	std::vector<size_t> _dims;  /**< dimensions */
	std::vector<T>      _data;  /**< actual data */

public:

	/**
	 * @brief Default constructor
	 */
    NDData () {};

    /**
     * @brief 1D-data (convenience constructor)
     *
     * @param n0 Vector length
     */
    NDData (const size_t n0) {
    	_dims.resize(1,n0);
    	_data.resize(prod(_dims));
    }

    /**
     * @brief 2D-data (convenience constructor)
     *
     * @param n0 First side
     * @param n1 Second side
     */
    NDData (const size_t n0, const size_t n1) {
    	_dims.resize(2);
    	_dims[0] = n0;
    	_dims[1] = n1;
    	_data.resize(prod(_dims));
    }

    /**
     * @brief 3D-data (convenience constructor)
     *
     * @param n0 First side
     * @param n1 Second side
     * @param n2 Third side
     */
    NDData (const size_t n0, const size_t n1, const size_t n2) {
    	_dims.resize(3);
    	_dims[0] = n0;
    	_dims[1] = n1;
    	_dims[2] = n2;
    	_data.resize(prod(_dims));
    }

    /**
     * @brief 4D-data (convenience constructor)
	 *
     * @param n0 First side
     * @param n1 Second side
     * @param n2 Third side
     */
    NDData (const size_t n0, const size_t n1, const size_t n2, const size_t n3) {
    	_dims.resize(3);
    	_dims[0] = n0;
    	_dims[1] = n1;
    	_dims[2] = n2;
    	_dims[3] = n3;
    	_data.resize(prod(_dims));
    }

    /**
     * @brief ND-Data
     *
     * @param dims  Side lengths (HDF5 hsize_t)
     */
	NDData (const std::vector<hsize_t>& dims) {
		_dims.resize(dims.size());
		std::reverse_copy (dims.begin(), dims.end(), _dims.begin());
    	_data.resize(prod(_dims));
	}

    /**
     * @brief ND-Data
     *
     * @param dims  Side lengths
     */
	NDData (const std::vector<size_t>& dims) {
		_dims = dims;
    	_data.resize(prod(_dims));
	}

	/**
	 * @brief Copy constructor
	 *
	 * @param data To copy
	 */
	NDData (const NDData& data) {
		*this = data;
	}

	/**
	 * @brief Assignement
	 *
	 * @param data To copy
	 */
	NDData& operator= (const NDData& data) {
		_dims = data._dims;
		_data = data._data;
		return *this;
	}

	/**
	 *
	 */
	inline size_t Dim (const size_t n = 0) const {
		return _dims[n];
	}
    
	inline std::vector<size_t> Dims () const {
		return _dims;
	}

	inline size_t Size () const {
		return _data.size();
	}
    
	inline size_t NDim() const {
		return _dims.size();
	}
    
	std::ostream& Print (std::ostream& os) const {
		os << "dims(";
		for (size_t i = 0; i < _dims.size(); ++i)
			os << _dims[i] << " ";
		os << ") range(" << minmax(_data) << ")";
		return os;
	}

	inline bool Empty () const {
		return _data.empty();
	}
    
	inline T& operator[] (const size_t p) {
        return _data[p];
    }
    
	inline T  operator[] (const size_t p) const {
        return _data[p];
    }

	inline const T*  CPtr (const size_t n = 0) const {
		return &_data[n];
	}

	inline T*  Ptr (const size_t n = 0) {
		return &_data[n];
	}

	inline std::vector<T> Data () const {
		return _data;
	}

};


template <class T>
inline static NDData<T> cumtrapz (const NDData<T>& data,
		const std::vector<T>& times = std::vector<T>()) {

	bool have_t = !(times.empty());

	NDData<T> ret (data.Dims());
	size_t ncol = ret.Size()/ret.Dim(0);
	size_t csz  = ret.Dim(0);

	for (size_t i = 0; i < ncol; ++i) {
		size_t os = i*csz;
		ret[os] = 0.;
		if (have_t)
			for (size_t j = 1; j < csz; ++j)
				ret[os+j] = ret[os+j-1] + data[os+j] * (times[os+j] - times[os+j-1]);
		else
			for (size_t j = 1; j < csz; ++j)
				ret[os+j] = ret[os+j-1] + .5 * (data[os+j] + data[os+j-1]);

	}

	return ret;

}


template <class T> inline std::ostream&
operator<< (std::ostream& os, NDData<T>& di) {
	return di.Print(os);
}

#endif
