#include <kface/Graph.hpp>
#include <kface/Find.hpp>
#include <kface/Gap.hpp>
#include <tclap/CmdLine.h>
#include <graphio/g6.h>
#include <graphio/csv.h>

using namespace kface;

template<typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& gap) {
	for (auto i : gap) {
		os << i << " ";
	}
	os << std::endl;
	return os;
}

void write_gap_matrix(std::ostream& os, const Graph& g, const std::vector<Vertex>& walk, const std::vector<unsigned int>& gap) {
	std::vector<Vertex> verts(num_vertices(g));
	for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) { verts[*vi.first] = *vi.first; }

	std::sort(verts.begin(), verts.end(),
			[&] (Vertex lhs, Vertex rhs) { return degree(lhs,g) > degree(rhs,g); });

	for (auto v : verts) {
		std::vector<unsigned int> vgap;
		for (size_t i = 0; i < walk.size(); ++i) {
			if (walk[i] == v) { vgap.push_back(gap[i]); }
		}
		std::sort(vgap.begin(), vgap.end());
		os << v << "(" << degree(v,g) << "): " << vgap;
	}
}

void write_gaps(std::ostream& os, const Graph& g, const std::vector<Vertex>& walk, kface::GapRepr gap_repr) {
	os << "walk: " << walk;
	std::vector<Vertex> rwalk(walk.rbegin(), walk.rend());
	std::vector<unsigned int> fgap;
	switch(gap_repr) {
		case kface::GapRepr::None: fgap = kface::build_vertex_gap(g, walk, true); break;
		case kface::GapRepr::Vertex: fgap = kface::build_vertex_gap(g, walk, true); break;
		case kface::GapRepr::Edge: fgap = kface::build_edge_gap(g, walk, true); break;
		case kface::GapRepr::Bio: fgap = kface::build_bio_gap(g, walk); break;
	}
	/* if (!use_egap) { */
	/* 	fgap = kface::build_vertex_gap(g, walk, true); */
	/* } else { */
	/* 	fgap = kface::build_edge_gap(g, walk, true); */
	/* } */
	os << "gap: " << fgap;
	/* auto bgap = kface::build_vertex_gap(g, walk, false); */
	/* os << "bgap: " << bgap; */

	/* os << std::endl << "forward gap matrix:" << std::endl; */
	/* write_gap_matrix(os, g, walk, fgap); */
	/* std::cout << std::endl; */

	/* os << "backward gap matrix:" << std::endl; */
	/* write_gap_matrix(os, g, walk, bgap); */
	std::cout << "------------------------------------" << std::endl;
}

int main(int argc, const char *argv[]) {
	std::string input_path, output_path;
	size_t min_gap, max_gap;
	bool use_egap;
	std::string gap_repr_str;
	try {
		using namespace TCLAP;
		CmdLine cmd("Command description", ' ', "0.9");
		ValueArg<std::string> inputArg("i", "input", "Input path", true, "", "string", cmd);
		ValueArg<size_t>  minArg("n", "min", "minimim gap", false, 0, "uint", cmd);
		ValueArg<size_t>  maxArg("x", "max", "minimim gap", false, 1000, "uint", cmd);
		ValueArg<std::string> gapArg("g", "gap-repr", "gap representation to use", false, "none"
				, "[none, edge, vertex, bio]", cmd);
		/* SwitchArg egapArg("e", "edge-gap", "Use and print the edge gap representation", cmd, false); */
		cmd.parse(argc, argv);

		input_path = inputArg.getValue();
		min_gap = minArg.getValue();
		max_gap = maxArg.getValue();
		/* use_egap = egapArg.getValue(); */
		gap_repr_str = gapArg.getValue();
	} catch (TCLAP::ArgException& e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
		return 0;
	}
	kface::GapRepr gap_repr;
	if (gap_repr_str == "none") {
		gap_repr = kface::GapRepr::None;
	} else if (gap_repr_str == "edge") {
		gap_repr = kface::GapRepr::Edge;
	} else if (gap_repr_str == "vertex") {
		gap_repr = kface::GapRepr::Vertex;
	} else if (gap_repr_str == "bio") {
		gap_repr = kface::GapRepr::Bio;
	} else {
		std::cerr << "error: '" << gap_repr << "' is an invalid 'gap-repr' argument, see help for valid arguments.";
		std::cerr << std::endl;
		return 0;
	}

	Graph g;
	std::cout << "input graph: " << input_path << std::endl;
	graphio::g6::read(input_path, g);
	size_t count = 0;
	for (auto it = edges(g); it.first != it.second; ++it.first) {
		Edge e = *(it.first);
		g[e].id = count++;
	}
	std::cout << "graph in csv format (|V| = " << num_vertices(g) << " |E| = " << num_edges(g) << "): " << std::endl;
	graphio::csv::write(std::cout, g);
	std::cout << std::endl;

	std::cout << "computing non-equivalent one face embeddings..." << std::endl;
	std::vector<std::vector<size_t>> walks;
	if (min_gap > 0 || max_gap < 1000) {
		walks = find_one_face(g, Algorithm::Orderly, min_gap, max_gap, gap_repr);
	} else {
		walks = find_one_face(g, Algorithm::Orderly);
	}
	std::cout << "found " << walks.size() << " embeddings" << std::endl;

	std::cout << "writing their gap representation" << std::endl << std::endl;
	for (const auto& walk : walks) {
		write_gaps(std::cout, g, walk, gap_repr);
		std::cout << std::endl;
	}

	return 0;
}
