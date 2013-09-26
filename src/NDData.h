#ifndef __ND_DATA_H__
#define __ND_DATA_H__

#include "TPOI.h"

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
	inline size_t Dim (const size_t n = 0) const { return _dims[n];	}
    
	inline std::vector<size_t> Dims () const { return _dims; }

	inline size_t Size () const { return _data.size(); }
    
	inline size_t NDim() const { return _dims.size(); }
    
	std::ostream& Print (std::ostream& os) const {
		os << "dims(";
		for (size_t i = 0; i < _dims.size(); ++i)
			os << _dims[i] << " ";
		os << ") range(" << minmax(_data) << ")";
		return os;
	}

	inline bool Empty () const { return _data.empty(); }
    
	inline T& operator[] (const size_t p)       { return _data[p]; }
    
	inline T  operator[] (const size_t p) const { return _data[p]; }

	inline T& operator() (const size_t p)       { return _data[p]; }

	inline T  operator() (const size_t p) const { return _data[p]; }

	inline T& operator() (const size_t n0, const size_t n1)
	                                            { return _data[n1*Dim(0)+n0]; }

	inline T  operator() (const size_t n0, const size_t n1) const
	                                            { return _data[n1*Dim(0)+n0]; }

	inline T& operator() (const size_t n0, const size_t n1, const size_t n2) {
		return _data[n2*Dim(0)*Dim(1)+n1*Dim(0)+n0];
	}

	inline T  operator() (const size_t n0, const size_t n1, const size_t n2) const {
		return _data[n2*Dim(0)*Dim(1)+n1*Dim(0)+n0];
	}

	inline const T* Ptr (const size_t n = 0) const { return &_data[n]; }

	inline T* Ptr (const size_t n = 0) { return &_data[n]; }

	inline std::vector<T> Data () const { return _data;	}

};


template <class T> inline static NDData<T>
cumtrapz (const NDData<T>& data,
		std::vector<T>& times = std::vector<T>(),
		std::vector<size_t> meta = std::vector<size_t>()) {

	if (!(times.empty()))
		assert (times.size() == data.Dim(0));
	else
		times = std::vector<T> (data.Dim(0),1.);

	if (!(meta.empty()))
		assert (meta.size() == data.Dim(0));
	else
		meta = std::vector<size_t> (data.Dim(0),0);

	NDData<T> ret (data.Dims());
	size_t ncol = ret.Size()/ret.Dim(0);
	size_t csz  = ret.Dim(0);

	for (size_t i = 0, os = i*csz; i < ncol; ++i)
		for (size_t j = 1; j < csz; ++j) {
			if (check_bit(meta[j], BIT(REFOCUS_T)))
				ret[os+j] = - ret[os+j-1];
			else if (check_bit(meta[j], BIT(EXCITE_T)))
				ret[os+j] =   0.;
			else
				ret[os+j] =   ret[os+j-1];
			ret[os+j] += .5 * (data[os+j] + data[os+j-1]) * (times[os+j] - times[os+j-1]);
		}

	return ret;

}


template <class T> inline static bool
ismatrix (const NDData<T>& data) {
	return data.Dims().size() == 2;
}

template <class T> inline static NDData<T>
transpose (const NDData<T>& data) {
	assert (ismatrix(data));
	NDData<T> ret (data.Dim(1),data.Dim(0));
	for (size_t j = 0; j < data.Dim(1); ++j)
		for (size_t i = 0; i < data.Dim(0); i++)
			ret(j,i) = data(i,j);
	return ret;
}


template <class T> inline std::ostream&
operator<< (std::ostream& os, NDData<T>& di) {
	return di.Print(os);
}

#endif
