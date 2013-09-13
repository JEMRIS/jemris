#ifndef __K_SPACE_H__
#define __K_SPACE_H__

template <class T>
struct KS {

    std::vector<T> _data;
    std::vector<size_t> _dims;

    KS (const size_t n) {
        Allocate (n,n);
    }

    Allocate (const std::vector<size_t> dims) {
        _dims = dims;
        _data.resize(prod(dims));
    }
    
};

template <class T>
class KSpace {

public:
    KSpace () {};
    KSpace (const KSpace& ks) {};
    ~KSpace () {};
    
private:
    std::vector<T> m_ks;

};

#endif
