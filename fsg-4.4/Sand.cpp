/*   
 *   Copyright 2006 Owen Piette
 *   See license.txt for terms.
 *
 */

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>

#include "Sand.h"
#include <wx/wx.h>
#include <wx/dcscreen.h>
#include <wx/dcbuffer.h>
#include <wx/datetime.h>
#include <wx/file.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <wx/menuitem.h>
#include <wx/treectrl.h>
#include <wx/tglbtn.h>
#include <wx/thread.h>
#include <wx/listctrl.h>
#include <wx/evtloop.h>

#include "Canvas.h"

#ifndef VERSION
#define VERSION custom
#endif

#ifndef CVERSION
#define CVERSION custom
#endif


char defaultFileContents[] = "#wxSand: Owen Piette's Falling Sand Game\n\
#Version 4.4, File subversion 2\n\
#element element r g b gravity slip density conductivity visible\n\
#self probabilityOfChanging element [probabilityOfChangingIntoThisElement element] [...]\n\
#hotself tempAtWhichThisElementSelfProbabilityStartsToIncreaseTo100 element\n\
#hotcolor element r g b\n\
#neighbor probabilityOfReacting element1 element2 [probabilityOfChangingIntoTheseElements element1 element2] [...]\n\
#hotneighbor tempAtWhichThisElementSelfProbabilityStartsToIncreaseTo100 element1 element2\n\
#explosion neighbor1 neighbor2 xspeed<255 yspeed<255\n\
#pexplosion neighbor1 neighbor2 diameter element\n\
\n\
group   Waters  Water  Steam  Saltwater  Spout\n\
group	Fires	Fire	Torch	TNT	Ember	FallingEmber\n\
sources Sand    Water  Salt   Oil\n\
\n\
#Erases.\n\
element	Empty	0	0	0	0.000000	1.000000	0.000000	0.000000	1\n\
#Eraser element.\n\
element	Drain	10	10	10	0.000000	1.000000	0.000000	0.000000	1\n\
neighbor 1.0	Drain	Water	1.0	Drain	Empty\n\
neighbor 1.0	Drain	Dust	1.0	Drain	Empty\n\
neighbor 1.0	Drain	Oil	1.0	Drain	Empty\n\
neighbor 1.0	Drain	Steam	1.0	Drain	Empty\n\
neighbor 1.0	Drain	Sand	1.0	Drain	Empty\n\
neighbor 1.0	Drain	Salt	1.0	Drain	Empty\n\
neighbor 1.0	Drain	Saltwater	1.0	Drain	Empty\n\
neighbor 1.0	Drain	MoltenCera	1.0	Drain	Empty\n\
neighbor 1.0	Drain	FallingEmber	1.0	Drain	Empty\n\
neighbor 1.0	Drain	ExplodingTNT	1.0	Drain	Empty\n\
#A solid element which reacts with nothing.\n\
element	Wall	128	128	128	0.000000	0.000000	1.000000	0.500000	1\n\
hotcolor  Wall  255     0     0\n\
#Does nothing. Useful for explosion aftermaths\n\
element	Dust	128	128	128	0.900000	0.000000	1.000000	0.500000	1\n\
#Burns other elements. Source of energy.\n\
element	Fire	247	63	63	-1.000000	1.000000	0.000000	1.100000	1\n\
self	0.050000	Fire	1.0000	Empty	\n\
hotcolor Fire   255    63     63\n\
neighbor 0.9	Fire	Water	1.0	Empty		Steam\n\
neighbor 0.75	Fire	Oil	1.0	Fire		Fire\n\
neighbor 1.0	Fire	Plant	1.0	Fire		Fire\n\
neighbor 0.005	Fire	Cera	1.0	MoltenCera	Ember\n\
neighbor 0.9	Fire	TNT	1.0	ExplodingTNT	Fire\n\
explosion	Fire	Oil	10	10\n\
explosion	Fire	TNT	100	100\n\
#Explodes\n\
element	TNT	50	50	50	0.000000	0.000000	1.000000	0.000000	1\n\
#Explodes\n\
element	ExplodingTNT	100	50	50	0.900000	1.000000	1.000000	0.000000	0\n\
self	0.02000	ExplodingTNT	1.0000	Fire\n\
neighbor 0.9	ExplodingTNT	TNT	1.0	ExplodingTNT	ExplodingTNT\n\
neighbor 1.0	ExplodingTNT	Wall	1.0	ExplodingTNT	Dust\n\
explosion	ExplodingTNT	TNT	100	100\n\
#Dissolves other elements. Turns to steam when heated.\n\
element	Water	32	32	255	0.700000	1.000000	0.500000	0.400000	1\n\
hotcolor Water  200     200     255\n\
self    0.001     Water   1.0     Steam\n\
hotself 50     Water\n\
#Grows in water.\n\
element	Plant	32	204	32	0.000000	0.000000	1.000000	0.000000	1\n\
neighbor 0.2	Plant	Water	1.0	Plant	Plant\n\
#Inert.\n\
element	Sand	238	204	128	0.900000	0.500000	0.900000	0.000000	1\n\
#Source of water.\n\
element	Spout	10	100	10	0.000000	0.000000	1.000000	0.000000	1\n\
neighbor 0.75	Spout	Empty	1.0	Spout		Water\n\
neighbor 0.5	Spout	Sand	1.0	Empty		Empty\n\
#Wax. Melts.\n\
element	Cera	238	221	204	0.000000	0.000000	1.000000	0.000000	1\n\
#Destroys almost everything.\n\
element	???	231	7	231	0.000000	1.000000	1.000000	0.000000	1\n\
self	0.100000	???	1.0000	Leftover???\n\
neighbor 1.0	???	Empty	1.0	???	???\n\
neighbor 1.0	???	Wall	1.0	???	???\n\
neighbor 1.0	???	Fire	1.0	???	???\n\
neighbor 1.0	???	Water	1.0	???	???\n\
neighbor 1.0	???	Sand	1.0	???	???\n\
neighbor 1.0	???	Spout	1.0	???	???\n\
neighbor 1.0	???	Cera	1.0	???	???\n\
neighbor 1.0	???	Oil	1.0	???	???\n\
neighbor 1.0	???	Salt	1.0	???	???\n\
neighbor 1.0	???	Ember	1.0	???	???\n\
neighbor 1.0	???	MoltenCera	1.0	???	???\n\
neighbor 1.0	???	Steam	1.0	???	???\n\
neighbor 1.0	???	Saltwater	1.0	???	???\n\
neighbor 1.0	???	Torch	1.0	???	???\n\
neighbor 0.2	???	Leftover???	1.0	Leftover???	Leftover???\n\
#Burns.\n\
element	Oil	128	64	64	0.700000	1.000000	0.200000	0.000000	1\n\
#Soluable in water.\n\
element	Salt	255	255	255	0.900000	0.400000	0.900000	0.000000	1\n\
#Burns for a little while.\n\
element	Ember	200	50	50	0.000000	0.200000	1.000000	0.000000	1\n\
self	0.002000	Ember	1.0000	Fire\n\
neighbor 0.2		Ember	Empty	1.0	Ember	Fire\n\
neighbor 0.9		Ember	Water	1.0	Empty	Steam\n\
neighbor 0.75		Ember	Oil	1.0	Ember	Fire\n\
neighbor 1.0		Ember	Plant	1.0	Ember	Fire\n\
neighbor 0.005		Ember	Cera	1.0	MoltenCera	Ember\n\
#Burns for a little while.\n\
element	FallingEmber	200	50	50	1.000000	0.200000	1.000000	0.000000	1\n\
self	0.002000	FallingEmber	1.0000	Fire\n\
neighbor 0.2		FallingEmber	Empty	1.0	FallingEmber	Fire\n\
neighbor 0.9		FallingEmber	Water	1.0	Empty	Steam\n\
neighbor 1.0		FallingEmber	Oil	1.0	FallingEmber	FallingEmber\n\
neighbor 1.0		FallingEmber	Plant	1.0	FallingEmber	Fire\n\
neighbor 0.005		FallingEmber	Cera	1.0	MoltenCera	Ember\n\
#Flowing cera. Will harden eventually.\n\
element	MoltenCera	255	220	200	0.800000	1.000000	1.000000	0.000000	0\n\
neighbor 0.01	MoltenCera	Wall	1.0	Cera	Wall\n\
neighbor 0.01	MoltenCera	Water	1.0	Cera	Water\n\
neighbor 0.01	MoltenCera	Sand	1.0	Cera	Sand\n\
neighbor 0.01	MoltenCera	Spout	1.0	Cera	Spout\n\
neighbor 0.01	MoltenCera	Cera	1.0	Cera	Cera\n\
neighbor 0.01	MoltenCera	Oil	1.0	Cera	Oil\n\
neighbor 0.01	MoltenCera	Salt	1.0	Cera	Salt\n\
neighbor 0.01	MoltenCera	Saltwater	1.0	Cera	Saltwater\n\
#Will eventually condense.\n\
element	Steam	85	85	255	-1.000000	1.000000	0.010000	0.000000	1\n\
self	0.001000	Steam	1.0000	Water\n\
#Salt will slowly come out of solution.\n\
element	Saltwater	0	0	150	0.700000	0.000000	0.500000	0.600000	1\n\
self	0.00001	Saltwater	0.5	Water	0.5	Salt\n\
neighbor 0.004	Water	Salt	1.0	Saltwater	Saltwater\n\
neighbor 0.001	Saltwater	Saltwater	1.0	Salt	Steam\n\
neighbor 0.04	Saltwater	Water	1.0	Water	Saltwater\n\
neighbor 0.9	Saltwater	Fire	1.0	Salt	Steam\n\
hotneighbor   50	Saltwater	Saltwater\n\
#Continuous fire.\n\
element	Torch	100	0	0	0.000000	1.000000	1.000000	1.000000	1\n\
neighbor 0.2	Torch	Empty	1.0	Torch	Fire\n\
neighbor 0.2	Torch	Water	1.0	Torch	Steam\n\
neighbor 0.2	Torch	Saltwater	0.5	Torch	Steam	0.5	Torch	Salt\n\
neighbor 0.2	Torch	Oil	1.0	Torch	Fire\n\
neighbor 0.2	Torch	Plant	1.0	Torch	Fire\n\
neighbor 0.2	Torch	Cera	1.0	Torch	MoltenCera\n\
#A helper element.\n\
element	Leftover???	200	0	0	0.000000	0.000000	1.000000	0.000000	0\n\
self	0.005000	Leftover???	1.0000	Empty	\n\
\n\
";


