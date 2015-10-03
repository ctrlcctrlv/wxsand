#include "Canvas.h"
#include <math.h>
#include <wx/image.h>

BEGIN_EVENT_TABLE(Canvas, wxWindow)
  EVT_ERASE_BACKGROUND(Canvas::OnEraseBG)
  EVT_RIGHT_DOWN(Canvas::OnMouseRightDown)
  EVT_LEFT_DOWN(Canvas::OnMouseLeftDown)
  EVT_LEFT_UP(Canvas::OnMouseLeftUp)
  EVT_MOTION(Canvas::OnMouseMove)
  EVT_TIMER(999, Canvas::OnTimer)
  EVT_TIMER(998, Canvas::OnSecondTimer)
  //EVT_IDLE(Canvas::OnIdle)
END_EVENT_TABLE()



Canvas::Canvas(wxWindow* parent, wxWindowID id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize) : wxWindow( parent, id, pos, size, wxCLIP_CHILDREN){
  g_canvas = this;

  //this->SetExtraStyle(wxWS_EX_PROCESS_IDLE);
  this->SetSizeHints(g_width,g_height,g_width,g_height);
  this->SetBackgroundColour(wxColor(_("BLACK")));
  sand_type = 1;
  pen_width = 4;
  drawAll = true;
  this->busyCalculating = false;
  this->doUpdate = false;

  memset(data, 0, g_width*g_height*sizeof(unsigned char));
  memset(energy, 0, g_width*g_height*sizeof(unsigned char));
  memset(calc, false, g_width*g_height*sizeof(unsigned char));
  memset(xspeed, 0, g_width*g_height*sizeof(char));
  memset(yspeed, 0, g_width*g_height*sizeof(char));
}

inline void drawData(int center){
  bitmapdata[((center)*3)+0] = colors[data[center]][energy[center]].Red();
  bitmapdata[((center)*3)+1] = colors[data[center]][energy[center]].Green();
  bitmapdata[((center)*3)+2] = colors[data[center]][energy[center]].Blue();
}


inline void found(int center, int other, int cx, int cy, int ox, int oy){
  unsigned char t = data[center];
  data[center] = data[other];
  data[other] = t;
  t = energy[center];
  energy[center] = energy[other];
  energy[other] = t;
  unsigned int txspeed = xspeed[center];
  xspeed[center] = xspeed[other];
  xspeed[other] = txspeed;
  unsigned int tyspeed = yspeed[center];
  yspeed[center] = yspeed[other];
  yspeed[other] = tyspeed;
  drawData(center);
  drawData(other);
  calc[center] = true;
  if (data[other] != 0)
    calc[other] = true;
}


void drawCircle(int centerx, int centery){
  int r = pen_width/2;
  int rr = r*r;
  for(int dx=-r;dx<=r;++dx){
    int x = centerx+dx;
    int dy = (int)round(sqrt(rr - dx*dx));
    for(int y=centery-dy;y<=centery+dy;++y){
      if (x > 0 && x < g_width-1 && y > 0 && y < g_height-1){
	int center = (g_width*y)+x;
	data[center] = sand_type;
	energy[center] = 0;
	xspeed[center] = 0;
	yspeed[center] = 0;
	drawData(center);
      }
    }
  }
}

void drawCircle(int centerx, int centery, int diameter, int sandtype){
  int r = diameter/2;
  int rr = r*r;
  for(int dx=-r;dx<=r;++dx){
    int x = centerx+dx;
    int dy = (int)round(sqrt(rr - dx*dx));
    for(int y=centery-dy;y<=centery+dy;++y){
      if (x > 0 && x < g_width-1 && y > 0 && y < g_height-1){
	int center = (g_width*y)+x;
	data[center] = sandtype;
	energy[center] = 0;
	xspeed[center] = 0;
	yspeed[center] = 0;
	drawData(center);
      }
    }
  }
}


