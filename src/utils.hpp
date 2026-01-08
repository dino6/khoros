#include <vector>
#include <cmath>

#pragma once

long nowNS();
long nowMS();

struct Vec2 {
	double x;
	double y;
};

struct Vec3 {
	double x {};
	double y {};
	double z {};
};

struct rgb {
	int r {};
	int g {};
	int b {};
};

struct Texture {
	int id {};
	double x {};
	double y {};
	double w {};
	double h {};
	int mode {};
};

struct Wall {
	double x1 {};
	double y1 {};
	double x2 {};
	double y2 {};
	double top {};
	double bot {};
	Texture texture {};

	void rotate(double centerX, double centerY, double a) {
		double tx1 = x1 - centerX;
		double ty1 = y1 - centerY;

		double tx2 = x2 - centerX;
		double ty2 = y2 - centerY;

		x1 = tx1*std::cos(a) + ty1*-std::sin(a) + centerX;
		y1 = tx1*std::sin(a) + ty1*std::cos(a) + centerY;

		x2 = tx2*std::cos(a) + ty2*-std::sin(a) + centerX;
		y2 = tx2*std::sin(a) + ty2*std::cos(a) + centerY;
	};
};

struct Plane {
	double z {};
	int n {};
	std::vector<double> x {};
	std::vector<double> y {};
	Texture texture {};

	Vec2 getCorner1() {
		double minX = x[0];
		double minY = y[0];
		for (int i = 1; i < n; i++) {
			if (x[i] < minX) minX = x[i];
			if (y[i] < minY) minY = y[i];
		}
		return Vec2 {minX, minY};
	}

	Vec2 getCorner2() {
		double maxX = x[0];
		double maxY = y[0];
		for (int i = 1; i < n; i++) {
			if (x[i] > maxX) maxX = x[i];
			if (y[i] > maxY) maxY = y[i];
		}
		return Vec2 {maxX, maxY};
	}

	void rotate(double centerX, double centerY, double a) {
		for (int i = 0; i < n; i++) {
			double tx = x[i] - centerX;
			double ty = y[i] - centerY;

			x[i] = tx*std::cos(a) + ty*-std::sin(a) + centerX;
			y[i] = tx*std::sin(a) + ty*std::cos(a) + centerY;
		}
	}

	void translate(double dx, double dy) {
		for (int i = 0; i < n; i++) {
			x[i] += dx;
			y[i] += dy;
		}
	}
};

struct Cartel {
	double x {};
	double y {};
	double top {};
	double bot {};
	double r {};
	int texture {};
};

struct Button {
	Wall w {};
	char *hint {};
	double dist {};
};

struct Bitmap {
	unsigned char* data {};
	int width {};
	int height {};
};

Vec2 wallRayIntersection(double x, double y, double a, Wall w);

double distToWall(double x, double y, Wall w);

Vec2 pointOnWallAtRadius(double x, double y, double r, Wall w);

std::vector<Vec3> planeRayIntersection(double x, double y, double a, Plane p);

Vec2 cartelRayIntersection(double x, double y, double a, Cartel c);

Bitmap readBMP(char* filename);

rgb pixelColor(Bitmap img, int i, int j, int mode);

char* concat(const char *a, const char *b);
