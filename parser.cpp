#include "parser.hpp"

std::vector<PointData> parseFile(const std::string& filename) {
	std::ifstream in(filename);
	if (in) {
		std::vector<PointData> vertices;
		std::string line;
		PointData vertex;
		while (in.good()) {
			in >> vertex.potential;
			in >> vertex.position[0];
			in >> vertex.position[1];

			vertices.push_back(vertex);
		}
		return vertices;
	}
	throw errno;
}

float getBound(const std::vector<PointData> &vertices) {
	float min = 0, max = 0;

	for (const auto& vertex : vertices) {
		min = std::min(min, vertex.potential);
		max = std::max(max, vertex.potential);
	}
	return std::max(std::abs(min), std::abs(max));
}