inline void checkForTransformation(int center, int other, int cx, int cy, int ox, int oy){
  unsigned char t1 = data[center];
  unsigned char t2 = data[other];
  
  if (doEnergy){
    if (data[other] != 0){
      double c = conductivity[data[center]];
      if (c > 0){
	if (energy[other] > energy[center]){
	  int de = energy[other]-energy[center];
	  if (c > 1)
	    c = 1;
	  de = (int)ceil(c*double(de));
	  	  
	  energy[other] -= de;
	  energy[center] += de;
	  
	  if (energy[center] > 99)
	  energy[center] = 99;
	  
	  drawData(center);
	  drawData(other);
	  calc[center] = true;
	  calc[other] = true;
	}
      }
    }
  }

  if (pexplosion[t1][t2] > -1){
    drawCircle(cx, cy, pexplosion[t1][t2], pexplosionelement[t1][t2]);
  }
  if (trans_prob[t1][t2] == 0) return;
  if (doWalls && (ox == 0 || ox == g_width-1 || oy == 0 || oy == g_height-1))
    return;



  if (rand() < ctrans_prob[t1][t2][energy[center]+energy[other]]){
    int r = (int)round(99.0*(double(rand())/double(RAND_MAX)));
    if (trans_energy[t1][t2] != -1){
      if (energy[center] >= trans_energy[t1][t2])
	energy[center] -= trans_energy[t1][t2];
      else
	energy[center] = 0;
      
      if (energy[other] >= trans_energy[t1][t2])
	energy[other] -= trans_energy[t1][t2];
      else
	energy[other] = 0;
    }

    data[center] = trans_center[t1][t2][r];
    data[other] = trans_neighbor[t1][t2][r];
    
    if (trans_xspeed[t1][t2] != 0){
      xspeed[center] = (int)round(trans_xspeed[t1][t2]*double(rand())/RAND_MAX);
      if (rand() < RAND_MAX/2)
	xspeed[center] = -xspeed[center];

      xspeed[other] = (int)round(trans_xspeed[t1][t2]*double(rand())/RAND_MAX);
      if (rand() < RAND_MAX/2)
	xspeed[other] = -xspeed[other];
    }
    
    if (trans_yspeed[t1][t2] != 0){
      yspeed[center] = (int)round(trans_yspeed[t1][t2]*double(rand())/RAND_MAX);
      if (rand() < RAND_MAX/2)
	yspeed[center] = -yspeed[center];
      yspeed[other] = (int)round(trans_yspeed[t1][t2]*double(rand())/RAND_MAX);
      if (rand() < RAND_MAX/2)
	yspeed[other] = -yspeed[other];
    }


    if (data[center] == 0){
      energy[center] = 0;
      xspeed[center] = 0;
      yspeed[center] = 0;
    }
    if (data[other] == 0){
      energy[other] = 0;
      xspeed[other] = 0;
      yspeed[other] = 0;
    }
    
    drawData(center);
    drawData(other);
    calc[center] = true;
    calc[other] = true;
    return;
  }
}


