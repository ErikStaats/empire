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
#include <stdlib.h>
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
 *   DELAY_TIME             Time in seconds to delay.
 */

#define COUNTRY_COUNT       6
#define TITLE_COUNT         4
#define DELAY_TIME          3


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
 *   number                 Player number.
 *   country                Player country.
 *   level                  Player level.
 *   title                  Player title.
 *   defeated               If true, player has been defeated.
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
 *   grainForSale           Bushels of grain for sale.
 *   grainPrice             Grain price.
 *   ratPct                 Percent of grain eaten by rats.
 *   grainHarvest           Grain harvest for year.
 */

typedef struct
{
    char                    name[80];
    int                     number;
    Country                *country;
    int                     level;
    char                    title[80];
    bool                    defeated;
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
    int                     grainForSale;
    float                   grainPrice;
    int                     ratPct;
    int                     grainHarvest;
} Player;


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

void StartScreen(void);

void GameSetupScreen(void);

void NewYearScreen(void);

void FoodScreen(Player *aPlayer);

void DrawFoodScreen(Player *aPlayer);

void TradeGrainAndLand(Player *aPlayer);

void BuyGrain(Player *aPlayer);

void SellGrain(Player *aPlayer);

void SellLand(Player *aPlayer);


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
    sleep(DELAY_TIME);
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
        playerCount = strtol(input, NULL, 0);
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
    sleep(DELAY_TIME);
}


/*
 * Feed people and army for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void FoodScreen(Player *aPlayer)
{
    /* Determine what percentage of grain the rats ate. */
    aPlayer->ratPct = (rand() % 30) + 1;
    aPlayer->grain -= (aPlayer->grain * aPlayer->ratPct) / 100;

    /* Determine the grain harvest. */
    aPlayer->grainHarvest =   (weather * aPlayer->land * 0.72)
                            + ((rand() % 500) + 1)
                            - (aPlayer->foundryCount * 500);
    if (aPlayer->grainHarvest < 0)
        aPlayer->grainHarvest = 0;
    aPlayer->grain += aPlayer->grainHarvest;

    /* Draw the food screen. */
    DrawFoodScreen(aPlayer);

    /* Trade grain and land. */
    TradeGrainAndLand(aPlayer);
}


/*
 * Draw the food screen for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void DrawFoodScreen(Player *aPlayer)
{
    Player  *player;
    Country *country;
    int      peopleRequire;
    int      armyRequire;;
    bool     anyGrainForSale;
    int      i;

    /* Get the player country. */
    country = aPlayer->country;

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
    printw("RATS ATE %d %% OF THE GRAIN RESERVE\n", aPlayer->ratPct);

    /* Display grain levels and treasury. */
    printw("GRAIN     GRAIN     PEOPLE     ARMY       ROYAL\n");
    printw("HARVEST   RESERVE   REQUIRE    REQUIRES   TREASURY\n");
    printw(" %6d    %6d    %6d     %6d     %6d\n",
           aPlayer->grainHarvest,
           aPlayer->grain,
           peopleRequire,
           armyRequire,
           aPlayer->treasury);
    printw("BUSHELS   BUSHELS   BUSHELS    BUSHELS    %s\n", country->currency);

    /* Display grain for sale. */
    printw("------GRAIN FOR SALE:\n");
    printw("                COUNTRY         BUSHELS         PRICE\n");
    anyGrainForSale = FALSE;
    for (i = 0; i < COUNTRY_COUNT; i++)
    {
        player = &(playerList[i]);
        if (player->grainForSale > 0)
        {
            anyGrainForSale = TRUE;
            printw(" %d              %-16s %-14d %5.2f\n",
                   player->number,
                   player->country->name,
                   player->grainForSale,
                   player->grainPrice);
        }
    }

    /* Display if no grain is for sale. */
    if (!anyGrainForSale)
    {
        printw("\n\nNO GRAIN FOR SALE . . .\n\n");
    }

    /* Refresh screen. */
    refresh();
}


/*
 * Trade grain and land for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 * 
 */

void TradeGrainAndLand(Player *aPlayer)
{
    char input[80];
    bool doneTrading;

    /* Trade grain and land. */
    doneTrading = FALSE;
    while (!doneTrading)
    {
        /* Draw food screen. */
        DrawFoodScreen(aPlayer);

        /* Display options. */
        move(14, 0);
        clrtoeol();
        printw("1) BUY GRAIN  2) SELL GRAIN  3) SELL LAND? ");
        getnstr(input, sizeof(input));

        /* Parse command. */
        switch (strtol(input, NULL, 0))
        {
            case 0 :
                doneTrading = TRUE;
                break;

            case 1 :
                BuyGrain(aPlayer);
                break;

            case 2 :
                SellGrain(aPlayer);
                break;

            case 3 :
                SellLand(aPlayer);
                break;

            default :
                break;
        }
    }
}


