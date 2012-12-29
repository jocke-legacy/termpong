#include <ncurses.h>
#include <time.h>

typedef struct {
   double x, y;
} Pos;

typedef struct {
   double yspeed;
   double xspeed;
   Pos pos;
} Ball;

typedef struct {
   int paddle_size;
   int row, col;
   Pos p1, p2;
   Ball b;
} Game;

int bound_in_box(int x1, int y1, int x2, int y2, int bx, int by);
Game process(Game g);
void render(Game g);
Game new_game(void);

int bound_in_box(int x1, int y1, int x2, int y2, int bx, int by) {
   if (bx >= x1 && bx <= x2) {
      if (by >= y1 && by <= y2) {
         return 1;
      }
   }

   return 0;
}

Game process(Game g) {
   if (bound_in_box(g.p1.x, g.p1.y, g.p1.x, g.p1.y + g.paddle_size, g.b.pos.x, g.b.pos.y)) {
      g.b.xspeed *= -1;
      g.b.yspeed = ((g.b.yspeed < 0 ? -1 : 1) * g.paddle_size/2 + g.b.pos.y - g.p1.y)/g.paddle_size*2;
   }
   else if (bound_in_box(g.p2.x, g.p2.y, g.p2.x, g.p2.y + g.paddle_size, g.b.pos.x, g.b.pos.y)) {
      g.b.xspeed *= -1;
      g.b.yspeed = ((g.b.yspeed < 0 ? -1 : 1) * g.paddle_size/2 + g.b.pos.y - g.p2.y)/g.paddle_size*2;
   }

   if (bound_in_box(0, 0, g.col, 0, g.b.pos.x, g.b.pos.y) ||
       bound_in_box(0, g.row, g.col, g.row, g.b.pos.x, g.b.pos.y)) {
      g.b.yspeed *= -1;
   }

   if (g.b.pos.y <= g.p2.y && g.p2.y > 1)
      g.p2.y -= 1;
   else if (g.b.pos.y >= g.p2.y + g.paddle_size && g.p2.y + g.paddle_size < g.row)
      g.p2.y += 1;
   

   g.b.pos.x += g.b.xspeed;
   g.b.pos.y += g.b.yspeed;

   return g;
}
   
void render(Game g) {
   int i;

   clear();

   /* Draw ball */
   mvaddch(g.b.pos.y, g.b.pos.x, 'o');

   /* Draw players */
   for (i = 0; i < g.paddle_size; i++) {
      mvaddch(g.p1.y + i, g.p1.x, '|');
      mvaddch(g.p2.y + i, g.p2.x, '|');
   }

   /* Draw walls */
   for (i = 0; i <= g.col; i++) {
      mvaddch(0, i, '-');
      mvaddch(g.row, i, '-');
   }
}

Game new_game(void) {
   Game g;

   g.row         = 24;
   g.col         = 80;
   g.b.pos.x     = g.col/2;
   g.b.pos.y     = g.row/2;
   g.b.yspeed    = 0.5f;
   g.b.xspeed    = 1;
   g.paddle_size = 4;
   g.p1.x        = 1;
   g.p1.y        = g.row/2 - g.paddle_size/2;
   g.p2.x        = g.col - 1;
   g.p2.y        = g.row/2 - g.paddle_size/2;

   return g;
}

int main(void) {
   int state;
   int ch;
   Game g;

   struct timespec t;
   t.tv_sec  = 0;
   t.tv_nsec = 50000000;

   initscr();
   timeout(0);
   cbreak();
   noecho();
   keypad(stdscr, TRUE);

   g = new_game();

   state = 1;
   while (state) {
      ch = getch();
      if (ch == KEY_F(12))
         g = new_game();

      if (ch == KEY_UP && g.p1.y > 1)
         g.p1.y -= 1;
      if (ch == KEY_DOWN && g.p1.y + g.paddle_size < g.row)
         g.p1.y += 1;

      g = process(g);
      render(g);

      refresh();
      nanosleep(&t, NULL);
   }

   endwin();

   return 0;
}

