/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef ScreenItem_h
#define ScreenItem_h

class ScreenItem
{
public:
	enum ItemType { ItemHero, ItemEnemy, ItemPowerUp };
	
	ScreenItem(ItemType);
	virtual ~ScreenItem();
	
	virtual void init();
	
	virtual float	*getPos();
	float	pos[3];
	float	vel[3];
	
	ItemType	itemType() { return screenItemType; }
	
	ScreenItem *next;
	ScreenItem *back;

protected:
	int		age;
	
private:
	ItemType	screenItemType;
	
	static int	itemCount;
};


#endif // ScreenItem_h
