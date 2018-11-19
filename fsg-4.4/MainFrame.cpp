#include "MainFrame.h"
#include "Canvas.h"
#include "Sand.h"

#include <math.h>

#include <wx/image.h>
#include "pause.xpm"
#include "refresh.xpm"

#define TIMERINTERVAL 33

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_SET_FOCUS(MainFrame::OnMove)
EVT_SIZE(MainFrame::OnSize)
EVT_LISTBOX(1000, MainFrame::OnGroupChoice)
EVT_LISTBOX(1001, MainFrame::OnPenChoice)

EVT_RADIOBUTTON(1010, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1011, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1012, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1013, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1014, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1015, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1016, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1017, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1018, MainFrame::OnPenSize)
EVT_RADIOBUTTON(1019, MainFrame::OnPenSize)

EVT_SPINCTRL(1020, MainFrame::OnTempSpin)

EVT_BUTTON(1025, MainFrame::OnButton)
EVT_BUTTON(1026, MainFrame::OnToggleButton)

EVT_MENU(1002, MainFrame::OnMenu)
EVT_MENU(1003, MainFrame::OnMenu)
EVT_MENU(1004, MainFrame::OnMenu)
EVT_MENU(1005, MainFrame::OnMenu)
EVT_MENU(1006, MainFrame::OnMenu)
EVT_MENU(1007, MainFrame::OnMenu)
EVT_MENU(1008, MainFrame::OnMenu)

EVT_MENU(1050, MainFrame::OnMenu)
EVT_MENU(1051, MainFrame::OnMenu)
EVT_MENU(1052, MainFrame::OnMenu)
EVT_MENU(1053, MainFrame::OnMenu)
EVT_MENU(1054, MainFrame::OnMenu)
EVT_MENU(1055, MainFrame::OnMenu)
EVT_MENU(1056, MainFrame::OnMenu)

EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos,
                     const wxSize& size, long style)
    : wxFrame(NULL, -1, title, pos, size, style) {
  g_mainFrame = this;

  wxMenu* fileMenu = new wxMenu;
  wxMenu* optionsMenu = new wxMenu;
  wxMenu* helpMenu = new wxMenu;

  fileMenu->Append(1050, _("New.."), _(""));
  fileMenu->AppendSeparator();
  fileMenu->Append(1051, _("Load sandbox.."), _(""));
  fileMenu->Append(1052, _("Save sandbox.."), _(""));
  fileMenu->AppendSeparator();
  fileMenu->Append(1055, _("Load physics.."), _(""));
  // fileMenu->Append(1056, _("Save physics.."), _(""));
  fileMenu->AppendSeparator();
  fileMenu->Append(1053, _("Exit"), _(""));

  wallsCB = new wxMenuItem(optionsMenu, 1002, _("Boundary Walls"), _(""),
                           wxITEM_CHECK);
  sourcesCB =
      new wxMenuItem(optionsMenu, 1003, _("Sources"), _(""), wxITEM_CHECK);
  drawCB = new wxMenuItem(optionsMenu, 1004, _("Paint Updates"), _(""),
                          wxITEM_CHECK);
  gravityCB =
      new wxMenuItem(optionsMenu, 1005, _("Gravity"), _(""), wxITEM_CHECK);
  energyCB =
      new wxMenuItem(optionsMenu, 1008, _("Energy"), _(""), wxITEM_CHECK);
  interactionsCB = new wxMenuItem(optionsMenu, 1006, _("Element Interactions"),
                                  _(""), wxITEM_CHECK);
  limitCB =
      new wxMenuItem(optionsMenu, 1007, _("Limit FPS"), _(""), wxITEM_CHECK);

  optionsMenu->Append(wallsCB);
  optionsMenu->Append(sourcesCB);
  optionsMenu->Append(drawCB);
  optionsMenu->Append(gravityCB);
  optionsMenu->Append(energyCB);
  optionsMenu->Append(interactionsCB);
  optionsMenu->Append(limitCB);

  helpMenu->Append(1054, _("About"), _(""));

  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append(fileMenu, _("File"));
  menuBar->Append(optionsMenu, _("Options"));
  menuBar->Append(helpMenu, _("Help"));

  SetMenuBar(menuBar);

  wallsCB->Check();
  sourcesCB->Check();
  drawCB->Check();
  gravityCB->Check();
  energyCB->Check();
  interactionsCB->Check();
  limitCB->Check();

  doWalls = wallsCB->IsChecked();
  doSources = sourcesCB->IsChecked();
  doDraw = drawCB->IsChecked();
  doGravity = gravityCB->IsChecked();
  doEnergy = energyCB->IsChecked();
  doInteractions = interactionsCB->IsChecked();
  doLimit = limitCB->IsChecked();

  statusbar = this->CreateStatusBar();
  statusbar->SetFieldsCount(3);
  statusbar->Show(true);

  g_canvas = new Canvas(this, -1, wxDefaultPosition, wxSize(g_width, g_height));

  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  {
    wxBoxSizer* RHSizer = new wxBoxSizer(wxVERTICAL);
    {
      wxBoxSizer* RH1 = new wxBoxSizer(wxHORIZONTAL);
      {
        RH1->Add(new wxBitmapButton(this, 1025, wxBitmap(refresh_xpm)), 0,
                 wxCENTER | wxALL, 10);
        RH1->Add(new wxBitmapButton(this, 1026, wxBitmap(pause_xpm)), 0,
                 wxCENTER | wxALL, 10);
      }

      wxStaticBox* sb4 = new wxStaticBox(this, -1, _("Sandbox Temperature"));
      wxStaticBoxSizer* RH5 = new wxStaticBoxSizer(sb4, wxHORIZONTAL);
      {
        wxSpinCtrl* ctrl = new wxSpinCtrl(this, 1020);
        RH5->Add(ctrl);
        ctrl->SetValue(22);
      }

      wxStaticBox* sb1 = new wxStaticBox(this, -1, _("Pen Size"));
      wxStaticBoxSizer* RH2 = new wxStaticBoxSizer(sb1, wxHORIZONTAL);
      {
        RH2->Add(new wxRadioButton(this, 1010, _("1")), 0, wxCENTER | wxLEFT,
                 2);
        RH2->Add(new wxRadioButton(this, 1011, _("2")), 0, wxCENTER | wxLEFT,
                 2);
        wxRadioButton* rb4 = new wxRadioButton(this, 1012, _("4"));
        RH2->Add(rb4, 0, wxCENTER | wxLEFT, 2);
        rb4->SetValue(true);
        RH2->Add(new wxRadioButton(this, 1013, _("8")), 0, wxCENTER | wxLEFT,
                 2);
        RH2->Add(new wxRadioButton(this, 1014, _("16")), 0, wxCENTER | wxLEFT,
                 2);
        RH2->Add(new wxRadioButton(this, 1015, _("32")), 0, wxCENTER | wxLEFT,
                 2);
        RH2->Add(new wxRadioButton(this, 1017, _("128")), 0, wxCENTER | wxLEFT,
                 2);
        // RH2->Add(new wxRadioButton(this, 1019, _("512")), 0, wxCENTER |
        // wxLEFT, 2);
      }

      wxStaticBox* sb2 = new wxStaticBox(this, -1, _("Element Selection"));
      wxStaticBoxSizer* RH3 = new wxStaticBoxSizer(sb2, wxHORIZONTAL);
      {
        groupSelections = new wxListBox(this, 1000, wxDefaultPosition,
                                        wxDefaultSize, 0, NULL, wxLB_SINGLE);
        penSelections = new wxListBox(this, 1001, wxDefaultPosition,
                                      wxDefaultSize, 0, NULL, wxLB_SINGLE);

        RH3->Add(groupSelections, 1, wxEXPAND);
        RH3->Add(penSelections, 1, wxEXPAND);
      }

      wxStaticBox* sb3 = new wxStaticBox(this, -1, _("Element Description"));
      wxStaticBoxSizer* RH4 = new wxStaticBoxSizer(sb3, wxHORIZONTAL);

      elementDescription = new wxTextCtrl(
          this, -1,
          _("Erases.                                                       "),
          wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);
      RH4->Add(elementDescription, 1, wxEXPAND);

      RHSizer->Add(RH1, 0);
      RHSizer->Add(RH5, 0, wxALL | wxEXPAND, 5);
      RHSizer->Add(RH2, 0, wxALL, 5);
      RHSizer->Add(RH3, 1, wxEXPAND);
      RHSizer->Add(RH4, 0, wxEXPAND | wxALL, 5);
    }

    sizer->Add(g_canvas, 0, wxADJUST_MINSIZE | wxALL, 10);
    sizer->Add(RHSizer, 1, wxEXPAND);
  }
  this->SetSizer(sizer);

  this->Layout();
  sizer->Fit(this);

  g_timer = new wxTimer(g_canvas, 999);
  g_timer->Start(TIMERINTERVAL);

  g_timer_second = new wxTimer(g_canvas, 998);
  g_timer_second->Start(1000);
}

