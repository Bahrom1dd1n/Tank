#ifndef __Shared_Image__
#define __Shared_Image__

#include<SDL_render.h>
#include<SDL_image.h>
#include<cmath>

class Shared_Image
{
protected:
	SDL_Renderer* ren;
	SDL_Texture* texture=nullptr;
	SDL_FRect rect;
	int* ref_count=nullptr;
public:

	Shared_Image() {};

	Shared_Image(Shared_Image& shared_image)
	{
		this->texture = shared_image.texture;
		this->ref_count = shared_image.ref_count;
		*(this->ref_count)++;
	}

	Shared_Image(const char* image_path)
	{
		SDL_Surface* surf = IMG_Load(image_path);
		this->texture = SDL_CreateTextureFromSurface(this->ren, surf);
		this->rect.w = float(surf->w);
		this->rect.h = float(surf->h);
		SDL_FreeSurface(surf);
	}

	Shared_Image(SDL_Texture* texture)
	{
		this->texture = texture;
		this->ref_count = new int(1);
		int w=0, h=0;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
		this->rect.w = float(w);
		this->rect.h = float(h);
	}

	void SetPosition(float x, float y)
	{
		this->rect.x = x;
		this->rect.y = y;
	}

	SDL_Texture* GetTexture()const
	{
		return this->texture;
	}

	void operator=(Shared_Image& shared_image)
	{
		this->Free();
		this->texture = shared_image.texture;
		this->ref_count = shared_image.ref_count;
		*(this->ref_count)++ ;
	}

	void operator=(SDL_Texture* texture)
	{
		this->Free();
		this->texture = texture;
		this->ref_count = new int(1);
		int w = 0, h = 0;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
		this->rect.w = float(w);
		this->rect.h = float(h);
	}

	void Free()
	{
		if (ref_count)
			(*ref_count)--;
		else
			return;

		if (texture && *ref_count == 0)
		{
			SDL_DestroyTexture(texture);
			delete ref_count;
		}
	}

	virtual void Render()
	{
		SDL_RenderCopyF(this->ren, this->texture, NULL, &(this->rect));
	}

	friend class Object;

	~Shared_Image()
	{
		this->Free();
	}
};

class Sahred_FImage:Shared_Image
{
protected:
	float angle = 0;
	SDL_FPoint rotation_point = { 0,0 };
public:

	void SetRotationPoint(SDL_FPoint& point)
	{
		this->rotation_point = point;
	}

	void Render()
	{
		SDL_FPoint* point = ((rotation_point.x)||(rotation_point.y)? &rotation_point : NULL);
		SDL_RenderCopyExF(this->ren, this->texture,NULL, &(this->rect),this->angle,point,SDL_FLIP_NONE);
	}
};

#endif // !__Shared_Image__
