#include "SDL.h"
#include "SDL_image.h"
#include "math.h"
#define min(A,B) (((A)>(B))?(B):(A))

inline int rand(int m)
{
	return rand()%m;
}

struct TLabda{
	float x,y,vx,vy;
	float sebesseg;
} labda;


const int BRICKNUM=100;

struct TBrick{
	float x,y;
	int type;
	bool letezik;
	float osszemegy;
} bricks[BRICKNUM];

const int PARTICLENUM=10000;
struct TParticle{
	float x,y;
	float vx,vy;
	int lifetime;
	int szin;
	void (*update)(TParticle *mit);
} particles[PARTICLENUM];
int particleszam=0;


void ParticleUpdate_Sima(TParticle *mit)
{
	mit->x+=mit->vx;
	mit->y+=mit->vy;
}

void ParticleUpdate_Gravitacios(TParticle *mit)
{
	mit->x+=mit->vx;
	mit->y+=mit->vy;
	mit->vy+=0.015;
}

int paddlex=0;


SDL_Surface *screen = NULL;
SDL_Surface *kep_hatter;
SDL_Surface *kep_labda;
SDL_Surface *kep_paddle;
SDL_Surface *kep_bricks;
SDL_Surface *kep_paletta;

void Draw(SDL_Surface* source, int x, int y)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, NULL, screen, &offset );
}

void DrawSprite(SDL_Surface* source, int x, int y, int cellax, int cellay, int melyikx, int melyiky)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;

	SDL_Rect src_rect;

	src_rect.x=cellax*melyikx;
	src_rect.y=cellay*melyiky;
	src_rect.w=cellax;
	src_rect.h=cellay;

    SDL_BlitSurface( source, &src_rect, screen, &offset );
}

void GameOver()
{
	labda.x=paddlex;
	labda.y=450;
	labda.sebesseg=3;
	labda.vx=1.23456789;
	labda.vy=-sqrt(labda.sebesseg*labda.sebesseg-labda.vx*labda.vx);

	for(int i=0;i<BRICKNUM;++i)
	{
		bricks[i].x=50+(i%10)*63;
		bricks[i].y=50+(i/10)*22;
		bricks[i].type=rand()%5;
		bricks[i].letezik=true;
	}
}

void LoadAll()
{
	kep_hatter = IMG_Load("data/hatter.jpg");
	kep_labda = IMG_Load("data/ball.png");
	kep_paddle = IMG_Load("data/paddle.png");
	kep_bricks = IMG_Load("data/bricks.png");
	kep_paletta = IMG_Load("data/csillampaletta.bmp");
	GameOver();
}

void RenderAll()
{
	Draw(kep_hatter,0,0);

	for(int i=0;i<BRICKNUM;++i)
	{
		if (bricks[i].letezik)
		{
			SDL_SetAlpha(kep_bricks,SDL_SRCALPHA,255);
			DrawSprite(kep_bricks,
					   bricks[i].x,bricks[i].y,
					   63,22,
					   bricks[i].type%3,bricks[i].type/3);
		}
		else
		if (bricks[i].osszemegy>0)
		{
			SDL_SetAlpha(kep_bricks,SDL_SRCALPHA,bricks[i].osszemegy*255);
			DrawSprite(kep_bricks,
					   bricks[i].x,bricks[i].y,
					   63,22,
					   bricks[i].type%3,bricks[i].type/3);
		}
	}

	for(int i=0;i<particleszam;++i)
	{
		DrawSprite(kep_paletta,
				   particles[i].x,particles[i].y,
				   4,4,
				   particles[i].szin,0);
	}


	Draw(kep_labda, labda.x,labda.y);
	
	Draw(kep_paddle, paddlex, 460);
	Draw(kep_labda, paddlex,490);
	Draw(kep_labda, paddlex+85,490);
	SDL_Flip(screen);
}

void AddParticle(float x,float y, 
				 float vx, float vy, 
				 int szin,int lifetime,
				 void (*update)(TParticle *mit)=ParticleUpdate_Sima)
{
	if (particleszam>=PARTICLENUM)
		return;

	particles[particleszam].x=x;
	particles[particleszam].y=y;
	particles[particleszam].vx=vx;
	particles[particleszam].vy=vy;
	particles[particleszam].szin=szin;
	particles[particleszam].lifetime=lifetime;
	particles[particleszam].update=update;
	++particleszam;
}