void Canvas::calculate(){

  if (doPause == true){
    return;
  }
  
  this->busyCalculating = true;
  
  ++frameCount;

  if (doSources){
    int space = g_width/(numberOfSources+1);

    for(int i=0;i<10;++i){
      if (rand() < RAND_MAX/4){
	for(int n=0;n<numberOfSources;++n){
	  data[g_width+(space*(n+1))+i] = (unsigned char)sources[n];
	  energy[g_width+(space*(n+1))+i] = 0;
	  xspeed[g_width+(space*(n+1))+i] = 0;
	  yspeed[g_width+(space*(n+1))+i] = 0;
	}
      }
    }
  }
  

  
  if (mouseIsDown){
    drawCircle(mousex, mousey);
  }


  for(int y=g_height-2;y>0;--y){
    int sx = 1;
    int ex = g_width-1;
    int dx = 1;
    if (rand() < 0.5*RAND_MAX){
      sx = g_width-2;
      ex = 1;
      dx = -1;
    }

    for(int x=sx;x!=ex;x+=dx){

      int center = (g_width*y)+x;
 
      if (data[center] == 0)
	continue;
      if (calc[center])
	continue;

      if (rand() < cdeath_prob[data[center]][energy[center]]){
	int r = (int)round(99.0*(double(rand())/double(RAND_MAX)));
	if (death_energy[data[center]] != -1)
	  energy[center] -= death_energy[data[center]];
	
	data[center] = death_center[data[center]][r];
	if (data[center] == 0)
	  energy[center] = 0;
	drawData(center);
	continue;
      }
      
      int left = center-1;
      int right = center+1;
      int up = center - g_width;
      int down = center + g_width;
      int downleft = center + g_width - 1;
      int downright = center + g_width + 1;
      int upleft = center - g_width - 1;
      int upright = center - g_width + 1;

      
      //Do momentum.
      //Momentum just slowly wears out.

      //TODO: Should x change at all??

      
      if (xspeed[center] != 0){
	if (xspeed[center] > 0){
	  xspeed[center] -= 1;
	  if ( rand() < (double(abs(xspeed[center]))/100.0)*RAND_MAX ){
	    if (data[right] != 0)
	      if (rand() > RAND_MAX/10){
		//xspeed[center] -= 4;
		//continue;
	      }

	    found(center, right, x, y, x+1, y);
	    continue;
	  }
	}
	else{
	  xspeed[center] += 1;
	  if ( rand() < (double(abs(xspeed[center]))/100.0)*RAND_MAX ){
	    if (data[left] != 0)
	      if (rand() > RAND_MAX/10){
		//xspeed[center] += 4;
		//continue;
	      }
	  
	    found(center, left, x, y, x-1, y);
	    continue;
	  }
	}
      }
      
      if (yspeed[center] != 0){
	if (yspeed[center] > 0){
	  yspeed[center] -= 1;
	  if ( rand() < (double(abs(yspeed[center]))/100.0)*RAND_MAX ){
	    if (data[down] != 0)
	      if (rand() > RAND_MAX/10){
		//yspeed[center] -= 4;
		//continue;
	      }

	    found(center, down, x, y, x, y+1);
	    continue;
	  }
	}
	else{
	  yspeed[center] += 1;
	  if ( rand() < (double(abs(yspeed[center]))/100.0)*RAND_MAX ){
	    if (data[up] != 0)
	      if (rand() > RAND_MAX/10){
		//yspeed[center] += 4;
		//continue;
	      }

	    found(center, up, x, y, x, y-1);
	    continue;
	  }
	}
      }
      

      
      //Do energy buildups/environment buildup.
      if (doEnergy){
	if (conductivity[data[center]] > 1.0){
	  int e = energy[center] + (int)round(99.0*(conductivity[data[center]]-1.0));
	  if (e > 99)
	    e = 99;
	  energy[center] = (char)e;
	  drawData(center);
	}
	if (rand() < conductivity[data[center]]*0.1*RAND_MAX){
	  if (data[left] == 0 ||
	      data[right] == 0 ||
	      data[up] == 0 ||
	      data[down] == 0 ||
	      data[upleft] == 0 ||
	      data[upright] == 0 ||
	      data[downleft] == 0 ||
	      data[downright] == 0){
	    if (energy[center] < g_T)
	      energy[center] += 1;
	    else if (energy[center] > g_T)
	      energy[center] -= 1;
	  }
	}
      }

      



      if (doInteractions){
	//Check for specific transformations.
	//Randomly check neighbors.

	int r = (int)round(47.0*double(rand())/RAND_MAX);
	switch(r){
	case(0):
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, right, x, y, x+1, y);
	  break;
	case(1):
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, left, x, y, x-1, y);
	  break;
	case(2):
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  break;
	case(3):
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  break;
	case(4):
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  break;
	case(5):
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  break;
	case(6):
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, right, x, y, x+1, y);
	  break;
	case(7):
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, left, x, y, x-1, y);
	  break;
	case(8):
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  break;
	case(9):
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  break;
	case(10):
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  break;
	case(11):
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  break;
	case(12):
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  break;
	case(13):
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  break;
	case(14):
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  break;
	case(15):
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  break;
	case(16):
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, down, x, y, x, y+1);
	  break;
	case(17):
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, up, x, y, x, y-1);
	  break;
	case(18):
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  break;
	case(19):
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  break;
	case(20):
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  break;
	case(21):
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  break;
	case(22):
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, up, x, y, x, y-1);
	  checkForTransformation(center, down, x, y, x, y+1);
	  break;
	case(23):
	  checkForTransformation(center, right, x, y, x+1, y);
	  checkForTransformation(center, left, x, y, x-1, y);
	  checkForTransformation(center, down, x, y, x, y+1);
	  checkForTransformation(center, up, x, y, x, y-1);
	  break;
	case(24):
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  break;
	case(25):
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  break;
	case(26):
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  break;
	case(27):
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  break;
	case(28):
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  break;
	case(29):
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  break;
	case(30):
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  break;
	case(31):
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  break;
	case(32):
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  break;
	case(33):
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  break;
	case(34):
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  break;
	case(35):
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  break;
	case(36):
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  break;
	case(37):
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  break;
	case(38):
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  break;
	case(39):
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  break;
	case(40):
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  break;
	case(41):
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  break;
	case(42):
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  break;
	case(43):
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  break;
	case(44):
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  break;
	case(45):
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  break;
	case(46):
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  break;
	case(47):
	  checkForTransformation(center, upright, x, y, x+1, y-1);
	  checkForTransformation(center, downright, x, y, x+1, y+1);
	  checkForTransformation(center, downleft, x, y, x-1, y+1);
	  checkForTransformation(center, upleft, x, y, x-1, y-1);
	  break;
	}
      }  
    
    
    
      if (doGravity){
      
	if (gravity[data[center]] == 0)
	  continue;

	if (yspeed[center] < 0)
	  continue;

	//gravity > 0

	//falling down
	//density down
	//falling downleft
	//density downleft
	//falling downright
	//density downright
	
	//gravity < 0

	//up
	//upleft
	//upright


	//left open
	//left pushing
	//left density

	//right open
	//right pushing
	//right density


	if (gravity[data[center]] > 0){
	  //Check down
	  //falling
	  if (!doWalls || y < g_height-2){
	    if (data[down] == 0){
	      if (rand() < cgravity[data[center]]){
		found(center, down, x, y, x, y+1);
		continue;
	      }
	    }
	    else{
	      //density
	      if (density[data[center]] > density[data[down]]){
		if (rand() < ccogravitydensity[data[center]][data[down]] ){
		  found(center, down, x, y, x, y+1);
		  continue;
		}
	      }
	    }
	  }

	  if (rand() < 0.5*RAND_MAX){ //left versus right.
	    if (!doWalls || (y < g_height-2 && x > 1)){
	      //Check down and left
	      //falling
	      if (data[downleft] == 0){
		if (rand() < cgravity[data[center]]){
		  found(center, downleft, x, y, x-1, y+1);
		  continue;
		}
	      }
	      else{
		//density
		if (density[data[center]] > density[data[downleft]]){
		  if ( rand() < ccogravitydensity[data[center]][data[downleft]] ){
		    found(center, downleft, x, y, x-1, y+1);
		    continue;
		  }
		}
	      }
	    }
	  }
	  else{//left versus right.
	    if (!doWalls || (y < g_height-2 && x < g_width-2)){
	      //Check down and right
	      //falling
	      if (data[downright] == 0){
		if (rand() < cgravity[data[center]]){
		  found(center, downright, x, y, x+1, y+1);
		  continue;
		}
	      }
	      else{
		//density
		if (density[data[center]] > density[data[downright]]){
		  if ( rand() < ccogravitydensity[data[center]][data[downright]] ){
		    found(center, downright, x, y, x+1, y+1);
		    continue;
		  }
		}
	      }
	    }
	  }
	}
	else{
	  //gravity < 0

	  //up or to the side?
	  if (rand() < 0.7*RAND_MAX){
	    if (!doWalls || y > 1){
	      //Check up
	      if (data[up] == 0){
		if (rand() < -cgravity[data[center]]){
		  found(center, up, x, y, x, y-1);
		  continue;
		}
	      }
	    }
	  }
	  else{
	    //Check up and left.
	    if (rand() < 0.5*RAND_MAX){ //left versus right.
	      if (!doWalls || (y > 1 && x > 1)){
		//floating
		if (data[upleft] == 0){
		  if (rand() < -cgravity[data[center]]){
		    found(center, upleft, x, y, x-1, y-1);
		    continue;
		  }
		}
	      }
	    }
	    else{	   //left versus right.
	      if (!doWalls || (y > 1 && x < g_width-2)){
		//Check up and right.
		if (data[upright] == 0){
		  if (rand() < -cgravity[data[center]]){
		    found(center, upright, x, y, x+1, y-1);
		    continue;
		  }
		}
	      }
	    }
	  }
	}

    
	if (rand() < 0.5*RAND_MAX){ //left versus right.
	  //Check left.
	  if (!doWalls || x > 1){
	    //Random opening.
	    if (data[left] == 0){
	      if (rand() < cslip[data[center]]){
		found(center, left, x, y, x-1, y);
		continue;
	      }
	    }
	    
	    //density
	    if (density[data[center]] > density[data[left]]){
	      if ( rand() < ccodensity[data[center]][data[left]] ){
		found(center, left, x, y, x-1, y);
		continue;
	      }
	    }
	  }
	}
	else{
	  //Check right.
	  if (!doWalls || x < g_width-2){
	    //Random opening.
	    if (data[right] == 0){
	      if (rand() < cslip[data[center]]){
		found(center, right, x, y, x+1, y);
		continue;
	      }
	      
	    }
	    
	    //density
	    if (density[data[center]] > density[data[right]]){
	      if ( rand() < ccodensity[data[center]][data[right]] ){
		found(center, right, x, y, x+1, y);
		continue;
	      }
	    }
	  }
	}
      }//doGravity


    }
  }

  int bottom = (g_height-1)*g_width;
  for(int x=0;x<g_width;++x){
    data[x]          = 0;
    data[bottom+x]   = 0;
    energy[x]        = 0;
    energy[bottom+x] = 0;
    drawData(x);
    drawData(bottom+x);
  }

  for(int y=0;y<g_height-1;++y){
    int leftside = y*g_width;
    int rightside = (y*g_width)+g_width-1;

    data[leftside]    = 0;
    data[rightside]   = 0;
    energy[leftside]  = 0;
    energy[rightside] = 0;
    drawData(leftside);
    drawData(rightside);
  }
  

  memset(calc, false, g_width*g_height*sizeof(unsigned char));
  this->busyCalculating = false;
}


