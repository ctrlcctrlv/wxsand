#ifndef CANVASH
#define CANVASH

#include <wx/wx.h>

#include "Sand.h"

class Canvas : public wxWindow {
 public:
  Canvas(wxWindow* parent, wxWindowID id, wxPoint, wxSize);

  void OnEraseBG(wxEraseEvent& e);
  void OnMouseRightDown(wxMouseEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  void OnMouseLeftUp(wxMouseEvent& event);
  void OnMouseMove(wxMouseEvent& event);
  void OnTimer(wxTimerEvent& event);
  void OnSecondTimer(wxTimerEvent& event);
  void Refresh();
  void OnIdle(wxIdleEvent& e);

  bool busyCalculating;
  bool doUpdate;
  void calculate();

 private:
  DECLARE_EVENT_TABLE()
};

#endif
