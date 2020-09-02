#include "permutation.hpp"

template <typename T>
Permutation<T>::Permutation() {
}

template <typename T>
Permutation<T>::~Permutation() {
}

template <typename T>
std::map<T,T> Permutation<T>::getFunction() {
    return m_fct;
}

template <typename T>
std::map<T,T> Permutation<T>::getInverseFunction() {
    return m_fct_inv;
}

template <typename T>
std::set<T> Permutation<T>::getModified() {
    return m_modified;
}

template <typename T>
void Permutation<T>::addElement(T const &a) {
    m_fct[a] = a;
    m_fct_inv[a] = a;
}

template <typename T>
void Permutation<T>::applyTransposition(T const &a, T const &b) {

    if(m_fct.find(a) == m_fct.end()) addElement(a);
    if(m_fct.find(b) == m_fct.end()) addElement(b);

    T a_ant = m_fct_inv[a];
    T b_ant = m_fct_inv[b];

    m_fct[a_ant] = b;
    m_fct_inv[b] = a_ant;
    m_fct[b_ant] = a;
    m_fct_inv[a] = b_ant;

    if(m_fct[a] != a) m_modified.insert(a); else m_modified.erase(a);
    if(m_fct[b] != b) m_modified.insert(b); else m_modified.erase(b);
}
