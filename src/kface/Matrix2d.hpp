#ifndef KFACE_MATRIX2D_HPP
#define KFACE_MATRIX2D_HPP
#include <vector>
#include <stddef.h>
#include "Util.hpp"

namespace kface {
	
template<typename T>
class Matrix2d {
public:
	Matrix2d(const std::vector<std::vector<T>>& data) {
		m_row_size = data.size();
		m_col_size = data[0].size();

		for (const auto& row : data) {
			m_data.insert(m_data.end(), row.begin(), row.end());
		}
	}

	const T& operator () (size_t r, size_t c) const {
		/* std::cout << "r: " << r << " c: " << c << " nr: " <<  m_row_size << std::endl; */
		/* std::cout << m_data << std::endl; */
		/* std::cout << "ret: " << m_data[r * m_row_size + c] << std::endl; */
		return m_data[r * m_col_size + c];
	}

	size_t num_rows() const { return m_row_size; }

private:
	/* data */
	size_t m_row_size, m_col_size;
	std::vector<T> m_data;
};

} /* namespace kface */
#endif /* ifndef KFACE_MATRIX2D_HPP */
