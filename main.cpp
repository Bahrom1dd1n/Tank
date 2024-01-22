#include<iostream>

#include<SDL_render.h>
#include<SDL_image.h>
#include<cmath>
#include<vector>
#include<SDL_timer.h>

#include"Objects_2.h"

const Uint32 delay = 20;

int main(int argc, char* args[])
{
	SDL_Window* win;

	win = SDL_CreateWindow("Game", 0, 20, window_width, window_height, SDL_WINDOW_SHOWN);
	
	main_ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	
	bool running = true;
	
	Tank player({ window_width * 0.5f,window_height * 0.5f },0.1f, "Sources/hull.png", "Sources/turret.png");
	player.SetControlKeys(26, 22, 7, 4);
	Bullet::bullet_texture = IMG_LoadTexture(main_ren, "Sources/bullet.png");
	Terrain ground(main_ren, "Sources/grass.png", window_width, window_height, &player, { window_width * 0.5F, window_height * 0.5F });

	
	for (int i = 0; i < 8; i++)
	{
		float w = 50.0F + i * 60.0F;
		SDL_FPoint center = { -200.0F + i * 700.0F,100.0F };
		Wall::walls.Emplace_back(center,  w,w );
	}

	int old_key = 0;
	auto keydown = [&](int key)
	{
		if (key == old_key)
			return;

		player.Control_Keydown(key);

		old_key = key;
		std::cout << " key value: " << key << std::endl;
	};
	auto keyup = [&](int key)
	{
		old_key = 0;
		player.Control_Keyup(key);
	};
	auto mousemove = [&](int x, int y)
	{
		player.RotateTurretToPoint(float(x)+viewpoint.x, float(y)+viewpoint.y);
	};
	auto mousedown = [&](int x, int y)
	{
		player.Fire();
		std::cout << " bullets: " << Bullet::bullets.GetSize() << '\n';
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
		ground.Move();
		
		player.Move();
		
		auto it = Bullet::bullets.Begin();
		for(int i=0;i<Bullet::bullets.GetSize();i++)
		{
			auto p = it;
			it.operator++();
			bool hit = false;
			Bullet* bullet = p.GetData();

			auto it2 = Wall::walls.Begin();
			for (int i = 0; i < Wall::walls.GetSize(); i++)
			{
				if (bullet->Collision(it2.GetData()))
				{
					Bullet::bullets.Remove(p);
					hit = true;
					std::cout << " Hit \n";
					break;
				}
				++it2;
			}
			
			if (hit)
				continue;

			bullet->Move(p);
		}

		auto it2 = Wall::walls.Begin();
		for (int i = 0; i < Wall::walls.GetSize(); i++)
		{
			Wall* wall = it2.GetData();
			
			player.StaticCollision(wall);

			wall->Render();
			++it2;
		}

		SDL_RenderPresent(main_ren);
		//stricting frame rate to 40 fps
		{
			time_elapsed = SDL_GetTicks()-start;
			if (time_elapsed < ::delay)
			{
				SDL_Delay(::delay - time_elapsed);
				time_elapsed = ::delay;
			}
		}
	}

	SDL_DestroyTexture(Bullet::bullet_texture);
	SDL_DestroyRenderer(main_ren);
	SDL_DestroyWindow(win);
	return 0;
}