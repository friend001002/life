#include "life.hpp"
//#include <iostream>

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::On_hello)
EVT_MENU(wxID_EXIT, MyFrame::On_exit)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame("Life game", wxPoint(70, 70), wxSize(500, 500));
  frame->SetBackgroundColour(wxColour(*wxWHITE));
  frame->Show(true);

  return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
                                      :  wxFrame(NULL, wxID_ANY, title, pos, size)
                                       , mouse_x_{}, mouse_y_{}
                                       , menu_h_{}
{
  wxMenu *menuFile = new wxMenu;

  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
    "Help string shown in status bar for this menu item");

  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  
  SetMenuBar(menuBar);

  menu_h_ = menuBar->GetBestVirtualSize().GetHeight();

  Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::On_paint));
  Connect(wxEVT_MOTION, wxMouseEventHandler(MyFrame::On_mouse));
  Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MyFrame::On_mouse));
}

MyFrame::~MyFrame()
{
  
}

void MyFrame::On_exit(wxCommandEvent& /*event*/)
{
  Close(true);
}

void MyFrame::On_hello(wxCommandEvent& /*event*/)
{
  wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::On_mouse(wxMouseEvent& event)
{
  if (event.Moving())
  {
    mouse_x_ = event.GetPosition().x;
    mouse_y_ = event.GetPosition().y;
  }

  if (event.Button(wxMOUSE_BTN_LEFT))
  {
    int col = mouse_x_ / 20;
    int row = mouse_y_ / 20;

    board_map_[col][row] = 1;
  }

  this->Refresh();
  this->Update();
}

void MyFrame::Draw_grid(wxPaintDC& dc)
{
  wxPen pen = wxPen(*wxBLACK, 2);
  dc.SetPen(pen);

  for (size_t x {2}; x <= 402; x += 20)
  {
    dc.DrawLine(x, 5, x, 405);
  }

  for (size_t y {5}; y <= 405; y += 20)
  {
    dc.DrawLine(2, y, 402, y);
  }

  pen = wxPen(*wxGREEN, 10);
  dc.SetPen(pen);
  
  for (size_t col {}; col < 20; ++col)
  {
    for (size_t row {}; row < 20; ++row)
    {
      if (board_map_[col][row] == 1)
      {
        //dc.DrawRectangle(col * 20 + 3, row * 20 + 5, 20, 20);
        dc.DrawLine(col * 20 + 10, row * 20 + 12, col * 20 + 11, row * 20 + 13);
      }
    }
  }
}

void MyFrame::Draw_mouse(wxPaintDC& dc)
{
  wxPen pen = wxPen(*wxRED, 2);
  dc.SetPen(pen);

  dc.DrawLine(mouse_x_ - 5, mouse_y_,     mouse_x_ + 5, mouse_y_);
  dc.DrawLine(mouse_x_,     mouse_y_ - 5, mouse_x_,     mouse_y_ + 5);
}

void MyFrame::On_paint(wxPaintEvent& /*event*/)
{
  wxPaintDC dc(this);

  //wxSize size = GetClientSize();

  Draw_grid(dc);

  Draw_mouse(dc);
}
