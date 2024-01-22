#ifndef __Objects__
#define __Objects__

#include<iostream>
#include<SDL_render.h>
#include<SDL_image.h>
#include<cmath>
#include"Shared_Texture.h"
#include<list>

const int window_width = 1200;
const int window_height = 700;
SDL_FPoint viewpoint = { 0.0f,0.0f };
int time_elapsed;


class Object
{
protected:
	SDL_FPoint center = { 0,0 };// coordinates of object in the play zone

	SDL_FPoint* points = nullptr;// boundary points of polygon when it is not rotated
	int num_points = 0;// number of boundary points
	float radious = 0;// distance to farest points2 from center
	bool fixed = true;

public:
	Object() {};

	Object(SDL_FPoint center, int num_points, SDL_FPoint* boundary_points)
	{
		this->center = center;
		this->num_points = num_points;
		this->points = new SDL_FPoint[num_points];

		radious = 0;

		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint p = boundary_points[i];
			this->points[i] = p;// copiing coordinates of all points 

			float dis = (center.x - p.x) * (center.x - p.x) + (center.y - p.y) * (center.y - p.y);//finding max distance
			if (dis > radious)
				radious = dis;
		}

		radious = sqrtf(radious);
	}

	bool Collision(Object* object)
	{
		{
			/*
				if object far enough
				we initially suppose both object as circles then check for collision

				if discantec between circles > sum of their radiouses then they are not colliding
				thus both objects are not collidiong

			*/

			float p = this->center.x - object->center.x;
			float q = this->center.y - object->center.y;
			p *= p;
			q *= q;
			p += q;
			q = this->radious + object->radious;
			q *= q;

			if (p > q)
				return false;
		}

		auto overlap = [](Object* obj_1, Object* obj_2)
		{
			int &num1 = obj_1->num_points;
			int &num2 = obj_2->num_points;;

			SDL_FPoint* points1 = obj_1->points;
			SDL_FPoint* points2 = obj_2->points;

			float &x1 = obj_1->center.x;
			float &y1= obj_1->center.y;

			float &x2= obj_2->center.x;
			float &y2= obj_2->center.y;

			for (int i = 0; i < num1; i++)
			{
				int j = (i + 1) % num1;
				SDL_FPoint axis = { points1[i].y - points1[j].y,points1[j].x - points1[i].x };

				float left1 = INFINITY, right1 = -INFINITY;

				float d = 0.0f;
				for (j = 0; j < num1; j++)
				{
					SDL_FPoint p = { points2[j].x + x1 ,points2[j].y + y1 };

					d = p.x * axis.x + p.y * axis.y;

					if (d < left1)
						left1 = d;

					if (d > right1)
						right1 = d;
				}

				
				d = (points2[j].x + x2) * axis.x + (points2[j].y + y2) * axis.y;
				if (left1 <= d && d <= right1)
					continue;
			
				bool overlap = false;
				bool sign = (d >= left1);

				for (j = 0; j < num2; j++)
				{
					d = (points2[j].x + x2) * axis.x + (points2[j].y + y2) * axis.y;
					if (d >= left1 != sign)
					{
						overlap = true;// there is overlapping
						break;
					}
					if (left1 <= d && d <= right1)
					{
						overlap = true;// there is overlapping
						break;
					}
				}

				if (!overlap)
					return false;// found seperating points2 
			}

			return true;// could not find seperating line for obj_1

		};

		if (!overlap(this, object))
			return false;

		return overlap(object, this);
	}
	
	bool StaticCollision(Object* object)
	{
		{
			/*
				if object far enough
				we initially suppose both object as circles then check for collision

				if discantec between circles > sum of their radiouses then they are not colliding
				thus both objects are not collidiong

			*/

			float p = this->center.x - object->center.x;
			float q = this->center.y - object->center.y;
			p *= p;
			q *= q;
			p += q;
			q = this->radious + object->radious;
			q *= q;

			if (p > q)
				return false;
		}

		auto push = [](Object* obj1, Object* obj2)
		{
			SDL_FPoint p1 = obj1->center;
			SDL_FPoint c2 = obj2->center;
			SDL_FPoint* &points1 = obj1->points;
			SDL_FPoint* &points2 = obj2->points;

			for (int p = 0; obj1->num_points; p++)
			{
				
				SDL_FPoint p2 = points1[p];

				SDL_FPoint displacement = { 0,0 };

				float dx1 = p2.x - p1.x;
				float dy1 = p2.y - p1.y;

				for (int q = 0; q < obj2->num_points; q++)
				{
					SDL_FPoint p3 = points2[q];
					SDL_FPoint p4 = points2[(q + 1) % obj2->num_points];

					float dx2 = p4.x - p3.x;
					float dy2 = p4.y - p3.y;

					float h = dx1 * dy2 - dx2 * dy1;

					float t1 = (dy2 * (p3.x+c2.x- p1.x) - dx2 * (p3.y+c2.y - p1.y)) / h;
					float t2 = (dy1 * (p3.x+c2.x - p1.x) - dx1 * (p3.y+c2.y - p1.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{
						displacement.x = (1.0f - t1) * dx1;
						displacement.y = (1.0f - t1) * dy1;

						if (obj1->fixed)
						{
							obj2->center.x += displacement.x;
							obj2->center.y += displacement.y;
							return true;
						}
						if(obj2->fixed)
						{
							obj1->center.x -= displacement.x;
							obj1->center.y -= displacement.y;
							return true;
						}
						displacement.x *= 0.5f;
						displacement.y *= 0.5f;

						obj2->center.x += displacement.x;
						obj2->center.y += displacement.y;

						obj1->center.x -= displacement.x;
						obj1->center.y -= displacement.y;

						return true;
					}
				}
			}

			return false;
		};

		if (push(this, object))
			return true;

		return push(object, this);
	}

	bool InsideScreen()
	{
		if (this->center.x + this->radious < viewpoint.x || this->center.x - this->radious > viewpoint.x+window_width)
			return false;

		if (this->center.y + this->radious < viewpoint.y || this->center.y - this->radious > viewpoint.y + window_height)
			return false;

		return true;
	}

	void SetPoints(int num_points,SDL_FPoint boundary_points[])
	{
		this->num_points = num_points;
		if (this->points)
			delete[] this->points;
		
		this->points = new SDL_FPoint[num_points];
		
		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint &p = boundary_points[i];
			this->points[i] = p;// copiing coordinates of all points 

			float dis = (center.x - p.x) * (center.x - p.x) + (center.y - p.y) * (center.y - p.y);//finding max distance
			if (dis > radious)
				radious = dis;
		}

		radious = sqrtf(radious);
	}

	~Object()
	{
		delete[] points;
	}
};

