#ifndef COGRED_SPARSE_SET_H
#define COGRED_SPARSE_SET_H

#include <vector>
#include <iterator>
#include <limits>
#include <algorithm>

namespace kface {

class SparseSet
{
public:
    typedef std::vector<std::size_t*> direct_access_sequence;
    typedef std::vector<std::size_t>  iteration_sequence;

    direct_access_sequence m_sparse;
    iteration_sequence m_dense;

public:
    typedef std::size_t value_type;
    typedef std::size_t size_type;
    typedef std::size_t key_type;
    typedef iteration_sequence::const_iterator iterator;
    typedef iterator const_iterator;

    typedef iteration_sequence::const_reverse_iterator reverse_iterator;
    typedef reverse_iterator const_reverse_iterator;

    SparseSet(std::size_t size) :m_sparse(size), m_dense()
    {
        m_dense.reserve(size);
    }

    SparseSet() :m_sparse(), m_dense() {}
	 SparseSet(const SparseSet& ss) = delete;
	 void operator = (const SparseSet& ss) = delete;

    void resize(std::size_t size)
    {
        m_sparse.resize(size, nullptr);
        m_dense.reserve(size);
        m_dense.clear();
    }

    void swap(SparseSet& s)
    {
        m_sparse.swap(s.m_sparse);
        m_dense.swap(s.m_dense);
    }

    bool test(std::size_t i) const
    {
        return m_sparse[i] != nullptr;
    }

    bool insert(std::size_t i)
    {
        if (m_sparse[i] != nullptr)
            return false;
        m_dense.push_back(i);
        m_sparse[i] = &m_dense.back();
        return true;
    }

    void erase(std::size_t i)
    {
        if (m_sparse[i] == nullptr)
            return;
        std::size_t* v = m_sparse[i];
        std::size_t p_index = m_dense.back();
        if (*v != p_index)
        {
            *v = p_index;
            m_sparse[p_index] = v;
        }
        m_sparse[i] = nullptr;
        m_dense.erase(m_dense.end() - 1);
    }

    void clear()
    {
        std::fill(m_sparse.begin(), m_sparse.end(), nullptr);
        m_dense.clear();
    }

    std::size_t size() const
    {
        return m_sparse.size();
    }

    std::size_t count() const
    {
        return m_dense.size();
    }

    bool empty() const
    {
        return m_dense.empty();
    }

    iterator begin() const
    {
        return m_dense.begin();
    }

    iterator end() const
    {
        return m_dense.end();
    }

    reverse_iterator rbegin() const
    {
        return m_dense.rbegin();
    }

    reverse_iterator rend() const
    {
        return m_dense.rend();
    }

    iterator lower_bound(value_type i) const
    {
        return std::lower_bound(m_dense.begin(), m_dense.end(), i);
    }

    iterator upper_bound(value_type i) const
    {
        return std::upper_bound(m_dense.begin(), m_dense.end(), i);
    }
};

} /* namespace cogred */

#endif /* ifndef COGRED_SPARSE_SET_H */
