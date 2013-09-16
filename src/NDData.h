#ifndef __ND_DATA_H__
#define __ND_DATA_H__

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

template<class T> inline static T _prod (const T a, const T b) {return (a*b);}
template<class T> inline static T prod (const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), (T)1, _prod<T>);
}
template<class T> inline static T sum (const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), (T)0);
}

const static std::string SLASH ("/");
const static std::string DSLASH ("//");

/**
 * @brief  Simple nd-data structure.
 */
template<class T>
class NDData {

	std::string         URN; /**< Data name (i.e. sample, sensitivities, signals .... )*/
	std::string         URL; /**< Path      (i.e. Group name in HDF5) not used for SimpleIO */

	std::vector<size_t> dims;  /**< dimensions */
	std::vector<T>      data;
    
    void Allocate () {
		data.resize(GetSize());
	}
    
	size_t GetSize () {
		return prod(dims);
	}
    
	size_t* Dims() {
		return dims.data();
	}
    
	size_t NDim() const {
		return dims.size();
	}
    
	std::ostream& Print (std::ostream& os) {
		os << this->URI().c_str();
		return os;
	}
    
	std::string URI () {
		std::string uri (URL + "/" + URN);
		size_t pos = uri.find(DSLASH);
		while (pos != std::string::npos) {
			uri.replace (pos, 2, SLASH);
			pos = uri.find(DSLASH);
		}
		return uri;
	}

	T& operator[] (const size_t p) {
        return data[p];
    }
    
	T  operator[] (const size_t p) const {
        return data[p];
    }

};


template <class T> inline std::ostream&
operator<< (std::ostream& os, NDData<T>& di) {
	return di.Print(os);
}

#endif
