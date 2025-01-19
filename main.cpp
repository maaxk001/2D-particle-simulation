#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

//Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct Particle {
	float x, y;
	float vx, vy;
	float size;

	Particle(float px, float py, float velX, float velY, float pSize) 
		: x(px), y(py), vx(velX), vy(velY), size(pSize) {}

	void update(float gravity, float dt) {
		//Apply gravity
		vy += gravity * dt;

		//Update position
		x += vx * dt;
		y += vy * dt;

		//Boundary collision
		if (x <= 0 || x + size >= SCREEN_WIDTH) {
			vx = -vx; //Reverse horizontal velocity
		}
		if (y <= 0 || y + size >= SCREEN_HEIGHT) {
			vy = -vy; // Reverse vertical velocity
			y = (y + size > SCREEN_HEIGHT) ? SCREEN_HEIGHT - size : y; //Adjust position
		}
	}

	void render(SDL_Renderer *renderer) {
		SDL_Rect rect = { (int)x, (int)y, (int)size, (int)size };
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
};

std::vector<Particle> createParticles(int count) {
	std::vector<Particle> particles;
	for (int i = 0; i < count; ++i) {
		float x = (float) (rand() % SCREEN_WIDTH);
		float y = (float) (rand() % SCREEN_HEIGHT);
		float vx = (rand() % 200 - 100) / 100.0f; //Random velocity [-1, 1]
		float vy = (rand() % 200 - 100) / 100.0f;
		float size = 5.0f + (rand() % 10); // Size [5, 15]
		particles.emplace_back(x, y, vx, vy, size);
	}

	return particles;
}

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	

	SDL_Window* window = SDL_CreateWindow(
		"2D Particle Simulator",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if (!window) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Create SDL Renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Seed random number generator
	srand((unsigned int)time(0));

	// Create particles
	const int PARTICLE_COUNT = 100;
	std::vector<Particle> particles = createParticles(PARTICLE_COUNT);

	// Simulation parameters
	const float GRAVITY = 200.0f; // Gravity strength
	const float DELTA_TIME = 0.016f; // ~60 FPS
	bool running = true;

	SDL_Event event;
	
	while (running) {
		// Handle events
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}

		for (auto& p : particles) {
			p.update(GRAVITY, DELTA_TIME);
		}

		// Render particles
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
		SDL_RenderClear(renderer);
		for (auto& p : particles) {
			p.render(renderer);
		}
		SDL_RenderPresent(renderer);

		SDL_Delay(16);
	}

	// Clean up SDL resources
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}