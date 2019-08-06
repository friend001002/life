#include "life.hpp"

using namespace std;

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(MNU_STEP, MyFrame::On_step)
  EVT_MENU(MNU_RUN, MyFrame::On_run)
  EVT_MENU(MNU_STOP, MyFrame::On_stop)
  EVT_MENU(MNU_CLEAR, MyFrame::On_clear)

  EVT_MENU(wxID_EXIT, MyFrame::On_exit)

  EVT_MENU(MNU_SIZE_20, MyFrame::On_20)
  EVT_MENU(MNU_SIZE_30, MyFrame::On_30)

  EVT_MENU(MNU_COLOR_RED, MyFrame::On_red)
  EVT_MENU(MNU_COLOR_GREEN, MyFrame::On_green)

  EVT_CLOSE(MyFrame::On_closing)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame("Conway's Game of Life", wxPoint(70, 70), wxSize(WIN_SIZE_20, WIN_SIZE_20));

  if (nullptr == frame)
  {
    cerr << "Failed to create a window. Error code 1\n";
    return false;
  }

  frame->SetBackgroundColour(wxColour(*wxWHITE));
  
  frame->SetMinSize(frame->GetSize());
  frame->SetMaxSize(frame->GetSize());

  frame->Show(true);

  return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
                                      :  wxFrame(NULL, wxID_ANY, title, pos, size)
                                       , mouse_x_{}, mouse_y_{}
                                       , menu_game_ {nullptr}, menu_size_{ nullptr }, menu_color_ {nullptr}
                                       , running_ {false}, stop_ {false}, changed_ {false}
                                       , timer_ {new wxTimer(this, 1)}
                                       , size_ {board_size_t::_20}, color_ {color_t::red}
{
  menu_game_ = new wxMenu;

  if (nullptr == menu_game_)
  {
    cerr << "Failed to create menu_game_. Error code 2\n";
    throw "Failed to create menu_game_. Error code 2\n";
  }

  menu_game_->Append(MNU_STEP,  "&Single step\tCtrl-S");
  menu_game_->Append(MNU_RUN,   "&Run\tCtrl-R");
  menu_game_->Append(MNU_STOP,  "S&top\tCtrl-T");
  menu_game_->Append(MNU_CLEAR, "&Clear\tCtrl-C");
  
  menu_game_->Enable(MNU_STOP, FALSE);
  
  menu_game_->AppendSeparator();
  menu_game_->Append(wxID_EXIT);

  menu_size_ = new wxMenu;

  if (nullptr == menu_size_)
  {
    cerr << "Failed to create menu_size_. Error code 3\n";
    throw "Failed to create menu_size_. Error code 3\n";
  }

  menu_size_->Append(MNU_SIZE_20, "&20x20");
  menu_size_->Append(MNU_SIZE_30, "&30x30");

  menu_size_->Enable(MNU_SIZE_20, FALSE);

  menu_color_ = new wxMenu;

  if (nullptr == menu_color_)
  {
    cerr << "Failed to create menu_color_. Error code 4\n";
    throw "Failed to create menu_color_. Error code 4\n";
  }

  menu_color_->Append(MNU_COLOR_RED, "&Red");
  menu_color_->Append(MNU_COLOR_GREEN, "&Green");

  menu_color_->Enable(MNU_COLOR_RED, FALSE);

  wxMenuBar *menu_bar = new wxMenuBar;

  if (nullptr == menu_bar)
  {
    cerr << "Failed to create menu_bar. Error code 5\n";
    throw "Failed to create menu_bar. Error code 5\n";
  }

  menu_bar->Append(menu_game_, "&Game");
  menu_bar->Append(menu_size_, "&Size");
  menu_bar->Append(menu_color_, "&Color");
  
  SetMenuBar(menu_bar);

  board_map_.resize((size_t)size_);
  for (size_t i {}; i < (size_t)size_; ++i)
  {
    board_map_[i].resize((size_t)size_);
  }

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

void MyFrame::On_closing(wxCloseEvent& event)
{
  // Prevent board_map_ premature destruction.
  std::lock_guard<std::mutex> lock(mutex_); 

  event.Skip(); // Actual closing.
}

void MyFrame::On_timer(wxCommandEvent& /*event*/)
{
  if (running_ == true)
  {
    SetStatusText("Running...");
  }
  else if (running_ == false)
  {
    menu_game_->Enable(MNU_STOP, FALSE);
    menu_game_->Enable(MNU_CLEAR, TRUE);
    menu_game_->Enable(MNU_RUN, TRUE);
    menu_game_->Enable(MNU_STEP, TRUE);

    if (size_ == board_size_t::_30)
    {
      menu_size_->Enable(MNU_SIZE_20, TRUE);
    }
    else if (size_ == board_size_t::_20)
    {
      menu_size_->Enable(MNU_SIZE_30, TRUE);
    }

    SetStatusText("Ready");
  }

  this->Refresh();
  this->Update();
}

void MyFrame::On_exit(wxCommandEvent& event)
{
  On_stop(event);

  Close(true);
}

void MyFrame::On_20(wxCommandEvent& event)
{
  if (size_ == board_size_t::_20)
  {
    return;
  }

  On_stop(event);
  On_clear(event);

  size_ = board_size_t::_20;

  board_map_.resize((size_t)size_);

  for (size_t i{}; i < (size_t)size_; ++i)
  {
    board_map_[i].resize((size_t)size_);
  }

  // Allow resizing.
  this->SetMinSize(wxSize(-1, -1));
  this->SetMaxSize(wxSize(-1, -1));

  this->SetSize(wxSize(WIN_SIZE_20, WIN_SIZE_20));

  this->SetMinSize(this->GetSize());
  this->SetMaxSize(this->GetSize());

  menu_size_->Enable(MNU_SIZE_20, FALSE);
  menu_size_->Enable(MNU_SIZE_30, TRUE);
}

void MyFrame::On_30(wxCommandEvent& event)
{
  if (size_ == board_size_t::_30)
  {
    return;
  }

  On_stop(event);
  On_clear(event);

  size_ = board_size_t::_30;

  board_map_.resize((size_t)size_);

  for (size_t i{}; i < (size_t)size_; ++i)
  {
    board_map_[i].resize((size_t)size_);
  }

  // Allow resizing.
  this->SetMinSize(wxSize(-1, -1));
  this->SetMaxSize(wxSize(-1, -1));

  this->SetSize(wxSize(WIN_SIZE_30, WIN_SIZE_30));

  this->SetMinSize(this->GetSize());
  this->SetMaxSize(this->GetSize());

  menu_size_->Enable(MNU_SIZE_30, FALSE);
  menu_size_->Enable(MNU_SIZE_20, TRUE);
}

void MyFrame::On_red(wxCommandEvent& /*event*/)
{
  color_ = color_t::red;

  menu_color_->Enable(MNU_COLOR_RED, FALSE);
  menu_color_->Enable(MNU_COLOR_GREEN, TRUE);
}

void MyFrame::On_green(wxCommandEvent& /*event*/)
{
  color_ = color_t::green;

  menu_color_->Enable(MNU_COLOR_RED, TRUE);
  menu_color_->Enable(MNU_COLOR_GREEN, FALSE);
}

size_t MyFrame::Alive_neighbours(size_t col, size_t row)
{
  size_t ret {};

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

    if (row < (size_t)size_ - 1)
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

  if (row < (size_t)size_ - 1)
  {
    if (board_map_[col][row + 1])
    {
      ++ret; // Bottom neighbour is alive
    }
  }

  if (col < (size_t)size_ - 1)
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

    if (row < (size_t)size_ - 1)
    {
      if (board_map_[col + 1][row + 1])
      {
        ++ret; // Right-bottom neighbour is alive
      }
    }
  }

  return ret;
}

