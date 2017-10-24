#ifndef KFACE_PRUNE_VECTOR
#define KFACE_PRUNE_VECTOR
#include "Util.hpp"
#include <vector>
#include <cstddef>
#include <iostream>

namespace kface {

template<typename Value, typename Prune>
class PruneVector {
public:
	using iterator = typename std::vector<Value>::iterator;
	using const_iterator = typename std::vector<Value>::const_iterator;
	PruneVector(Prune&& p): m_prune(p), m_checkpoint(1,0) {}
	PruneVector(const std::vector<Value>& data, Prune&& p): m_data(data), m_prune(p) {}
	PruneVector(std::vector<Value>&& data, Prune&& p): m_data(data), m_prune(p) {}

	iterator begin() { return m_data.begin(); }
	const_iterator begin() const { return m_data.begin(); }
	/* iterator end() { return m_data.end(); } */
	/* const_iterator end() const { return m_data.end(); } */
	iterator end() { return m_data.begin() + m_checkpoint.back(); }
	const_iterator end() const { return (m_data.begin() + m_checkpoint.back()); }

	size_t size() const { return m_checkpoint.back(); }
	size_t rewind() const { return m_rewind; }

	void push_back(const Value& v) { push_back(Value(v)); }
	void push_back(Value&& v) { m_data.push_back(v); ++m_checkpoint[0]; }

	Value& operator [] (size_t i) { return m_data[i]; }

	template<typename Pruner>
	void prune(size_t v, Pruner pruner) {
		m_checkpoint.push_back(m_checkpoint.back());
		for (size_t i = 0; i < m_checkpoint.back(); ++i) {
			if (pruner(v, m_data[i])) {
				--m_checkpoint.back();
				std::swap(m_data[i], m_data[m_checkpoint.back()]);
				--i;
			}
		}
	}
	bool canon_prune(size_t v) {
		m_checkpoint.push_back(m_checkpoint.back());
		for (size_t i = 0; i < m_checkpoint.back(); ++i) {
			int res = m_prune(v, m_data[i]);
			if (res < 0) {
				--m_checkpoint.back();
				std::swap(m_data[i], m_data[m_checkpoint.back()]);
				--i;
			} else if (res > 0) {
				m_rewind = i;
				return false;
			}
		}
		return true;
	}

	bool canon_prune() {
		m_checkpoint.push_back(m_checkpoint.back());
		for (size_t i = 0; i < m_checkpoint.back(); ++i) {
			int res = m_prune(m_data[i]);
			if (res < 0) {
				--m_checkpoint.back();
				std::swap(m_data[i], m_data[m_checkpoint.back()]);
				--i;
			} else if (res > 0) {
				m_rewind = i;
				return false;
			}
		}
		return true;
	}
	void prune(size_t v) {
		/* m_checkpoint.push_back(m_discard.size()); */
		/* std::cout << m_checkpoint << std::endl; */
		/* size_t old_size = m_discard.size(); */
		/* size_t c = m_checkpoint.back(); */
		m_checkpoint.push_back(m_checkpoint.back());
		for (size_t i = 0; i < m_checkpoint.back(); ++i) {
			if (m_prune(v, m_data[i])) {
				--m_checkpoint.back();
				std::swap(m_data[i], m_data[m_checkpoint.back()]);
				--i;
			}
		}
		/* for (size_t i = 0; i < m_data.size(); ++i) { */
		/* 	if (!m_prune(v, m_data[i])) { continue; } */
		/* 	m_discard.push_back(m_data[i]); */
		/* 	std::swap(m_data[i], m_data.back()); */
		/* 	m_data.pop_back(); */
		/* 	--i; */
		/* } */
		/* if (old_size == m_discard.size() && m_checkpoint.size() > 0) { */
		/* 	if (m_checkpoint.back() < 0) { */
		/* 		--m_checkpoint.back(); */
		/* 	} else { */
		/* 		m_checkpoint.push_back(-1); */
		/* 	} */
		/* } else { */
		/* 	m_checkpoint.push_back(old_size); */
		/* } */
	}

	void backtrack() {
		m_checkpoint.pop_back();
		/* if (m_checkpoint.back() >= 0) { */
		/* 	m_data.insert(m_data.end(), m_discard.begin() + m_checkpoint.back(), m_discard.end()); */
		/* 	m_discard.erase(m_discard.begin() + m_checkpoint.back(), m_discard.end()); */
		/* 	m_checkpoint.pop_back(); */
		/* } else if (m_checkpoint.back() == -1) { */
		/* 	m_checkpoint.pop_back(); */
		/* } else { */
		/* 	++m_checkpoint.back(); */
		/* } */
		/* m_data.insert(m_data.end(), m_discard.begin() + m_checkpoint.back(), m_discard.end()); */
		/* m_discard.erase(m_discard.begin() + m_checkpoint.back(), m_discard.end()); */
		/* m_checkpoint.pop_back(); */
	}

private:
	/* data */
	Prune m_prune;
	size_t m_rewind;
	std::vector<size_t> m_checkpoint;
	std::vector<Value> m_data;
	std::vector<Value> m_discard;
};
	
} /* namespace kface */
#endif /* ifndef KFACE_PRUNE_VECTOR */
