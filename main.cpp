#include<iostream>

#include<SDL_render.h>
#include<SDL_image.h>
#include<cmath>
#include<vector>
#include<SDL_timer.h>

#include"Objects_2.h"

const Uint32 delay = 20;

void Run()
{
	bool running = true;

	
	Controller player(Tank::Create({ window_width * 0.5f,window_height * 0.5f }, 0.1f, "Sources/hull.png", "Sources/turret.png"),26, 22, 7, 4);
	Bullet::bullet_texture.Load("Sources/bullet.png");
	//Terrain ground(main_ren, "Sources/grass.png", window_width, window_height, &player, { window_width * 0.5F, window_height * 0.5F });
	Wall::LoadWallsFromFile("map");
	int old_key = 0;
	auto keydown = [&](int key)
	{
		if (key == old_key)
			return;

		player.OnKeyDown(key);

		old_key = key;
		std::cout << " key value: " << key << std::endl;
	};
	auto keyup = [&](int key)
	{
		old_key = 0;
		player.OnKeyUp(key);
	};
	auto mousemove = [&](int x, int y)
	{
		player.GetTareget().RotateTurretToPoint(float(x) + viewpoint.x, float(y) + viewpoint.y);
	};
	auto mousedown = [&](int x, int y)
	{
		player.GetTareget().Fire();
		std::cout << " bullets: " << Bullet::bullets.size() << '\n';
	};
	auto update = [&]() {
		
		player.MoveTarget();

		auto next = Bullet::bullets.begin();
		while(next != Bullet::bullets.end())
		{
			bool hit = false;
			auto it = next;
			next++;
			
			for (auto it2 = Wall::walls.begin(); it2!=Wall::walls.end(); it2++)
			{
				if (it->Collision(&(*it2)))
				{
					Bullet::bullets.erase(it);
					hit = true;
					std::cout << " Hit \n";
					break;
				}
			}

			if (hit)
				continue;

			it->Move();
		}

		for (auto it = Wall::walls.begin(); it!= Wall::walls.end(); it++)
		{
			Wall* wall = &(*it);

			player.GetTareget().StaticCollision(wall);

			wall->Render();
		}

		SDL_RenderPresent(main_ren);
	};

	SDL_Event event;
	while (running)
	{
		int start = SDL_GetTicks();
		// event handling
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				keydown(event.key.keysym.scancode);
				break;

			case SDL_KEYUP:
				keyup(event.key.keysym.scancode);
				break;

			case SDL_MOUSEMOTION:
				mousemove(event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mousedown(event.button.x, event.button.y);
				break;
			}
		}
		SDL_SetRenderDrawColor(main_ren, 0, 0, 0, 0);
		SDL_RenderClear(main_ren);
		SDL_SetRenderDrawColor(main_ren, 255, 255, 255, 0);
		//ground.Move();
		update();
		//stricting frame rate to 40 fps
		{
			time_elapsed = SDL_GetTicks() - start;
			if (time_elapsed < ::delay)
			{
				SDL_Delay(::delay - time_elapsed);
				time_elapsed = ::delay;
			}
		}
	}
}

int main(int argc, char* args[])
{
	SDL_Window* win;
	win = SDL_CreateWindow("Game", 0, 20, window_width, window_height, SDL_WINDOW_SHOWN);
	main_ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	::Run();
	SDL_DestroyRenderer(main_ren);
	SDL_DestroyWindow(win);
	return 0;
}