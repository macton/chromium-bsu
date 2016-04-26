#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Q: What are the types that are actually used for Explosions::drawExplo()?
// grep "drawExplo" *.cpp
//   Explosions.cpp: if(exploRoot[EnemyDestroyed]->next)     drawExplo(EnemyDestroyed);
//   Explosions.cpp: if(exploRoot[EnemyDamage]->next)        drawExplo(EnemyDamage);
//   Explosions.cpp: if(exploRoot[HeroDestroyed]->next)      drawExplo(HeroDestroyed);
//   Explosions.cpp: if(exploRoot[HeroDamage]->next)         drawExplo(HeroDamage);
enum ExploType 
{  
  EnemyDestroyed, 
  EnemyDamage, 
  EnemyAmmo00, 
  EnemyAmmo01, 
  EnemyAmmo02, 
  EnemyAmmo03, 
  EnemyAmmo04,
  HeroDestroyed, 
  HeroDamage, 
  HeroAmmo00, 
  HeroAmmo01, 
  HeroAmmo02,
  HeroShields,
  PowerBurst,
  AddLife, 
  LoseLife, 
  ScoreLife,
  Electric, 
  Glitter,
  NumExploTypes 
};
typedef enum ExploType ExploType;

// From Explosions.h
float  exploSize[NumExploTypes][2];
float  exploStay[NumExploTypes];
float  exploPause[NumExploTypes][3];  /**< # frames to not allow explosions. [0] = count, [1] = base, [2] = flag */

// From - void Explosions::drawExplo(ExploType type)
void
drawExplo(ExploType type, float age)
{
  // float  age;
  float  ex, ey;
  float  exs, eys;

  // #note Input position (p) is offset, not relevant to output animation generated
  // float  *p;
  
  float  clr, tmp;
  float  xoff,yoff;

  // #note the only thing we need from thisExplo is size, age and next
  //   - size is a global scalar, so we don't need to analyze per-frame
  //   - age is the time counter, so we can pass that in.
  //   - next is to iterate over each instance. we only need one for purposes here.
  // Explo  *thisExplo;
  
  // #note Remove rendering features - not looking at rendering.
  // glColor4f(1.0, 1.0, 1.0, 1.0);
  // glBindTexture(GL_TEXTURE_2D, tex[type]);
  
  // thisExplo = exploRoot[type]->next;
  // glBegin(GL_QUADS);
  // while(thisExplo)
  {
    // #note game->speedAdj is both a global modifier and not actually used.
    // age = thisExplo->age*game->speedAdj;
    if( age >= 0)
    {
      clr = age/exploStay[type];

      // #note ignore type instance scalar thisExplo->size - doesn't change base animation
      // ex = thisExplo->size*exploSize[type][0]*clr;
      // ey = thisExplo->size*exploSize[type][1]*clr;
      ex = exploSize[type][0]*clr;
      ey = exploSize[type][1]*clr;

      exs = ex * 0.7;
      eys = ey * 0.7;
      tmp = clr*clr;
      clr = 1.2-clr;
      tmp = 0.5+clr*0.5;
      // glColor4f(tmp, tmp, tmp, clr);


      // if(type == HeroDamage)
      //   p = game->hero->pos;
      // else
      //   p = thisExplo->pos;

      // #note since xoff and yoff are constant, variations solely based on those offsets are not relevent to output.
      // if(type == EnemyDestroyed)
      // {
        // xoff = 0.1;
        // yoff = 0.3;
        // glTexCoord2f(0.0, 1.0); glVertex3f(p[0]-exs+xoff, p[1]+eys+yoff, p[2]);
        // glTexCoord2f(0.0, 0.0); glVertex3f(p[0]-exs+xoff, p[1]-eys+yoff, p[2]);
        // glTexCoord2f(1.0, 0.0); glVertex3f(p[0]+exs+xoff, p[1]-eys+yoff, p[2]);
        // glTexCoord2f(1.0, 1.0); glVertex3f(p[0]+exs+xoff, p[1]+eys+yoff, p[2]);

        // xoff = -0.2;
        // yoff = -0.4;
        // glTexCoord2f(0.0, 1.0); glVertex3f(p[0]-exs+xoff, p[1]+eys+yoff, p[2]);
        // glTexCoord2f(0.0, 0.0); glVertex3f(p[0]-exs+xoff, p[1]-eys+yoff, p[2]);
        // glTexCoord2f(1.0, 0.0); glVertex3f(p[0]+exs+xoff, p[1]-eys+yoff, p[2]);
        // glTexCoord2f(1.0, 1.0); glVertex3f(p[0]+exs+xoff, p[1]+eys+yoff, p[2]);
      // }
      // xoff =  0.0;
      // yoff = -0.3;
      // glTexCoord2f(0.0, 1.0); glVertex3f(p[0]-ex+xoff, p[1]+ey+yoff, p[2]);
      // glTexCoord2f(0.0, 0.0); glVertex3f(p[0]-ex+xoff, p[1]-ey+yoff, p[2]);
      // glTexCoord2f(1.0, 0.0); glVertex3f(p[0]+ex+xoff, p[1]-ey+yoff, p[2]);
      // glTexCoord2f(1.0, 1.0); glVertex3f(p[0]+ex+xoff, p[1]+ey+yoff, p[2]);
     
      // Q: What does this loop actually calculate?
      // One frame of (4 channels):
      //   exs = x-scale (half rect width)
      //   eys = y-scale (half rect height)
      //   tmp = RGB
      //   clr = Alpha
      printf("%0.6f %0.6f %0.6f %0.6f\n",exs,eys,tmp,clr);
    }
    
    // thisExplo = thisExplo->next; //ADVANCE
  }
  // glEnd();
}