void MyFrame::Do_step()
{
  mutex_.lock();

  changed_ = false;

  std::vector<std::vector<int>> tmp_map = board_map_;

  for (size_t col{}; col < (size_t)size_; ++col)
  {
    for (size_t row{}; row < (size_t)size_; ++row)
    {
      size_t num { Alive_neighbours(col, row) };

      if (board_map_[col][row] && num < 2)
      {
        tmp_map[col][row] = 0; // Underpopulation.

        changed_ = true;
      }
      
      if (board_map_[col][row] && num > 3)
      {
        tmp_map[col][row] = 0; // Overpopulation.

        changed_ = true;
      }

      if (board_map_[col][row] == 0 && num == 3)
      {
        tmp_map[col][row] = 1; // Reproduction.

        changed_ = true;
      }
    }
  }

  board_map_ = tmp_map;

  mutex_.unlock();
}

void MyFrame::On_step(wxCommandEvent& /*event*/)
{
  stop_ = false;
  running_ = false;

  SetStatusText("Step...");

  menu_size_->Enable(MNU_SIZE_20, FALSE);
  menu_size_->Enable(MNU_SIZE_30, FALSE);

  Do_step();

  if (size_ == board_size_t::_30)
  {
    menu_size_->Enable(MNU_SIZE_20, TRUE);
  }
  else if (size_ == board_size_t::_20)
  {
    menu_size_->Enable(MNU_SIZE_30, TRUE);
  }

  SetStatusText("Ready");
}

