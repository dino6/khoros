#ifdef WINDOWS
#include <SDL.h>
#include <SDL_ttf.h>
#define UINT uint32_tm
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define UINT uint
#endif

#include <stdio.h>
#include <iostream>
#include <vector>

#include "utils.hpp"
#include "geom.hpp"
#include "ref.hpp"

#define PI 3.14159265358979323846
#define SQRT2 1.4142135623730951
#define SQRT3 1.7320508075688772
#define SQRT5 2.23606797749979

#define COLS 200
#define ROWS 200

#define FONT "./res/LiberationSans-Regular.ttf"

#define RADIUS 5
#define ZAGUAN 3

class Framework {
public:
	// Contructor which initialize the parameters.
	Framework(int width_, int height_): width(width_), height(height_) {
		SDL_Init(SDL_INIT_VIDEO);       // Initializing SDL as Video
		SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS, &window, &renderer);
		SDL_SetWindowTitle(window, constants::title);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);      // setting draw color
		SDL_RenderClear(renderer);      // Clear the newly created window
		SDL_RenderPresent(renderer);    // Reflects the changes done in the
		SDL_ShowCursor(SDL_DISABLE);

		SDL_SetWindowMouseGrab(window, SDL_TRUE);
		SDL_WarpMouseInWindow(window, width/2, height/2);

		TTF_Init();
	}

	// Destructor
	~Framework() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void run() {
		setup();

		long last = nowNS();
		double ticks = 60.0;
		double ns = 1E9 / ticks;
		double delta = 0;

		long timer = nowMS();
		int updates = 0;
		int frames = 0;

		while(running) {
			long now = nowNS();
			delta += (now - last) / ns;
			deltaT = (now - last) / ns / ticks;
			last = now;

			while (delta >= 1) {
				tick();
				updates++;
				delta--;
			}
			render();
			frames++;

			if (nowMS() - timer > 1000) {
				timer += 1000;
				std::cout << "FRAMES: " << frames << " UPDATES: " << updates << " DELTA: " << deltaT << std::endl;
				if (doTimer) std::cout << "CHRONOMETERS: TICK: " << chronoTick/1000.0 << " INTERSECTIONS: " << chronoIntersect/1000.0 << " PAINTING: " << chronoPaint/1000.0 << std::endl;
				chronoTick = 0;
				chronoIntersect = 0;
				chronoPaint = 0;
				frames = 0;
				updates = 0;
			}	
		}
	}

	void start() {
		if (running) return;
		running = 1;
		run();
	}

	void stop() {
		running = 0;
	}


