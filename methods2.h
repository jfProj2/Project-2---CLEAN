#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
#include <curses.h>
#include <ncurses.h>
#include <menu.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


//PROTOTYPES
string load_file(string filename);
bool close_file(int &fd);
void run_editor(const char * fileptr);
string get_filename(const char * fileptr);
void file_to_screen(string data, WINDOW * win, int h, int w);
int show_menu_window(int menu_h, int menu_w, int menu_y, int menu_x);
string add_char(WINDOW* win, int& x, int& y,int ch, string data);
string remove_char(string s);
void save_file(string filename, string data);
bool file_exists(const char * fileptr);
int show_error_window(int code, int err_h, int err_w, int err_y, int err_x);
int show_save_window(int save_h, int save_w, int save_y, int save_x);
int show_open_window(int open_h, int open_w, int open_y, int open_x);
//FUNCTION IMPLEMENTATION



int show_save_window(int save_h, int save_w, int save_y, int save_x){
  WINDOW *save_win;
  WINDOW *save_subwin;
  MENU *save_menu;
  ITEM **items;
  initscr();
  const char * choices[] = {"Yes", "No", "Cancel"};
  int n_choices = ARRAY_SIZE(choices);
  items = (ITEM**)calloc(n_choices+1, sizeof(ITEM*));
  for(int i = 0; i < n_choices; i++){
    items[i] = new_item(choices[i], NULL);
  }
  items[n_choices] = (ITEM*)NULL;
  save_menu = new_menu((ITEM**)items);
  save_win = newwin(save_h, save_w, save_y, save_x);
  save_subwin = derwin(save_win, (save_h/2), (save_w/2), (save_h*0.4), (save_w/3));
  keypad(save_win, true);

  set_menu_win(save_menu, save_win);
  set_menu_sub(save_menu, save_subwin);
  set_menu_mark(save_menu, "* ");
  mvwprintw(save_win, 0, 0,"Do you wish to save the current text?");
  box(save_win, '*', '*');
  wrefresh(save_win);
  cbreak();
  post_menu(save_menu);
  wrefresh(save_win);

  int key = 1;
  int choice = 0;
  bool selected = false;

  while(!selected){
    key = wgetch(save_win);
    switch(key){
    case KEY_DOWN:
      menu_driver(save_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(save_menu, REQ_UP_ITEM);
      break;
    case 10:
      //MORE TO DO HERE?
      selected = true;
      break;
    }
    wrefresh(save_win);
  }

  if(selected){
    ITEM *cur_item = current_item(save_menu);
    const char * it = item_name((const ITEM*)cur_item);
    if(strcmp(it,"No") == 0){
      choice = 0;
    } else if (strcmp(it,"Yes") == 0){
      choice = 1;
    } else if (strcmp(it,"Cancel") == 0){
      choice = 2;
    }
  }

  unpost_menu(save_menu);
  for(int i=0; i < n_choices; i++){
    free_item(items[i]);
  }

  wborder(save_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  mvwprintw(save_win, save_y+10, save_x+10,"");
  wrefresh(save_win);
  //mvprintw(2, (menu_w/2)-4, NULL);
  wrefresh(save_win);
  free_menu(save_menu);
  wrefresh(save_win);
  delwin(save_subwin);
  delwin(save_win);
  // endwin();
  return choice; //USER EXITS MENU (F1 Pressed)
}


int show_open_window(int open_h, int open_w, int open_y, int open_x){
   WINDOW *open_win;
   WINDOW *open_subwin;
    MENU *open_menu;
    ITEM **items;
    initscr();
    const char * choices[] = {"Open"};
    int n_choices = ARRAY_SIZE(choices);
    items = (ITEM**)calloc(n_choices+1, sizeof(ITEM*));
    for(int i = 0; i < n_choices; i++){
      items[i] = new_item(choices[i], NULL);
    }
    items[n_choices] = (ITEM*)NULL;
    open_menu = new_menu((ITEM**)items);
    open_win = newwin(open_h, open_w, open_y, open_x);
    open_subwin = derwin(open_win, (open_h/2), (open_w/2), (open_h*0.4), (open_w/3));
    keypad(open_win, true);
    set_menu_win(open_menu, open_win);
    set_menu_sub(open_menu, open_subwin);
    set_menu_mark(open_menu, "* ");
    box(open_win, '*', '*');
    wrefresh(open_win);
    cbreak();
    post_menu(open_menu);
    wrefresh(open_win);

    int key = 1;
    int choice = 0;
    bool selected = false;

    while(!selected){
      key = wgetch(open_win);
      switch(key){
      case KEY_DOWN:
        menu_driver(open_menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(open_menu, REQ_UP_ITEM);
        break;
      case 10:
        //MORE TO DO HERE?
        selected = true;
        break;
      }
      wrefresh(open_win);
    }

    if(selected){
      ITEM *cur_item = current_item(open_menu);
      const char * it = item_name((const ITEM*)cur_item);
      if(strcmp(it,"Open") == 0){
        choice = 1;
      }
    }

    unpost_menu(open_menu);
    for(int i=0; i < n_choices; i++){
      free_item(items[i]);
    }

    wborder(open_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(open_win);
    //mvprintw(2, (menu_w/2)-4, NULL);
    wrefresh(open_win);
    free_menu(open_menu);
    wrefresh(open_win);
    delwin(open_subwin);
    delwin(open_win);
    // endwin();
    return choice; //USER EXITS MENU (F1 Pressed)
}

int show_error_window(int code, int err_h, int err_w, int err_y, int err_x){
  WINDOW *error_win;
  WINDOW *error_subwin;
  MENU *error_menu;
  ITEM **items;
  initscr();
  const char * choices[] = {"Yes", "No"};
  int n_choices = ARRAY_SIZE(choices);
  items = (ITEM**)calloc(n_choices+1, sizeof(ITEM*));
  for(int i = 0; i < n_choices; i++){
    items[i] = new_item(choices[i], NULL);
  }
  items[n_choices] = (ITEM*)NULL;
  error_menu = new_menu((ITEM**)items);
  error_win = newwin(err_h, err_w, err_y, err_x);
  error_subwin = derwin(error_win, (err_h/2), (err_w/2), (err_h*0.4), (err_w/3));
  keypad(error_win, true);


  set_menu_win(error_menu, error_win);
  set_menu_sub(error_menu, error_subwin);
  set_menu_mark(error_menu, "* ");
  box(error_win, '*', '*');
  wrefresh(error_win);
  cbreak();
  post_menu(error_menu);
  wrefresh(error_win);

  int key = 1;
  int choice = 0;
  bool selected = false;

  while(key != KEY_F(1) && !selected){
    key = wgetch(error_win);
    switch(key){
    case KEY_DOWN:
      menu_driver(error_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(error_menu, REQ_UP_ITEM);
      break;
    case 10:
      //MORE TO DO HERE?
      selected = true;
      break;
    }
    wrefresh(error_win);
  }

  if(selected){
    ITEM *cur_item = current_item(error_menu);
    const char * it = item_name((const ITEM*)cur_item);
    if(strcmp(it,"Open") == 0){
      choice = 1;
    } else if (strcmp(it,"Save") == 0){
      choice = 2;
    } else if (strcmp(it,"Save As") == 0){
      choice = 3;
    } else if (strcmp(it,"Exit") == 0){
      choice = 4;
    }
  }

  unpost_menu(error_menu);
  for(int i=0; i < n_choices; i++){
    free_item(items[i]);
  }

  wborder(error_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(error_win);
  //mvprintw(2, (menu_w/2)-4, NULL);
  wrefresh(error_win);
  free_menu(error_menu);
  wrefresh(error_win);
  delwin(error_subwin);
  delwin(error_win);
  // endwin();
  return choice; //USER EXITS MENU (F1 Pressed)
}

string get_filename(const char * fileptr){
  if (fileptr == nullptr){
    return "Untitled";
  }
  else {
    string fn = fileptr;
    return fn;
  }
}

string load_file(string filename){
  int fd = open(filename.c_str(), O_RDWR | O_APPEND /*| O_CREAT, S_IRWXU*/);
  if(fd < 0){
    //int errorNum = errno;
    //error window
    //return int referring to type of error
    return "";
  }
  else{
    const int BUFF_SIZE = 1024;
    char buffer[BUFF_SIZE];
    int n = 0;
    string data = "";
    while((n = read(fd, buffer, BUFF_SIZE)) > 0){
      //write?
      for(int i = 0; i < n; i++){
          data += buffer[i];
      }
    }
    return data;
  }
}

bool close_file(int &fd){
  fd = close(fd);
  if(fd < 0){
    int errorNum = errno;
    printf("\nclose_file: fd: %i: Error.\n", fd);
    printf("errno: %i - %s\n\n",errorNum, strerror(errorNum));
    return false;
  }
  return true;
}

void run_editor(const char *fileptr){
  WINDOW *term_win;
  WINDOW *edit_win;
  string filename = get_filename(fileptr);
  bool exists = file_exists(fileptr);
  initscr();
  cbreak();
  noecho();
  int term_h, term_w;
  getmaxyx(stdscr, term_h, term_w);
  int menu_h = term_h/2;
  int menu_w = term_w/2;
  int menu_x = (term_w / 4), menu_y = (term_h / 4);
  int edit_x = 1, edit_y = 1;
  int edit_h = (term_h * 0.90);
  int edit_w = (term_w * 0.90);
  term_win = newwin(term_h, term_w, 0, 0);
  edit_win = derwin(term_win, edit_h, edit_w, edit_x, edit_y);

  int ex = 2;
  int ey = 2;
  //FIGURE OUT PADS?
  //edit_win = newpad(edit_h,edit_w);

  touchwin(edit_win);
  keypad(term_win, true);
  keypad(edit_win, true);
  refresh();
  mvwprintw(term_win, 0, 0, "F1: Menu");
  mvwprintw(term_win, 0, (term_w/2)-8, "CSCI 1730 Editor");
  mvwprintw(term_win, (term_h-1), 0, filename.c_str());

  box(edit_win, '*', '*');
  string text = load_file(filename);
  file_to_screen(text, edit_win, edit_h, edit_w);
  wrefresh(term_win);
  wrefresh(edit_win);
  int key = 1;
  bool running = true;
  string appended = "";
  while(running){
    refresh();
    bool saved = false;
    key = wgetch(edit_win);
    text = add_char(edit_win,ex, ey,key, text);
    //getyx(edit_win, ey, ex);
    //wmove(edit_win,ey, ex);
    refresh();
    wrefresh(edit_win);
    file_to_screen(text, edit_win, edit_h, edit_w);

    switch(key){
    case KEY_F(1):
      int selection = show_menu_window(menu_h, menu_w, menu_y, menu_x);
      wrefresh(term_win);
      if(selection == 0){
        wrefresh(term_win);
        wrefresh(edit_win);
      }
      else if(selection == 1){
        int savePrompt = show_save_window(menu_h, menu_w, menu_y, menu_x);
        if(savePrompt == 0){ //Do not save
          filename = show_open_window(menu_h, menu_w, menu_y, menu_x);
          text = load_file(filename);
          file_to_screen(text, edit_win, edit_h, edit_w);

        }
        else if(savePrompt == 1){ //Yes, save


        }
        else{  //Cancel, return to current edit

        }
      }
      else if(selection == 2){
        save_file(filename, text);
      }
      else if(selection == 3){
        //save as
      }
      else if(selection == 4){
        running = false;
      }
      break;
    }
    wrefresh(term_win);
    wrefresh(edit_win);
  }
  delwin(edit_win);
  delwin(term_win);;
  endwin();
}



string add_char(WINDOW * win, int& x, int& y, int ch, string s){
  //getyx(win, y, x);
  int max_x, max_y;
  getmaxyx(win, max_y, max_x);
  // mvwprintw(win,
  string data = "";
  int len = s.size();
  if(ch == 127 || ch == 8){
    if(len > 0){
      data += remove_char(s);
      }
    }
    else if(ch == KEY_DOWN){
      if(y < max_y -1){
	y++;
      }
	wmove(win, y, x);
	data = s;
    } else if(ch == KEY_UP){
      if(y > 2){
	y--;
      }
      wmove(win, y, x);
      data = s;
    } else if(ch == KEY_LEFT){
      // wmove(win, y, x-1);
      if(x > 2){
	x--;
       }
       wmove(win, y, x);
       data = s;
    } else if(ch == KEY_RIGHT){
      	if(x < max_x - 1){
	x++;
	}
	wmove(win, y, x);
      data = s;
  } else {
    data = s + (char)ch;
  }
  move(y, x);
  return data;

}

string remove_char(string s){
  /* string data = "";
  for(unsigned int i = 0; i < (s.size() - 1); i++){
    data[i] += s[i];
  }
   data += '\b';
   return data;*/
  string data = s.substr(0,s.size()-1);
  data += '\b';
  return data;
}

int show_menu_window(int menu_h, int menu_w, int menu_y, int menu_x){
  WINDOW *menu_win;
  WINDOW *menu_subwin;
  MENU *main_menu;
  ITEM **items;
  initscr();
  const char * choices[] = {"Open","Save","Save As", "Exit"};
  int n_choices = ARRAY_SIZE(choices);
  items = (ITEM**)calloc(n_choices+1, sizeof(ITEM*));
  for(int i = 0; i < n_choices; i++){
    items[i] = new_item(choices[i], NULL);
  }
  items[n_choices] = (ITEM*)NULL;
  main_menu = new_menu((ITEM**)items);
  menu_win = newwin(menu_h, menu_w, menu_y, menu_x);
  menu_subwin = derwin(menu_win, (menu_h/2), (menu_w/2), (menu_h*0.4), (menu_w/3));
  keypad(menu_win, true);
  set_menu_win(main_menu, menu_win);
  set_menu_sub(main_menu, menu_subwin);
  set_menu_mark(main_menu, "* ");
  box(menu_win, '*', '*');
  wrefresh(menu_win);
  cbreak();
  post_menu(main_menu);
  wrefresh(menu_win);

  int key = 1;
  int choice = 0;
  bool selected = false;

  while(key != KEY_F(1) && !selected){
    key = wgetch(menu_win);
    switch(key){
    case KEY_DOWN:
      menu_driver(main_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(main_menu, REQ_UP_ITEM);
      break;
    case 10:
      //MORE TO DO HERE
      //menu_driver(main_menu,REQ_TOGGLE_ITEM);
      // cur_item= current_item(main_menu);
      selected = true;
      break;
    }
    wrefresh(menu_win);
  }

  if(selected){
    ITEM *cur_item = current_item(main_menu);
    const char * it = item_name((const ITEM*)cur_item);
    if(strcmp(it,"Open") == 0){
      choice = 1;
    } else if (strcmp(it,"Save") == 0){
      choice = 2;
    } else if (strcmp(it,"Save As") == 0){
      choice = 3;
    } else if (strcmp(it,"Exit") == 0){
      choice = 4;
    }
  }

  unpost_menu(main_menu);
  for(int i=0; i < n_choices; i++){
    free_item(items[i]);
  }

  wborder(menu_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(menu_win);
  //mvprintw(2, (menu_w/2)-4, NULL);
  wrefresh(menu_win);
  free_menu(main_menu);
  wrefresh(menu_win);
  delwin(menu_subwin);
  delwin(menu_win);
  // endwin();
  return choice; //USER EXITS MENU (F1 Pressed)
}


void file_to_screen(string data, WINDOW * win, int height, int width){
  int x = 1;
  int y = 1;
  int w = width - 1;
  for(unsigned int i = 0; i < data.size(); i++){
    char ch = data[i];
    mvwprintw(win, x, y, "%c", ch);
    if(y == (w - 1) || ch == '\n'){
      x++;
      y = 1;
      //mvwprintw(win, x, y,"%c", '\n');
    }
    y++;
  }
  //mvwprintw(win, x, y, "%c", '\0');
}

bool file_exists(const char * fileptr){
  return(fileptr != nullptr);
}

void save_file(string filename, string s){
  int fd = open(filename.c_str(), O_RDWR| O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  string data = s;
  int n = s.size();
  char * buffer = new char[n];
  buffer = (char*)data.c_str();
  if(write(fd, buffer, n) == -1){
    //error check here
  }
}

//can open existing file
//save file
//exit
//menu can be used
//typing - not backspace or random keys