IMPLEMENT_APP(Sand)




wxString names[MAXNUMBEROFELEMENTS];
wxColor colors[MAXNUMBEROFELEMENTS][100];

double gravity[MAXNUMBEROFELEMENTS];
double slip[MAXNUMBEROFELEMENTS];
double density[MAXNUMBEROFELEMENTS];
double conductivity[MAXNUMBEROFELEMENTS];
bool visible[MAXNUMBEROFELEMENTS];

double death_prob[MAXNUMBEROFELEMENTS];
unsigned char death_center[MAXNUMBEROFELEMENTS][100];
int death_energy[MAXNUMBEROFELEMENTS];

double trans_prob[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];   //Probability that something will happen.
unsigned char trans_center[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS][100];//Probability that that something will be this.
unsigned char trans_neighbor[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS][100];
int trans_energy[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
unsigned char trans_xspeed[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
unsigned char trans_yspeed[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];

short int pexplosion[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
unsigned char pexplosionelement[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];

int cgravity[MAXNUMBEROFELEMENTS];
int ccodensity[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];
int cslip[MAXNUMBEROFELEMENTS];
int ccogravitydensity[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS];

int ctrans_prob[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS][100];
int cdeath_prob[MAXNUMBEROFELEMENTS][100];

wxString groupNames[MAXNUMBEROFELEMENTS];
int groups[MAXNUMBEROFELEMENTS][MAXNUMBEROFELEMENTS]; //groups[groupNumber][N] = element, or -1.
int numberOfGroups;
int numberOfGroupItems[MAXNUMBEROFELEMENTS];
int groupChoices[MAXNUMBEROFELEMENTS];

int numberOfElements;

unsigned char sources[MAXNUMBEROFELEMENTS];
int numberOfSources;
int elementChoices[MAXNUMBEROFELEMENTS];

wxString elementDescriptions[MAXNUMBEROFELEMENTS];

MainFrame* g_mainFrame;
Canvas* g_canvas;
bool g_isRunning;
unsigned char data[MAXSIZE];
unsigned char energy[MAXSIZE];
unsigned char calc[MAXSIZE];
unsigned char bitmapdata[MAXSIZE*4];

char xspeed[MAXSIZE];
char yspeed[MAXSIZE];

wxMenuItem* wallsCB;
wxMenuItem* sourcesCB;
wxMenuItem* drawCB;
wxMenuItem* gravityCB;
wxMenuItem* energyCB;
wxMenuItem* interactionsCB;
wxMenuItem* limitCB;

bool doWalls;
bool doSources;
bool doDraw;
bool doGravity;
bool doEnergy;
bool doInteractions;
bool doLimit;
bool doPause;
bool doResize;

bool drawAll;
wxString sandboxFilename;
wxString physicsFilename;
wxListBox* penSelections;
wxListBox* groupSelections;
wxTreeItemId penSelectionIds[MAXNUMBEROFELEMENTS*MAXNUMBEROFELEMENTS];
wxTreeItemId penSelectionElements[MAXNUMBEROFELEMENTS*MAXNUMBEROFELEMENTS];
wxTreeItemId groupSelectionIds[MAXNUMBEROFELEMENTS*MAXNUMBEROFELEMENTS];
int numberOfPenSelectionIds;
int numberOfGroupSelectionIds;
wxTimer* g_timer;
wxTimer* g_timer_second;
bool mouseIsDown;
int mousex, mousey;
int lastmousex, lastmousey;
int group_type;
unsigned char sand_type;
int pen_width;
wxStatusBar* statusbar;
double previous_now;
int g_width;
int g_height;
int g_T;

wxTextCtrl* elementDescription;

wxDateTime startTime;
int frameCount;




bool DropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames){
  
  if (filenames[0].First(_(".txt")) != -1 || filenames[0].First(_(".TXT")) != -1 ){
    g_mainFrame->loadPhysics(filenames[0]);
  }
  else{
    g_mainFrame->loadSandbox(filenames[0]);
  }

  if (filenames.GetCount() == 2){
    if (filenames[1].First(_(".txt")) != -1 || filenames[1].First(_(".TXT")) != -1 ){
      g_mainFrame->loadPhysics(filenames[1]);
    }
    else{
      g_mainFrame->loadSandbox(filenames[1]);
    }
  }

  return true;
}



/* this is executed upon startup, like 'main()' in non-wxWidgets programs */
bool Sand::OnInit()
{

  g_mainFrame = NULL;
  g_canvas = NULL;
  mouseIsDown = false;
  statusbar = NULL;
  g_width = 640;
  g_height = 480;
  doPause = false;
  startTime = wxDateTime::Now();
  doResize = false;
  g_T = 22;

  numberOfGroups = 0;
  numberOfElements = 17;
  numberOfSources = 0;


  wxString str = _("Owen Piette's Falling Sand Game ");
  str += _(VERSION);
  g_mainFrame = new MainFrame(str, wxDefaultPosition, wxDefaultSize);//, wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN);
  g_mainFrame->Show(TRUE);

  ::wxInitAllImageHandlers();

  sandboxFilename = _("");
  physicsFilename = _("");

  FILE* tfile = fopen("default.txt", "r");
  if (tfile == NULL){
  
    FILE* file = fopen("default.txt", "w");
    fprintf(file, defaultFileContents);
    fclose(file);
  }
  else{
    fclose(tfile);
  }

  if (this->argc > 1){
    if (wxString(argv[1]).First(_(".txt")) != -1 || wxString(argv[1]).First(_(".TXT")) != -1 ){
      physicsFilename = wxString(argv[1]);
      g_mainFrame->loadPhysics(physicsFilename);
    }
    else{
      sandboxFilename = wxString(argv[1]);
      g_mainFrame->loadSandbox(sandboxFilename);
    }
  }
  if (this->argc > 2){
    if (wxString(argv[2]).First(_(".txt")) != -1 || wxString(argv[2]).First(_(".TXT")) != -1 ){
      physicsFilename = wxString(argv[2]);
      g_mainFrame->loadPhysics(physicsFilename);
    }
    else{
      sandboxFilename = wxString(argv[2]);
      g_mainFrame->loadSandbox(sandboxFilename);
    }
  }

  if (physicsFilename == _("")){
    physicsFilename = _("default.txt");
    g_mainFrame->loadPhysics(physicsFilename);
  }

  g_mainFrame->SetDropTarget(new DropTarget());



  //Load Settings
  FILE* file = fopen("settings.ini", "r");
  if (file){
    char line[1024];
    fgets(line, 1024, file);
    if (atoi(&line[15]) == 0)
      wallsCB->Check(false);
    fgets(line, 1024, file);
    if (atoi(&line[8]) == 0)
      sourcesCB->Check(false);
    fgets(line, 1024, file);
    if (atoi(&line[14]) == 0)
      drawCB->Check(false);
    fgets(line, 1024, file);
    if (atoi(&line[8]) == 0)
      gravityCB->Check(false);
    fgets(line, 1024, file);
    if (atoi(&line[7]) == 0)
      energyCB->Check(false);
    fgets(line, 1024, file);
    if (atoi(&line[21]) == 0)
      interactionsCB->Check(false);
    fgets(line, 1024, file);
    if (atoi(&line[10]) == 0)
      limitCB->Check(false);
    fclose(file);
  }

  doWalls = wallsCB->IsChecked();
  doSources = sourcesCB->IsChecked();
  doDraw = drawCB->IsChecked();
  doGravity = gravityCB->IsChecked();
  doEnergy = energyCB->IsChecked();
  doInteractions = interactionsCB->IsChecked();
  doLimit = limitCB->IsChecked();

  return true;
}



int Sand::OnRun(){

  wxEventLoop* eventLoop = new wxEventLoop();
  wxEventLoop::SetActive(eventLoop);
  m_mainLoop = eventLoop;

  g_isRunning = true;

  while(g_isRunning){

    if ((g_canvas->doUpdate || !doLimit) && !g_canvas->busyCalculating){
      g_canvas->doUpdate = false;
      g_canvas->calculate();
      g_canvas->Refresh();
      
    }

    while (!Pending() && ProcessIdle());

    while (Pending())
 	Dispatch();

    //::wxYield();
  }

  m_mainLoop = NULL;
  return 0;
}
