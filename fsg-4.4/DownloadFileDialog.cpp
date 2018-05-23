#include "DownloadFileDialog.h"
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/protocol/http.h>
#include <wx/textfile.h>
#include <wx/txtstrm.h>
#include <wx/url.h>

BEGIN_EVENT_TABLE(DownloadFileDialog, wxFrame)
EVT_BUTTON(1000, DownloadFileDialog::OnOK)
END_EVENT_TABLE()

DownloadFileDialog::DownloadFileDialog(const wxString& title,
                                       const wxPoint& pos, const wxSize& size,
                                       long style)
    : wxFrame(NULL, -1, title, pos, size, style) {
  lbPhysicsChoices = new wxListBox(this, 3000, wxDefaultPosition, wxDefaultSize,
                                   0, NULL, wxLB_SINGLE);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  {
    sizer->Add(lbPhysicsChoices, 1, wxEXPAND);
    sizer->Add(new wxButton(this, 1000, _("OK")), 0, wxCENTER | wxTOP, 2);
  }
  this->SetSizer(sizer);

  wxURL listURL("http://www.fallingsandgame.com/wiki/index.php/WxSand_Mods");
  // listURL.SetProxy("207.32.43.252:8080");

  wxInputStream* is = listURL.GetInputStream();
  if (is == NULL) {
    wxMessageDialog* msg =
        new wxMessageDialog(this, "Error connecting to the internet.");
    msg->ShowModal();
    return;
  }

  wxTextInputStream* tis = new wxTextInputStream(*(is));

  wxString line;
  int index = 0;
  while (!line.Contains("</body>")) {
    line = tis->ReadLine();
    wxString url = line;
    wxString description1 = line;
    wxString description2 = line;
    if (line.Contains("<li>")) {
      int startIndex = line.Find("href=\"");
      url = line.Mid(startIndex + 6);
      int endIndex = url.Find("\"");
      url = url.Mid(0, endIndex);
      urlArray[index] = url;
      ++index;

      startIndex = description1.Find("<li>");
      description1 = description1.Mid(startIndex + 4);
      startIndex = description1.Find(">");
      description1 = description1.Mid(startIndex + 1);

      endIndex = description1.Find("</a>");
      description1 = description1.Mid(0, endIndex);

      startIndex = description2.Find("</a>");
      description2 = description2.Mid(startIndex + 4);

      lbPhysicsChoices->Append(description1 + description2);
    }
  }

  // sizer->Fit(this);

  this->physFilename = _("default.txt");
}

void DownloadFileDialog::OnOK(wxCommandEvent& event) {
  wxString filename = urlArray[lbPhysicsChoices->GetSelection()];

  wxURL fileURL(_("http://www.fallingsandgame.com") + filename);
  // fileURL.SetProxy("207.32.43.252:8080");

  wxInputStream* is = fileURL.GetInputStream();
  if (is == NULL) {
    wxString str = _("Error loading from the file from the internet. ");
    str += _("http://www.fallingsandgame.com") + filename;
    wxMessageDialog* msg = new wxMessageDialog(this, str);
    msg->ShowModal();
    return;
  }

  wxTextInputStream* tis = new wxTextInputStream(*(is));

  wxTextFile of("downloadedPhysics.txt");
  if (of.Exists()) {
    of.Open();
    of.Clear();
  } else {
    of.Create();
  }

  int state = 0;
  int count = 0;
  while (!is->Eof() && state < 2 && count < 10000) {
    wxString line = tis->ReadLine();
    if (line.Contains("<pre>")) {
      state = 1;
      continue;
    }
    if (line.Contains("</pre>")) state = 2;

    if (state == 1) {
      of.AddLine(line);
    }

    ++count;
  }

  if (state != 2) {
    wxMessageDialog* msg = new wxMessageDialog(
        this, "Couldn't find the physics file on the Wiki page.");
    msg->ShowModal();
    of.Write();
    of.Close();
    this->Destroy();
    return;
  }

  of.Write();
  of.Close();

  this->physFilename = _("downloadedPhysics.txt");
  physicsFilename = this->physFilename;
  g_mainFrame->loadPhysics(this->physFilename);

  this->Destroy();
}