void AddLabdaParticle(int mennyi=1)
{
	for (int i=0;i<mennyi;++i)
	AddParticle(labda.x+5,labda.y+5,
		        rand(100)/100.0-0.5,rand(100)/100.0-0.5,
				30+rand(10),
				rand(50)+50);
}

void ParticleUpdate_Csillagszoro(TParticle *mit)
{
	ParticleUpdate_Gravitacios(mit);
	if (mit->lifetime==1)
		for (int i=0;i<50;++i)
		AddParticle(mit->x+5,mit->y+5,
					rand(100)/100.0-0.5,rand(100)/100.0-0.5,
					rand(10),
					rand(50)+50);
	else
		AddParticle(mit->x,mit->y,0,0,rand(10),rand(20)+10);
}

void GameLogic()
{
	labda.x+=labda.vx;
	labda.y+=labda.vy;

	
	if (labda.x<0)
	{
		labda.x=0;
		labda.vx=-labda.vx;
	}
	if(labda.x>790)
	{
		labda.x=790;
		labda.vx=-labda.vx;
	}

	if (labda.y<0)
	{
		labda.y=0;
		labda.vy=-labda.vy;
	}
	

	if(labda.y>450 &&
		labda.x>paddlex-5 && labda.x<paddlex+90)
	{
		labda.y=450;
		labda.vy=-labda.vy;

		labda.sebesseg+=0.05;
		labda.vx=((labda.x-paddlex-40)/60.0)*labda.sebesseg;
		labda.vy=-sqrt(labda.sebesseg*labda.sebesseg
			           -labda.vx*labda.vx);
		for (int i=0;i<4;++i)
			AddParticle(labda.x,labda.y,
			            rand(100)/100.0-0.5,-1-rand(100)/100.0,
						0,100+rand(100),
						ParticleUpdate_Csillagszoro);
	}

	if(labda.y>500)
	{
		GameOver();
	}

	for (int i=0;i<BRICKNUM;++i)
	{
		if(bricks[i].letezik)
		{
			if (bricks[i].x<labda.x+10 && bricks[i].x>labda.x-63 &&
				bricks[i].y<labda.y+10 && bricks[i].y>labda.y-22)
			{
				bricks[i].letezik=false;
				bricks[i].osszemegy=1;
				AddLabdaParticle(50);
				float dx=min((labda.x+10)-bricks[i].x,
							  bricks[i].x-(labda.x-63));

				float dy=min((labda.y+10)-bricks[i].y,
							  bricks[i].y-(labda.y-22));

				if (dx<dy)
					labda.vx=-labda.vx;
				else
					labda.vy=-labda.vy;

				break;
			}
		}else //nem letezik
		{
			if (bricks[i].osszemegy>0)
				bricks[i].osszemegy-=0.02;
			else
				bricks[i].osszemegy=0;
		}
	}

	for(int i=0;i<particleszam;++i)
	{
		if (particles[i].update)
			particles[i].update(&(particles[i]));
		--particles[i].lifetime;
	}

	for(int i=0;i<particleszam;)
		if (particles[i].lifetime<=0)
		{
			particles[i]=particles[particleszam-1];
			--particleszam;
		}else
			++i;
}

int main(int argc, char** argv)
{
	SDL_Init( SDL_INIT_EVERYTHING );
	screen = SDL_SetVideoMode( 800, 500, 32, SDL_HWSURFACE );

	LoadAll();

	unsigned legutobbi_tick=SDL_GetTicks();
	while(1)
	{
		SDL_Event event;
		if(SDL_PollEvent( &event ))
		{
			switch(event.type)
			{
			case SDL_MOUSEMOTION:
				paddlex=event.motion.x;
			break;
			case SDL_QUIT:
				return 0;
			}
		}else
		{
			while(legutobbi_tick<SDL_GetTicks())
			{
				GameLogic();
				legutobbi_tick+=10;
			}
			RenderAll();
		}
	}
	return 0;
}