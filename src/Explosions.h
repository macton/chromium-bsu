/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Explosions_h
#define Explosions_h

#include "compatibility.h"
#include <GL/gl.h>

class Explo;

//====================================================================
class Explosions
{
public:
	enum ExploType {	EnemyDestroyed, EnemyDamage, 
						EnemyAmmo00, EnemyAmmo01, EnemyAmmo02, EnemyAmmo03, EnemyAmmo04,
						HeroDestroyed, HeroDamage, 
						HeroAmmo00, HeroAmmo01, HeroAmmo02,
						HeroShields,
						PowerBurst,
						AddLife, LoseLife, ScoreLife,
						Electric, Glitter,
						NumExploTypes };
	Explosions();
	~Explosions();
	
	Explo	*addExplo(ExploType t, float p[3], int age = 0, float size = 1.0);
	Explo	*addElectric(float p[3], float v[3], float clr[4], int age = 0, float size = 1.0);
	Explo	*addGlitter(float p[3], float v[3], float clr[4], int age = 0, float size = 1.0);
	
	void	update();
	void	drawGL();
	void	clear();

	void	loadTextures();
	void	deleteTextures();
	
private:
	Explo	*exploRoot[NumExploTypes];
	Explo	*exploPool;
	GLuint	tex[NumExploTypes];
	float	exploSize[NumExploTypes][2];
	float	exploStay[NumExploTypes];
	float	exploPause[NumExploTypes][3];	/**< # frames to not allow explosions. [0] = count, [1] = base, [2] = flag */
	
	void	drawExplo(ExploType);
	void	drawAmmo(ExploType);
	void	drawBurst(ExploType);
	void	drawShields(ExploType);
	void	drawLife(ExploType);
	void	drawElectric(ExploType);
	void	drawGlitter(ExploType);
	
	Explo	*getNewExplo();
	void 	killExplo(Explo *dead);
};

//====================================================================
class Explo
{
public:
	Explo();
	~Explo();
	
	void	init(float p[3], int a = 0, float s = 1.0);
	void	init(float p[3], float v[3], float c[4], int a = 0, float s = 1.0);
	
	float	pos[3];
	float	vel[3];
	float	clr[4];
	int		age;
	float	size;
	
	Explo *back;
	Explo *next;

private:
	static int exploCount;
};

#endif // Explosions_h
