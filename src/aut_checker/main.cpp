#include <graphio/g6.h>
#include <dtrace/graph/base.h>
#include <tclap/CmdLine.h>
#include <fstream>
#include <vector>
#include <morphism/aut.h>

int main(int argc, const char *argv[]) {
	std::string input_path;
	bool print_gens;
	try {
		using namespace TCLAP;
		CmdLine cmd("Command description", ' ', "0.9");
		ValueArg<std::string> inputArg("i", "input", "File input path", true, "", "string");
		cmd.add(inputArg);
		SwitchArg printArg("p", "print-gens", "print automorphism generatorts", cmd, false);
		cmd.parse(argc, argv);
		input_path = inputArg.getValue();
		print_gens = printArg.getValue();
	} catch (TCLAP::ArgException &e) {  // catch any exceptions  
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
		return 0;
	}

	std::ifstream in(input_path);
	if (!in.is_open()) {
		std::cout << "Can't open file.\n";
		return 0;
	}
	std::vector<dtrace::Graph> gs;
	graphio::g6::read(in, gs);
	in.close();

	unsigned c = 0;
	for (auto &g : gs) {
		++c;
		auto auts = morphism::find_automorphisms(g);
		if (!print_gens) {
			if (auts.size() == 0) {
				graphio::g6::write(g, std::cout);
			}
		} else {
					/* for (auto aut : auts) { */
					/* 	for (auto v : aut) { */
					/* 		std::cout << v << " "; */
					/* 	} */
					/* 	std::cout << std::endl; */
					/* } */
			for (size_t i = 0; i < auts.size(); ++i) {
				auto g = morphism::perm2group(auts[i]);
				for (auto e : g) {
					/* std::cout << "(" << e.first << "," << e.second << ")"; */
					std::cout << "(";
					for (size_t j = 0; j < e.size(); ++j) {
						std::cout << e[j];
						if (j < e.size() - 1) { std::cout << ","; }
					}
					std::cout << ")";
				}
				if (i < auts.size() - 1) { std::cout << "|"; }
			}
			if (auts.size() == 0) {
				std::cout << "()";
			} 
			std::cout << std::endl;
		}
	}
	
	return 0;
}
