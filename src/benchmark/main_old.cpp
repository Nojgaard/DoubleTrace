#include <dtrace/graph.h>
#include <dtrace/bruteforce/face_trace.h>
#include <dtrace/io.h>
#include <iostream>
#include <exception>
#include <tclap/CmdLine.h>
#include <dtrace/util.h>
#include <unordered_map>
#include <dtrace/traversor.h>
#include <dtrace/canon/emb_naive.h>
#include <dtrace/benchmark/bench.h>

size_t min_expression(const std::vector<size_t> &gaps) {
	size_t i = 0, j = 1, k = 0;
	while (i < gaps.size() && j < gaps.size() && k < gaps.size()) {
		size_t ik = (i + k) % gaps.size();
		size_t jk = (j + k) % gaps.size();
		if (gaps.at(ik) == gaps.at(jk)) { ++k; } 
		else if (gaps.at(ik) > gaps.at(jk)) {
			i = i + k + 1;
			if (i <= j) { i = j + 1; }
			k = 0;
		} else {
			j = j + k + 1;
			if (j <= i) { j = i + 1; }
			k = 0;
		}
	}
	size_t min_pos = std::min(i, j);
	return min_pos;
}

void print_graph(const dtrace::Graph& g) {
	for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) {
		dtrace::Vertex v = *vi.first;
		std::cout << v << ": ";
		for (auto ai = adjacent_vertices(v, g); ai.first != ai.second; ++ai.first) {
			std::cout << *ai.first << " ";
		}
		std::cout << std::endl;
	}
}

void print_embedding(const std::pair<std::vector<dtrace::Vertex>, std::vector<dtrace::Vertex>> &fe) {
	for (dtrace::Vertex v : fe.first) {
		std::cout << v << " ";
	}
	std::cout << "| ";
	for (dtrace::Vertex v : fe.second) {
		std::cout << v << " ";
	}
	std::cout << std::endl;
}

void print_embedding(const std::vector<dtrace::Face>& fs) {
	for (auto& f : fs) {
		for (dtrace::Vertex v : f) {
			std::cout << v << " ";
		}
		std::cout << "| ";
	}
	std::cout << std::endl;
}

std::vector<dtrace::Edge> find_boundaries(const std::vector<dtrace::Edge>& face, const dtrace::Graph& g) {
	std::vector<int> num_traversed(num_edges(g), 0);


	for (const dtrace::Edge& e : face) {
		++num_traversed[g[boost::edge(source(e, g), target(e, g), g).first].idx];
	}
	std::vector<dtrace::Edge> out;
	for (const dtrace::Edge &e : face) {
		if (num_traversed[g[boost::edge(source(e, g), target(e, g), g).first].idx] == 1) {
			out.push_back(e);
		}
	}
	return out;
}

bool is_equal(std::vector<dtrace::Face>& f1, std::vector<dtrace::Face>& f2) {
	auto rf1 = f1;
	for (auto& f : rf1) {
		std::reverse(f.begin(), f.end());
		std::rotate(f.begin(), f.begin() + min_expression(f), f.end());
	}
	std::sort(rf1.begin(), rf1.end());

	for (auto& f : f2) {
		std::rotate(f.begin(), f.begin() + min_expression(f), f.end());
	}
	std::sort(f2.begin(), f2.end());
	return rf1 == f2;
}

void find_duplicates(std::vector<std::vector<dtrace::bruteforce::Face>>& embs
		, std::unordered_map<dtrace::canon::Embedding, std::vector<dtrace::Vertex>>& found, 
		const dtrace::Graph& g) {
	std::cout << "dealing with: " << embs.size() << std::endl;
	std::unordered_map<dtrace::canon::Embedding, std::vector<dtrace::Face>> m;
	int revs = 0, u = 0;
	for (auto& emb : embs) {
		std::vector<dtrace::Face> fs;
		for (auto& f : emb) {
			fs.push_back(dtrace::Face());
			for (auto pv : f) {
				fs.back().push_back(pv.first);
			}
		}
		dtrace::canon::Embedding c(fs, g);
		if (m.find(c) != m.end()) {
			bool eq = is_equal(fs, m[c]);
			if (!eq) {
				++u;
				std::cout << "duplicate!" << std::endl;
				std::cout << ((eq) ? "is equal" : "is not equal") << std::endl;
				print_embedding(fs);
				print_embedding(m[c]);
				c.print_gaps();
				m.find(c)->first.print_gaps();
			} else {
				++revs;
			}
		} else {
			m[c] = fs;
		}
	}
	std::cout << "reversals: " << revs << std::endl;
	std::cout << "unique: " << u << std::endl;
	if (u != 0) { return; }

	int not_found = 0;
	for (auto& kv : m) {
		if (found.find(kv.first) == found.end()) {
			std::cout << "not found!" << std::endl;
			print_embedding(kv.second);
			kv.first.print_gaps();
			++not_found;
		} else {
			/* std::cout << "found!" << std::endl; */
			/* print_embedding(found.find(kv.first)->second); */
			/* print_embedding(kv.second); */
		}
	}
	std::cout << "not found: " << not_found << std::endl;
}

