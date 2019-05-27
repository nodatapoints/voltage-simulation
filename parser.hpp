#pragma once

#include <vector>
#include <fstream>
#include <string>

struct PointData {
	float potential;
	float position[2];
};

std::vector<PointData> parseFile(const std::string& filename); 
