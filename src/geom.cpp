#include <vector>

#include "utils.hpp"

#define PI 3.14159265358979323846
#define SQRT2 1.4142135623730951
#define SQRT3 1.7320508075688772
#define SQRT5 2.23606797749979

#define R 5.0
#define Z 3.0
#define H1 4.0
#define H2 1.0
#define H 5.0

Texture floorTexture = Texture {0, 0, 0, 1, 1, 0};
Texture wallTexture = Texture {1, 0, 0, 1, 1, 0};
Texture railingTexture = Texture {2, 0, 0, 0.2, 1, 1};
Texture doorTexture = Texture {3, 0, 0, 1, 1, 0};

void pushWall(int z, std::vector<Wall> *walls, Wall w) {
	w.rotate(0, 0, PI*2/3*z);
	walls->push_back(w);
}

void pushPlane(int y, int z, std::vector<Plane> *planes, Plane p) {
	double delta = 2*R + Z;
	p.translate(0, y*delta);
	p.rotate(0, 0, PI*2/3*z);
	planes->push_back(p);
}

void buildRoom(int y, int z, std::vector<Wall> *walls, std::vector<Plane> *planes) {
	double delta = 2*R + Z;
	
	// menudo desastre. al menos funciona.
	
	// PAREDES HABITACIÓN (R/2,Rsqrt(3)/2) -- (R,0) -- (R/2,-Rsqrt(3)/2)
	pushWall(z, walls, Wall {R/2, y*delta + R*SQRT3/2, R, y*delta, z*H + H1/2, z*H - H1/2, wallTexture});
	pushWall(z, walls, Wall {R, y*delta, R/2, y*delta - R*SQRT3/2, z*H + H1/2, z*H - H1/2, wallTexture});

	// PAREDES HABITACIÓN (-R/2,Rsqrt(3)/2) -- (-R,0) -- (-R/2,-Rsqrt(3)/2)
	pushWall(z, walls, Wall {-R/2, y*delta + R*SQRT3/2, -R, y*delta, z*H + H1/2, z*H - H1/2, wallTexture});
	pushWall(z, walls, Wall {-R, y*delta, -R/2, y*delta - R*SQRT3/2, z*H + H1/2, z*H - H1/2, wallTexture});

	// PAREDES ENTRADAS (R/4,Rsqrt(15)/4) -- (R/2,Rsqrt(3)/2)
	pushWall(z, walls, Wall {R/4, y*delta + R*SQRT3*SQRT5/4, R/2, y*delta + R*SQRT3/2, z*H + H1/2, z*H - H1/2, doorTexture});
	pushWall(z, walls, Wall {R/4, y*delta - R*SQRT3*SQRT5/4, R/2, y*delta - R*SQRT3/2, z*H + H1/2, z*H - H1/2, doorTexture});
	pushWall(z, walls, Wall {-R/4, y*delta + R*SQRT3*SQRT5/4, -R/2, y*delta + R*SQRT3/2, z*H + H1/2, z*H - H1/2, doorTexture});
	pushWall(z, walls, Wall {-R/4, y*delta - R*SQRT3*SQRT5/4, -R/2, y*delta - R*SQRT3/2, z*H + H1/2, z*H - H1/2, doorTexture});

	// PAREDES ENTRADAS (R/4,Rsqrt(15)/4) -- (R/4,Rsqrt(15)/4 + D)
	pushWall(z, walls, Wall {R/4, y*delta + R*SQRT3*SQRT5/4, R/4, y*delta + delta - R*SQRT3*SQRT5/4, z*H + H1/2, z*H - H1/2, wallTexture});
	pushWall(z, walls, Wall {-R/4, y*delta + R*SQRT3*SQRT5/4, -R/4, y*delta + delta - R*SQRT3*SQRT5/4, z*H + H1/2, z*H - H1/2, wallTexture});

	if (z != 0) {
		pushWall(z, walls, Wall {R/4, -y*delta - R*SQRT3*SQRT5/4, R/4, -y*delta - delta + R*SQRT3*SQRT5/4, z*H + H1/2, z*H - H1/2, wallTexture});
		pushWall(z, walls, Wall {-R/4, -y*delta - R*SQRT3*SQRT5/4, -R/4, -y*delta - delta + R*SQRT3*SQRT5/4, z*H + H1/2, z*H - H1/2, wallTexture});
	}

	// BARANDILLAS
	pushWall(z, walls, Wall {-R/4, y*delta + R*SQRT3/4, R/4, y*delta + R*SQRT3/4, z*H - H1/2 + 1, z*H - H1/2, railingTexture});
	pushWall(z, walls, Wall {R/4, y*delta + R*SQRT3/4, R*2/4, y*delta, z*H - H1/2 + 1, z*H - H1/2, railingTexture});
	pushWall(z, walls, Wall {R*2/4, y*delta, R/4, y*delta - R*SQRT3/4, z*H - H1/2 + 1, z*H - H1/2, railingTexture});
	pushWall(z, walls, Wall {R/4, y*delta - R*SQRT3/4, -R/4, y*delta - R*SQRT3/4, z*H - H1/2 + 1, z*H - H1/2, railingTexture});
	pushWall(z, walls, Wall {-R/4, y*delta - R*SQRT3/4, -R*2/4, y*delta, z*H - H1/2 + 1, z*H - H1/2, railingTexture});
	pushWall(z, walls, Wall {-R*2/4, y*delta, -R/4, y*delta + R*SQRT3/4, z*H - H1/2 + 1, z*H - H1/2, railingTexture});

	// ENTRESUELOS
	if (z != 1) {
		pushWall(z, walls, Wall {-R/4, y*delta + R*SQRT3/4, R/4, y*delta + R*SQRT3/4, z*H + H1/2 + H2, z*H + H1/2, wallTexture});
		pushWall(z, walls, Wall {R/4, y*delta + R*SQRT3/4, R*2/4, y*delta, z*H + H1/2 + H2, z*H + H1/2, wallTexture});
		pushWall(z, walls, Wall {R*2/4, y*delta, R/4, y*delta - R*SQRT3/4, z*H + H1/2 + H2, z*H + H1/2, wallTexture});
		pushWall(z, walls, Wall {R/4, y*delta - R*SQRT3/4, -R/4, y*delta - R*SQRT3/4, z*H + H1/2 + H2, z*H + H1/2, wallTexture});
		pushWall(z, walls, Wall {-R/4, y*delta - R*SQRT3/4, -R*2/4, y*delta, z*H + H1/2 + H2, z*H + H1/2, wallTexture});
		pushWall(z, walls, Wall {-R*2/4, y*delta, -R/4, y*delta + R*SQRT3/4, z*H + H1/2 + H2, z*H + H1/2, wallTexture});
	}

	// OTRA SALA
	if (z == 0 && y != 0) {
		// PAREDES ARRIBA
		pushWall(z, walls, Wall {R/2, y*delta + y*R*SQRT3/2, -R/2, y*delta + y*R*SQRT3/2, (z+1)*H + H1/2, (z+1)*H - H1/2, wallTexture});	
		pushWall(z, walls, Wall {R/2, y*delta + y*R*SQRT3/2, R, y*delta, (z+1)*H + H1/2, (z+1)*H - H1/2, wallTexture});
		pushWall(z, walls, Wall {-R/2, y*delta + y*R*SQRT3/2, -R, y*delta, (z+1)*H + H1/2, (z+1)*H - H1/2, wallTexture});

		// PAREDES ABAJO
		pushWall(z, walls, Wall {R/2, y*delta + y*R*SQRT3/2, -R/2, y*delta + y*R*SQRT3/2, (z-1)*H + H1/2, (z-1)*H - H1/2, wallTexture});	
		pushWall(z, walls, Wall {R/2, y*delta + y*R*SQRT3/2, R, y*delta, (z-1)*H + H1/2, (z-1)*H - H1/2, wallTexture});
		pushWall(z, walls, Wall {-R/2, y*delta + y*R*SQRT3/2, -R, y*delta, (z-1)*H + H1/2, (z-1)*H - H1/2, wallTexture});
	
		// ENTRESUELOS
		pushWall(z, walls, Wall {-R/4, y*delta + y*R*SQRT3/4, R/4, y*delta + y*R*SQRT3/4, (z-1)*H + H1/2 + H2, (z-1)*H + H1/2, wallTexture});
		pushWall(z, walls, Wall {R/4, y*delta + y*R*SQRT3/4, R*2/4, y*delta, (z-1)*H + H1/2 + H2, (z-1)*H + H1/2, wallTexture});
		pushWall(z, walls, Wall {-R/4, y*delta + y*R*SQRT3/4, -R*2/4, y*delta, (z-1)*H + H1/2 + H2, (z-1)*H + H1/2, wallTexture});
		
		// BARANDILLAS
		pushWall(z, walls, Wall {-R/4, y*delta + y*R*SQRT3/4, R/4, y*delta + y*R*SQRT3/4, (z+1)*H - H1/2 + H2, (z+1)*H - H1/2, railingTexture});
		pushWall(z, walls, Wall {R/4, y*delta + y*R*SQRT3/4, R*2/4, y*delta, (z+1)*H - H1/2 + H2, (z+1)*H - H1/2, railingTexture});
		pushWall(z, walls, Wall {-R/4, y*delta + y*R*SQRT3/4, -R*2/4, y*delta, (z+1)*H - H1/2 + H2, (z+1)*H - H1/2, railingTexture});
	}

	// SUELO (R,R) -- (-R,R) -- (-R,-R) -- (R, -R)
	if (z == 0) {
		pushPlane(y, z, planes, Plane {z*H - H1/2, 4, {R, -R, -R, R}, {R,R,-R,-R}, floorTexture});
		pushPlane(y, z, planes, Plane {z*H + H1/2, 4, {R, -R, -R, R}, {R,R,-R,-R}, floorTexture});
	} else if (z == 1) {
		pushPlane(y, z, planes, Plane {z*H + H1/2, 4, {R, -R, -R, R}, {R,R,-R,-R}, wallTexture});
	} else if (z == -1) {
		pushPlane(y, z, planes, Plane {z*H - H1/2, 4, {R, -R, -R, R}, {R,R,-R,-R}, wallTexture});
	}
	
	if (z != 1) pushPlane(y, z, planes, Plane {z*H - H1/2, 4, {-R/2, R/2, R/2, -R/2},
		{R*SQRT3/2, R*SQRT3/2, delta - R*SQRT3/2, delta - R*SQRT3/2}, wallTexture});
	if (z != -1) pushPlane(y, z, planes, Plane {z*H + H1/2, 4, {-R/2, R/2, R/2, -R/2},
		{R*SQRT3/2, R*SQRT3/2, delta - R*SQRT3/2, delta - R*SQRT3/2}, wallTexture});

	if (z == 1) pushPlane(y, z, planes, Plane {z*H + H1/2, 4, {-R/2, R/2, R/2, -R/2},
		{-R*SQRT3/2, -R*SQRT3/2, -delta + R*SQRT3/2, -delta + R*SQRT3/2}, wallTexture});
	if (z == -1) pushPlane(y, z, planes, Plane {z*H - H1/2, 4, {-R/2, R/2, R/2, -R/2},
		{-R*SQRT3/2, -R*SQRT3/2, -delta + R*SQRT3/2, -delta + R*SQRT3/2}, wallTexture});

}

void buildGeom(std::vector<Wall> *walls, std::vector<Plane> *planes) {
	buildRoom(0, 0, walls, planes);	
	buildRoom(1, 0, walls, planes);
	buildRoom(-1, 0, walls, planes);

	buildRoom(0, 1, walls, planes);
	buildRoom(0, -1, walls, planes);
}
