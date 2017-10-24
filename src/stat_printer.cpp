#include <dtrace/stat_printer.h>
#include <iostream>
#include <iomanip>

namespace dtrace {
	
StatPrinter::StatPrinter( const Stats& stats, double start_interval, double inc_interval
		                  , double max_interval, bool print_header, std::ostream& os)
	: stats(stats)
	, interval(start_interval)
	, inc_interval(inc_interval)
	, max_interval(max_interval)
	, next_update(interval)
	, start(std::chrono::system_clock::now())
   , os(os)	{
		if (print_header) {
			this->print_header();
		}
}

void StatPrinter::print_col(const std::string& c) const {
	os << std::setw(col_width) << std::left << c;
}

void StatPrinter::print_col(double c) const {
	os << std::setw(col_width) << std::left << std::setprecision(4) << c;
}

void StatPrinter::print_col(int c) const {
	os << std::setw(col_width) << std::left << c;
}

void StatPrinter::print_header() const {
	print_col("time");
	print_col("traces");
	print_col("cemb");
	print_col("emb");
	print_col("gauts");
	print_col("auts");
	print_col("verts");
	print_col("edges");
	os << std::endl;
}

void StatPrinter::print_stats(const std::chrono::duration<double>& time) const {
	print_col(time.count());
	print_col(stats.num_traces);
	print_col(stats.num_cand_embeddings);
	print_col(stats.num_embeddings);
	print_col(stats.gen_auts);
	print_col(stats.auts);
	print_col(stats.num_vertices);
	print_col(stats.num_edges);
	os << std::endl;
}

void StatPrinter::print_stats() const {
	if (!stats.enable_print) { return; }
	print_stats(stats.time);
}

void StatPrinter::update() {
	if (!stats.enable_print) { return; }
	auto current_update = std::chrono::system_clock::now();
	std::chrono::duration<double> dt = current_update - start;
	if (dt.count() < next_update) { return; }

	std::chrono::duration<double> time = std::chrono::system_clock::now() - start;
	print_stats(time);
	next_update += interval;
	interval = std::min(interval*inc_interval, max_interval);
	/* last_update = current_update; */
}

} /* namespace dtrace */
