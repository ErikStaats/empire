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
#include "population.h"


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

static void StartScreen(void);

static void GameSetupScreen(void);

static void NewYearScreen(void);

static void SummaryScreen(void);

static void PlayHuman(Player *aPlayer);

static void PlayCPU(Player *aPlayer);


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
 *   gameOver               If true, game is over.
 */

Player playerList[COUNTRY_COUNT];
int playerCount = 0;
int year = 0;
int weather;
int barbarianLand = 6000;
int gameOver = FALSE;


/*------------------------------------------------------------------------------
 *
 * Main entry point.
 */

int main(argc, argv)
{
    Player *player;
    int     i, j;

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

    /* Run game until it's over. */
    while (!gameOver)
    {
        /* Start a new year. */
        NewYearScreen();

        /* Go through each player. */
        for (i = 0; i < COUNTRY_COUNT; i++)
        {
            /* Get player. */
            player = &(playerList[i]);

            /* Skip dead players. */
            if (player->dead)
                continue;

            /* Play as human or CPU. */
            if (player->human)
                PlayHuman(player);
            else
                PlayCPU(player);

            /* Stop if game over. */
            if (gameOver)
                break;
        }

        /* Stop if game over. */
        if (gameOver)
            break;

        /* Display summary. */
        SummaryScreen();
    }

    /* End nCurses. */
    endwin();

    return 0;
}


/*------------------------------------------------------------------------------
 *
 * External functions.
 */

/*
 *   Return a random integer value between 1 and the value specified by range,
 * inclusive (i.e., [1, range]).  If range is 0, return 0.
 *
 *   range                  Range of random value.
 */

int RandRange(int range)
{
    return (range > 0) ? (rand() % range) + 1 : 0;
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
    } while (playerCount > COUNTRY_COUNT);

    /* Get the player names. */
    for (i = 0; i < playerCount; i++)
    {
        /* Get the country and player records. */
        country = &(countryList[i]);
        player = &(playerList[i]);

        /* Player is human. */
        player->human = TRUE;

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

        /* Player is not human. */
        player->human = FALSE;

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
        player->dead = FALSE;
        player->land = 10000;
        player->grain = 15000 + RandRange(10000);
        player->treasury = 1000;
        player->serfCount = 2000;
        player->soldierCount = 20;
        player->nobleCount = 1;
        player->merchantCount = 25;
        player->armyEfficiency = 15;
        player->customsTax = 20;
        player->salesTax = 5;
        player->incomeTax = 35;
        player->marketplaceCount = 0;
        player->grainMillCount = 0;
        player->foundryCount = 0;
        player->shipyardCount = 0;
        player->palaceCount = 0;
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
    weather = RandRange(ArraySize(weatherList));

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


/*
 * Display a summary of all players.
 */

static void SummaryScreen(void)
{
    char     input[80];
    Player  *player;
    Country *country;
    int      i;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display summary. */
    printw("SUMMARY\n");
    printw("NOBLES   SOLDIERS   MERCHANTS   SERFS   LAND    PALACE\n\n");
    for (i = 0; i < COUNTRY_COUNT; i++)
    {
        /* Get the country and player records. */
        country = &(countryList[i]);
        player = &(playerList[i]);

        /* Skip dead players. */
        if (player->dead)
            continue;

        /* Display player summary. */
        printw("%s %s OF %s\n", player->title, player->name, country->name);
        printw(" %3d       %5d       %5d    %6d   %5d  %3d%%\n",
               player->nobleCount,
               player->soldierCount,
               player->merchantCount,
               player->serfCount,
               player->land,
               player->palaceCount);
    }

    /* Wait for player. */
    printw("<ENTER>? ");
    getnstr(input, sizeof(input));
}


/*
 * Play the human player specified by aPlayer.
 *
 *   aPlayer                Human player.
 */

static void PlayHuman(Player *aPlayer)
{
    int i;

    /* Show grain screen. */
    GrainScreen(aPlayer);

    /* Show population screen. */
    PopulationScreen(aPlayer);

    /* If all human players have died, end game. */
    for (i = 0; i < playerCount; i++)
    {
        if (!playerList[i].dead)
            break;
    }
    if (i == playerCount)
    {
        gameOver = TRUE;
        return;
    }

    /* Show investments screen. */
    InvestmentsScreen(aPlayer);

    /* Show attack screen. */
    AttackScreen(aPlayer);
}


/*
 * Play the CPU player specified by aPlayer.
 *
 *   aPlayer                CPU player.
 */

static void PlayCPU(Player *aPlayer)
{
    /* Reset screen. */
    clear();
    move(0, 0);

    /* Announce player's turn. */
    printw("ONE MOMENT -- %s %s'S TURN . . .", aPlayer->title, aPlayer->name);
    refresh();
    sleep(DELAY_TIME);
}

