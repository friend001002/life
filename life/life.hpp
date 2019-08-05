#ifndef _LIFE_CPP_
#define _LIFE_CPP_

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
//#include <msvc/wx/setup.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class MyApp : public wxApp
{
  public:

  virtual bool OnInit();
};

class MyFrame : public wxFrame
{
  public:

  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  virtual ~MyFrame();

  private:

  void On_hello(wxCommandEvent& event);
  void On_exit(wxCommandEvent& event);
  void On_paint(wxPaintEvent& event);
  void On_mouse(wxMouseEvent& event);

  void Draw_mouse(wxPaintDC& dc);
  void Draw_grid(wxPaintDC& dc);

  wxDECLARE_EVENT_TABLE();

  int mouse_x_;
  int mouse_y_;

  int menu_h_;

  int board_map_[21][20];
};

enum
{
  ID_Hello = 1
};

#endif _LIFE_CPP_