void Canvas::Refresh(){

  wxString str(_(""));
  str.Printf(_("%s [%d]"), names[data[(g_width*mousey)+mousex]].c_str(), int(energy[(g_width*mousey)+mousex]));
  statusbar->SetStatusText(str, 2);


  if (doDraw){
    wxClientDC dc(this);

    dc.BeginDrawing();
    
    wxMemoryDC memdc;
    wxImage image(g_width, g_height, bitmapdata, true);
    wxBitmap bmp(image);
    memdc.SelectObject(bmp);

    dc.Blit(0,0,g_width, g_height, &memdc, 0, 0);
    
    if (!drawAll){
      dc.EndDrawing();
      return;
    }
    else{
      for(int center=0;center<g_width*g_height;++center){
	bitmapdata[(center*3)+0] = colors[data[center]][energy[center]].Red();
	bitmapdata[(center*3)+1] = colors[data[center]][energy[center]].Green();
	bitmapdata[(center*3)+2] = colors[data[center]][energy[center]].Blue();
      }
      drawAll = false;
    }
    

    dc.EndDrawing();

  }
}

void Canvas::OnTimer(wxTimerEvent& event){
  g_canvas->doUpdate = true;
  //::wxWakeUpIdle();
}

void Canvas::OnSecondTimer(wxTimerEvent& event){
  wxString str(_(""));
  str.Printf(_("%d fps"), frameCount);
  frameCount = 0;
  statusbar->SetStatusText(str,0);

  wxString str2(_(""));
  str2.Printf(_("%02d:%02d"), wxDateTime::Now().Subtract(startTime).GetHours(), 
	     wxDateTime::Now().Subtract(startTime).GetMinutes());
  statusbar->SetStatusText(str2,1);
}

