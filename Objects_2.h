#ifndef __Objects__
#define __Objects__

#include<iostream>
#include<SDL_render.h>
#include<SDL_image.h>
#include<cmath>
#include"Shared_Texture.h"
#include"Big_data_array.h"
#include<fstream>
#include<list>

const int window_width = 1200;
const int window_height = 700;
int time_elapsed;

SDL_FPoint viewpoint = { 0.0f,0.0f };
SDL_Renderer* main_ren;

class Object
{
protected:
	SDL_FPoint center = { 0,0 };// coordinates of object in the play zone
	
	float most_right = 0.0F;//distance on x axis to farest point on the right 
	float most_left = 0.0F;//distance on x axis to farest point in the left , always negative
	float most_top = 0.0F;//distance on y axis to farest point on the top , always negative
	float most_bottom = 0.0F;//distance on y axis to farest point on the bottom
	int num_points = 0;// number of boundary points
	//---------------------------------------- order should not be chabged

	SDL_FPoint* points = nullptr;// boundary points of polygon
	//float radious = 0;// distance to farest points2 from center
	bool fixed = true;

public:
	inline Object() {};

	Object(SDL_FPoint center, int num_points, SDL_FPoint* boundary_points)
	{
		this->center = center;
		this->num_points = num_points;
		this->points = new SDL_FPoint[num_points];
		
		float m_r = -FLT_MAX, m_l = FLT_MAX;
		float m_b = -FLT_MAX, m_t = FLT_MAX;
		
		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint& p = boundary_points[i];
			this->points[i] = p;// copiing coordinates of all points 

			m_r = std::fmaxf(p.x, m_r);
			m_l = std::fminf(p.x, m_l);
			m_b = std::fmaxf(p.y, m_b);
			m_t = std::fminf(p.y, m_t);
		}