class MovingObject:public Object
{
protected:

	SDL_FPoint* original_points=nullptr;// boundary points of polygon when it is not rotated
	
	float angle=0;//rotated angle
	float speed = 0;// moving speed 
	float cos_a = 0;
	float sin_a = -1;
	short forward=0;
public:


	MovingObject() {};

	MovingObject(SDL_FPoint center, int num_points, SDL_FPoint* boundary_points, float speed):Object(center,num_points,boundary_points)
	{
		this->speed = speed;
		this->original_points = new SDL_FPoint[num_points];
		memcpy(this->original_points, boundary_points, num_points);
	}

	void SetPoints(int num_points, SDL_FPoint boundary_points[])
	{
		this->num_points = num_points;
		if (this->points)
		{
			delete[] this->points;
			delete[] this->original_points;
		}

		this->points = new SDL_FPoint[num_points];
		this->original_points = new SDL_FPoint[num_points];

		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint& p = boundary_points[i];
			this->points[i]=this->original_points[i] = p;// copiing coordinates of all points 

			float dis = (center.x - p.x) * (center.x - p.x) + (center.y - p.y) * (center.y - p.y);//finding max distance
			if (dis > radious)
				radious = dis;
		}

		radious = sqrtf(radious);
	}

	void MoveForward()
	{
		if (!forward)
			return;
		float distance = this->forward * this->speed * time_elapsed;
		this->center.x += distance * this->sin_a;
		this->center.y -= distance * this->cos_a;
	}

	void RotateBy(float da)
	{
		this->angle += da;
		float rad = angle * M_PI / 180;
		cos_a = cosf(rad);
		sin_a = sinf(rad);
		
		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint& p = original_points[i];
			points[i] = { p.x * cos_a - p.y * sin_a,p.x * sin_a + p.y * cos_a};
		}
	}

	friend class Terrain;

	~MovingObject()
	{
		delete original_points;
	}
};


class Bullet :MovingObject
{
private:
	SDL_Renderer* ren;
	SDL_FRect rect;
	int life_time = 0;

public:
	static SDL_Texture* bullet_texture;
	static std::list<Bullet> bullets;

