#include <iostream>
#include <kface/Find.hpp>
#include <kface/Graph.hpp>
#include <graphio/g6.h>
#include <graphio/csv.h>
#include <fstream>

int main(int argc, const char *argv[]) {
	std::cout << "test" << std::endl;
	std::string p(argv[1]);
	kface::Graph g;
	graphio::g6::read(p, g);
	/* std::ifstream in(p); */
	/* try { */
	/* 	graphio::csv::read(in, g); */
	/* } catch (std::exception& e) { */
	/* 	std::cerr << e.what() << std::endl; */
	/* 	in.close(); */
	/* 	exit(0); */
	/* } */

	kface::print_graph(g);

	/* kface::find_one_face(g); */
	/* int walks = kface::count_one_face(g); */
	/* int walks = kface::count_kface_embeddings(2, g); */
	auto walks = kface::count_one_face(g);
	std::cout << "SOLUTIONS: " << walks << std::endl;
	return 0;
}
