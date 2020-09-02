#include <map>
#include <set>
#include <vector>

template<typename T>
class Permutation {
private:
    std::map<T,T>  m_fct;
    std::map<T,T>  m_fct_inv;
    std::set<T>    m_modified;

public:
    Permutation();
    ~Permutation();
    std::map<T,T> getFunction();
    std::map<T,T> getInverseFunction();
    std::set<T>   getModified();
    void          addElement(T const &a);
    void          applyTransposition(T const &a,T const &b);
};