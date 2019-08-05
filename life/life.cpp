#include "life.hpp"
//#include <iostream>

using namespace std;

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(MNU_STEP, MyFrame::On_step)
  EVT_MENU(MNU_RUN, MyFrame::On_run)
  EVT_MENU(MNU_STOP, MyFrame::On_stop)
  EVT_MENU(MNU_CLEAR, MyFrame::On_clear)

  EVT_MENU(wxID_EXIT, MyFrame::On_exit)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame("Conway's Game of Life", wxPoint(70, 70), wxSize(500, 500));
  frame->SetBackgroundColour(wxColour(*wxWHITE));
  
  frame->SetMinSize(frame->GetSize());
  frame->SetMaxSize(frame->GetSize());

  frame->Show(true);

  return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
                                      :  wxFrame(NULL, wxID_ANY, title, pos, size)
                                       , mouse_x_{}, mouse_y_{}
                                       , menu_ {nullptr}
                                       , running_ {false}, stop_{false}
                                       , timer_ {new wxTimer(this, 1)}
{
  menu_ = new wxMenu;

  menu_->Append(MNU_STEP,  "&Single step\tCtrl-S");
  menu_->Append(MNU_RUN,   "&Run\tCtrl-R");
  menu_->Append(MNU_STOP,  "S&top\tCtrl-T");
  menu_->Append(MNU_CLEAR, "&Clear\tCtrl-C");

  menu_->Enable(MNU_STOP, FALSE);

  menu_->AppendSeparator();
  menu_->Append(wxID_EXIT);

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(menu_, "&Game");
  
  SetMenuBar(menu_bar);

  CreateStatusBar();
  SetStatusText("Ready");

  Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::On_paint));

  Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MyFrame::On_mouse));

  Connect(wxEVT_TIMER, wxCommandEventHandler(MyFrame::On_timer));
  
  timer_->Start(100);
}

MyFrame::~MyFrame()
{
  delete timer_;
}

void MyFrame::On_timer(wxCommandEvent& /*event*/)
{
  this->Refresh();
  this->Update();
}

void MyFrame::On_exit(wxCommandEvent& event)
{
  On_stop(event);

  Close(true);
}

size_t MyFrame::Alive_neighbours(size_t col, size_t row)
{
  size_t ret {};

  //mutex_.lock();

  if (col > 0)
  {
    if (board_map_[col - 1][row])
    {
      ++ret; // Left neighbour is alive
    }

    if (row > 0)
    {
      if (board_map_[col - 1][row - 1])
      {
        ++ret; // Left-top neighbour is alive
      }
    }

    if (row < COL_ROWS - 1)
    {
      if (board_map_[col - 1][row + 1])
      {
        ++ret; // Left-bottom neighbour is alive
      }
    }
  }

  if (row > 0)
  {
    if (board_map_[col][row - 1])
    {
      ++ret; // Top neighbour is alive
    }
  }

  if (row < COL_ROWS - 1)
  {
    if (board_map_[col][row + 1])
    {
      ++ret; // Bottom neighbour is alive
    }
  }

  if (col < COL_ROWS - 1)
  {
    if (board_map_[col + 1][row])
    {
      ++ret; // Right neighbour is alive
    }

    if (row > 0)
    {
      if (board_map_[col + 1][row - 1])
      {
        ++ret; // Right-top neighbour is alive
      }
    }

    if (row < COL_ROWS - 1)
    {
      if (board_map_[col + 1][row + 1])
      {
        ++ret; // Right-bottom neighbour is alive
      }
    }
  }

  //mutex_.unlock();

  return ret;
}

void MyFrame::Do_step()
{
  int tmp_map[COL_ROWS][COL_ROWS];
  
  mutex_.lock();

  for (size_t col{}; col < COL_ROWS; ++col)
  {
    for (size_t row{}; row < COL_ROWS; ++row)
    {
      tmp_map[col][row] = board_map_[col][row];
    }
  }

  for (size_t col{}; col < COL_ROWS; ++col)
  {
    for (size_t row{}; row < COL_ROWS; ++row)
    {
      size_t num { Alive_neighbours(col, row) };

      if (board_map_[col][row] && num < 2)
      {
        tmp_map[col][row] = 0; // Underpopulation.
      }
      
      if (board_map_[col][row] && num > 3)
      {
        tmp_map[col][row] = 0; // Overpopulation.
      }

      if (board_map_[col][row] == 0 && num == 3)
      {
        tmp_map[col][row] = 1; // Reproduction.
      }
    }
  }

  for (size_t col{}; col < COL_ROWS; ++col)
  {
    for (size_t row{}; row < COL_ROWS; ++row)
    {
      board_map_[col][row] = tmp_map[col][row];
    }
  }

  mutex_.unlock();
}

