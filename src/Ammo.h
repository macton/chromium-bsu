/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Ammo_h
#define Ammo_h

//====================================================================
class ActiveAmmo
{
public:
	ActiveAmmo();
	~ActiveAmmo();
	
	void	init(float p[3], float v[3], float d);
	
	inline void	updatePos() { pos[0]+=vel[0]; pos[1]+=vel[1]; pos[2]+=vel[2];} 

	float	pos[3];
	float	vel[3];
	float	damage;
	
	ActiveAmmo *back;
	ActiveAmmo *next;

private:
	static int ammoCount;
};


#endif // Ammo_h
