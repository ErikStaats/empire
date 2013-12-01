/*------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 *
 * TRS-80 Empire game grain screen source file.
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

/* Local includes. */
#include "empire.h"


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

static void DrawGrainScreen(Player *aPlayer);

static void TradeGrainAndLand(Player *aPlayer);

static void BuyGrain(Player *aPlayer);

static void SellGrain(Player *aPlayer);

static void SellLand(Player *aPlayer);


/*------------------------------------------------------------------------------
 *
 * External grain screen functions.
 */

/*
 * Manage grain for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void GrainScreen(Player *aPlayer)
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

    /* Draw the grain  screen. */
    DrawGrainScreen(aPlayer);

    /* Trade grain and land. */
    TradeGrainAndLand(aPlayer);
}


/*------------------------------------------------------------------------------
 *
 * Internal grain screen functions.
 */

/*
 * Draw the grain  screen for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

static void DrawGrainScreen(Player *aPlayer)
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

static void TradeGrainAndLand(Player *aPlayer)
{
    char input[80];
    bool doneTrading;

    /* Trade grain and land. */
    doneTrading = FALSE;
    while (!doneTrading)
    {
        /* Draw grain  screen. */
        DrawGrainScreen(aPlayer);

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

static void BuyGrain(Player *aPlayer)
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

static void SellGrain(Player *aPlayer)
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

static void SellLand(Player *aPlayer)
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

