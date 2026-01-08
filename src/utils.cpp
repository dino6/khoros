#include <chrono>
#include <cmath>
#include <vector>
#include <cstdio>
#include <iostream>

#include "utils.hpp"

long nowNS() {
	auto time = std::chrono::system_clock::now();
	auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time.time_since_epoch());
	return ns.count();
}

long nowMS() {
	auto time = std::chrono::system_clock::now();
	auto ns = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
	return ns.count();
}

Vec2 wallRayIntersection(double x, double y, double a, Wall w) {
	double dx = w.x1 - w.x2;
	double dy = w.y1 - w.y2;
	double det = dy*std::cos(a) - dx*std::sin(a);
	if (std::abs(det) < 1E-9) return Vec2 {-1, 0};

	double d = dy*(w.x1-x) - dx*(w.y1-y);
	double l = -std::sin(a)*(w.x1-x) + std::cos(a)*(w.y1-y);

	return Vec2 {d/det, l/det};
}

double distToWall(double x, double y, Wall w) {
	double l = (x-w.x1)*(w.x2-w.x1) + (y-w.y1)*(w.y2-w.y1);
	l /= (w.x2-w.x1)*(w.x2-w.x1) + (w.y2-w.y1)*(w.y2-w.y1);
	
	if (l < 0) l = 0;
	if (l > 1) l = 1;
	
	return std::sqrt((l*(w.x2-w.x1) + w.x1 - x)*(l*(w.x2-w.x1) + w.x1 - x) + (l*(w.y2-w.y1) + w.y1 - y)*(l*(w.y2-w.y1) + w.y1 - y));
}

Vec2 pointOnWallAtRadius(double x, double y, double r, Wall w) {
	double l = (x-w.x1)*(w.x2-w.x1) + (y-w.y1)*(w.y2-w.y1);
	l /= (w.x2-w.x1)*(w.x2-w.x1) + (w.y2-w.y1)*(w.y2-w.y1);
		
	if (l < 0) l = 0;
	if (l > 1) l = 1;

	Vec2 d {x - l*(w.x2-w.x1) - w.x1, y - l*(w.y2-w.y1) - w.y1};
	double mag = std::sqrt((l*(w.x2-w.x1) + w.x1 - x)*(l*(w.x2-w.x1) + w.x1 - x) + (l*(w.y2-w.y1) + w.y1 - y)*(l*(w.y2-w.y1) + w.y1 - y));
	d.x /= mag;
	d.y /= mag;

	return Vec2 {x + d.x*r, y + d.y*r};
}

std::vector<Vec3> planeRayIntersection(double x, double y, double a, Plane p) {
	std::vector<Vec3> intersects = {Vec3 {-1, -1, -1}, Vec3 {-1, -1, -1}};

	Vec2 min = Vec2 {-1, 0};
	Vec2 max = Vec2 {-1, 0};

	int minIdx = -1;
	int maxIdx = -1;

	int noIntersection = 1;

	for (int i = 0; i < p.n; i++) {
		Wall temp = Wall {p.x[i], p.y[i], p.x[(i+1)%p.n], p.y[(i+1)%p.n], 0, 0};
		Vec2 intersect = wallRayIntersection(x, y, a, temp);

		if (intersect.x == -1) continue;
		if (intersect.y < 0 || intersect.y > 1) continue;

		if (min.x == -1) {
			min = intersect;
			max = intersect;

			minIdx = i;
			maxIdx = i;

			continue;
		}

		if (intersect.x < min.x) {
			min = intersect;
			minIdx = i;
			noIntersection = 0;
		}

		if (intersect.x > max.x) {
			max = intersect;
			maxIdx = i;
			noIntersection = 0;
		}
	}

	if (noIntersection) return intersects;

	intersects[0] = Vec3 {max.x, max.y, maxIdx};
	intersects[1] = Vec3 {min.x, min.y, minIdx};

	return intersects;
}

Vec2 cartelRayIntersection(double x, double y, double a, Cartel c) {
	double dx = x - c.x;	
	double dy = y - c.y;

	double dist = std::sqrt(dx*dx + dy*dy);
	double det = c.r/dist * (dx*std::cos(a) + dy*std::sin(a));

	double l = -c.r*dist/det;
	double m = (-dx*std::sin(a) + dy*std::cos(a))/det;

	return Vec2 {l, m};
}

Bitmap readBMP(char* filename) {
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];

    // read the 54-byte header
    fread(info, sizeof(unsigned char), 54, f); 

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    // allocate 3 bytes per pixel
    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size];

    // read the rest of the data at once
    fread(data, sizeof(unsigned char), size, f); 
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            // flip the order of every 3 bytes
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }

    return Bitmap {data, width, height};
}

rgb pixelColor(Bitmap img, int i, int j, int mode) {
	if (mode == 0) { // clamp
		if (i < 0) i = 0;
		if (i >= img.width) i = img.width - 1;
		if (j < 0) j = 0;
		if (j >= img.height) j = img.height - 1;
	} else if (mode == 1) { // loop
		i %= img.width;
		j %= img.height;
		if (i < 0) i += img.width;
		if (j < 0) j += img.height;
	}
	int idx = 3 * ((img.height - j - 1) * img.width + i);
	return rgb {img.data[idx], img.data[idx + 1], img.data[idx + 2]};
}