void MainFrame::OnTempSpin(wxSpinEvent& event) { g_T = event.GetPosition(); }

void ShowPenSelections() {
  penSelections->Clear();

  int firstVisible = -1;
  for (int i = 0; i < numberOfGroupItems[group_type]; ++i) {
    int n_element = groups[group_type][i];
    if (visible[n_element]) {
      if (firstVisible == -1) firstVisible = n_element;
      penSelections->Append(names[n_element]);
    }
  }
  penSelections->SetSelection(0);
  sand_type = firstVisible;
  elementDescription->SetValue(elementDescriptions[sand_type]);

  // todo:Get this to work.
  g_mainFrame->Layout();
}

void RefreshPenList() {
  groupSelections->Clear();
  penSelections->Clear();

  bool visibleGroups[MAXNUMBEROFELEMENTS];
  for (int i = 0; i < numberOfGroups; ++i) {
    visibleGroups[i] = false;
  }

  int firstVisibleGroup = -1;
  for (int k = 0; k < numberOfGroups; ++k) {
    for (int i = 0; i < numberOfGroupItems[k]; ++i) {
      int n_element = groups[k][i];
      if (visible[n_element]) {
        if (firstVisibleGroup == -1) firstVisibleGroup = k;
        visibleGroups[k] = true;
      }
    }
  }

  for (int i = 0; i < numberOfGroups; ++i)
    if (visibleGroups[i]) groupSelections->Append(groupNames[i]);

  group_type = firstVisibleGroup;
  groupSelections->SetSelection(0);

  // Now show pens.
  ShowPenSelections();

  group_type = 0;
  sand_type = 0;

  // todo: fix this
  g_mainFrame->Layout();
}