		this->most_right = m_r;
		this->most_left = m_l;
		this->most_top = m_t;
		this->most_bottom = m_b;
	}

	bool Collision(Object* object)
	{
		{
			/*
			* Checking for rectangular collision , if their rectangles not collide , they are indeed not intersecting
			*/

			if (this->most_left + this->center.x > object->center.x + object->most_right)
				return false;
			if (this->most_right + this->center.x < object->center.x + object->most_left)
				return false;

			if (this->most_bottom + this->center.y < object->most_top + object->center.y)
				return false;
			if (this->most_top + this->center.y > object->most_bottom + object->center.y)
				return false;
		}

		auto overlap = [](Object* obj_1, Object* obj_2)
		{
			int& num1 = obj_1->num_points;
			int& num2 = obj_2->num_points;;

			SDL_FPoint* points1 = obj_1->points;
			SDL_FPoint* points2 = obj_2->points;

			float& x1 = obj_1->center.x;
			float& y1 = obj_1->center.y;

			float& x2 = obj_2->center.x;
			float& y2 = obj_2->center.y;

			for (int i = 0; i < num1; i++)
			{
				int j = (i + 1) % num1;
				SDL_FPoint axis = { points1[i].y - points1[j].y,points1[j].x - points1[i].x };//////////////////////

				float left1 = INFINITY, right1 = -INFINITY;

				float d = 0.0f;
				for (j = 0; j < num1; j++)
				{
					SDL_FPoint p = { points1[j].x + x1 ,points1[j].y + y1 };

					d = p.x * axis.x + p.y * axis.y;

					if (d < left1)
						left1 = d;

					if (d > right1)
						right1 = d;
				}


				d = (points2[0].x + x2) * axis.x + (points2[0].y + y2) * axis.y;
				if (left1 <= d && d <= right1)
					continue;

				bool overlap = false;
				bool sign = (d >= left1);

				for (j = 1; j < num2; j++)
				{
					d = (points2[j].x + x2) * axis.x + (points2[j].y + y2) * axis.y;
					if ((d >= left1) != sign)
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
			* Checking for rectangular collision , if their rectangles not collide , they are indeed not intersecting
			*/

			if (this->most_left + this->center.x > object->center.x + object->most_right)
				return false;
			if (this->most_right + this->center.x < object->center.x + object->most_left)
				return false;

			if (this->most_bottom + this->center.y < object->most_top + object->center.y)
				return false;
			if (this->most_top + this->center.y > object->most_bottom + object->center.y)
				return false;
		}

		auto push = [](Object* poly1, Object* poly2)
		{
			SDL_FPoint& p1 = poly1->center;
			SDL_FPoint& c2 = poly2->center;
			int& num1 = poly1->num_points;
			int& num2 = poly2->num_points;
			auto& points1 = poly1->points;
			auto& points2 = poly2->points;
			for (int p = 0; p < num1; p++)
			{
				SDL_FPoint p2 = points1[p];
				p2.x += p1.x; p2.y += p1.y;

				SDL_FPoint displacement = { 0,0 };

				float dx1 = p2.x - p1.x;
				float dy1 = p2.y - p1.y;

				// ...against edges of this polygon
				for (int q = 0; q < num2; q++)
				{
					SDL_FPoint p3 = points2[q];
					p3.x += c2.x; p3.y += c2.y;

					SDL_FPoint p4 = points2[(q + 1) % num2];
					p4.x += c2.x; p4.y += c2.y;

					// Standard "off the shelf" line segment intersection

					float dx2 = p4.x - p3.x;
					float dy2 = p4.y - p3.y;

					float h = dx1 * dy2 - dx2 * dy1;

					float t1 = (dy2 * (p3.x - p1.x) - dx2 * (p3.y - p1.y)) / h;
					float t2 = (dy1 * (p3.x - p1.x) - dx1 * (p3.y - p1.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{
						displacement.x = (1.0f - t1) * dx1*2.2F;
						displacement.y = (1.0f - t1) * dy1*2.2F;

						if (poly1->fixed)
						{
							poly2->center.x += displacement.x;
							poly2->center.y += displacement.y;
							return true;
						}

						if (poly2->fixed)
						{
							poly1->center.x -= displacement.x;
							poly1->center.y -= displacement.y;
							return true;
						}

						displacement.x *= 0.5f;
						displacement.y *= 0.5f;


						poly1->center.x -= displacement.x;
						poly1->center.y -= displacement.y;
						poly2->center.x += displacement.x;
						poly2->center.y += displacement.y;
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

	inline bool InsideScreen()
	{
		if (this->most_right+this->center.x < viewpoint.x || this->most_left+this->center.x > viewpoint.x + window_width)
			return false;

		if (this->most_bottom+this->center.y < viewpoint.y || this->most_top + this->center.y > viewpoint.y + window_height)
			return false;

		return true;
	}

	void SetPoints(int num_points, SDL_FPoint boundary_points[])
	{
		this->num_points = num_points;
		
		if (this->points)
			delete[] this->points;

		this->points = new SDL_FPoint[num_points];

		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint& p = boundary_points[i];
			this->points[i] = p;// copiing coordinates of all points 

			this->most_right = std::fmaxf(p.x, this->most_right);
			this->most_left = std::fminf(p.x, this->most_left);
			this->most_bottom = std::fmaxf(p.y, this->most_bottom);
			this->most_top = std::fminf(p.y, this->most_top);
		}

	}

	inline ~Object()
	{
		// if boundary points created dynamicaly its points dhold be delted , else if boundary points created with object
		//its points points should be assigned to nullptr!!!!
		if(this->points)
			delete[] this->points;
		this->points = nullptr;
	}
};

class MovingObject :public Object
{
protected:
	SDL_FPoint* original_points = nullptr;// boundary points of polygon when it is not rotated
	float angle = 0;//rotated angle
	float speed = 0;// moving speed 
	float cos_a = 1;
	float sin_a = 0;
	short forward = 0;
public:

	MovingObject() {};

	MovingObject(SDL_FPoint center, int num_points, SDL_FPoint* boundary_points, float speed) :Object(center, num_points, boundary_points)
	{
		this->speed = speed;
		this->original_points = new SDL_FPoint[num_points];
		memcpy(this->original_points, boundary_points, num_points);
	}

	void SetPoints(int num_points,SDL_FPoint *boundary_points)
	{

		// if poinst of object needs to be created dynamically its num_points should be 0
		if(!this->num_points){
			if (this->points)// if poinst of object needs to be created dynamically its poinst should be nullptr
			{
				delete[] this->points;
				delete[] this->original_points;
			}
			this->points = new SDL_FPoint[num_points];
			this->original_points = new SDL_FPoint[num_points];
		}
		this->num_points = num_points;
		
		float m_r = -FLT_MAX, m_l = FLT_MAX;
		float m_b = -FLT_MAX, m_t = FLT_MAX;

		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint& p = boundary_points[i];
			this->points[i] = p;// copiing coordinates of all points 
			this->original_points[i] = p;

			// identifying edges of object
			m_r = std::fmaxf(p.x, m_r);
			m_l = std::fminf(p.x,m_l);
			m_b = std::fmaxf(p.y, m_b);
			m_t = std::fminf(p.y, m_t);
		}
		
		this->most_right = m_r;
		this->most_left = m_l;
		this->most_top = m_t;
		this->most_bottom = m_b;
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
		if (!da)
			return;

		this->angle += da;
		float rad = angle * M_PI / 180;
		cos_a = cosf(rad);
		sin_a = sinf(rad);

		float m_r = -FLT_MAX, m_l = FLT_MAX;
		float m_b = -FLT_MAX, m_t = FLT_MAX;
		
		for (int i = 0; i < num_points; i++)
		{
			SDL_FPoint& original = original_points[i];
			SDL_FPoint& point = points[i];

			point = { original.x * cos_a - original.y * sin_a,original.x * sin_a + original.y * cos_a };
			
			if (point.x > m_r)
				m_r = point.x;
			if (point.x < m_l)
				m_l = point.x;

			if (point.y > m_b)
				m_b = point.y;
			if (point.y < m_t)
				m_t = point.y;		
		}

		this->most_right = m_r;
		this->most_left = m_l;
		this->most_top = m_t;
		this->most_bottom = m_b;
	}

	~MovingObject()
	{
		// if boundary points created dynamicaly its points must be delted , else if boundary points created with object
		//its points original_points should be assigned to nullptr!!!!
		if(original_points)
			delete[] original_points;
		original_points = nullptr;
	}

	friend class Terrain;
};

class Bullet :public MovingObject
{
private:
	SDL_FPoint p_array[2];
	SDL_FPoint op_array[2];

	SDL_FRect rect;
	int life_time = 0;
	static float velocity;
public:
	static SDL_Texture* bullet_texture;
	static Big_array<Bullet> bullets;

	inline Bullet() {};

	Bullet(const SDL_FPoint& center,const float& angle)
	{
		this->center = center;
		this->speed = Bullet::velocity;
		int w = 0, h = 0;
		SDL_QueryTexture(bullet_texture, NULL, NULL, &w, &h);
		this->rect = { center.x - w * 0.5f,center.y - h * 0.5f,float(w),float(h) };
		this->forward = 1;

		this->num_points = 2;
		this->points = p_array;
		this->original_points = op_array;

		SDL_FPoint boundary_points[2] = { 
			{0.0F,-this->rect.h * 0.5f},
			{0.0F,this->rect.h * 0.5f}
		};
		this->SetPoints(2, boundary_points);

		//boundary points should be rotated after setting originla points!!!
		this->angle = 0;// angle of bullet will not change 
		this->RotateBy(angle);// so once it is created its angle rotated once
	}
	
	void Render()
	{
		if (!this->InsideScreen())
			return;
		this->rect.x = this->center.x - viewpoint.x - this->rect.w * 0.5f;
		this->rect.y = this->center.y - viewpoint.y - this->rect.h * 0.5f;

		SDL_RenderCopyExF(main_ren, this->bullet_texture, NULL, &(this->rect), this->angle, NULL, SDL_FLIP_NONE);
	}

	bool Move(Big_array<Bullet>::Iterator &iter)
	{
		life_time += time_elapsed;
		if (life_time > 2000)
		{
			Bullet::bullets.Remove(iter);
			return false;
		}

		this->MoveForward();
		this->Render();

		return true;
	}

	inline ~Bullet()
	{
		this->points = this->original_points = nullptr;
	}
};
Big_array<Bullet> Bullet::bullets(40);
SDL_Texture* Bullet::bullet_texture=nullptr;// bullet_texture must be initialized in main !!!
float Bullet::velocity = 0.5f;


class Tank :public MovingObject
{
private:

	SDL_FPoint p_array[4];// boundary points of polygon when it is rotated (it will created dynamically)
	SDL_FPoint op_array[4];// boundary points of polygon when it is "not" rotated (it will created dynamically)
	SDL_Texture* body;// texture of body of the tank
	SDL_Texture* head;// texture of the turret

	SDL_FRect body_rect;
	SDL_FRect head_rect;

	float head_angle;// angle of turret

	float body_rotate = 0.0f;// is body of the tank rotating(0: not rotating; >0: roating right ;<0 rotatong left)

	SDL_FPoint body_to_head;//distance from body_rect to head_rect
	SDL_FPoint head_rotate_point;// the turret will rotate relative to this point
	//control keys:
	
	short front;
	short back;
	short right;
	short left;
	
	short health = 100;// hp of player
	unsigned int fire_time = 0;// time since last fire
	short  reload_time = 800;// time to reload ammo

public:

	Tank(const SDL_FPoint& center, float speed, SDL_Texture* body_image, SDL_Texture* head_image)
	{
		this->center = center;
		this->speed = speed;
		this->head = head_image;
		this->body = body_image;
		this->fixed = false;

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

		SDL_FPoint edge_points[4] = {
			{-body_rect.w * 0.5f,-body_rect.h * 0.5f},
			{body_rect.w * 0.5f,-body_rect.h * 0.5f},
			{body_rect.w * 0.5f,body_rect.h * 0.5f},
			{-body_rect.w * 0.5f,body_rect.h * 0.5f}
		};

		this->points = p_array;
		this->original_points = op_array;
		this->num_points = 4;// if boundary points created with object , its num_points must be initialized before calling SetPoints()

		this->SetPoints(4, edge_points);
	}

	Tank(const SDL_FPoint& center, float speed, const char* body_path, const char* head_path)
	{
		this->center = center;
		this->speed = speed;
		this->fixed = false;
		SDL_Surface* surf = IMG_Load(head_path);
		this->head = SDL_CreateTextureFromSurface(main_ren, surf);

		this->head_rect = { center.x - surf->w,center.y - surf->h,float(surf->w),float(surf->h) };
		SDL_FreeSurface(surf);

		surf = IMG_Load(body_path);
		this->body = SDL_CreateTextureFromSurface(main_ren, surf);
		this->body_rect = { center.x - surf->w * 0.5f,center.y - surf->h * 0.5f,float(surf->w),float(surf->h) };
		SDL_FreeSurface(surf);

		body_to_head.x = (body_rect.w - head_rect.w) * 0.5f;//calculatiing disctance from body_rect.x to head_rect.x
		body_to_head.y = body_rect.h * 0.5f - head_rect.h * 0.7;// in order draw turret in the center of body

		head_rotate_point = { head_rect.w * 0.5f,head_rect.h * 0.7f };		
		this->points = p_array;
		this->original_points = op_array;
		this->num_points = 4;// if boundary points created with object , its num_points must be initialized before calling SetPoints()

		SDL_FPoint edge_points[4] = { 
			{-body_rect.w * 0.5f,-body_rect.h * 0.5f},
			{body_rect.w * 0.5f,-body_rect.h * 0.5f},
			{body_rect.w * 0.5f,body_rect.h * 0.5f},
			{-body_rect.w * 0.5f,body_rect.h * 0.5f}
		};

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
		const float rad = 180 / M_PI;
		head_angle = atanf(px / py) * rad;
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

		SDL_FRect temp{ this->most_left+this->center.x-viewpoint.x,this->most_top+this->center.y-viewpoint.y,this->most_right - this->most_left,this->most_bottom - this->most_top };
		SDL_RenderFillRectF(main_ren, &temp);

		SDL_RenderCopyExF(main_ren, this->body, NULL, &(this->body_rect), this->angle, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyExF(main_ren, this->head, NULL, &(this->head_rect), this->head_angle, &head_rotate_point, SDL_FLIP_NONE);
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

		if (key == this->left)
		{
			this->body_rotate = -0.1f;
			return true;
		}
		if (key == this->right)
		{
			this->body_rotate = 0.1f;
			return true;
		}

		return false;
	}

	bool Control_Keyup(short key)// returns true if gven key is control key of the tank 
	{
		if (key == this->front || key == this->back)
		{
			this->forward = 0;
			return true;
		}

		if (key == this->right || key == this->left)
		{
			this->body_rotate = 0.0f;
			return true;
		}
		return false;
	}

	void Fire()
	{
		std::cout << " fire time = " << fire_time << '\n';
		if (fire_time < reload_time)
			return;
		fire_time = 0;
		float rad = this->head_angle * (M_PI / 180);

		SDL_FPoint p = { this->center.x + head_rotate_point.y * sinf(rad), this->center.y - this->head_rotate_point.y * cosf(rad) };
		Bullet::bullets.Emplace_back(p, this->head_angle);
	}

	void Move()
	{
		this->MoveForward();

		float distance = this->forward * this->speed * time_elapsed;
		viewpoint.x += distance * this->sin_a;
		viewpoint.y -= distance * this->cos_a;

		this->RotateBy(body_rotate * time_elapsed);
		this->Render();
		
		if(this->fire_time<=this->reload_time)
			this->fire_time += time_elapsed;
	}

	~Tank()
	{
		this->points = this->original_points = nullptr;
		SDL_DestroyTexture(this->head);
		SDL_DestroyTexture(this->body);
	}

	Tank() = default;

	friend class Terrain;
};

class Wall :public Object
{
private:
	SDL_Vertex* vertices=nullptr;
	static SDL_Colour color;
	int* vertex_indecies=nullptr;
	const static short data_size;
	void ReadFromFile(std::ifstream& file, int position = -1)
	{
		if (position > 0)//if position given -1 then it will writo to posion where "write poiter" loacated of file
			file.seekg(position);

		//file.read((char*)this, Wall::data_size);
		
		file.read((char*)&this->center, sizeof(SDL_FPoint));
		file.read((char*)&this->most_right, sizeof(float));
		file.read((char*)&this->most_left, sizeof(float));
		file.read((char*)&this->most_top, sizeof(float));
		file.read((char*)&this->most_bottom, sizeof(float));

		file.read((char*)&num_points, sizeof(int));
		this->points = new SDL_FPoint[num_points];
		file.read((char*)points, num_points *sizeof(SDL_FPoint));
		this->InitVertices();
	}
public:

	static std::list<Wall> walls;
	Wall() = default;
	
	Wall(Wall&& obj)
	{

	}

	void UpdateVertices()
	{
		float x = this->center.x - viewpoint.x;
		float y = this->center.y - viewpoint.y;


		for (int i = 0; i <num_points; i++)
		{
			this->vertices[i].position.x = this->points[i].x + x;
			this->vertices[i].position.y = this->points[i].y + y;
		}
		this->vertices[this->num_points].position = { x,y };
	}

	void InitVertices()
	{		
		this->vertices = new SDL_Vertex[this->num_points+1];
		this->vertex_indecies = new int[3 * this->num_points];

		for (int i = 0,j=0; i <num_points; i++,j+=3)
		{
			this->vertices[i] = { this->points[i],this->color,{0,0}};
			
			vertex_indecies[j] = num_points;
			vertex_indecies[j + 1] = i;
			vertex_indecies[j + 2] = i + 1;
		}

		this->vertices[this->num_points] = { this->center,{220,220,220},{0,0}};
		vertex_indecies[3 * num_points-1]=0;
	}

	Wall(const SDL_FPoint& center)
	{
		this->center = center;
		this->fixed = true;
		this->InitVertices();
	}

	void Render()
	{
		if (!this->InsideScreen())
			return;

		this->UpdateVertices();
		SDL_RenderGeometry(main_ren, NULL, vertices, num_points+1,vertex_indecies,num_points*3);

		/*float x = center.x - viewpoint.x;
		float y = center.y - viewpoint.y;

		for (size_t i = this->num_points-1; i >0 ; i--)
		{
			SDL_FPoint& p1 = this->points[i];
			SDL_FPoint& p2 = this->points[i-1];
			SDL_RenderDrawLineF(main_ren, p1.x + x, p1.y + y, p2.x + x, p2.y + y);
		}

		SDL_RenderDrawLineF(main_ren, points[0].x +x , points[0].y+y, points[num_points-1].x +x, 
			points[num_points-1].y +y);

		SDL_RenderDrawLineF(main_ren, points[0].x + x, points[0].y + y, x, y);*/
	}

	static void LoadWallsFromFile(const char* file_name)
	{
		std::ifstream file(file_name, std::ios::binary);
		if (!file.is_open())
		{
			std::cout << " File not found!" << std::endl;
			return;
		}
		int num_obj;
		file.read((char*)&num_obj, sizeof(int));
		

		while (num_obj-- > 0)
		{
			Wall::walls.emplace_back();
			Wall::walls.back().ReadFromFile(file);
		}
		file.close();
	}

	~Wall()
	{
		if (this->vertices)
			delete[] this->vertices;
		if (this->vertex_indecies)
			delete[] this->vertex_indecies;
	}
};
const short Wall::data_size = sizeof(SDL_FPoint) + 4 * sizeof(float);
SDL_Color Wall::color = { 180,180,180,255};
std::list<Wall> Wall::walls;

class Terrain
{
private:
	SDL_Renderer* ren;
	SDL_Texture* texture;
	SDL_FRect dstrect;

	Tank* main_char;
	SDL_FPoint char_position = { 0.0F,0.0F };// positon of main charrracter inside screen

	float x = 0.0f, y = 0.0f;// starting position of drawing seamles(continious) texture

	int row = 0, col = 0;// number of col and row to fill screeen with texture

public:
	bool moving = false;
	float dx = 0.0f, dy = 0.0f;// speed of movement

	Terrain(SDL_Renderer* renderer, const char* texture_path, int screen_width, int screen_height, Tank* main_charracter,const SDL_FPoint& main_char_posion)
	{
		this->ren = renderer;
		this->main_char = main_charracter;
		this->char_position = main_char_posion;
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
			dstrect.x = x + i * dstrect.w;
			for (int j = 0; j < row; j++)
			{
				dstrect.y = y + j * dstrect.h;
				SDL_RenderCopyF(ren, this->texture, NULL, &dstrect);
			}
		}
	}

	void Move()
	{
		if (main_char->forward||main_char->body_rotate) {

			if (x >= 0)
				x -= dstrect.w;
			else
				if (x <= -dstrect.w)
					x = 0.0f;

			if (y >= 0)
				y -= dstrect.h;
			else
				if (y <= -dstrect.h)
					y = 0.0f;

			float x1 = viewpoint.x;
			float y1 = viewpoint.y;

			viewpoint.x = main_char->center.x - char_position.x;
			viewpoint.y = main_char->center.y - char_position.y;

			this->x += x1-viewpoint.x;
			this->y += y1-viewpoint.y;
		}

		this->Render();
	}

	~Terrain()
	{
		SDL_DestroyTexture(this->texture);
	}
};
#endif // !__Objects__