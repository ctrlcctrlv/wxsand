#ifndef MAINFRAMEH
#define MAINFRAMEH

#include <string.h>
#include <wx/spinctrl.h>
#include <wx/wx.h>
#include "Sand.h"

class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);
  void OnSize(wxSizeEvent& event);
  void OnMove(wxFocusEvent& event);

  void OnGroupChoice(wxCommandEvent& event);
  void OnPenChoice(wxCommandEvent& event);
  void OnPenSize(wxCommandEvent& event);
  void OnTempSpin(wxSpinEvent& event);

  void OnMenu(wxCommandEvent& event);
  void OnButton(wxCommandEvent& event);
  void OnToggleButton(wxCommandEvent& event);

  void loadPhysics(wxString);
  void loadSandbox(wxString);

  void OnClose(wxCloseEvent& event);

 private:
  DECLARE_EVENT_TABLE()
};

#endif