void MainFrame::loadSandbox(wxString filename) {
  wxImage loadBitmap;
  loadBitmap.LoadFile(filename);
  g_width = loadBitmap.GetWidth();
  g_height = loadBitmap.GetHeight();
  drawAll = true;

  memset(energy, 0, g_width * g_height * sizeof(unsigned char));

  for (int y = 0; y < g_height; ++y) {
    for (int x = 0; x < g_width; ++x) {
      int i = (y * g_width) + x;

      data[i] = 0;

      // Find the closest.
      unsigned int smallestDistance = 256 + 256 + 256;

      unsigned char red = loadBitmap.GetRed(x, y);
      unsigned char green = loadBitmap.GetGreen(x, y);
      unsigned char blue = loadBitmap.GetBlue(x, y);

      if (red == 0 && green == 0 && blue == 0) continue;

      for (int j = 0; j < numberOfElements; ++j) {
        if (j == 8) continue;
        unsigned int dist = int(abs(red - colors[j][0].Red())) +
                            int(abs(green - colors[j][0].Green())) +
                            int(abs(blue - colors[j][0].Blue()));
        if (dist < smallestDistance) {
          smallestDistance = dist;
          data[i] = j;
        }
      }
    }
  }

  g_canvas->SetSizeHints(g_width, g_height, g_width, g_height);
  // g_canvas->SetClientSize(g_width,g_height);
  g_mainFrame->Layout();
  g_mainFrame->GetSizer()->Fit(g_mainFrame);
}

void loadFileError(int lineNumber, const wxString message) {
  wxString str(_(""));
  str.Printf("Error on line %d: %s", lineNumber, message.c_str());

  wxMessageDialog dlg(g_mainFrame, str, _("Error"), wxOK);
  dlg.ShowModal();

  for (int i = 0; i < MAXNUMBEROFELEMENTS; ++i) {
    numberOfGroupItems[i] = 0;
    conductivity[i] = 0;
    visible[i] = 0;
    death_energy[i] = -1;
    death_prob[i] = 0;

    for (int j = 0; j < MAXNUMBEROFELEMENTS; ++j) {
      trans_energy[i][j] = -1;
      trans_prob[i][j] = 0;
      trans_xspeed[i][j] = 0;
      trans_yspeed[i][j] = 0;
      pexplosion[i][j] = -1;

      for (int k = 0; k < 100; ++k) {
        trans_center[i][j][k] = 0;
        trans_neighbor[i][j][k] = 0;
        death_center[i][k] = 0;
        ctrans_prob[i][j][k] = 0;

        cdeath_prob[i][k] = 0;
        colors[i][k] = wxColor(0, 0, 0);
      }
    }
  }

  numberOfGroups = 0;
  numberOfElements = 0;

  memset(data, 0, g_width * g_height * sizeof(unsigned char));
  memset(energy, 0, g_width * g_height * sizeof(unsigned char));
}

void clearPhysics() {
  for (int i = 0; i < MAXNUMBEROFELEMENTS; ++i) {
    sources[i] = 0;
    conductivity[i] = 0;
    visible[i] = 1;
    death_energy[i] = -1;
    death_prob[i] = 0;

    for (int j = 0; j < MAXNUMBEROFELEMENTS; ++j) {
      trans_energy[i][j] = -1;
      trans_prob[i][j] = 0;
      trans_xspeed[i][j] = 0;
      trans_yspeed[i][j] = 0;
      pexplosion[i][j] = -1;

      for (int k = 0; k < 100; ++k) {
        trans_center[i][j][k] = 0;
        trans_neighbor[i][j][k] = 0;
        death_center[i][k] = 0;
        cdeath_prob[i][k] = 0;
        ctrans_prob[i][j][k] = 0;

        colors[i][k] = wxColor(0, 0, 0);
      }
    }
  }
  numberOfGroups = 0;
  numberOfElements = 0;
  numberOfSources = 0;
}

