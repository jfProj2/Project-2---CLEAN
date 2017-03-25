//INCLUDES
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

#include "methods2.h"

using namespace std;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int main(const int argc, const char * argv[]);

void run_editor(const char * fileptr);

int show_menu_window(int menu_h, int menu_w, int menu_y, int menu_x);

string load_file(const char * fileptr);

bool close_file(int &fd);

string get_filename(const char * fileptr);

void file_to_screen(string data, WINDOW * win, int height, int width);

string add_char(char ch, string data);
string remove_char(string s);
void save_file(string filename, string data);
bool file_exists(const char * fileptr);
int show_error_window(int code, int err_h, int err_w, int err_y, int err_x);
int show_open_window(int open_h, int open_w, int open_y, int open_x);
int show_save_window(int save_h, int save_w, int save_y, int save_x);
//########
//MAIN
//########
int main(const int argc, const char * argv[]){
  //int errorNum = 0;
  if(argc == 1){

    run_editor("<N/A>");

  }
  else if(argc == 2){
    run_editor(argv[1]);
    //int FileDesc = IO::openFile(argv[1]);
    //If failure to open, create an error window inside the editor
    /**
    if(FileDesc < 0){
      errorNum = errno;
      printf("1730ed: [%s]: Error.\n", argv[1].c_str());
      printf("errno: %i - %s\n\n", errorNum, strerror(errorNum));
      return EXIT_FAILURE;
    }
    else{


    }
    **/
  }
  else{
    printf("ERROR: Too many arguments to ./1730ed\n");
    printf("Retry run command with proper syntax\n\n");
    printf("Ex: ./1730ed   or   ./1730ed filename.txt\n");
    return EXIT_FAILURE;
  }

  return 22;
}
