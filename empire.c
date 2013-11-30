/*------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 *
 * TRS-80 Empire game source file.
 *
 *------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *
 * Includes.
 */

#include <ncurses.h>


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

void StartScreen(void);

void GameSetup(void);


/*------------------------------------------------------------------------------
 *
 * Defs.
 */

/*
 * Game defs.
 *
 *   COUNTRY_COUNT          Count of the number of countries.
 */

#define COUNTRY_COUNT       6


/*------------------------------------------------------------------------------
 *
 * Structure defs.
 */

/*
 * This structure contains fields for a player record.
 *
 *   name                   Player name.
 */

typedef struct
{
    char                    name[80];
} Player;


/*------------------------------------------------------------------------------
 *
 * Globals.
 */

/*
 * Country  list.
 */

char *countryList[COUNTRY_COUNT] =
{
    "AUVEYRON", "BRITTANY", "BAVARIA", "QUATARA", "BARCELONA", "SVEALAND",
};


/*
 * Country  list.
 */

char *rulerList[COUNTRY_COUNT] =
{
    "MONTAIGNE", "ARTHUR", "MUNSTER", "KHOTAN", "FERDINAND", "HJODOLF",
};


/*
 * Game state variables.
 *
 *   playerList             List of players.
 *   playerCount            Count of the number of human players.
 */

Player playerList[COUNTRY_COUNT];
int playerCount = 0;


/*------------------------------------------------------------------------------
 *
 * Main entry point.
 */

int main(argc, argv)
{
    /* Initialize the screen. */
    initscr();

    /* Resize the window to 16x64 and set it to scroll. */
    wresize(stdscr, 16, 64);
    scrollok(stdscr, TRUE);

    /* Display the game start screen. */
    StartScreen();

    /* Set up game options. */
    GameSetup();

    /* End nCurses. */
    endwin();

    return 0;
}


/*------------------------------------------------------------------------------
 *
 * Internal functions.
 */

/*
 * Display the game start splash screen.
 */

void StartScreen(void)
{
    /* Clear screen. */
    clear();

    /* Display splash screen. */
    move(0, 20);
    printw("E M P I R E\n");
    move(7, 0);
    printw("(ALWAYS HIT <ENTER> TO CONTINUE)");
    refresh();

    /* Delay. */
    sleep(3);
}


/*
 * Set up the game options.
 */

void GameSetup()
{
    char input[80];
    int i;

    /* Clear screen. */
    clear();

    /* Get the number of players. */
    move(0, 0);
    do
    {
        printw("HOW MANY PEOPLE ARE PLAYING? ");
        refresh();
        getnstr(input, sizeof(input));
        playerCount = atoi(input);
    } while ((playerCount < 1) || (playerCount > 6));

    /* Get the player names. */
    for (i = 0; i < playerCount; i++)
    {
        printw("WHO IS THE RULER OF %s? ", countryList[i]);
        getnstr(playerList[i].name, sizeof(playerList[i]));
    }
    for (; i < COUNTRY_COUNT; i++)
    {
        snprintf(playerList[i].name,
                 sizeof(playerList[i].name),
                 "%s",
                 rulerList[i]);
    }
}

