#ifndef _LIFE_CPP_
#define _LIFE_CPP_

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
//#include <msvc/wx/setup.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <thread>
#include <atomic>
#include <mutex>

enum
{
  COL_ROWS = 20
};

enum
{
  MNU_STEP = 1,
  MNU_RUN,
  MNU_STOP,
  MNU_CLEAR
};

enum
{
  BOARD_SIZE_1 = 410
};

enum
{
  CELL_SIZE_1 = 20
};

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

  void On_step(wxCommandEvent& event);
  void On_run(wxCommandEvent& event);
  void On_stop(wxCommandEvent& event);
  void On_clear(wxCommandEvent& event);

  void On_exit(wxCommandEvent& event);

  void On_paint(wxPaintEvent& event);
  void On_mouse(wxMouseEvent& event);

  void On_timer(wxCommandEvent& event);

  //void Draw_mouse(wxPaintDC& dc);
  void Draw_grid(wxPaintDC& dc);

  void MyFrame::Do_run();
  void MyFrame::Do_step();

  size_t Alive_neighbours(size_t col, size_t row);

  wxDECLARE_EVENT_TABLE();

  /*static const size_t cols_ { 21 };
  static const size_t rows_ { 20 };*/

  int mouse_x_;
  int mouse_y_;

  int board_map_[COL_ROWS][COL_ROWS];

  wxMenu *menu_;

  std::atomic<bool> running_;
  std::atomic<bool> stop_;
  //std::atomic<bool> clear_;

  wxTimer *timer_;

  std::mutex mutex_;
};

#endif _LIFE_CPP_
