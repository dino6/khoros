#include <vector>

#include "utils.hpp"

#pragma once

void buildGeom(std::vector<Wall> *walls, std::vector<Plane> *planes);
void buildRoom(int y, int z, std::vector<Wall> *walls, std::vector<Plane> *planes);
