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
#include <string.h>
#include <time.h>


/*------------------------------------------------------------------------------
 *
 * Defs.
 */

/*
 * Game defs.
 *
 *   COUNTRY_COUNT          Count of the number of countries.
 *   TITLE_COUNT            Count of the number of ruler titles.
 */

#define COUNTRY_COUNT       6
#define TITLE_COUNT         4


/*------------------------------------------------------------------------------
 *
 * Structure defs.
 */

/*
 * This structure contains fields for a country record.
 *
 *   name                   Country name.
 *   rulerName              Default ruler name.
 *   currency               Currency.
 *   titleList              List of ruler titles.
 */

typedef struct
{
    char                    name[80];
    char                    rulerName[80];
    char                    currency[80];
    char                    titleList[TITLE_COUNT][80];
} Country;


/*
 * This structure contains fields for a player record.
 *
 *   name                   Player name.
 *   country                Player country.
 *   level                  Player level.
 *   title                  Player title.
 *   land                   Land in acres.
 *   grain                  Grain reserves in bushels.
 *   treasury               Currency treasury.
 *   serfCount              Count of number of serfs.
 *   soldierCount           Count of number of soldiers.
 *   nobleCount             Count of number of nobles.
 *   merchantCount          Count of number of merchants.
 *   armyEfficiency         Efficiency of army.
 *   customsTax             Customs tax.
 *   salesTax               Sales tax.
 *   incomeTax              Income tax.
 *   foundryCount           Count of number of foundries.
 */

typedef struct
{
    char                    name[80];
    Country                *country;
    int                     level;
    char                    title[80];
    int                     land;
    int                     grain;
    int                     treasury;
    int                     serfCount;
    int                     soldierCount;
    int                     nobleCount;
    int                     merchantCount;
    int                     armyEfficiency;
    int                     customsTax;
    int                     salesTax;
    int                     incomeTax;
    int                     foundryCount;
} Player;


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

void StartScreen(void);

void GameSetupScreen(void);

void NewYearScreen(void);

void FoodScreen(Player *aPlayer);


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
            /* Show food screen. */
            FoodScreen(&(playerList[i]));
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

void GameSetupScreen(void)
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
        playerCount = atoi(input);
    } while ((playerCount < 1) || (playerCount > 6));

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

        /* Initialize the player's country. */
        player->country = country;

        /* Initialize the player's level and title. */
        player->level = 0;
        snprintf(player->title,
                 sizeof(player->title),
                 "%s",
                 country->titleList[player->level]);

        /* Initialize the player's state. */
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
    }
}


/*
 * Start a new year.
 */

void NewYearScreen(void)
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


/*
 * Feed people and army for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void FoodScreen(Player *aPlayer)
{
    Country *country;
    int      grainHarvest;
    int      peopleRequire;
    int      armyRequire;;
    int      ratPct;

    /* Get the player country. */
    country = aPlayer->country;

    /* Determine what percentage of grain the rats ate. */
    ratPct = (rand() % 30) + 1;
    aPlayer->grain -= (aPlayer->grain * ratPct) / 100;

    /* Determine the grain harvest. */
    grainHarvest =   (weather * aPlayer->land * 0.72)
                   + ((rand() % 500) + 1)
                   - (aPlayer->foundryCount * 500);
    if (grainHarvest < 0)
        grainHarvest = 0;
    aPlayer->grain += grainHarvest;

    /* Determine the amount of grain required by the people. */
    peopleRequire = 5 * (  aPlayer->serfCount
                         + aPlayer->merchantCount
                         + (3 * aPlayer->nobleCount));

    /* Determine the amount of grain required by the army. */
    armyRequire = 8 * aPlayer->soldierCount;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display the ruler and country. */
    printw("%s %s OF %s\n", aPlayer->title, aPlayer->name, country->name);

    /* Display how much grain the rats ate. */
    printw("RATS ATE %d %% OF THE GRAIN RESERVE\n", ratPct);

    /* Display grain levels and treasury. */
    printw("GRAIN     GRAIN     PEOPLE     ARMY       ROYAL\n");
    printw("HARVEST   RESERVE   REQUIRE    REQUIRES   TREASURY\n");
    printw(" %6d    %6d    %6d     %6d     %6d\n",
           grainHarvest,
           aPlayer->grain,
           peopleRequire,
           armyRequire,
           aPlayer->treasury);
    printw("BUSHELS   BUSHELS   BUSHELS    BUSHELS    %s\n", country->currency);

    /* Display grain for sale. */
    printw("------GRAIN FOR SALE:\n");
    printw("                COUNTRY         BUSHELS         PRICE\n");

    /* Refresh screen. */
    refresh();

    /* Delay. */
    sleep(3);
}

