#include <catch/catch.hpp>
/* #include "../Enumerate.hpp" */
#include <kface/Find.hpp>
#include <graphio/g6.h>
#include "tg5.cpp"
#include "tg6.cpp"

namespace kface {
namespace test {

TEST_CASE("Find Graphs v5 f1 alg:brute", "[find_v5_f1_brute]") {
	for (const auto& gp : tg5) {
		Graph g;
		graphio::g6::read(gp.first, g);
		auto num_faces = count_one_face(g, 100, Algorithm::Brute);
		REQUIRE(num_faces.second);
		REQUIRE(num_faces.first == gp.second);
	}
}

TEST_CASE("Find Graphs v6 f1 alg:brute", "[find_v6_f1_brute]") {
	for (const auto& gp : tg6) {
		Graph g;
		graphio::g6::read(gp.first, g);
		/* size_t num_faces = count_one_face(g); */
		auto num_faces = count_one_face(g, 100, Algorithm::Brute);
		/* if (num_faces.first != (int)gp.second) { */
		/* 	std::cerr << "Graph: " << gp.first << std::endl; */
		/* } */
		REQUIRE(num_faces.second);
		REQUIRE(num_faces.first == gp.second);
		/* REQUIRE(num_faces == gp.second); */
	}
}

TEST_CASE("Find Graphs v5 f1 alg:algebraic", "[find_v5_f1_algebraic]") {
	for (const auto& gp : tg5) {
		Graph g;
		graphio::g6::read(gp.first, g);
		auto num_faces = count_one_face(g, 100, Algorithm::Orderly);
		REQUIRE(num_faces.second);
		REQUIRE(num_faces.first == gp.second);
	}
}

TEST_CASE("Find Graphs v6 f1 alg:algebraic", "[find_v6_f1_algebraic]") {
	for (const auto& gp : tg6) {
		Graph g;
		graphio::g6::read(gp.first, g);
		auto num_faces = count_one_face(g, 100, Algorithm::Orderly);
		REQUIRE(num_faces.second);
		REQUIRE(num_faces.first == gp.second);
	}
}

TEST_CASE("Find Graphs v5 f1 alg:gap", "[find_v5_f1_gap]") {
	for (const auto& gp : tg5) {
		Graph g;
		graphio::g6::read(gp.first, g);
		auto num_faces = count_one_face(g, 100, Algorithm::Gap);
		REQUIRE(num_faces.second);
		REQUIRE(num_faces.first == gp.second);
	}
}

TEST_CASE("Find Graphs v6 f1 alg:gap", "[find_v6_f1_gap]") {
	for (const auto& gp : tg6) {
		Graph g;
		graphio::g6::read(gp.first, g);
		auto num_faces = count_one_face(g, 100, Algorithm::Gap);
		REQUIRE(num_faces.second);
		REQUIRE(num_faces.first == gp.second);
	}
}
	
} /* namespace test */
} /* namespace kface */