// From - Explosions::Explosions()
void
init( void )
{
  // game = Global::getInstance();
  
  int    i;  
  for(i = 0; i < (int)NumExploTypes; i++)
  {
    // tex[i]  = 0;
    // exploRoot[i] = new Explo();
    exploSize[i][0] = 0.5;
    exploSize[i][1] = 0.5;
    exploStay[i] = 20.0;
    exploPause[i][0] = 0;
    exploPause[i][1] = 0;
    exploPause[i][2] = 0;
  }
  // exploPool = new Explo();
  // loadTextures();
  
  
  exploSize[EnemyDestroyed][0] = 1.35;
  exploSize[EnemyDestroyed][1] = 1.35;
  exploStay[EnemyDestroyed]  = 30.0;

  exploSize[EnemyDamage][0]  = 1.0;
  exploSize[EnemyDamage][1]  = 1.0;
  exploStay[EnemyDamage]    = 20.0;
  exploPause[EnemyDamage][1]  = 3;

  exploSize[EnemyAmmo00][0]  = 1.5;
  exploSize[EnemyAmmo00][1]  = 1.5;
  exploStay[EnemyAmmo00]    = 15.0;
  exploPause[EnemyAmmo00][1]  = 1;
  
  exploSize[EnemyAmmo01][0]  = 0.5;
  exploSize[EnemyAmmo01][1]  = 0.5;
  exploStay[EnemyAmmo01]    = 10.0;
  exploPause[EnemyAmmo01][1]  = 3;
  
  exploSize[EnemyAmmo02][0]  = 1.7;
  exploSize[EnemyAmmo02][1]  = 1.7;
  exploStay[EnemyAmmo02]    = 10.0;
  exploPause[EnemyAmmo02][1]  = 2;
  
  exploSize[EnemyAmmo03][0]  = 1.7;
  exploSize[EnemyAmmo03][1]  = 1.7;
  exploStay[EnemyAmmo03]    = 10.0;
  exploPause[EnemyAmmo03][1]  = 2;
  
  exploSize[EnemyAmmo04][0]  = 2.0;
  exploSize[EnemyAmmo04][1]  = 1.5;
  exploStay[EnemyAmmo04]    = 10.0;
  exploPause[EnemyAmmo04][1]  = 5;
  
  exploSize[HeroDestroyed][0] = 1.5;
  exploSize[HeroDestroyed][1] = 1.5;
  exploStay[HeroDestroyed]  = 25.0;

  exploSize[HeroDamage][0]  = 1.1;
  exploSize[HeroDamage][1]  = 1.1;
  exploStay[HeroDamage]    = 25.0;
  exploPause[HeroDamage][1]  = 3;

  exploSize[HeroAmmo00][0]  = 0.25;
  exploSize[HeroAmmo00][1]  = 0.25;
  exploStay[HeroAmmo00]    = 10.0;
  
  exploSize[HeroAmmo01][0]  = 0.5;
  exploSize[HeroAmmo01][1]  = 1.0;
  exploStay[HeroAmmo01]    = 15.0;
  exploPause[HeroAmmo01][1]  = 1;
  
  exploSize[HeroAmmo02][0]  = 0.9;
  exploSize[HeroAmmo02][1]  = 1.0;
  exploStay[HeroAmmo02]    = 23.0;
  
  exploSize[HeroShields][0]  = 1.6;
  exploSize[HeroShields][1]  = 1.6;
  exploStay[HeroShields]    = 25.0;
  exploPause[HeroShields][1]  = 5;
  
  exploSize[PowerBurst][0]  = 1.8;
  exploSize[PowerBurst][1]  = 1.8;
  exploStay[PowerBurst]    = 35.0;
  
  exploSize[AddLife][0]    = 2.5;
  exploSize[AddLife][1]    = 2.5;
  exploStay[AddLife]      = 25.0;
  
  exploSize[LoseLife][0]    = 3.5;
  exploSize[LoseLife][1]    = 3.5;
  exploStay[LoseLife]      = 35.0;
  
  exploSize[ScoreLife][0]    = 3.5;
  exploSize[ScoreLife][1]    = 3.5;
  exploStay[ScoreLife]    = 35.0;
  
  exploSize[Electric][0]    = 1.7;
  exploSize[Electric][1]    = 0.5;
  exploStay[Electric]      = 43.0;
  exploPause[Electric][1]    = 0;
  
  exploSize[Glitter][0]    = 0.8;
  exploSize[Glitter][1]    = 1.0;
  exploStay[Glitter]      = 20.0;
  exploPause[Glitter][1]    = 0;
}


int
main( void )
{
  init();

  // Type under test:
  ExploType test_type = EnemyDestroyed;
  // ExploType test_type = EnemyDamage;
  // ExploType test_type = HeroDestroyed;
  // ExploType test_type = HeroDamage;

  // Q Why are all of these floats,... really?
  float lifetime = exploStay[test_type];
  float age      = 1.0f; // #note Since zero is ignored, life implicitly starts at 1.0
  while (age < lifetime)
  {
    drawExplo(test_type, age);
    age += 1.0f;
  }

  return 0;
}
