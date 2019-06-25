#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <cstring>

SDL_Surface* screen;
SDL_Surface* kep_hatter;
SDL_Surface* kep_jatekos;
SDL_Surface* kep_meteor[3];
SDL_Surface* kep_robbanas;
SDL_Surface* kep_loves;

class MozgoLofasz{
public:
	float posx;
	float posy;
	float sebx;
	float seby;
	float sebesseg_szorzo;
	MozgoLofasz():posx(0),posy(0),sebx(0),seby(0),sebesseg_szorzo(1){};
	void update()
	{
		posx+=sebx;
		posy+=seby;

		sebx*=sebesseg_szorzo;
		seby*=sebesseg_szorzo;

		if (posx<-60)
			posx=800;
		if (posx>800)
			posx=-60;

		if (posy<-60)
			posy=500;
		if (posy>500)
			posy=-60;	
	};
};

class LovesLofasz: public MozgoLofasz{
public:
	int lifetime;

	LovesLofasz():MozgoLofasz(),lifetime(0){};

	void update()
	{
		MozgoLofasz::update();
		--lifetime;
	}
};

class MeteorLofasz: public MozgoLofasz{
public:
	int size;
	MeteorLofasz():MozgoLofasz(),size(2){};
};


MozgoLofasz jatekos;
float jatekosszog;

float robbx,robby;
int robbanasvane=10000;

MeteorLofasz meteorok[1000];
int meteorszam=6;

LovesLofasz lovesek[1000];
int lovesszam=0;
int reload=0;

int halalok=0;

