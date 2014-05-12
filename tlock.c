#include <curses.h>
#include <locale.h>
#include <shadow.h> //FIXME: use pam
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>

#define KEY_ESCAPE 27

#ifdef COLORVALS
const short colorvals[][3] = COLORVALS;
#endif

int num_colors;

void die(int eval, const char *msg, ...)
{
  va_list ap;

  endwin();
  va_start(ap, msg);
  vfprintf(stderr, msg, ap);
  va_end(ap);

  exit(eval);
}

int startup(void)
{
  int i;

  srand(time(0));
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  noqiflush();
  curs_set(0);
  atexit((void(*)(void))endwin);

  if(has_colors())
  {
#ifdef COLORVALS
    if(can_change_color())
    { 
      num_colors = sizeof(colorvals);

      for(i = 0; i < num_colors && i < COLORS && i < COLOR_PAIRS; ++i)
      {
        init_color(i, colorvals[i][0], colorvals[i][1], colorvals[i][2]);
        init_pair(i + 1, COLOR_BLACK, i);
      }
    }
    else
#endif
    {
      start_color();

      init_pair(1, COLOR_BLACK, COLOR_RED);
      init_pair(2, COLOR_BLACK, COLOR_GREEN);
      init_pair(3, COLOR_BLACK, COLOR_YELLOW);
      init_pair(4, COLOR_BLACK, COLOR_BLUE);
      init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
      init_pair(6, COLOR_BLACK, COLOR_CYAN);

      num_colors = 6;
    }
  }

  return 0;
}

char *getshadowpw()
{
  char *user;
  struct spwd *spentp;

  if(!(user = getenv("USER")))
    die(1, "Cannot retrieve username.\n");

  if(!(spentp = getspnam(user)))
    die(1, "Cannot read shadow file.\n");

  return spentp->sp_pwdp;
}

void readpw(char *shadowpw)
{
  char userpw[256];
  int c, r = 1, i = 0;
  WINDOW *winp;

  winp = newwin(0,0,0,0);

  while((c = getch()) != ERR)
    switch(c)
    {
      case '\n':
        userpw[i] = '\0';
        if(!strcmp(crypt(userpw, shadowpw), shadowpw))
          return;
      case KEY_ESCAPE:
        i = 0;
        wbkgd(winp, ' ' | COLOR_PAIR(0));
        wrefresh(winp);
        break;
      case KEY_BACKSPACE:
        if(i > 0) --i;
        break;
      default:
        r = (r + (rand() % (num_colors - 1))) % num_colors + 1;
        wbkgd(winp, ' ' | COLOR_PAIR(r));
        wrefresh(winp);

       userpw[i] = c;
        if(i < 254) ++i;
        else i = 0;
        break;
    }

    delwin(winp);
}

int main(int argc, char **argv)
{
  char *shadowpw;

  if(startup()) return EXIT_FAILURE;

  shadowpw = getshadowpw();
  readpw(shadowpw);

  return EXIT_SUCCESS;
}
