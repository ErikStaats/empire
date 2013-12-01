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

/* System includes. */
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Local includes. */
#include "empire.h"
#include "grain.h"


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

static void StartScreen(void);

static void GameSetupScreen(void);

static void NewYearScreen(void);


/*------------------------------------------------------------------------------
 *
 * Globals.
 */

/*
 * Country  list.
 */

Country countryList[COUNTRY_COUNT] =
{
    /* AUVEYRON. */
    {
        .name = "AUVEYRON",
        .rulerName = "MONTAIGNE",
        .currency = "FRANCS",
        .titleList = { "CHEVALIER", "PRINCE", "ROI", "EMPEREUR", },
    },

    /* BRITTANY. */
    {
        .name = "BRITTANY",
        .rulerName = "ARTHUR",
        .currency = "FRANCS",
        .titleList = { "SIR", "PRINCE", "KING", "EMPEROR", },
    },

    /* BAVARIA. */
    {
        .name = "BAVARIA",
        .rulerName = "MUNSTER",
        .currency = "MARKS",
        .titleList = { "RITTER", "PRINZ", "KONIG", "KAISER", },
    },

    /* QUATARA. */
    {
        .name = "QUATARA",
        .rulerName = "KHOTAN",
        .currency = "DINARS",
        .titleList = { "HASID", "CALIPH", "SHEIK", "SHAH", },
    },

    /* BARCELONA. */
    {
        .name = "BARCELONA",
        .rulerName = "FERDINAND",
        .currency = "PESETA",
        .titleList = { "CABALLERO", "PRINCIPE", "REY", "EMPERADORE", },
    },

    /* SVEALAND. */
    {
        .name = "SVEALAND",
        .rulerName = "HJODOLF",
        .currency = "KRONA",
        .titleList = { "RIDDARE", "PRINS", "KUNG", "KEJSARE", },
    },
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
 *   barbarianLand          Amount of barbarian land in acres.
 *   done                   If true, game is done.
 */

Player playerList[COUNTRY_COUNT];
int playerCount = 0;
int year = 0;
int weather;
int barbarianLand = 6000;
int done = FALSE;


/*------------------------------------------------------------------------------
 *
 * Main entry point.
 */

int main(argc, argv)
{
    Player *player;
    int     i;

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
    GameSetupScreen();

    /* Run game until it's done. */
    while (!done)
    {
        /* Start a new year. */
        NewYearScreen();

        /* Go through each human player. */
        for (i = 0; i < playerCount; i++)
        {
            /* Show grain screen. */
            GrainScreen(&(playerList[i]));
        }
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

static void StartScreen(void)
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
    sleep(DELAY_TIME);
}


/*
 * Set up the game options.
 */

static void GameSetupScreen(void)
{
    Country *country;
    Player  *player;
    char     input[80];
    int      i, j;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Get the number of players. */
    do
    {
        printw("HOW MANY PEOPLE ARE PLAYING? ");
        refresh();
        getnstr(input, sizeof(input));
        playerCount = strtol(input, NULL, 0);
    } while (playerCount > 6);

    /* Get the player names. */
    for (i = 0; i < playerCount; i++)
    {
        /* Get the country and player records. */
        country = &(countryList[i]);
        player = &(playerList[i]);

        /* Get the country's ruler's name. */
        printw("WHO IS THE RULER OF %s? ", country->name);
        getnstr(player->name, sizeof(player->name));
        for (j = 0; j < strlen(player->name); j++)
        {
            player->name[j] = toupper(player->name[j]);
        }
    }
    for (; i < COUNTRY_COUNT; i++)
    {
        /* Get the country and player records. */
        country = &(countryList[i]);
        player = &(playerList[i]);

        /* Get the country's ruler's name. */
        snprintf(player->name, sizeof(player->name), "%s", country->rulerName);
    }

    /* Initialize the player records. */
    for (i = 0; i < COUNTRY_COUNT; i++)
    {
        /* Get the country and player records. */
        country = &(countryList[i]);
        player = &(playerList[i]);

        /* Initialize the player's number. */
        player->number = i + 1;

        /* Initialize the player's country. */
        player->country = country;

        /* Initialize the player's level and title. */
        player->level = 0;
        snprintf(player->title,
                 sizeof(player->title),
                 "%s",
                 country->titleList[player->level]);

        /* Initialize the player's state. */
        player->defeated = FALSE;
        player->land = 10000;
        player->grain = 15000 + ((rand() % 10000) + 1);
        player->treasury = 1000;
        player->serfCount = 2000;
        player->soldierCount = 20;
        player->nobleCount = 1;
        player->merchantCount = 25;
        player->armyEfficiency = 15;
        player->customsTax = 20;
        player->salesTax = 5;
        player->incomeTax = 35;
        player->foundryCount = 0;
        player->grainForSale = 0;
        player->grainPrice = 0.0;
    }
}


/*
 * Start a new year.
 */

static void NewYearScreen(void)
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
    sleep(DELAY_TIME);
}

