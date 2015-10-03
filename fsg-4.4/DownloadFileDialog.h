#ifndef DLFH
#define DLFH

#include "Sand.h"
#include <wx/wx.h>

class DownloadFileDialog : public wxFrame {
public:
  DownloadFileDialog(const wxString& title, const wxPoint& pos, const wxSize& size, 
		     long style = wxDEFAULT_FRAME_STYLE);

  void OnOK(wxCommandEvent& event);

private:
  wxString physFilename;
  wxListBox* lbPhysicsChoices;
  wxString urlArray[1024];

  DECLARE_EVENT_TABLE()
};


#endif
