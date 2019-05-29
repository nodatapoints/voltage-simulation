#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <tuple>

struct PointData {
	float potential;
	float position[2];
};

std::vector<PointData> parseFile(const std::string& filename); 
float getBound(const std::vector<PointData> &vertices);