void Canvas::OnEraseBG(wxEraseEvent& e){
  //Do absolutely nothing, thereby halting this event.
  //drawAll = true;
  //g_canvas->Refresh();
}

void drawLine(int x2, int y2, int x1, int y1){
  int dx = x1 - x2;
  int dy = y1 - y2;

  int t_max = abs(dx);
  if (abs(dy) > abs(dx))
    t_max = abs(dy);

  if (t_max == 0){
    drawCircle(x1, y1);
  }
  else{
    double ddx = double(dx)/double(t_max);
    double ddy = double(dy)/double(t_max);
    
    double x=x2;
    double y=y2;
    for(int t=0;t<=t_max;++t){
      drawCircle((int)round(x),(int)round(y));
      x += ddx;
      y += ddy;
    }
  }
}

void Canvas::OnMouseLeftDown(wxMouseEvent& event){
  mouseIsDown = true;
  mousex = event.GetX();
  mousey = event.GetY();
  lastmousex = mousex;
  lastmousey = mousey;

  drawCircle(mousex, mousey);
}

void Canvas::OnMouseLeftUp(wxMouseEvent& event){
  mouseIsDown = false;
}


void Canvas::OnMouseRightDown(wxMouseEvent& event){
  mousex = event.GetX();
  mousey = event.GetY();
  
  drawLine(lastmousex, lastmousey, mousex, mousey);

  lastmousex = mousex;
  lastmousey = mousey;
}

void Canvas::OnMouseMove(wxMouseEvent& event){
  if (event.LeftIsDown()){
    mousex = event.GetX();
    mousey = event.GetY();

    drawLine(lastmousex, lastmousey, mousex, mousey);

    lastmousex = mousex;
    lastmousey = mousey;
  }
  else{
    mousex = event.GetX();
    mousey = event.GetY();
    
    mouseIsDown = false;

    wxString str("");
    str.Printf(_("%s [%d]"), names[data[(g_width*mousey)+mousex]].c_str(), (int)energy[(g_width*mousey)+mousex]);
    statusbar->SetStatusText(str, 2);
  }
}




/*
void Canvas::OnIdle(wxIdleEvent& e){
  if ((g_canvas->doUpdate || !doLimit) && !g_canvas->busyCalculating){
    g_canvas->doUpdate = false;
    g_canvas->calculate();
    g_canvas->Refresh();
    
    ::wxWakeUpIdle();
  }
}
*/



