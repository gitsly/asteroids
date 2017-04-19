#ifndef __Game_H__
#define __Game_H__

class CEdict;

class CGame
{
	public:
		CGame();
		~CGame();

		CEdict *player;		
		CEdict *world;		

		float camera_z;
		float width, height;
		int ticks;

		void LoadModels();
		void Init();			
		void Respawn();
		void Think();			
		void HandleInput();
};

#endif