void Draw(SDL_Surface* source, int x, int y)
{
    SDL_Rect offset;
	offset.x = x-source->w/2;
    offset.y = y-source->h/2;
    SDL_BlitSurface( source, NULL, screen, &offset );
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

int eddig;

void Init()
{
	SDL_Init( SDL_INIT_EVERYTHING );
	screen  = SDL_SetVideoMode( 800, 500, 32, SDL_HWSURFACE );
	
	kep_hatter  = IMG_Load("data/hatter.jpg");
	kep_jatekos = IMG_Load("data/player.png");
	kep_robbanas= IMG_Load("data/explode.png");
	kep_loves   = IMG_Load("data/plasma.png");

	kep_meteor[0]= IMG_Load("data/meteor.png");
	kep_meteor[1]= IMG_Load("data/meteorbig.png");
	kep_meteor[2]= IMG_Load("data/meteorsuperbig.png");

	for (int i=0;i<meteorszam;++i)
	{
		meteorok[i].sebx=(rand()%1000-500)*0.001;
		meteorok[i].seby=(rand()%1000-500)*0.001;
	}

	jatekos.sebesseg_szorzo=0.99;
	eddig=SDL_GetTicks();
}

bool lenyomva[SDLK_LAST];

bool HandleEvents()
{
	SDL_Event event;
	while(SDL_PollEvent( &event ))
	{
		if (event.type==SDL_QUIT)
			return true;
		if (event.type==SDL_KEYDOWN)
		{
			if (event.key.keysym.sym==SDLK_ESCAPE)
				return true;
			lenyomva[event.key.keysym.sym]=true;
		}
		if (event.type==SDL_KEYUP)
		{
			lenyomva[event.key.keysym.sym]=false;
		}
	}

	return false;
}

void Render()
{
	Draw(kep_hatter,400,250);

	int spritind;
	
	if (robbanasvane<100)
	{
		spritind=robbanasvane/10;
		DrawSprite(kep_robbanas,robbx,robby,spritind%3,spritind/3,120);
	}
	for (int i=0;i<meteorszam;++i)
			Draw(kep_meteor[meteorok[i].size],meteorok[i].posx,meteorok[i].posy);

	for (int i=0;i<lovesszam;++i)
			Draw(kep_loves,lovesek[i].posx,lovesek[i].posy);

	spritind=(jatekosszog/(2.0*3.141592654))*72;
	DrawSprite(kep_jatekos,jatekos.posx,jatekos.posy,spritind%10,spritind/10,60);

	SDL_Flip(screen);
}

bool meteorutkozik(int i, float x, float y, int tav)
{
	float dx=x-meteorok[i].posx;
	float dy=y-meteorok[i].posy;
	float meteorsize=16;
	if (meteorok[i].size==1)
		meteorsize=32;
	if (meteorok[i].size==2)
		meteorsize=64;

	meteorsize+=tav;
	return dx*dx+dy*dy<meteorsize*meteorsize;
}

void Mechanics()
{
	int maxiter=0;
	while(eddig<SDL_GetTicks())
	{
		
		if (lenyomva[SDLK_RIGHT])
			jatekosszog+=0.1;
		if (lenyomva[SDLK_LEFT])
			jatekosszog-=0.1;
			
		while (jatekosszog<0)
			jatekosszog+=2.0*3.141592654;
	
		while (jatekosszog>2.0*3.141592654)
			jatekosszog-=2.0*3.141592654;

		if (lenyomva[SDLK_UP])
		{
			jatekos.sebx+=sin(jatekosszog)*0.1;
			jatekos.seby+=-cos(jatekosszog)*0.1;
		}
		/*if (lenyomva[SDLK_UP])
			jatekos.seby-=0.1;*/

		jatekos.update();

		for (int i=0;i<meteorszam;++i)
			meteorok[i].update();


		robbanasvane+=1;
		
		for (int i=0;i<meteorszam;++i)
		{
			if (meteorutkozik(i,jatekos.posx,jatekos.posy,30))
			{
				robbx=jatekos.posx;
				robby=jatekos.posy;
				robbanasvane=0;
				jatekos.posx=rand()%800;
				jatekos.posy=rand()%500;
				jatekos.sebx=0;
				jatekos.seby=0;
				++halalok;
			}
		}


		for (int i=0;i<meteorszam;)
		{
			bool torold=false;
			for (int j=0;j<lovesszam;++j)
			{
				if(meteorutkozik(i,lovesek[j].posx,lovesek[j].posy,5))
				{
					robbx=meteorok[i].posx;
					robby=meteorok[i].posy;
					robbanasvane=0;

					lovesek[j].posx=meteorok[i].posx;
					lovesek[j].posy=meteorok[i].posy;

					if (meteorok[i].size==0)
						torold=true;
					else
					{
						meteorok[i].sebx=(rand()%1000-500)*0.001;
						meteorok[i].seby=(rand()%1000-500)*0.001;
						--meteorok[i].size;

						for (int buntetes=0;buntetes<4;++buntetes)
						{
							meteorok[meteorszam]=meteorok[i];
							meteorok[meteorszam].sebx=(rand()%1000-500)*0.001;
							meteorok[meteorszam].seby=(rand()%1000-500)*0.001;
							++meteorszam;
						}
					}
					lovesek[j]=lovesek[lovesszam-1];
					--lovesszam;
					break;
				}
			}
			if (torold)
			{
				meteorok[i]=meteorok[meteorszam-1];
				--meteorszam;
			}
			else
				++i;
		}


		if (reload<=0 && lenyomva[SDLK_SPACE])
		{
			lovesek[lovesszam].posx=jatekos.posx;
			lovesek[lovesszam].posy=jatekos.posy;
			lovesek[lovesszam].sebx=sin(jatekosszog)*5;
			lovesek[lovesszam].seby=-cos(jatekosszog)*5;
			lovesek[lovesszam].lifetime=100;
			++lovesszam;
			reload=20;
		}
		--reload;

		for (int i=0;i<lovesszam;++i)
			lovesek[i].update();

		for (int i=0;i<lovesszam;)
			if (lovesek[i].lifetime<=0)
			{
				lovesek[i]=lovesek[lovesszam-1];
				--lovesszam;
			}
			else
				++i;

		eddig+=10;
		if(++maxiter>10)
		{
			eddig=SDL_GetTicks();
			break;
		}
	}
}


int main( int argc, char* argv[] )
{
	
	Init();
	while(1)
	{
		if (HandleEvents())
			break;
		Mechanics();
		Render();
		char title[1024];
		sprintf(title,"SDL-es csodajatek, amiben aszteroidok vannak. Halalok szama: %d",halalok);
		SDL_WM_SetCaption(title,0);
	}

	return 0;
}