	Bullet() {};
	Bullet(SDL_Renderer* renderer,SDL_FPoint center, float angle, float speed)
	{
		this->ren = renderer;
		this->center = center;

		SDL_FPoint boundary_points[2] = { -this->rect.h * 0.5,this->rect.h * 0.5f };
		this->SetPoints(2, boundary_points);
		this->angle = 0;
		this->RotateBy(angle);
		this->speed = speed;
		int w = 0, h = 0;
		SDL_QueryTexture(bullet_texture, NULL, NULL, &w, &h);
		this->rect = {center.x - w * 0.5f,center.y - h * 0.5f,float(w),float(h) };
		this->forward = 1;
	}
	void Render()
	{
		if (!this->InsideScreen())
			return;
		this->rect.x = this->center.x - viewpoint.x - this->rect.w * 0.5f;
		this->rect.y = this->center.y - viewpoint.y - this->rect.h * 0.5f;
		
		SDL_RenderCopyExF(this->ren, this->bullet_texture, NULL, &(this->rect), this->angle, NULL, SDL_FLIP_NONE);
	}

	void Move(std::list<Bullet>::iterator turn)
	{
		life_time += time_elapsed;
		if (life_time > 1000)
		{
			this->~Bullet();
			
			Bullet::bullets.erase(turn);
			return;
		}
		
		this->MoveForward();
		this->Render();
	}

	~Bullet()
	{
		
	}
};
std::list<Bullet> Bullet::bullets;
SDL_Texture* Bullet::bullet_texture = nullptr;

class Tank:public MovingObject
{
private:

	SDL_Renderer* ren;
	SDL_Texture* body;// texture of body of the tank
	SDL_Texture* head;// texture of the turre

	SDL_FRect body_rect;
	SDL_FRect head_rect;
	
	float head_angle;
	float body_rotate = 0.0f;// is body of the tank rotating(0: not rotating; >0: roating right ;<0 rotatong left)
	float turret_rotate_to = 0.0f;
	float turret_rotating = 0.0f;//is turret rotating(0: not rotating; >0: roating right ;<0 rotatong left)

	SDL_FPoint body_to_head;//distance from body_rect to head_rect
	SDL_FPoint head_rotate_point;// the turret will rotate relative to this point
	//control keys:
	short front;
	short back;
	short right;
	short left;
	short health = 100;// hp of player
	short fire_time = 0;// time since last fire
	short  reload_time = 800;// time to reload ammo

public:
	static std::list<Tank> tanks;

	Tank(SDL_Renderer* renderer, SDL_FPoint center, float speed, SDL_Texture* body_image, SDL_Texture* head_image)
	{
		this->ren = renderer;
		this->center = center;
		this->speed = speed;
		this->head = head_image;
		this->body = body_image;
		
		// taking dimensions of textures
		int w = 0, h = 0;
		SDL_QueryTexture(body_image, NULL, NULL, &w, &h);
		this->body_rect.w = float(w);
		this->body_rect.h = float(h);

		SDL_QueryTexture(head_image, NULL, NULL, &w, &h);
		this->head_rect.w = float(w);
		this->head_rect.h = float(h);

		body_to_head.x = (body_rect.w - head_rect.w) * 0.5f;//calculatiing disctance from body_rect.x to head_rect.x
		body_to_head.y = body_rect.h * 0.5f - head_rect.h * 0.7;// in order draw turret in the center of body

		head_rotate_point = { head_rect.w * 0.5f,head_rect.h * 0.7f };

		SDL_FPoint edge_points[4] = { -body_rect.w * 0.5f,-body_rect.h * 0.5f,body_rect.w * 0.5f,body_rect.h * 0.5f };
		this->SetPoints(4, edge_points);
	}

	Tank(SDL_Renderer* renderer, SDL_FPoint center, float speed, const char* body_path, const char* head_path)
	{
		this->ren = renderer;
		this->center = center;
		this->speed = speed;
		SDL_Surface* surf = IMG_Load(head_path);
		this->head = SDL_CreateTextureFromSurface(this->ren,surf);
		
		this->head_rect = { center.x - surf->w,center.y - surf->h,float(surf->w),float(surf->h) };
		SDL_FreeSurface(surf);

		surf = IMG_Load(body_path);
		this->body = SDL_CreateTextureFromSurface(this->ren, surf);
		this->body_rect = { center.x - surf->w * 0.5f,center.y - surf->h * 0.5f,float(surf->w),float(surf->h) };
		SDL_FreeSurface(surf);

		body_to_head.x = (body_rect.w - head_rect.w) * 0.5f;//calculatiing disctance from body_rect.x to head_rect.x
		body_to_head.y = body_rect.h * 0.5f - head_rect.h * 0.7;// in order draw turret in the center of body
		
		head_rotate_point = { head_rect.w * 0.5f,head_rect.h * 0.7f };

		SDL_FPoint edge_points[4] = { -body_rect.w * 0.5f,-body_rect.h * 0.5f,body_rect.w * 0.5f,body_rect.h * 0.5f };
		this->SetPoints(4, edge_points);
	}

	void SetControlKeys(short front, short back, short right, short left)
	{
		this->front = front;
		this->back = back;
		this->right = right;
		this->left = left;
	}

