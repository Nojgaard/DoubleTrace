#ifndef DTRACE_STAT_PRINTER_H
#define DTRACE_STAT_PRINTER_H
/* #include <boost/chrono.hpp> */
#include <chrono>
#include <iostream>
#include <dtrace/stats.h>

namespace dtrace {
	
class StatPrinter {
public:
	StatPrinter( const Stats& stats, double start_interval, double inc_interval
			     , double max_interval, bool printer_header = true, std::ostream& os = std::cout);
	void print_header() const;
	void print_stats() const;
	void print_stats(const std::chrono::duration<double>& time) const;
	void update();
private:
	void print_col(const std::string& s) const;
	void print_col(double s) const;
	void print_col(int s) const;

	const Stats& stats;
	double interval, inc_interval, max_interval, next_update;
	std::chrono::system_clock::time_point start;
	int col_width = 10;
	std::ostream& os;
};

} /* namespace dtrace */
#endif /* ifndef DTRACE_STAT_PRINTER_H */