void MainFrame::loadPhysics(wxString filename) {
  clearPhysics();

  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    loadFileError(0, _("Could not open file."));
    return;
  }

  char line[200];

  // First, find out what version of file this is.
  int version = 0;
  while (fgets(line, 200, file) != NULL) {
    if (line[0] == '#') {
      if (strstr(line, "subversion 1") != NULL)
        version = 1;
      else if (strstr(line, "subversion 2") != NULL)
        version = 2;
    }
  }

  if (version == 0) {
    loadFileError(0, _("Could not find a line that says \"subversion #\"."));
    fclose(file);
    return;
  }

  rewind(file);

  if (version == 1) {
    int state = 0;  // 0=between elements, 1=have read an element.
    numberOfElements = 0;
    while (fgets(line, 200, file) != NULL) {
      if (line[0] == '#') {
        // skip. comment.
      } else if (line[0] == ' ' || line[0] == '\t' || line[0] == '\n' ||
                 line[0] == '\r') {
        // Empty line.
        state = 0;
      } else {
        if (state == 0) {
          char name[128];
          sscanf(line, "%s\t", name);
          names[numberOfElements] = wxString(name);
          ++numberOfElements;
          state = 1;
        } else {
          // Just skip for now.
        }
      }
    }

    rewind(file);
    state = 0;
    int elementNumber = -1;
    wxString comment = _("");
    while (fgets(line, 200, file) != NULL) {
      if (line[0] == '#') {
        // comment.
      } else if (line[0] == ' ' || line[0] == '\t' || line[0] == '\n' ||
                 line[0] == '\r') {
        // Empty line.
        state = 0;
      } else {
        if (state == 0) {
          ++elementNumber;
          char name[128];
          int red, green, blue;
          char deathname[128];
          float d, s, g, dr;

          sscanf(line, "%s\t%d\t%d\t%d\t%f\t%f\t%f\t%s\t%f", name, &red, &green,
                 &blue, &d, &s, &g, deathname, &dr);

          density[elementNumber] = (double)d;
          slip[elementNumber] = (double)s;
          gravity[elementNumber] = (double)g;

          for (int i = 0; i < numberOfElements; ++i) {
            if (wxString(deathname) == names[i]) {
              for (int k = 0; k < 100; ++k) {
                death_center[elementNumber][k] = i;
                death_prob[elementNumber] = (double)dr;
                colors[elementNumber][k] = wxColour(
                    (unsigned char)(red + (double(k) / 100.0) * (255 - red)),
                    (unsigned char)(green +
                                    (double(k) / 100.0) * (255 - green)),
                    (unsigned char)(blue + (double(k) / 100.0) * (255 - blue)));
              }
            }
          }

          state = 1;
        } else {
          // Reading element interactions.
          char neighbor[128];
          char to[128];
          char from[128];
          float prob = 0;

          sscanf(line, "%s\t%s\t%s\t%f", neighbor, to, from, &prob);

          int n_neighbor = -1;
          int n_to = -1;
          int n_from = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(neighbor)) n_neighbor = i;
            if (names[i] == wxString(to)) n_to = i;
            if (names[i] == wxString(from)) n_from = i;
          }

          if (n_neighbor != -1 && n_to != -1 && n_from != -1) {
            for (int k = 0; k < 100; ++k) {
              trans_center[elementNumber][n_neighbor][k] = n_to;
              trans_neighbor[elementNumber][n_neighbor][k] = n_from;
              trans_prob[elementNumber][n_neighbor] = (double)prob;
            }
          }
        }
      }
    }
  } else if (version == 2) {
    // Then, count the number of elements and get their names.
    numberOfGroups = 1;
    groupNames[0] = _("Misc");
    numberOfGroupItems[0] = 0;
    numberOfElements = 0;

    wxString comment = _("");
    while (fgets(line, 200, file) != NULL) {
      if (line[0] == '#') {
        // comment.
        comment = wxString(line).Mid(1);
      } else if (line[0] == ' ' || line[0] == '\t' || line[0] == '\n' ||
                 line[0] == '\r') {
        // skip
        comment = _("");
      } else {
        char* type = strtok(line, " \t\n\r");
        if (strcmp(type, "element") == 0) {
          char* name = strtok(NULL, " \t\n\r");
          names[numberOfElements] = wxString(name);
          elementDescriptions[numberOfElements] = comment;
          ++numberOfElements;
        } else if (strcmp(type, "group") == 0) {
          char* name = strtok(NULL, " \t\n\r");
          groupNames[numberOfGroups] = wxString(name);
          ++numberOfGroups;
          comment = _("");
        } else {
          comment = _("");
        }
      }
    }

    if (numberOfElements > MAXNUMBEROFELEMENTS) {
      loadFileError(0, _("Error: Too many elements."));
    }

    numberOfSources = 0;
    rewind(file);

    // Now fill in the elements.
    int elementNumber = 0;
    int lineNumber = 0;
    while (fgets(line, 200, file) != NULL) {
      ++lineNumber;
      if (line[0] == '#' || line[0] == ' ' || line[0] == '\t' ||
          line[0] == '\n' || line[0] == '\r') {
        // skip.
      } else {
        char type[128];
        sscanf(line, "%s", type);
        if (strcmp(type, "element") == 0) {
          char name[128];
          int red;
          int green;
          int blue;
          float g, s, d, c;
          int v;

          sscanf(line, "%s%s%d%d%d%f%f%f%f%d", type, name, &red, &green, &blue,
                 &g, &s, &d, &c, &v);

          gravity[elementNumber] = (double)g;
          slip[elementNumber] = (double)s;
          density[elementNumber] = (double)d;
          conductivity[elementNumber] = (double)c;
          visible[elementNumber] = (bool)v;

          for (int k = 0; k < 100; ++k) {
            colors[elementNumber][k] = wxColour(
                (unsigned char)red, (unsigned char)green, (unsigned char)blue);
          }

          ++elementNumber;
        } else if (strcmp(type, "hotcolor") == 0) {
          char element[128];
          int red;
          int green;
          int blue;

          sscanf(line, "%s%s%d%d%d", type, element, &red, &green, &blue);

          int n_element = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(element)) n_element = i;
          }

          if (n_element == -1) {
            loadFileError(lineNumber,
                          _("Invalid hotcolor line. Unknown element."));
            fclose(file);
            return;
          }

          unsigned char ored = colors[n_element][0].Red();
          unsigned char ogreen = colors[n_element][0].Green();
          unsigned char oblue = colors[n_element][0].Blue();

          for (int k = 1; k < 100; ++k) {
            double nred = ored + (double(k) / 99.0) * (red - ored);
            double ngreen = ogreen + (double(k) / 99.0) * (green - ogreen);
            double nblue = oblue + (double(k) / 99.0) * (blue - oblue);

            colors[n_element][k] =
                wxColour((unsigned char)nred, (unsigned char)ngreen,
                         (unsigned char)nblue);
          }
        } else if (strcmp(type, "sources") == 0) {
          char* gar = strtok(line, " \t\n\r");
          gar[0] = 0;

          char* element = strtok(NULL, " \t\n\r");
          if (element == NULL) {
            loadFileError(lineNumber, _("Invalid sources line. Need at least 1 "
                                        "element in the group."));
            fclose(file);
            return;
          }

          while (element != NULL) {
            int n_element = -1;
            for (int i = 0; i < numberOfElements; ++i) {
              if (names[i] == wxString(element)) n_element = i;
            }

            if (n_element == -1) {
              loadFileError(lineNumber,
                            _("Invalid sources line. Unknown element."));
              fclose(file);
              return;
            }

            sources[numberOfSources] = n_element;
            ++numberOfSources;
            element = strtok(NULL, " \t\n\r");
          }

        } else if (strcmp(type, "group") == 0) {
          char* gar = strtok(line, " \t\n\r");
          char* group = strtok(NULL, " \t\n\r");

          if (gar == NULL || group == NULL) {
            loadFileError(lineNumber, _("Invalid group line."));
            fclose(file);
            return;
          }

          int n_group = -1;
          for (int i = 0; i < numberOfGroups; ++i) {
            if (groupNames[i] == wxString(group)) n_group = i;
          }

          numberOfGroupItems[n_group] = 0;

          char* element = strtok(NULL, " \t\n\r");

          if (element == NULL) {
            loadFileError(
                lineNumber,
                _("Invalid group line. Need at least 1 element in the group."));
            fclose(file);
            return;
          }

          while (element != NULL) {
            int n_element = -1;
            for (int i = 0; i < numberOfElements; ++i) {
              if (names[i] == wxString(element)) n_element = i;
            }

            if (n_element == -1) {
              loadFileError(lineNumber,
                            _("Invalid group line. Unknown element."));
              fclose(file);
              return;
            }

            groups[n_group][numberOfGroupItems[n_group]] = n_element;
            ++numberOfGroupItems[n_group];
            element = strtok(NULL, " \t\n\r");
          }

        } else if (strcmp(type, "self") == 0) {
          // deathrate. "self [prob] [element] [prob][element] ..."
          char* gar = strtok(line, " \t\n\r");
          char* thisprob = strtok(NULL, " \t\n\r");
          char* center = strtok(NULL, " \t\n\r");

          if (gar == NULL || thisprob == NULL || center == NULL) {
            loadFileError(lineNumber, _("Invalid self line."));
            fclose(file);
            return;
          }

          float n_thisprob = 0;
          sscanf(thisprob, "%f", &n_thisprob);

          int n_center = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(center)) n_center = i;
          }

          if (n_center == -1) {
            loadFileError(lineNumber, _("Invalid self line. Unknown element."));
            fclose(file);
            return;
          }

          death_prob[n_center] = (double)n_thisprob;

          char* prob = strtok(NULL, " \t\n\r");

          if (prob == NULL) {
            loadFileError(lineNumber,
                          _("Invalid self line. Need at least 1 probability."));
            fclose(file);
            return;
          }

          int currentProbIndex = 0;
          while (prob != NULL) {
            char* s_trans_center = strtok(NULL, " \t\n\r");

            if (s_trans_center == NULL) {
              loadFileError(lineNumber,
                            _("Invalid self line. Need at least 1 element."));
              fclose(file);
              return;
            }

            float n_prob;
            sscanf(prob, "%f", &n_prob);

            int n_trans_center = -1;
            for (int i = 0; i < numberOfElements; ++i) {
              if (names[i] == wxString(s_trans_center)) n_trans_center = i;
            }

            if (n_trans_center == -1) {
              loadFileError(lineNumber,
                            _("Invalid self line. Unknown element."));
              fclose(file);
              return;
            }

            // Fill the probability array.
            for (int i = currentProbIndex;
                 i < 100 && i < currentProbIndex + int(round(100.0 * n_prob));
                 ++i) {
              death_center[n_center][i] = n_trans_center;
            }
            currentProbIndex = currentProbIndex + int(round(100.0 * n_prob));

            prob = strtok(NULL, " \t\n\r");
          }

          for (int i = currentProbIndex; i < 100; ++i) {
            death_center[n_center][i] = n_center;
          }
        } else if (strcmp(type, "hotself") == 0) {
          // deathrate. "hotself [prob] [element]"
          char* gar = strtok(line, " \t\n\r");
          char* thisprob = strtok(NULL, " \t\n\r");
          char* center = strtok(NULL, " \t\n\r");

          if (gar == NULL || thisprob == NULL || center == NULL) {
            loadFileError(lineNumber, _("Invalid hotself line."));
            fclose(file);
            return;
          }

          int n_thisprob = 0;
          sscanf(thisprob, "%d", &n_thisprob);

          int n_center = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(center)) n_center = i;
          }

          if (n_center == -1) {
            loadFileError(lineNumber,
                          _("Invalid hotself line. Unknown element."));
            fclose(file);
            return;
          }

          death_energy[n_center] = n_thisprob;
        } else if (strcmp(type, "neighbor") == 0) {
          // neighbor [prob] [element] [element] [prob][element][element] ...
          // Reading element interactions.
          char* gar = strtok(line, " \t\n\r");
          char* thisprob = strtok(NULL, " \t\n\r");
          char* center = strtok(NULL, " \t\n\r");
          char* neighbor = strtok(NULL, " \t\n\r");

          if (gar == NULL || thisprob == NULL || center == NULL ||
              neighbor == NULL) {
            loadFileError(lineNumber, _("Invalid neighbor line."));
            fclose(file);
            return;
          }

          float n_thisprob = 0;
          sscanf(thisprob, "%f", &n_thisprob);

          int n_center = -1;
          int n_neighbor = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(center)) n_center = i;
            if (names[i] == wxString(neighbor)) n_neighbor = i;
          }

          if (n_center == -1 || n_neighbor == -1) {
            loadFileError(lineNumber,
                          _("Invalid neighbor line. Unknown element."));
            fclose(file);
            return;
          }

          trans_prob[n_center][n_neighbor] = (double)n_thisprob;

          char* prob = strtok(NULL, " \t\n\r");

          if (prob == NULL) {
            loadFileError(
                lineNumber,
                _("Invalid neighbor line. Must have at least 1 probability."));
            fclose(file);
            return;
          }

          int currentProbIndex = 0;
          while (prob != NULL) {
            char* s_trans_center = strtok(NULL, " \t\n\r");
            char* s_trans_neighbor = strtok(NULL, " \t\n\r");

            float n_prob;
            sscanf(prob, "%f", &n_prob);

            int n_trans_center = -1;
            int n_trans_neighbor = -1;
            for (int i = 0; i < numberOfElements; ++i) {
              if (names[i] == wxString(s_trans_center)) n_trans_center = i;
              if (names[i] == wxString(s_trans_neighbor)) n_trans_neighbor = i;
            }

            if (n_trans_center == -1 || n_trans_neighbor == -1) {
              loadFileError(lineNumber,
                            _("Invalid neighbor line. Unknown element."));
              fclose(file);
              return;
            }

            // Fill the probability array.
            for (int i = currentProbIndex;
                 i < 100 && i < currentProbIndex + int(round(100.0 * n_prob));
                 ++i) {
              trans_center[n_center][n_neighbor][i] = n_trans_center;
              trans_neighbor[n_center][n_neighbor][i] = n_trans_neighbor;
            }
            currentProbIndex = currentProbIndex + int(round(100.0 * n_prob));

            prob = strtok(NULL, " \t\n\r");
          }
          if (currentProbIndex != 100) {
            loadFileError(lineNumber, _("Invalid neighbor line. All "
                                        "probabilities must add up to 1."));
            fclose(file);
            return;
          }

        } else if (strcmp(type, "hotneighbor") == 0) {
          // hotneighbor [prob] [element] [element]
          // Reading element interactions.
          char* gar = strtok(line, " \t\n\r");
          char* thisprob = strtok(NULL, " \t\n\r");
          char* center = strtok(NULL, " \t\n\r");
          char* neighbor = strtok(NULL, " \t\n\r");

          if (gar == NULL || thisprob == NULL || center == NULL ||
              neighbor == NULL) {
            loadFileError(lineNumber, _("Invalid hotneighbor line."));
            fclose(file);
            return;
          }

          int n_thisprob = 0;
          sscanf(thisprob, "%d", &n_thisprob);

          int n_center = -1;
          int n_neighbor = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(center)) n_center = i;
            if (names[i] == wxString(neighbor)) n_neighbor = i;
          }

          if (n_center == -1 || n_neighbor == -1) {
            loadFileError(lineNumber,
                          _("Invalid hotneighbor line. Unknown element."));
            fclose(file);
            return;
          }

          trans_energy[n_center][n_neighbor] = n_thisprob;
        } else if (strcmp(type, "pexplosion") == 0) {
          // pexplosion [element] [element] [diameter] [element]
          // Reading element interactions.
          char* gar = strtok(line, " \t\n\r");
          char* center = strtok(NULL, " \t\n\r");
          char* neighbor = strtok(NULL, " \t\n\r");
          char* diameter = strtok(NULL, " \t\n\r");
          char* pelement = strtok(NULL, " \t\n\r");
          if (gar == NULL || center == NULL || neighbor == NULL ||
              diameter == NULL || pelement == NULL) {
            loadFileError(lineNumber, _("Invalid pexplosion line."));
            fclose(file);
            return;
          }

          int n_center = -1;
          int n_neighbor = -1;
          int n_pelement = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(center)) n_center = i;
            if (names[i] == wxString(neighbor)) n_neighbor = i;
            if (names[i] == wxString(pelement)) n_pelement = i;
          }

          if (n_center == -1 || n_neighbor == -1 || n_pelement == -1) {
            loadFileError(lineNumber,
                          _("Invalid pexplosion line. Unknown element."));
            fclose(file);
            return;
          }

          int n_diameter = 0;
          sscanf(diameter, "%d", &n_diameter);

          pexplosion[n_center][n_neighbor] = (short int)n_diameter;
          pexplosionelement[n_center][n_neighbor] = (unsigned char)n_pelement;

        } else if (strcmp(type, "explosion") == 0) {
          // explosion [element] [element]
          // Reading element interactions.
          char* gar = strtok(line, " \t\n\r");
          char* center = strtok(NULL, " \t\n\r");
          char* neighbor = strtok(NULL, " \t\n\r");
          char* xsp = strtok(NULL, " \t\n\r");
          char* ysp = strtok(NULL, " \t\n\r");

          if (gar == NULL || center == NULL || neighbor == NULL ||
              xsp == NULL || ysp == NULL) {
            loadFileError(lineNumber, _("Invalid explosion line."));
            fclose(file);
            return;
          }

          int n_center = -1;
          int n_neighbor = -1;
          for (int i = 0; i < numberOfElements; ++i) {
            if (names[i] == wxString(center)) n_center = i;
            if (names[i] == wxString(neighbor)) n_neighbor = i;
          }

          if (n_center == -1 || n_neighbor == -1) {
            loadFileError(lineNumber,
                          _("Invalid explosion line. Unknown element."));
            fclose(file);
            return;
          }

          int n_xsp = 0;
          sscanf(xsp, "%d", &n_xsp);

          int n_ysp = 0;
          sscanf(ysp, "%d", &n_ysp);

          trans_xspeed[n_center][n_neighbor] = (unsigned char)n_xsp;
          trans_yspeed[n_center][n_neighbor] = (unsigned char)n_ysp;
        } else {
          // loadFileError(lineNumber, _("Invalid line. Unrecognized keyword.
          // Ignoring.")); fclose(file); return;
          wxString str(_(""));
          str.Printf(
              "Error on line %d: Invalid line. Unrecognized keyword. Ignoring.",
              lineNumber);
          wxMessageDialog dlg(g_mainFrame, str, _("Error"), wxOK);
          dlg.ShowModal();
        }
      }
    }
  }

  // Figure out the misc group.
  bool grouped[MAXNUMBEROFELEMENTS];
  for (int i = 0; i < numberOfElements; ++i) grouped[i] = false;

  for (int i = 0; i < numberOfGroups; ++i) {
    for (int j = 0; j < numberOfGroupItems[i]; ++j) {
      grouped[groups[i][j]] = true;
    }
  }

  for (int i = 0; i < numberOfElements; ++i) {
    if (!grouped[i]) {
      groups[0][numberOfGroupItems[0]] = i;
      numberOfGroupItems[0] += 1;
    }
  }

  // Precalculate some commonly used probabilities.
  for (int i = 0; i < numberOfElements; ++i) {
    cgravity[i] = (int)round(gravity[i] * RAND_MAX);
    cslip[i] = (int)round(slip[i] * RAND_MAX);

    for (int k = 0; k < 100; ++k) {
      if (death_energy[i] == -1)
        cdeath_prob[i][k] = (int)round(death_prob[i] * RAND_MAX);
      else {
        // double inc = death_prob[i]/(99.0-double(death_energy[i]));
        // printf("%f\n", inc);
        if (k > death_energy[i]) {
          cdeath_prob[i][k] = (int)round(
              death_prob[i] * RAND_MAX);  //(int)round(double(k-death_energy[i])
                                          //* inc * RAND_MAX);
        } else {
          cdeath_prob[i][k] = 0;
        }
        // printf("%d,", cdeath_prob[i][k]);
      }
    }
    // printf("\n");
    // cdeath_prob[i] = (int)round(death_prob[i]*RAND_MAX);

    for (int j = 0; j < numberOfElements; ++j) {
      ccogravitydensity[i][j] = (int)round(
          fabs(gravity[i]) * fabs(density[i] - density[j]) * RAND_MAX);
      // probability that these will switch places.
      // High difference = high prob. Low difference=low prob.
      ccodensity[i][j] = (int)round(fabs(density[i] - density[j]) * RAND_MAX);

      // double inc = trans_prob[i][j]/(99.0-double(trans_energy[i][j]));
      for (int k = 0; k < 100; ++k) {
        if (trans_energy[i][j] == -1)
          ctrans_prob[i][j][k] = (int)round(trans_prob[i][j] * RAND_MAX);
        else if (k > trans_energy[i][j]) {
          ctrans_prob[i][j][k] =
              (int)round(trans_prob[i][j] *
                         RAND_MAX);  //(int)round(double(k-trans_energy[i][j])
                                     //* inc) * RAND_MAX;
        } else {
          ctrans_prob[i][j][k] = 0;
        }
      }

      // printf("%s + %s @ %d = \n", (char*)(names[i].c_str()),
      // (char*)(names[j].c_str()), ctrans_prob[i][j][0]);

      // ctrans_prob[i][j] = (int)round(trans_prob[i][j]*RAND_MAX);
    }
  }

  // Update selections.
  RefreshPenList();

  fclose(file);
}