	void RotateTurretToPoint(float x, float y)
	{
		float px = x - this->center.x;
		float py = this->center.y - y;
		if (py == 0)
		{
			if (px > 0)
				this->head_angle = 90;
			else
				this->head_angle = -90;
			return;
		}

		head_angle = atanf(px / py) * 180 / M_PI;
		if (py < 0)
			head_angle += 180;
	}

	void Render()
	{
		if (!this->InsideScreen())
			return;
		this->body_rect.x = this->center.x - viewpoint.x - body_rect.w * 0.5f;
		this->body_rect.y = this->center.y - viewpoint.y - body_rect.h * 0.5f;

		this->head_rect.x = this->body_rect.x + this->body_to_head.x;
		this->head_rect.y = this->body_rect.y + this->body_to_head.y;

		SDL_RenderCopyExF(this->ren, this->body, NULL, &(this->body_rect), this->angle, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyExF(this->ren, this->head, NULL, &(this->head_rect), this->head_angle, &head_rotate_point, SDL_FLIP_NONE);
	}

	bool Control_Keydown(short key)// returns true if gven key is control key of the tank 
	{
		if (key == this->front)
		{
			this->forward = 1;
			return true;
		}
		if (key == this->back)
		{
			this->forward = -1;
			return true;
		}
		
		if (key == this->right)
		{
			this->body_rotate = 0.1f;
			return true;
		}

		if (key == this->left)
		{
			this->body_rotate = -0.1f;
			return true;
		}

		return false;
	}

	bool Control_Keyup(short key)// returns true if gven key is control key of the tank 
	{
		if (key == this->front|| key == this->back)
		{
			this->forward = 0;
			return true;
		}

		if (key == this->right|| key == this->left)
		{
			this->body_rotate = 0.0f;
			return true;
		}
		return false;
	}

	void Fire()
	{
		if (fire_time < reload_time)
			return;
		fire_time = 0;
		float rad = this->head_angle * (M_PI / 180);
		SDL_FPoint c = { this->center.x + head_rotate_point.y * sinf(rad),this->center.y - this->head_rotate_point.y * cosf(rad) };
		Bullet::bullets.emplace_back(this->ren, std::move(c), this->head_angle, 2.0f);
	}

	void Move()
	{
		this->MoveForward();
		this->RotateBy(body_rotate*time_elapsed);
		this->Render();
		fire_time += time_elapsed;
	}

	~Tank()
	{
		SDL_DestroyTexture(this->head);
		SDL_DestroyTexture(this->body);
	}
};
std::list<Tank> Tank::tanks;

class Terrain
{
private:

	SDL_Renderer* ren;
	SDL_Texture* texture;
	SDL_FRect dstrect;

	MovingObject* main_char;

	float x = 0.0f, y = 0.0f;// starting position of drawing seamles(continious) texture

	int row = 0, col = 0;// number of col and row to fill screeen with texture

public:
	bool moving = false;
	float dx = 0.0f, dy = 0.0f;// speed of movement

	Terrain(SDL_Renderer* renderer,int screen_width, int screen_height, Tank* main_charracter, const char* texture_path)
	{
		this->ren = renderer;
		this->main_char = main_charracter;
		SDL_Surface* surf = IMG_Load(texture_path);
		texture = SDL_CreateTextureFromSurface(renderer, surf);
		dstrect.w = surf->w;
		dstrect.h = surf->h;
		row = screen_height / surf->h + 2;
		col = screen_width / surf->w + 2;
		SDL_FreeSurface(surf);
	}

	void Render()
	{
		for (int i = 0; i < col; i++)
		{
			dstrect.x = x + float(i * dstrect.w);
			for (int j = 0; j < row; j++)
			{
				dstrect.y = y + float(j * dstrect.h);
				SDL_RenderCopyF(ren, this->texture, NULL, &dstrect);
			}
		}
	}

	void Move()
	{
		if (main_char->forward) {

			if (x > 0)
				x -= dstrect.w;
			else
				if (x < -dstrect.w)
					x = 0.0f;

			if (y > 0)
				y -= dstrect.h;
			else
				if (y < -dstrect.h)
					y = 0.0f;

			float dis_x = main_char->forward * main_char->speed * time_elapsed;//total moved distance
			
			float dis_y = -dis_x * main_char->cos_a;// 
			dis_x *= main_char->sin_a;

			this->x -= dis_x;
			this->y -= dis_y;

			viewpoint.x += dis_x;
			viewpoint.y += dis_y;
		}

		this->Render();
	}

	~Terrain()
	{
		SDL_DestroyTexture(this->texture);
	}
};

#endif // !__Objects__