/*
 * Buy grain for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void BuyGrain(Player *aPlayer)
{
    Player *seller;
    int     sellerIndex;
    int     grain;
    char    input[80];
    bool    validSeller;
    bool    validGrain;

    /* Get the seller from which to buy grain. */
    validSeller = FALSE;
    do
    {
        move(14, 0);
        clrtoeol();
        printw("FROM WHICH COUNTRY  (GIVE #)? ");
        getnstr(input, sizeof(input));
        sellerIndex = strtol(input, NULL, 0);
        if ((sellerIndex >= 0) & (sellerIndex <= COUNTRY_COUNT))
        {
            validSeller = TRUE;
        }
    } while (!validSeller);
    if (sellerIndex > 0)
        seller = &(playerList[sellerIndex - 1]);
    else
        seller = NULL;

    /* Validate that the seller has grain for sale. */
    if ((seller == NULL) || (seller->defeated) || (seller->grainForSale == 0))
    {
        printw("THAT COUNTRY HAS NONE FOR SALE!");
        refresh();
        sleep(DELAY_TIME);
        return;
    }

    /* Cannot buy grain from self. */
    if (seller == aPlayer)
    {
        printw("YOU CANNOT BUY GRAIN THAT YOU HAVE PUT ONTO THE MARKET!");
        refresh();
        sleep(DELAY_TIME);
        return;
    }

    /* Get the amount of grain to buy. */
    validGrain = FALSE;
    do
    {
        move(14, 0);
        clrtoeol();
        printw("HOW MANY BUSHELS? ");
        getnstr(input, sizeof(input));
        grain = strtol(input, NULL, 0);
        if (grain > seller->grainForSale)
        {
            printw("YOU CAN'T BUY MORE GRAIN THEN THEY ARE SELLING!");
            refresh();
            sleep(DELAY_TIME);
            move(14, 0);
            clrtoeol();
            move(15, 0);
            clrtoeol();
            refresh();
        }
        else
        {
            validGrain = TRUE;
        }
    } while (!validGrain);
}


/*
 * Sell grain for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 * 
 */

void SellGrain(Player *aPlayer)
{
    int   grainToSell;
    float grainPrice;
    char  input[80];
    bool  validGrainToSell;
    bool  validGrainPrice;

    /* Get the amount of grain to sell. */
    validGrainToSell = FALSE;
    do
    {
        move(14, 0);
        clrtoeol();
        printw("HOW MANY BUSHELS DO YOU WISH TO SELL? ");
        getnstr(input, sizeof(input));
        grainToSell = strtol(input, NULL, 0);
        if (grainToSell <= aPlayer->grain)
        {
            validGrainToSell = TRUE;
        }
        else
        {
            move(14, 0);
            clrtoeol();
            printw("%s %s, PLEASE THINK AGAIN\n",
                   aPlayer->title,
                   aPlayer->name);
            printw("YOU ONLY HAVE %d BUSHELS.", aPlayer->grain);
            refresh();
            sleep(DELAY_TIME);
            move(14, 0);
            clrtoeol();
            move(15, 0);
            clrtoeol();
            refresh();
        }
    } while (!validGrainToSell);

    /* Get the price at which to sell grain. */
    validGrainPrice = FALSE;
    do
    {
        move(14, 0);
        clrtoeol();
        printw("WHAT WILL BE THE PRICE PER BUSHEL? ");
        getnstr(input, sizeof(input));
        grainPrice = strtod(input, NULL);
        if (grainPrice <= 15.0)
        {
            validGrainPrice = TRUE;
        }
        else
        {
            printw("BE REASONABLE . . .EVEN GOLD COSTS LESS THAN THAT!");
            refresh();
            sleep(DELAY_TIME);
            move(14, 0);
            clrtoeol();
            move(15, 0);
            clrtoeol();
            refresh();
        }
    } while (!validGrainPrice);

    /* Update the total grain for sale and price. */
    aPlayer->grainPrice =
          (  (aPlayer->grainPrice * ((float) aPlayer->grainForSale))
           + (grainPrice * ((float) grainToSell)))
        / ((float) (aPlayer->grainForSale + grainToSell));
    aPlayer->grainForSale += grainToSell;
    aPlayer->grain -= grainToSell;
}


/*
 * Sell land for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 * 
 */

void SellLand(Player *aPlayer)
{
    int   landToSell;
    char  input[80];
    bool  validLandToSell;

    /* Sell land. */
    validLandToSell = FALSE;
    do
    {
        /* Display the price per acre. */
        move(14, 0);
        clrtoeol();
        printw("THE BARBARIANS WILL GIVE YOU 2 %s PER ACRE",
               aPlayer->country->currency);
        refresh();
        sleep(DELAY_TIME);

        /* Get the number of acres to sell. */
        move(14, 0);
        clrtoeol();
        printw("HOW MANY ACRES WILL YOU SELL THEM? ");
        getnstr(input, sizeof(input));
        landToSell = strtol(input, NULL, 0);
        if (landToSell < 0)
        {
            validLandToSell = FALSE;
        }
        else if (((float) landToSell) <= (0.95 * ((float) aPlayer->land)))
        {
            validLandToSell = TRUE;
        }
        else
        {
            printw("YOU MUST KEEP SOME LAND FOR THE ROYAL PALACE!");
            refresh();
            sleep(DELAY_TIME);
            move(14, 0);
            clrtoeol();
            move(15, 0);
            clrtoeol();
            refresh();
        }
    } while (!validLandToSell);

    /* Update land and treasury. */
    aPlayer->treasury += 2 * landToSell;
    aPlayer->land -= landToSell;
}