bool OnNew(wxWindow* window) {
  // New
  wxString choices[] = {_("320x240 (fastest)"), _("640x480"), _("800x600"),
                        _("1024x768"), _("1280x1024 (slowest)")};
  wxSingleChoiceDialog dialog(NULL, _("Select an area size"), _("New"), 5,
                              choices);

  if (dialog.ShowModal() == wxID_OK) {
    if (dialog.GetSelection() == 0) {
      g_width = 320;
      g_height = 240;
    } else if (dialog.GetSelection() == 1) {
      g_width = 640;
      g_height = 480;
    } else if (dialog.GetSelection() == 2) {
      g_width = 800;
      g_height = 600;
    } else if (dialog.GetSelection() == 3) {
      g_width = 1024;
      g_height = 768;
    } else if (dialog.GetSelection() == 4) {
      g_width = 1280;
      g_height = 1024;
    }
    return true;
  }
  return false;
}

void MainFrame::OnMenu(wxCommandEvent& event) {
  if (event.GetId() == 1050) {
    OnNew(this);

    drawAll = true;
    memset(data, 0, g_width * g_height * sizeof(unsigned char));
    memset(energy, 0, g_width * g_height * sizeof(unsigned char));

    g_canvas->SetSizeHints(g_width, g_height, g_width, g_height);
    // g_canvas->SetClientSize(g_width,g_height);
    g_mainFrame->Layout();
    g_mainFrame->GetSizer()->Fit(g_mainFrame);

    g_canvas->Refresh();
  } else if (event.GetId() == 1051) {
    // Load
    wxFileDialog dialog(this, _("Load from a file"), _(""), _(""),
                        _("Any Image Files (*.*)|*.*"), wxFD_OPEN);

    if (dialog.ShowModal() == wxID_OK) {
      sandboxFilename = dialog.GetPath();
      loadSandbox(sandboxFilename);
    }
  } else if (event.GetId() == 1052) {
    // Save
    wxFileDialog dialog(this, _("Save to a file"), _(""), _(""),
                        _("PNG files (*.png)|*.png|BMP files (*.bmp)|*.bmp"),
                        wxFD_SAVE);

    if (dialog.ShowModal() == wxID_OK) {
      wxString filename = dialog.GetPath();
      unsigned char* bitmapdata2 = new unsigned char[g_width * g_height * 3];
      for (int i = 0; i < g_width * g_height; ++i) {
        bitmapdata2[(i * 3)] = colors[data[i]][0].Red();
        bitmapdata2[(i * 3) + 1] = colors[data[i]][0].Green();
        bitmapdata2[(i * 3) + 2] = colors[data[i]][0].Blue();
      }
      wxImage saveBitmap(g_width, g_height, bitmapdata2, true);
      wxBitmap bmp(saveBitmap);

      if (dialog.GetFilterIndex() == 0 || filename.Find(_(".png")) != -1 ||
          filename.Find(_(".PNG")) != -1) {
        bmp.SaveFile(filename, wxBITMAP_TYPE_PNG);
      } else {
        bmp.SaveFile(filename, wxBITMAP_TYPE_BMP);
      }

      delete[] bitmapdata2;
    }
  } else if (event.GetId() == 1053) {
    // Exit

    delete g_timer;
    delete g_timer_second;

    g_mainFrame->Close(true);
  } else if (event.GetId() == 1054) {
    // About
    wxString str = _("");
    str.Printf(
        _("Owen Piette's Falling Sand Game\nCopyright Owen Piette "
          "2006.\nVersion %s, compiled on %s\nCheck for updates "
          "at:\nhttp://www.piettes.com/fallingsandgame/\nThanks to coppro for "
          "his contributions.\nThanks to Troy Larson for the refresh button "
          "idea.\nThanks to purple100 for pexplosion.\nThanks to all the "
          "people at www.fallingsandgame.com for their support.\n\n"),
        VERSION, CVERSION);
    wxMessageDialog dlg(this, str, _("About"));
    dlg.ShowModal();
  } else if (event.GetId() == 1055) {
    // Load physics

    wxFileDialog dialog(this, _("Load from a file"), _(""), _(""),
                        _("Physics Files (*.txt)|*.txt"), wxFD_OPEN);
    if (dialog.ShowModal() == wxID_OK) {
      physicsFilename = dialog.GetPath();
      loadPhysics(physicsFilename);
    }
  } else if (event.GetId() == 1056) {
    // Save physics

    wxMessageDialog dlg(g_mainFrame,
                        _("Sorry, the save feature has been temporarily "
                          "disabled in this version."),
                        _("Error"), wxOK);
    dlg.ShowModal();

  } 
  doWalls = wallsCB->IsChecked();
  doSources = sourcesCB->IsChecked();
  doDraw = drawCB->IsChecked();
  doGravity = gravityCB->IsChecked();
  doEnergy = energyCB->IsChecked();
  doInteractions = interactionsCB->IsChecked();
  doLimit = limitCB->IsChecked();

  if (doLimit) {
    if (!(g_timer->IsRunning())) g_timer->Start(TIMERINTERVAL);
  } else if ((g_timer->IsRunning()))
    g_timer->Stop();
}