private:
	int height;     // Height of the window
	int width;      // Width of the window
	SDL_Renderer *renderer = NULL;      // Pointer for the renderer
	SDL_Window *window = NULL;      // Pointer for the window
	SDL_Event event;

	int running = 0;

	double fovx = PI/2;
	double fovy = PI/2;

	double camX = 0;
	double camY = -RADIUS;
	double camZ = 0;
	double camA = PI/2;

	double playerR = 0.1;
	double playerHigh = 0.2;
	double playerLow = 1;

	int roomX = 0;
	int roomY = 0;
	int roomZ = 0;

	UINT vels;

	int noclip = 0;
	int loop = 1;
	int fog = 1;

	std::vector<Wall> walls {};
	std::vector<Plane> planes {};
	std::vector<Cartel> cartels {};

	std::vector<Button> buttons {};
	int currentButton = -1;

	std::vector<int> wallsVisible {};

	rgb rgbBuffer[COLS][ROWS];
	double zBuffer[COLS][ROWS];

	SDL_Texture *screenTexture;	

	rgb fogCol {120, 100, 100};
	double fogDense = 0.3;

	Bitmap imgs[5];

	int focused = 1;
	Vec2 mouse, mouseDif;

	double chronoTick = 0;
	double chronoIntersect = 0;
	double chronoPaint = 0;
	int doTimer = 0;
	
	double swingT = 0;
		double swingAmount = 0;
	int moving = 0;

	int transitionDoor = 0;
	int transitioning = 0; // trans rights
	double transT = 0;

	double deltaT = 0;

	TTF_Font *font;

	void setup() {
    		screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, COLS, ROWS);

		cartels.push_back(Cartel {RADIUS/2, 3-RADIUS, 6, 3, 0.25, 4});

		buttons.push_back(Button {{RADIUS/4.0, RADIUS*SQRT3*SQRT5/4, RADIUS/2.0, RADIUS*SQRT3/2, 2, -2, NULL}, "Subir", 2});
		buttons.push_back(Button {{-RADIUS/4.0, RADIUS*SQRT3*SQRT5/4, -RADIUS/2.0, RADIUS*SQRT3/2, 2, -2, NULL}, "Bajar", 2});
		buttons.push_back(Button {{-RADIUS/4.0, -RADIUS*SQRT3*SQRT5/4, -RADIUS/2.0, -RADIUS*SQRT3/2, 2, -2, NULL}, "Subir", 2});
		buttons.push_back(Button {{RADIUS/4.0, -RADIUS*SQRT3*SQRT5/4, RADIUS/2.0, -RADIUS*SQRT3/2, 2, -2, NULL}, "Bajar", 2});


		buildGeom(&walls, &planes);

		for (int i = 0; i < walls.size(); i++) wallsVisible.push_back(1);

		imgs[0] = readBMP("./res/floor.bmp");
		imgs[1] = readBMP("./res/wall.bmp");
		imgs[2] = readBMP("./res/railing.bmp");
		imgs[3] = readBMP("./res/wall2.bmp");
		imgs[4] = readBMP("./res/_bingus.bmp");

		font = TTF_OpenFont(FONT, 64);
	}

	void tick() {
		chronoTick -= nowMS();

		int buttonPress = 0;
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) stop();
			else if (event.type == SDL_KEYDOWN) switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					stop();
					break;	
				case SDLK_w:
					vels |= 1;	
					break;
				case SDLK_s:
					vels |= 1<<1;	
					break;
				case SDLK_d:
					vels |= 1<<2;
					break;
				case SDLK_a:
					vels |= 1<<3;
					break;	
				case SDLK_e:
					vels |= 1<<4;
					break;
				case SDLK_q:
					vels |= 1<<5;
					break;
				case SDLK_RIGHT:
					vels |= 1<<6;
					break;
				case SDLK_LEFT:
					vels |= 1<<7;
					break;
				case SDLK_UP:
					vels |= 1<<8;
					break;
				case SDLK_DOWN:
					vels |= 1<<9;
					break;
				case SDLK_n:
					noclip = !noclip;
					break;
				case SDLK_l:
					loop = !loop;
					break;
				case SDLK_f:
					fog = !fog;
					break;
				case SDLK_SPACE:
					buttonPress = 1;
					break;
			} else if (event.type == SDL_KEYUP) switch (event.key.keysym.sym) {
				case SDLK_w:
					vels &= ~1;
					break;
				case SDLK_s:
					vels &= ~(1<<1);
					break;
				case SDLK_d:
					vels &= ~(1<<2);
					break;
				case SDLK_a:
					vels &= ~(1<<3);
					break;	
				case SDLK_e:
					vels &= ~(1<<4);
					break;
				case SDLK_q:
					vels &= ~(1<<5);
					break;
				case SDLK_RIGHT:
					vels &= ~(1<<6);
					break;
				case SDLK_LEFT:
					vels &= ~(1<<7);
					break;
				case SDLK_UP:
					vels &= ~(1<<8);
					break;
				case SDLK_DOWN:
					vels &= ~(1<<9);
					break;
			} else if (event.type == SDL_MOUSEMOTION) {
				mouse = Vec2 {event.motion.x, event.motion.y};
			} /*else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				SDL_SetWindowMouseGrab(window, SDL_TRUE);
				SDL_WarpMouseInWindow(window, width/2, height/2);
			} else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) SDL_SetWindowMouseGrab(window, SDL_FALSE);*/
		}
		
		if (((SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0) && focused) {
			mouseDif = Vec2 {mouse.x - width/2, mouse.y - height/2};	
			SDL_WarpMouseInWindow(window, width/2, height/2);
		} else if (((SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0)) {
			SDL_WarpMouseInWindow(window, width/2, height/2);
			mouseDif = Vec2 {0, 0};
		}
		focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;

		double speed = 0.05;

		if (transitioning == 1) speed = 0;

		moving = 0;

		if (vels & 1) {
			camX += speed * std::cos(camA);
			camY += speed * std::sin(camA);

			moving = 1;
		}
		if (vels & 1 << 1) {
			camX -= speed * std::cos(camA);
			camY -= speed * std::sin(camA);

			moving = 1;
		}
		if (vels & 1 << 2) {
			camX += speed * -std::sin(camA);
			camY += speed * std::cos(camA);

			moving = 1;
		}
		if (vels & 1 << 3) {
			camX -= speed * -std::sin(camA);
			camY -= speed * std::cos(camA);

			moving = 1;
		}
		if (noclip && vels & 1 << 4) camZ += 0.1;
		if (noclip && vels & 1 << 5) camZ -= 0.1;
		if (vels & 1 << 6) camA += 0.1;
		if (vels & 1 << 7) camA -= 0.1;

		/*if (vels & 1 << 8) {
			fovx /= 1.01;
			fovy /= 1.01;
		}
		if (vels & 1 << 9) {
			fovx *= 1.01;
			fovy *= 1.01;
		}*/

		if (transitioning == 1) moving = 0;
		
		if (!noclip) for (Wall w : walls) {
			if (w.bot > camZ + playerHigh) continue;
			if (w.top < camZ - playerLow) continue;
			if (distToWall(camX, camY, w) <= playerR) {
				Vec2 newPos = pointOnWallAtRadius(camX, camY, playerR, w);
				camX = newPos.x;
				camY = newPos.y;
			}
		}

		if (loop && camY >= RADIUS+ZAGUAN/2.0) { // habitación hacia adelante
			camY -= 2*RADIUS+ZAGUAN;
			roomX += std::round(std::cos(PI*roomZ/3.0) - std::sin(PI*roomZ/3.0)/SQRT3);
			roomY += std::round(2/SQRT3*std::sin(PI*roomZ/3.0));

			std::cout << "ROOM: " << roomX << " " << roomY << " " << roomZ << std::endl;
		}
		if (loop && camY < -RADIUS-ZAGUAN/2.0) { // habitación hacia atrás
			camY += 2*RADIUS+ZAGUAN;
			roomX -= std::round(std::cos(PI*roomZ/3.0) - std::sin(PI*roomZ/3.0)/SQRT3);
			roomY -= std::round(2/SQRT3*std::sin(PI*roomZ/3.0));

			std::cout << "ROOM: " << roomX << " " << roomY << " " << roomZ << std::endl;
		}

		currentButton = -1;
		for (int i = 0; i < buttons.size(); i++) {
			Button b = buttons[i];
			Vec2 intersect = wallRayIntersection(camX, camY, camA, b.w);
			if (intersect.x >= 0 && intersect.x <= b.dist && intersect.y >= 0 && intersect.y <= 1) currentButton = i;	
		}

		if (buttonPress && currentButton >= 0) {
			transitioning = 1;
			transitionDoor = currentButton;
		}

		// cutscene!
		if (transitioning == 1) {
			transT += deltaT;

			if (transT > 1 - deltaT/2 && transT <= 1 + deltaT/2) {
				roomZ += transitionDoor % 2 == 0 ? 1 : -1;
				
				camX = -RADIUS*(transitionDoor % 2 == 0 ? 1 : -1)*(transitionDoor < 2 ? 1 : -1)*5.0/6*3/8;
				camY = RADIUS*(transitionDoor < 2 ? 1 : -1)*5.0/6*(SQRT3/4 + SQRT3*SQRT5/8);

				camA = PI + std::atan2(camY, camX);
				//std::cout << "ROOM: " << roomX << " " << roomY << " " << roomZ << std::endl;
			}

			if (transT >= 2) {
				transitioning = 0;
				transT = 0;
				transitionDoor = 0;
			}
		}

		camA += mouseDif.x * 0.005;

		while (camA < -PI) camA += 2*PI;
		while (camA > PI) camA -= 2*PI;
		
		chronoTick += nowMS();
	}

	void render() {
		SDL_SetRenderDrawColor(renderer, fogCol.r, fogCol.g, fogCol.b, 255);
		SDL_RenderClear(renderer);

		//SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		double dw = (double) width/COLS;
		double dh = (double) height/ROWS;
		double near = 0.5/std::tan(fovy/2);

		// check visible walls
		for (int i = 0; i < walls.size(); i++) {
			Wall w = walls[i];

			double dot1 = std::cos(camA) * (w.x1 - camX) + std::sin(camA) * (w.y1 - camY);
			double dot2 = std::cos(camA) * (w.x2 - camX) + std::sin(camA) * (w.y2 - camY);

			//dot1 /= std::sqrt((w.x1 - camX)*(w.x1 - camX) + (w.y1 - camY)*(w.y1 - camY));
			//dot2 /= std::sqrt((w.x2 - camX)*(w.x2 - camX) + (w.y2 - camY)*(w.y2 - camY));

			wallsVisible[i] = dot1 > 0 || dot2 > 0;
		}

		double swingHorizontal = std::sin(2*PI*swingT) * 0.1 * swingAmount;
		camX += swingHorizontal * std::sin(camA);
		camY += swingHorizontal * std::cos(camA);
		camZ += -std::cos(4*PI*swingT) * 0.1 * swingAmount;

		double leftAngles[walls.size()];
		double rightAngles[walls.size()];
		for (int i = 0; i < walls.size(); i++) {
			if (!wallsVisible[i]) {
				leftAngles[i] = -2*PI;
				rightAngles[i] = -2*PI;
				continue;
			}

			Wall w = walls[i];

			double a1 = std::atan2(w.y1 - camY, w.x1 - camX) - camA;
			double a2 = std::atan2(w.y2 - camY, w.x2 - camX) - camA;

			while (a1 < -PI) a1 += 2*PI;
			while (a1 > PI) a1 -= 2*PI;
			while (a2 < -PI) a2 += 2*PI;
			while (a2 > PI) a2 -= 2*PI;

			leftAngles[i] = std::min(a1, a2);
			rightAngles[i] = std::max(a1, a2);
		}

		void *pixels;
		int pitch;
		SDL_LockTexture(screenTexture, NULL, &pixels, &pitch);

		for (int i = 0; i < COLS; i++) {
			renderCol(i, dw, dh, near, pixels, leftAngles, rightAngles);
		}

		SDL_UnlockTexture(screenTexture);

		camX -= swingHorizontal * std::sin(camA);
		camY -= swingHorizontal * std::cos(camA);
		camZ -= -std::cos(4*PI*swingT) * 0.1 * swingAmount;

		swingT += 0.6*deltaT;
		while (swingT > 1) swingT -= 1;

		swingAmount += (moving - swingAmount) * deltaT * 5;

		SDL_RenderCopy(renderer, screenTexture, NULL, NULL);

		if (currentButton != -1) {
			SDL_Surface *textSurface = TTF_RenderText_Solid(font, buttons[currentButton].hint, {0, 0, 0, 255});
			SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_Rect textRect = {width/2 - textSurface->w/2, height/2 - textSurface->h/2, textSurface->w, textSurface->h};
			SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
		}

		SDL_RenderPresent(renderer);
	}

	void renderCol(int i, double dw, double dh, double near, void *pixels, double *leftAngles, double *rightAngles) {
		for (int j = 0; j < ROWS; j++) {
			rgbBuffer[i][j] = rgb {0, 0, 0};
			zBuffer[i][j] = -1;
		}
		
		double a = ((double) i/COLS - 0.5) * fovx + camA;

		chronoIntersect -= nowMS();

		// WALL INTERSECTIONS
		for (int k = 0; k < walls.size(); k++) {
			Wall w = walls[k];
			if (!wallsVisible[k]) continue;

			if (a - camA < leftAngles[k] || a - camA > rightAngles[k]) continue;

			Vec2 intersect = wallRayIntersection(camX, camY, a, w);
			if (intersect.x < 0 || intersect.y > 1 || intersect.y < 0) continue;	

			double top = height/2.0 - height*(w.top - camZ)*near/intersect.x;
			double bot = height/2.0 - height*(w.bot - camZ)*near/intersect.x;

			Bitmap img = imgs[w.texture.id];

			for (int j = 0; j < ROWS; j++) {
				if (j*dh < top || j*dh > bot) continue;

				double dist = intersect.x;
				if (zBuffer[i][j] >= 0 && zBuffer[i][j] < dist) continue;

				double wallX = intersect.y;
				double wallY = (j*dh - top)/(bot - top);

				int imgX = (int) ((wallX - w.texture.x)/w.texture.w*img.width);
				int imgY = (int) ((wallY - w.texture.y)/w.texture.h*img.height);

				rgb col = pixelColor(img, imgX, imgY, w.texture.mode);
				if (col.r == 2 && col.g == 9 && col.b == 2) continue;
				
				zBuffer[i][j] = dist;
				rgbBuffer[i][j] = col;
			}
		}

		for (Plane plane : planes) {
			std::vector<Vec3> intersects = planeRayIntersection(camX, camY, a, plane);
			if (intersects[0].x < 0) continue; // handles null intersection and behind player
			
			double top, bot;

			// this is still awful
			if (plane.z > camZ) {
				if (intersects[1].x > 0) {
					top = height/2.0*(1 + 2*(-plane.z + camZ)*near/intersects[1].x);
					bot = top + height*(-plane.z + camZ)*near*(1/intersects[0].x - 1/intersects[1].x);
				} else {
					top = 0;
					bot = height/2.0*(1 + 2*(-plane.z + camZ)*near/intersects[0].x);
				}
			} else {
				if (intersects[1].x > 0) {
					top = height/2.0*(1 + 2*(-plane.z + camZ)*near/intersects[0].x);
					bot = top + height*(-plane.z + camZ)*near*(1/intersects[1].x - 1/intersects[0].x);
				} else {
					top = height/2.0*(1 + 2*(-plane.z + camZ)*near/intersects[0].x);
					bot = height;
				}
			}

			Bitmap img = imgs[plane.texture.id];	

			for (int j = 0; j < ROWS; j++) {
				if (j*dh < top || j*dh > bot) continue;

				double dist = abs((plane.z - camZ)/(0.5 - j*dh/height)*near);
				if (zBuffer[i][j] >= 0 && zBuffer[i][j] < dist) continue;

				Vec2 corner1 = plane.getCorner1();
				Vec2 corner2 = plane.getCorner2();

				double worldX = camX + std::cos(a)*dist;
				double worldY = camY + std::sin(a)*dist;

				int imgX = (worldX - corner1.x)/(corner2.x - corner1.x)*img.width;
				int imgY = (worldY - corner1.y)/(corner2.y - corner1.y)*img.height;

				rgb col = pixelColor(img, imgX, imgY, 1);
				if (col.r == 2 && col.g == 9 && col.b == 2) continue;

				zBuffer[i][j] = dist;
				rgbBuffer[i][j] = col;
			}

		}

		// CARTEL INTERSECTIONS
		for (Cartel c : cartels) {
			Vec2 intersect = cartelRayIntersection(camX, camY, a, c);
			if (intersect.x < 0 || intersect.y > 1 || intersect.y < -1) continue;	

			double top = height/2.0 - height*(c.top - camZ)*near/intersect.x;
			double bot = height/2.0 - height*(c.bot - camZ)*near/intersect.x;

			Bitmap img = imgs[c.texture];	

			int imgX = (int) (img.width*(1-intersect.y)/2);
			for (int j = 0; j < ROWS; j++) {
				if (j*dh < top || j*dh > bot) continue;

				double dist = intersect.x;
				if (zBuffer[i][j] >= 0 && zBuffer[i][j] < dist) continue;

				int imgY = (int) ((j*dh - top)/(bot - top)*img.height);

				rgb col = pixelColor(img, imgX, imgY, 1);
				if (col.r == 2 && col.g == 9 && col.b == 2) continue;
				zBuffer[i][j] = dist;
				rgbBuffer[i][j] = col;
			}
		}

		chronoIntersect += nowMS();

		chronoPaint -= nowMS();

		Uint32 *dst;
		dst = (Uint32*) ((Uint8*) pixels + i*4);
		for (int j = 0; j < ROWS; j++) {
			rgb color = rgbBuffer[i][j];
			if (zBuffer[i][j] == -1) color = fogCol;
			else if (fog) {
				double realDist = zBuffer[i][j] * std::sqrt(1 + (j-ROWS/2.0)*(j-ROWS/2.0)/ROWS/ROWS);
				color.r = fogCol.r + (color.r - fogCol.r)*std::exp(-fogDense*realDist);
				color.g = fogCol.g + (color.g - fogCol.g)*std::exp(-fogDense*realDist);
				color.b = fogCol.b + (color.b - fogCol.b)*std::exp(-fogDense*realDist);
			}

			if (transitioning) {
				double m = 0;
				if (transT < 0.75) m = 1 - 4*transT/3;
				if (transT > 1.25) m = 4*(transT-1.25)/3;
				color.r *= m;
				color.g *= m;
				color.b *= m;
			}

			/*if (zBuffer[i][j] == -1) color = rgb {0, 0, 0};
			else {
				double realDist = zBuffer[i][j] * std::sqrt(1 + (j-ROWS/2.0)*(j-ROWS/2.0)/ROWS/ROWS);
				color = rgb {0, 255*cos(realDist*10+swingT*PI)*cos(realDist*10+swingT*PI), 0};
			}*/

			*dst = (0xFF000000|(color.r<<16)|(color.g<<8)|color.b); 
			dst += COLS;
		}

		chronoPaint += nowMS();
	}

};

int main(int argc, char* args[]) {
	Framework fw(1000, 800);

	fw.start();

	return 0;
}
