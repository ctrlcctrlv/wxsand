#ifndef SANDH
#define SANDH

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/thread.h>
#include <wx/dnd.h>
#include <wx/treectrl.h>
#include <wx/datetime.h>

#include "MainFrame.h"

#define MAXNUMBEROFELEMENTS 255
#define MAXSIZE 1280*1024



class MainFrame;
class Canvas;



class Sand : public wxApp {
public:
  virtual bool OnInit();
  virtual int OnRun();
};




class DropTarget : public wxFileDropTarget {
 public:
  virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

};



extern wxString names[MAXNUMBEROFELEMENTS];
extern wxColor colors[MAXNUMBEROFELEMENTS][100];

extern double gravity[MAXNUMBEROFELEMENTS];
extern double slip[MAXNUMBEROFELEMENTS];
extern double density[MAXNUMBEROFELEMENTS];
extern double conductivity[MAXNUMBEROFELEMENTS];
extern bool visible[MAXNUMBEROFELEMENTS];

extern double death_prob[MAXNUMBEROFELEMENTS];
extern unsigned char death_center[MAXNUMBEROFELEMENTS][100];
extern int death_energy[MAXNUMBEROFELEMENTS];

extern double trans_prob[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];   //Probability that something will happen.
extern unsigned char trans_center[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS][100];//Probability that that something will be this.
extern unsigned char trans_neighbor[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS][100];
extern int trans_energy[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
extern unsigned char trans_xspeed[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
extern unsigned char trans_yspeed[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];

extern short int pexplosion[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
extern unsigned char pexplosionelement[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];

extern int cgravity[MAXNUMBEROFELEMENTS];
extern int ccodensity[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
extern int cslip[MAXNUMBEROFELEMENTS];
extern int ccogravitydensity[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];

extern int ctrans_prob[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS][100];
extern int cdeath_prob[MAXNUMBEROFELEMENTS][100];

extern wxString groupNames[MAXNUMBEROFELEMENTS];
extern int groups[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS]; //groups[groupNumber][N] = element, or -1.
extern int numberOfGroups;
extern int numberOfGroupItems[MAXNUMBEROFELEMENTS];
extern int groupChoices[MAXNUMBEROFELEMENTS];

extern int numberOfElements;

extern unsigned char sources[MAXNUMBEROFELEMENTS];
extern int numberOfSources;
extern int elementChoices[MAXNUMBEROFELEMENTS];

extern wxString elementDescriptions[MAXNUMBEROFELEMENTS];

extern MainFrame* g_mainFrame;
extern Canvas* g_canvas;
extern bool g_isRunning;
extern unsigned char data[MAXSIZE];
extern unsigned char energy[MAXSIZE];
extern unsigned char calc[MAXSIZE];
extern unsigned char bitmapdata[MAXSIZE*4];

extern char xspeed[MAXSIZE];
extern char yspeed[MAXSIZE];


extern wxMenuItem* wallsCB;
extern wxMenuItem* sourcesCB;
extern wxMenuItem* drawCB;
extern wxMenuItem* gravityCB;
extern wxMenuItem* energyCB;
extern wxMenuItem* interactionsCB;
extern wxMenuItem* limitCB;

extern bool doWalls;
extern bool doSources;
extern bool doDraw;
extern bool doGravity;
extern bool doEnergy;
extern bool doInteractions;
extern bool doLimit;
extern bool doPause;
extern bool doResize;

extern bool drawAll;
extern wxString sandboxFilename;
extern wxString physicsFilename;
extern wxListBox* penSelections;
extern wxListBox* groupSelections;
extern wxTreeItemId penSelectionIds[MAXNUMBEROFELEMENTS*MAXNUMBEROFELEMENTS];
extern wxTreeItemId penSelectionElements[MAXNUMBEROFELEMENTS*MAXNUMBEROFELEMENTS];
extern wxTreeItemId groupSelectionIds[MAXNUMBEROFELEMENTS*MAXNUMBEROFELEMENTS];
extern int numberOfPenSelectionIds;
extern int numberOfGroupSelectionIds;
extern wxTimer* g_timer;
extern wxTimer* g_timer_second;
extern bool mouseIsDown;
extern int mousex, mousey;
extern int lastmousex, lastmousey;
extern int group_type;
extern unsigned char sand_type;
extern int pen_width;
extern wxStatusBar* statusbar;
extern double previous_now;
extern int g_width;
extern int g_height;
extern int g_T;

extern wxTextCtrl* elementDescription;

extern wxDateTime startTime;
extern int frameCount;






#endif