void MainFrame::OnButton(wxCommandEvent& event) {
  if (event.GetId() == 1025) {
    wxMessageDialog dlg(this, _("Are you certain?"), _("Refresh and Reload"),
                        wxYES_NO);
    if (dlg.ShowModal() == wxID_YES) {
      if (sandboxFilename == _("")) {
        // Refresh
        drawAll = true;
        memset(data, 0, g_width * g_height * sizeof(unsigned char));
        memset(energy, 0, g_width * g_height * sizeof(unsigned char));

      } else {
        // reload
        loadSandbox(sandboxFilename);
      }

      if (physicsFilename != _("")) {
        loadPhysics(physicsFilename);
      }
    }
  }
}

void MainFrame::OnToggleButton(wxCommandEvent& event) {
  if (event.GetId() == 1026) {
    if (doPause == false)
      doPause = true;
    else
      doPause = false;
  }
}

void MainFrame::OnGroupChoice(wxCommandEvent& event) {
  wxString groupName = event.GetString();
  group_type = 0;
  for (int i = 0; i < numberOfGroups; ++i) {
    if (groupNames[i] == groupName) {
      group_type = i;
      break;
    }
  }

  ShowPenSelections();
}

void MainFrame::OnPenChoice(wxCommandEvent& event) {
  wxString penName = event.GetString();
  sand_type = 0;
  for (int i = 0; i < numberOfElements; ++i) {
    if (names[i] == penName) {
      sand_type = i;
      break;
    }
  }

  elementDescription->SetValue(elementDescriptions[sand_type]);
}