void MyFrame::On_step(wxCommandEvent& /*event*/)
{
  stop_ = false;
  running_ = false;

  SetStatusText("Step...");

  Do_step();

  SetStatusText("Ready");
}

void MyFrame::Do_run()
{
  running_ = true;
  stop_ = false;

  SetStatusText("Running...");

  do
  {
    Do_step();

    this_thread::sleep_for(100ms);
  }
  while (false == stop_);
  
  menu_->Enable(MNU_STOP, FALSE);
  menu_->Enable(MNU_CLEAR, TRUE);
  menu_->Enable(MNU_RUN, TRUE);
  menu_->Enable(MNU_STEP, TRUE);

  running_ = false;
}

void MyFrame::On_run(wxCommandEvent& /*event*/)
{
  menu_->Enable(MNU_CLEAR, FALSE);
  menu_->Enable(MNU_RUN, FALSE);
  menu_->Enable(MNU_STEP, FALSE);
  menu_->Enable(MNU_STOP, TRUE);

  thread run_thread(&MyFrame::Do_run, this);
  run_thread.detach();
}

void MyFrame::On_stop(wxCommandEvent& /*event*/)
{
  SetStatusText("Stop...");

  stop_ = true;

  SetStatusText("Ready");
}

void MyFrame::On_clear(wxCommandEvent& /*event*/)
{
  SetStatusText("Clear...");

  mutex_.lock();

  for (size_t col{}; col < COL_ROWS; ++col)
  {
    for (size_t row{}; row < COL_ROWS; ++row)
    {
      board_map_[col][row] = 0;
    }
  }

  mutex_.unlock();

  SetStatusText("Ready");
}

void MyFrame::On_mouse(wxMouseEvent& event)
{
  if (running_)
  {
    return;
  }

  if (event.Moving())
  {
   
  }

  if (event.Button(wxMOUSE_BTN_LEFT))
  {
    mouse_x_ = event.GetPosition().x;
    mouse_y_ = event.GetPosition().y;

    int tmp_x = mouse_x_ - (CELL_SIZE_1 / 2) < 0 ? 0 : mouse_x_ - (CELL_SIZE_1 / 2);
    int tmp_y = mouse_y_ - (CELL_SIZE_1 / 2) < 0 ? 0 : mouse_y_ - (CELL_SIZE_1 / 2);

    int col = tmp_x / CELL_SIZE_1;
    int row = tmp_y / CELL_SIZE_1;

    if (col >= COL_ROWS || row >= COL_ROWS)
    {
      return;
    }

    mutex_.lock();

    board_map_[col][row] == 0 ? board_map_[col][row] = 1 : board_map_[col][row] = 0;

    mutex_.unlock();

    this->Refresh();
    this->Update();
  }
}

void MyFrame::Draw_grid(wxPaintDC& dc)
{
  wxPen pen = wxPen(*wxBLACK, 2);
  dc.SetPen(pen);

  for (size_t x {10}; x <= BOARD_SIZE_1; x += CELL_SIZE_1)
  {
    dc.DrawLine(x, 10, x, BOARD_SIZE_1);
  }

  for (size_t y {10}; y <= BOARD_SIZE_1; y += CELL_SIZE_1)
  {
    dc.DrawLine(10, y, BOARD_SIZE_1, y);
  }

  pen = wxPen(*wxRED, 10);
  dc.SetPen(pen);

  mutex_.lock();

  for (size_t col {}; col < COL_ROWS; ++col)
  {
    for (size_t row {}; row < COL_ROWS; ++row)
    {
      if (board_map_[col][row] == 1)
      {
        dc.DrawLine(col * CELL_SIZE_1 + 18, row * CELL_SIZE_1 + 19, col * CELL_SIZE_1 + 20, row * CELL_SIZE_1 + 20);
      }
    }
  }

  mutex_.unlock();
}

void MyFrame::On_paint(wxPaintEvent& /*event*/)
{
  wxPaintDC dc(this);

  Draw_grid(dc);
}
