#ifndef _LIFE_CPP_
#define _LIFE_CPP_

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
//#include <msvc/wx/setup.h>
#include "wx/colordlg.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

enum class board_size_t : size_t
{
  _20 = 20,
  _30 = 30
};

enum class color_t : size_t
{
  red = 1,
  green,
  custom
};

enum
{
  MNU_STEP = 1,
  MNU_RUN,
  MNU_STOP,
  MNU_CLEAR,

  MNU_SIZE_20,
  MNU_SIZE_30,

  MNU_COLOR_RED,
  MNU_COLOR_GREEN,
  MNU_COLOR_CUSTOM
};

enum
{
  WIN_SIZE_20 = 500, 
  WIN_SIZE_30 = 700
};

enum
{
  CELL_SIZE_1 = 20
};

enum
{
  BOARD_MARGIN = 10
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

  void On_20(wxCommandEvent& event);
  void On_30(wxCommandEvent& event);

  void On_red(wxCommandEvent& event);
  void On_green(wxCommandEvent& event);
  void On_custom_color(wxCommandEvent& event);

  void On_paint(wxPaintEvent& event);
  void On_mouse(wxMouseEvent& event);

  void On_timer(wxCommandEvent& event);

  void On_closing(wxCloseEvent& event);
  
  void Draw_grid(wxPaintDC& dc);

  void Do_run();
  void Do_step();

  size_t Alive_neighbours(size_t col, size_t row);

  wxDECLARE_EVENT_TABLE();

  int mouse_x_;
  int mouse_y_;
  
  std::vector<std::vector<int>> board_map_;

  wxMenu *menu_game_;
  wxMenu *menu_size_;
  wxMenu *menu_color_;

  board_size_t size_;
  color_t      color_;
  wxColourData custom_color_;

  std::atomic<bool> running_;
  std::atomic<bool> stop_;
  std::atomic<bool> changed_;

  wxTimer *timer_;

  std::mutex mutex_;
};

#endif //_LIFE_CPP_
