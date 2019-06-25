#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

const int SCREEN_W=512;
const int SCREEN_H=448;

SDL_Surface* screen;

void Draw(SDL_Surface* source,int x, int y)
{
	SDL_Rect rct={};
	rct.x=x;
	rct.y=y;
	SDL_BlitSurface(source,0,screen,&rct);
}

void DrawSprite(SDL_Surface* source, int x, int y, int indx, int indy, int sprite_size)
{
	
	SDL_Rect offset;
	SDL_Rect spriterect;
    offset.x = x-sprite_size/2;
    offset.y = y-sprite_size/2;

	spriterect.w=sprite_size;
	spriterect.h=sprite_size;

	spriterect.x=indx*sprite_size;
	spriterect.y=indy*sprite_size;

    SDL_BlitSurface( source, &spriterect, screen, &offset );
}

SDL_Surface* hatterkep;
SDL_Surface* kacsakep;
SDL_Surface* hudkep; 
SDL_Surface* kutyakep; 
Mix_Chunk* kacsahang;
Mix_Chunk* sikerhang;
Mix_Chunk* loveshang1;
Mix_Chunk* loveshang2;


#define KACSA_STATE_REPUL 0
#define KACSA_STATE_GUVADT 1
#define KACSA_STATE_DEAD 2
#define KACSA_STATE_FLYAWAY 4

#define GAME_STATE_VANKACSA 0
#define GAME_STATE_SIKERULT 1
#define GAME_STATE_NEM_SIKERULT 2


struct KacsaStruct{
	int x,y,sx,sy;
	int animstate;
	int animvaltas;
	int iranyvaltas;
	int state;
} kacsa={};

int state=0;
int statetime=0;

void ResetKacsa()
{
	kacsa.x=SCREEN_W/2;
	kacsa.y=290;
	kacsa.state=KACSA_STATE_REPUL;
}

void InitStuff()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	screen=SDL_SetVideoMode(SCREEN_W,SCREEN_H,32,SDL_HWSURFACE);
	hatterkep=IMG_Load("data/hatter.png");
	kacsakep =IMG_Load("data/duckx.png");
	hudkep   =IMG_Load("data/hud.png");
	kutyakep =IMG_Load("data/kutya2.png");
	
	Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096);
	kacsahang = Mix_LoadWAV("data/duck_loop.wav");
	sikerhang = Mix_LoadWAV("data/fuck_yea.wav");
	loveshang1 = Mix_LoadWAV("data/shot_fail.wav");
	loveshang2 = Mix_LoadWAV("data/shot_kill.wav");

	ResetKacsa();
}

void DrawStuff()
{
	SDL_FillRect(screen,0,0x0080ff);
	int sprite_y_index=0;

	if (state==GAME_STATE_VANKACSA)
	{
		if (kacsa.state==KACSA_STATE_REPUL)
		{
			if(kacsa.sx>0)
				if(kacsa.sy!=0)
					sprite_y_index=2;
				else
					sprite_y_index=4;
			else
				if(kacsa.sy!=0)
					sprite_y_index=3;
				else
					sprite_y_index=5;

				DrawSprite(kacsakep,kacsa.x,kacsa.y,kacsa.animstate,sprite_y_index,80);
		}
		else if (kacsa.state==KACSA_STATE_GUVADT)
		{
			DrawSprite(kacsakep,kacsa.x,kacsa.y,0,0,80);
		}
		else
		{
			DrawSprite(kacsakep,kacsa.x,kacsa.y,kacsa.animstate,1,80);
		}
	}
	else if (state == GAME_STATE_SIKERULT)
	{
		if (statetime<150)
			DrawSprite(kutyakep,kacsa.x,350-(statetime)/2,2,0,115);
		else
		if( statetime<200)
			DrawSprite(kutyakep,kacsa.x,275,2,0,115);
		else
			DrawSprite(kutyakep,kacsa.x,275+(statetime-200)/2,2,0,115);


	}
	Draw(hatterkep,0,0);
	Draw(hudkep,30,380);
	SDL_Flip(screen);		
}

void StuffStuff()
{
	if (state==GAME_STATE_VANKACSA)
	{
		kacsa.x+=kacsa.sx*3;
		kacsa.y+=kacsa.sy*3;
		--kacsa.animvaltas;
		if(kacsa.animvaltas<0)
		{
			kacsa.animstate=(kacsa.animstate+1)%3;
			kacsa.animvaltas=10;
		}

		if (kacsa.state==KACSA_STATE_REPUL)
		{
			--kacsa.iranyvaltas;
			if(kacsa.iranyvaltas<0)
			{
				kacsa.sx= (rand()&1) ? 1 : -1;
				kacsa.sy=rand()%3-1;
				kacsa.iranyvaltas=40;
			}

			
			

			if (kacsa.x>SCREEN_W)
			{
				kacsa.x=SCREEN_W;
				kacsa.sx=-1;
			}

			if (kacsa.y>280)
			{
				kacsa.y=280;
				kacsa.sy=-1;
			}

			if (kacsa.x<0)
			{
				kacsa.x=0;
				kacsa.sx=1;
			}

			if (kacsa.y<0)
			{
				kacsa.y=0;
				kacsa.sy=1;
			}
		}
		else if (kacsa.state==KACSA_STATE_GUVADT)
		{
			kacsa.sx=kacsa.sy=0;
			--kacsa.iranyvaltas;
			if(kacsa.iranyvaltas<0)
				kacsa.state=KACSA_STATE_DEAD;
		}
		else if (kacsa.state==KACSA_STATE_DEAD)
		{
			kacsa.sy=1;
			if (kacsa.y>300)
			{
				kacsa.y=1000;
				state=GAME_STATE_SIKERULT;
				statetime=0;
				
			}
		}
		else
			SDL_Quit();
	}
	else if (state == GAME_STATE_SIKERULT)
	{
		++statetime;
		if (statetime==70)
			Mix_PlayChannel(-1,sikerhang,0);
		if ( statetime>500)
		{
			ResetKacsa();
			state = GAME_STATE_VANKACSA;
		}
	}

}

void MouseClickStuff(int x,int y)
{
	if (abs(kacsa.x-x)<40 &&
		abs(kacsa.y-y)<40)
	{
		kacsa.state=KACSA_STATE_GUVADT;
		kacsa.iranyvaltas=50;
		Mix_PlayChannel(-1,loveshang2,0);
	}
	else
		Mix_PlayChannel(-1,loveshang1,0);
}


int main(int argc, char** argv)
{
	InitStuff();
	SDL_Event evt;

	int legutobbi_ss=SDL_GetTicks();

	while(1)
	{
		if(SDL_PollEvent(&evt))
		{
			if(evt.type==SDL_QUIT)
				break;
			if(evt.type==SDL_MOUSEBUTTONDOWN)
				MouseClickStuff(evt.button.x,evt.button.y);
			if (evt.type==SDL_KEYDOWN &&evt.key.keysym.sym==SDLK_x)
				MouseClickStuff(kacsa.x,kacsa.y);
				
		}
		else
			DrawStuff();

		while (legutobbi_ss<SDL_GetTicks()-10)
		{
			StuffStuff();
			legutobbi_ss+=10;
		}
	}

	return 0;
}