void MainFrame::OnPenSize(wxCommandEvent& event) {
  if (event.GetId() == 1010)
    pen_width = 1;
  else if (event.GetId() == 1011)
    pen_width = 2;
  else if (event.GetId() == 1012)
    pen_width = 4;
  else if (event.GetId() == 1013)
    pen_width = 8;
  else if (event.GetId() == 1014)
    pen_width = 16;
  else if (event.GetId() == 1015)
    pen_width = 32;
  else if (event.GetId() == 1016)
    pen_width = 64;
  else if (event.GetId() == 1017)
    pen_width = 128;
  else if (event.GetId() == 1018)
    pen_width = 256;
  else if (event.GetId() == 1019)
    pen_width = 512;
}

void MainFrame::OnSize(wxSizeEvent& event) {
  doResize = true;
  drawAll = true;
  g_mainFrame->Layout();
}

void MainFrame::OnMove(wxFocusEvent& event) {
  drawAll = true;
  if (g_canvas) {
    g_canvas->Refresh();
  }
}

void MainFrame::OnClose(wxCloseEvent& event) {
  FILE* file = fopen("settings.ini", "w");
  if (!file) return;
  fprintf(file, "Boundary Walls:%d\n", (int)doWalls);
  fprintf(file, "Sources:%d\n", (int)doSources);
  fprintf(file, "Paint Updates:%d\n", (int)doDraw);
  fprintf(file, "Gravity:%d\n", (int)doGravity);
  fprintf(file, "Energy:%d\n", (int)doEnergy);
  fprintf(file, "Element Interactions:%d\n", (int)doInteractions);
  fprintf(file, "Limit FPS:%d\n", (int)doLimit);
  fclose(file);

  this->Destroy();
  g_isRunning = false;
}