int num_unique(std::vector<std::vector<dtrace::bruteforce::Face>>& embs, dtrace::Graph& g) {
	std::unordered_map<dtrace::canon::EmbNaive, std::vector<dtrace::Face> > m;
	for (auto& emb : embs) {
		std::vector<dtrace::Face> fs;
		for (auto& f : emb) {
			fs.push_back(dtrace::Face());
			for (auto pv : f) {
				fs.back().push_back(pv.first);
			}
		}
		dtrace::canon::EmbNaive tc(fs, g);
		if (m.find(tc) == m.end()) {
			m[tc] = fs;
		}
	}
	return m.size();
}

int j = 1;

void sanity_check(dtrace::Graph &g) {
		size_t i = 0;
		for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
			g[*ei.first].idx = i++;
		}
		for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
			g[*ei.first].req_dir = dtrace::EdgeDirection::ANTI;
		}
		/* auto one_face_embeddings = dtrace::embedding::find_one_face_embeddings(g); */
		/* auto test_one_emb = dtrace::benchmark::find_n_face_embeddings(g, 1); */
		/* if (one_face_embeddings.size() > 0) { */
		/* 	return; */
		/* } */
		std::cout << std::endl;
		auto test_embs = dtrace::bruteforce::find_two_face_embeddings(g);
		/* int num_exp = num_unique(test_embs, g); */
		/* int num_exp = 0; */
		std::cout << "DONE (TEST)\n";
		/* auto tembs = dtrace::embedding::find_two_face_embeddings(g); */
		dtrace::Traversor t(g);
		auto tembs = t.find_two_face_embeddings();
		std::cout << "DONE (REAL)\n";
		std::cout << ": " << tembs.size() << "/" << (test_embs.size()/2)  << std::endl;
		/* std::cout << ": " << tembs.size() << "/" << num_exp  << std::endl; */
		int num_auts = dtrace::morphism::find_automorphisms(g).size();
		if (test_embs.size() != tembs.size()*2) {
		/* if ((size_t)num_exp != tembs.size()) { */
			/* std::cout << "EXPECTED EMBEDDINGS: " << test_embs.size() / 2 << std::endl; */
			/* std::cout << "EXPECTED EMBEDDINGS: " << num_exp << std::endl; */
			std::cout << "FOUND EMBEDDINGS: " << tembs.size() << std::endl;
			print_graph(g);
			/* find_duplicates(test_embs, tembs, g); */
			std::unordered_map<dtrace::canon::EmbNaive, std::vector<dtrace::Face> > m;
			for (auto& emb : test_embs) {
				std::vector<dtrace::Face> fs;
				for (auto& f : emb) {
					fs.push_back(dtrace::Face());
					for (auto pv : f) {
						fs.back().push_back(pv.first);
					}
				}
				dtrace::canon::EmbNaive tc(fs, g);
				if (m.find(tc) == m.end()) {
					m[tc] = fs;
				}
			}
			exit(0);
			assert(test_embs.size() == tembs.size()*2);
		}
}

int main(int argc, const char *argv[]) {
	std::string input_path;
	try {
		using namespace TCLAP;
		CmdLine cmd("Command description", ' ', "0.9");
		ValueArg<std::string> inputArg("i", "input", "File input path", true, "", "string");
		cmd.add(inputArg);
		cmd.parse(argc, argv);
		input_path = inputArg.getValue();
	} catch (TCLAP::ArgException &e) {  // catch any exceptions  
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
		return 0;
	}

	std::vector<dtrace::Graph> gs;
	dtrace::read_graphs(input_path, gs);

	int j = 0;
	for (auto &g : gs) {
		/* print_graph(g); */
		std::cout << "graph: " << j++;
		size_t i = 0;
		for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
			g[*ei.first].idx = i++;
		}
		for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
			g[*ei.first].req_dir = dtrace::EdgeDirection::ANTI;
		}
		sanity_check(g);
	}

	return 0;
}