void MyFrame::Do_run()
{
  running_ = true;
  stop_ = false;

  do
  {
    Do_step();

    this_thread::sleep_for(100ms);
  }
  while (false == stop_ && true == changed_);
  
  running_ = false;
}

void MyFrame::On_run(wxCommandEvent& /*event*/)
{
  menu_game_->Enable(MNU_CLEAR, FALSE);
  menu_game_->Enable(MNU_RUN, FALSE);
  menu_game_->Enable(MNU_STEP, FALSE);
  menu_game_->Enable(MNU_STOP, TRUE);

  menu_size_->Enable(MNU_SIZE_20, FALSE);
  menu_size_->Enable(MNU_SIZE_30, FALSE);

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

  for (size_t col{}; col < (size_t)size_; ++col)
  {
    for (size_t row{}; row < (size_t)size_; ++row)
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

  if (event.Button(wxMOUSE_BTN_LEFT))
  {
    mouse_x_ = event.GetPosition().x;
    mouse_y_ = event.GetPosition().y;

    int tmp_x = mouse_x_ - (CELL_SIZE_1 / 2) < 0 ? 0 : mouse_x_ - (CELL_SIZE_1 / 2);
    int tmp_y = mouse_y_ - (CELL_SIZE_1 / 2) < 0 ? 0 : mouse_y_ - (CELL_SIZE_1 / 2);

    int col = tmp_x / CELL_SIZE_1;
    int row = tmp_y / CELL_SIZE_1;

    if (col >= (size_t)size_ || row >= (size_t)size_)
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

  size_t board_size { CELL_SIZE_1 * (size_t)size_ + 10 };

  for (size_t x { BOARD_MARGIN }; x <= board_size; x += CELL_SIZE_1)
  {
    dc.DrawLine(x, BOARD_MARGIN, x, board_size);
  }

  for (size_t y { BOARD_MARGIN }; y <= board_size; y += CELL_SIZE_1)
  {
    dc.DrawLine(BOARD_MARGIN, y, board_size, y);
  }

  if (color_ == color_t::red)
  {
    pen = wxPen(*wxRED, 10);
  }
  else if (color_ == color_t::green)
  {
    pen = wxPen(wxColor(0, 100, 0), 10);
  }
  
  dc.SetPen(pen);

  mutex_.lock();

  for (size_t col {}; col < (size_t)size_; ++col)
  {
    for (size_t row {}; row < (size_t)size_; ++row)
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
