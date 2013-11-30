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
#include <time.h>


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

void StartScreen(void);

void GameSetup(void);

void NewYear(void);


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
 * Ruler list.
 */

char *rulerList[COUNTRY_COUNT] =
{
    "MONTAIGNE", "ARTHUR", "MUNSTER", "KHOTAN", "FERDINAND", "HJODOLF",
};


/*
 * Weather list.
 */

char *weatherList[] =
{
    "POOR WEATHER. NO RAIN. LOCUSTS MIGRATE.",
    "EARLY FROSTS. ARID CONDITIONS.",
    "FLASH FLOODS. TOO MUCH RAIN.",
    "AVERAGE WEATHER. GOOD YEAR.",
    "FINE WEATHER. LONG SUMMER.",
    "FANTASTIC WEATHER ! GREAT YEAR !",
};


/*
 * Game state variables.
 *
 *   playerList             List of players.
 *   playerCount            Count of the number of human players.
 *   year                   Current year.
 *   weather                Weather for year, 1 based.
 *   done                   If true, game is done.
 */

Player playerList[COUNTRY_COUNT];
int playerCount = 0;
int year = 0;
int weather;
int done = FALSE;


/*------------------------------------------------------------------------------
 *
 * Macros.
 */

/*
 * Return the length of the array specified by aArray.
 *
 *   aArray                 Array to get length.
 */

#define ArraySize(aArray) (sizeof((aArray)) / sizeof((aArray)[0]))


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

    /* Seed the random number generator. */
   srand(time(NULL));

    /* Display the game start screen. */
    StartScreen();

    /* Set up game options. */
    GameSetup();

    /* Run game until it's done. */
    while (!done)
    {
        /* Start a new year. */
        NewYear();
    }

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

void GameSetup(void)
{
    char input[80];
    int i;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Get the number of players. */
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


/*
 * Start a new year.
 */

void NewYear(void)
{
    /* Update year. */
    year++;

    /* Update weather. */
    weather = (rand() % ArraySize(weatherList)) + 1;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display the year. */
    printw("YEAR %d\n\n", year);

    /* Display the weather. */
    printw("%s\n", weatherList[weather - 1]);

    /* Refresh screen. */
    refresh();

    /* Delay. */
    sleep(3);
}

