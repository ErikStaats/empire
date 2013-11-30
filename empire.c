#include <ncurses.h>

void StartScreen(void);

int main(argc, argv)
{
    /* Initialize the screen. */
    initscr();

    /* Display the game start screen. */
    StartScreen();

    /* End nCurses. */
    endwin();

    return 0;
}

void StartScreen(void)
{
    /* Display splash screen. */
    move(0, 20);
    printw("E M P I R E\n");
    move(7, 0);
    printw("(ALWAYS HIT <ENTER> TO CONTINUE)");
    refresh();
    sleep(4);
}

