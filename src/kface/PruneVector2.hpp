#ifndef KFACE_PRUNE_VECTOR2
#define KFACE_PRUNE_VECTOR2
#include "Util.hpp"
#include <vector>
#include <cstddef>
#include <iostream>

namespace kface {

template<typename Value, typename Prune>
class PruneVector2 {
public:
	using iterator = typename std::vector<Value>::iterator;
	using const_iterator = typename std::vector<Value>::const_iterator;
	PruneVector2(Prune&& p): m_prune(p), m_checkpoint(1,std::make_pair(0,0)) {}
	PruneVector2(const std::vector<Value>& data, Prune&& p): m_data(data), m_prune(p) {}
	PruneVector2(std::vector<Value>&& data, Prune&& p): m_data(data), m_prune(p) {}

	iterator begin() { return m_data.begin(); }
	const_iterator begin() const { return m_data.begin(); }
	iterator end() { return m_data.begin() + end_idx(); }
	const_iterator end() const { return m_data.begin() + end_idx(); }

	std::pair<const_iterator, const_iterator> cands() const { 
		return std::make_pair(m_data.begin() + start_idx(), end());
	}

	size_t start_idx() const { return m_checkpoint.back().first; }
	size_t end_idx() const { return m_checkpoint.back().second; }
	size_t size() const { return m_checkpoint.back(); }

	void push_back(const Value& v) { push_back(Value(v)); }
	void push_back(Value&& v) { m_data.push_back(v); ++m_checkpoint[0].second; }

	const Value& operator [] (size_t i) { return m_data[i]; }

	void prune(size_t v) {
		m_checkpoint.push_back(m_checkpoint.back());
		for (size_t i = start_idx(); i < end_idx(); ++i) {
			int res = m_prune(v, m_data[i]);
			if (res < 0) {
				--m_checkpoint.back().second;
				std::swap(m_data[i], m_data[m_checkpoint.back().second]);
				--i;
			} else if (res > 0) {
				std::swap(m_data[i], m_data[m_checkpoint.back().first]);
				++m_checkpoint.back().first;
			}
		}
	}

	void backtrack() {
		m_checkpoint.pop_back();
	}

private:
	/* data */
	Prune m_prune;
	std::vector<std::pair<size_t, size_t>> m_checkpoint;
	std::vector<Value> m_data;
	std::vector<Value> m_discard;
};
	
} /* namespace kface */
#endif /* ifndef KFACE_PRUNE_VECTOR2 */
