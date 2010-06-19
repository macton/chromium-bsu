/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef GroundSegment_h
#define GroundSegment_h

class Ground;

//====================================================================
class GroundSegment
{
public:
	GroundSegment(float p[3], float s[2], Ground *prnt);
	virtual ~GroundSegment();

	virtual void	drawGL() = 0;

	double	pos[3];
	float	size[2];
	float	age;
	
	GroundSegment *back;
	GroundSegment *next;
	
	Ground	*parent;
	
friend class Ground;
};

#endif //GroundSegment_h
