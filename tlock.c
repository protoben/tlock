#include <curses.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>

#ifdef USE_PAM
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#else
#include <shadow.h>
#endif

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
    start_color();

#ifdef COLORVALS
    if(can_change_color())
    { 
      num_colors = sizeof(colorvals) / sizeof(*colorvals);

      for(i = 0; i < num_colors && i < COLORS && i < COLOR_PAIRS; ++i)
      {
        init_color(i + 1, colorvals[i][0], colorvals[i][1], colorvals[i][2]);
        init_pair(i + 1, COLOR_BLACK, i + 1);
      }

      num_colors = i - 1;
    }
    else
#endif
    {
      num_colors = 6;

      init_pair(1, COLOR_BLACK, COLOR_RED);
      init_pair(2, COLOR_BLACK, COLOR_GREEN);
      init_pair(3, COLOR_BLACK, COLOR_YELLOW);
      init_pair(4, COLOR_BLACK, COLOR_BLUE);
      init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
      init_pair(6, COLOR_BLACK, COLOR_CYAN);
    }
  }

  return 0;
}

#ifdef USE_PAM
int tlock_conv(int num_msg, const struct pam_message **msg,
               struct pam_response **resp, void *appdata_ptr)
{
  char *userpw = (char*)appdata_ptr;

  if(num_msg != 1) return PAM_CONV_ERR;

  *resp = malloc(sizeof(struct pam_response));
  if(!*resp) return PAM_BUF_ERR;

  (*resp)->resp_retcode = 0;
  (*resp)->resp = malloc(strlen(userpw));
  if(!(*resp)->resp) { free(*resp); return PAM_BUF_ERR; }
  strcpy((*resp)->resp, userpw);

  return PAM_SUCCESS;
}

int checkpw(char *userpw)
{
  int err;
  char *user;
  struct pam_conv conv = {.conv = tlock_conv, .appdata_ptr = userpw};
  pam_handle_t *handlep = NULL;

  if(!(user = getenv("USER")))
    die(1, "Cannot retrieve username.\n");

  err = pam_start("tlock", user, &conv, &handlep);
  if(err != PAM_SUCCESS)
    die(1, "pam_start(): %s\n", pam_strerror(handlep, err));

  err = pam_authenticate(handlep, PAM_SILENT);
  switch(err)
  {
    case PAM_SUCCESS: pam_end(handlep, err); return 0;
    case PAM_AUTH_ERR: pam_end(handlep, err); return -1;
    default: die(1, "pam_authenticate(): %s\n", pam_strerror(handlep, err));
  }
}
#else
int checkpw(char *userpw)
{
  char *user;
  struct spwd *spentp;

  if(!(user = getenv("USER")))
    die(1, "Cannot retrieve username.\n");

  if(!(spentp = getspnam(user)))
    die(1, "Cannot read shadow file.\n");

  return strcmp(crypt(userpw, spentp->sp_pwdp), spentp->sp_pwdp);
}
#endif

void readpw()
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
        if(!checkpw(userpw))
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
  if(startup()) return EXIT_FAILURE;

  readpw();

  return EXIT_SUCCESS;
}
