/*------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 *
 * TRS-80 Empire game investments screen source file.
 *
 *------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *
 * Includes.
 */

/* System includes. */
#include <math.h>
#include <ncurses.h>
#include <string.h>

/* Local includes. */
#include "empire.h"


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

/*
 * Investment prototypes.
 */

static void DrawInvestmentsScreen(Player *aPlayer);

static void DisplayInvestments(Player *aPlayer);

static void ComputeRevenues(Player *aPlayer);


/*
 * Tax prototypes.
 */

static void SetTaxes(Player *aPlayer);

static void SetCustomsTax(Player *aPlayer);

static void SetSalesTax(Player *aPlayer);

static void SetIncomeTax(Player *aPlayer);

static void DisplayTaxRevenues(Player *aPlayer);


/*
 * Buy investment prototypes.
 */

static void BuyInvestments(Player *aPlayer);

static void BuyMarketplaces(Player *aPlayer);

static void BuyGrainMills(Player *aPlayer);

static void BuyFoundries(Player *aPlayer);

static void BuyShipyards(Player *aPlayer);

static void BuySoldiers(Player *aPlayer);

static void BuyPalaces(Player *aPlayer);

static bool ValidateInvestment(Player *aPlayer,
                               int investment,
                               int investmentCount);


/*------------------------------------------------------------------------------
 *
 * Globals.
 */

/*
 * Investment list.
 */

#define INVESTMENT_MARKETPLACE 1
#define INVESTMENT_GRAIN_MILL  2
#define INVESTMENT_FOUNDRY     3
#define INVESTMENT_SHIPYARD    4
#define INVESTMENT_SOLDIER     5
#define INVESTMENT_PALACE      6


/*
 * Investment cost list.
 */

int investmentCost[] =
{
    1000, 2000, 7000, 8000, 8, 5000,
};


/*------------------------------------------------------------------------------
 *
 * External investments screen functions.
 */

/*
 * Manage investments for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void InvestmentsScreen(Player *aPlayer)
{
    /* Compute revenues. */
    ComputeRevenues(aPlayer);

    /* Draw the investments screen. */
    DrawInvestmentsScreen(aPlayer);

    /* Set taxes. */
    SetTaxes(aPlayer);

    /* Buy investments. */
    BuyInvestments(aPlayer);
}


/*------------------------------------------------------------------------------
 *
 * Investments functions.
 */

/*
 * Draw the investments screen for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

static void DrawInvestmentsScreen(Player *aPlayer)
{
    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display tax revenues. */
    DisplayTaxRevenues(aPlayer);

    /* Display investments. */
    DisplayInvestments(aPlayer);
}


/*
 * Display investments for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void DisplayInvestments(Player *aPlayer)
{
    /* Display investments. */
    move(5, 0);
    printw("INVESTMENTS     NUMBER          PROFITS         COST\n");
    printw("1) MARKETPLACES % -6d          % -6d          1000\n",
           aPlayer->marketplaceCount, aPlayer->marketplaceRevenue);
    printw("2) GRAIN MILLS  % -6d          % -6d          2000\n",
           aPlayer->grainMillCount, aPlayer->grainMillRevenue);
    printw("3) FOUNDRIES    % -6d          % -6d          7000\n",
           aPlayer->foundryCount, aPlayer->foundryRevenue);
    printw("4) SHIPYARDS    % -6d          % -6d          8000\n",
           aPlayer->shipyardCount, aPlayer->shipyardRevenue);
    printw("5) SOLDIERS     % -6d          % -6d          8\n",
           aPlayer->soldierCount, aPlayer->soldierRevenue);
    printw("6) PALACE        %d%% COMPLETED                   5000\n",
           10 * aPlayer->palaceCount);
}


/*
 * Compute revenues for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void ComputeRevenues(Player *aPlayer)
{
    int  marketplaceRevenue;
    int  grainMillRevenue;
    int  foundryRevenue;
    int  shipyardRevenue;
    int  salesTaxRevenue;
    int  incomeTaxRevenue;

    /* Determine marketplace revenue. */
    marketplaceRevenue =
          (  12
           * (aPlayer->merchantCount + RandRange(35) + RandRange(35))
           / (aPlayer->salesTax + 1))
        + 5;
    marketplaceRevenue = aPlayer->marketplaceCount * marketplaceRevenue;
    aPlayer->marketplaceRevenue = pow(marketplaceRevenue, 0.9);

    /* Determine grain mill revenue. */
    grainMillRevenue = 
          ((int) (5.8 * ((float) aPlayer->grainHarvest + RandRange(250))))
        / (20*aPlayer->incomeTax + 40*aPlayer->salesTax + 150);
    grainMillRevenue = aPlayer->grainMillCount * grainMillRevenue;
    aPlayer->grainMillRevenue = pow(grainMillRevenue, 0.9);

    /* Determine the foundry revenue. */
    foundryRevenue = aPlayer->soldierCount + RandRange(150) + 400;
    foundryRevenue = aPlayer->foundryCount * foundryRevenue;
    foundryRevenue = pow(foundryRevenue, 0.9);

    /* Determine the shipyard revenue. */
    shipyardRevenue =
        (  4*aPlayer->merchantCount
         + 9*aPlayer->marketplaceCount
         + 15*aPlayer->foundryCount);
    shipyardRevenue = aPlayer->shipyardCount * shipyardRevenue * weather;
    shipyardRevenue = pow(shipyardRevenue, 0.9);

    /* Determine the army revenue. */
    aPlayer->soldierRevenue = -8 * aPlayer->soldierCount;

    /* Determine customs tax revenue. */
    aPlayer->customsTaxRevenue =   aPlayer->customsTax
                                 * aPlayer->immigrated
                                 * (RandRange(40) + RandRange(40))
                                 / 100;

    /* Determine sales tax revenue. */
    salesTaxRevenue =
        (  ((int) (1.8 * ((float) aPlayer->merchantCount)))
         + 33*aPlayer->marketplaceRevenue
         + 17*aPlayer->grainMillRevenue
         + 50*aPlayer->foundryRevenue
         + 70*aPlayer->shipyardRevenue);
    salesTaxRevenue = pow(salesTaxRevenue, 0.85);
    aPlayer->salesTaxRevenue =
          aPlayer->salesTax
        * (salesTaxRevenue + 5*aPlayer->nobleCount + aPlayer->serfCount)
        / 100;

    /* Determine income tax revenue. */
    incomeTaxRevenue =
          ((int) (1.3 * ((float) aPlayer->serfCount)))
        + 145*aPlayer->nobleCount
        + 39*aPlayer->merchantCount
        + 99*aPlayer->marketplaceCount
        + 99*aPlayer->grainMillCount
        + 425*aPlayer->foundryCount
        + 965*aPlayer->shipyardCount;
    incomeTaxRevenue = aPlayer->incomeTax * incomeTaxRevenue / 100;
    aPlayer->incomeTaxRevenue = pow(incomeTaxRevenue, 0.97);

    /* Update treasury. */
    aPlayer->treasury +=   aPlayer->customsTaxRevenue
                         + aPlayer->salesTaxRevenue
                         + aPlayer->incomeTaxRevenue
                         + aPlayer->marketplaceRevenue
                         + aPlayer->grainMillRevenue
                         + aPlayer->foundryRevenue
                         + aPlayer->shipyardRevenue
                         + aPlayer->soldierRevenue;
}


/*------------------------------------------------------------------------------
 *
 * Tax functions.
 */

/*
 * Set taxes for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void SetTaxes(Player *aPlayer)
{
    char input[80];
    bool done;

    /* Set taxes. */
    done = FALSE;
    while (!done)
    {
        /* Draw the investments screen. */
        DrawInvestmentsScreen(aPlayer);

        /* Get tax to set. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("1) CUSTOMS DUTY  2) SALES TAX  3) INCOME TAX? ");
        getnstr(input, sizeof(input));

        /* Parse input. */
        switch (strtol(input, NULL, 0))
        {
            case 0 :
                done = TRUE;
                break;

            case 1 :
                SetCustomsTax(aPlayer);
                break;

            case 2 :
                SetSalesTax(aPlayer);
                break;

            case 3 :
                SetIncomeTax(aPlayer);
                break;

            default :
                break;
        }
    }
}


/*
 * Set customs tax for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void SetCustomsTax(Player *aPlayer)
{
    char input[80];
    int  customsTax;
    bool validCustomsTax;

    /* Get the new customs tax. */
    validCustomsTax = FALSE;
    do
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("GIVE NEW CUSTOMS TAX (MAX=50%)? ");
        getnstr(input, sizeof(input));
        customsTax = strtol(input, NULL, 0);
        if ((customsTax >= 0) && (customsTax <= 50))
            validCustomsTax = TRUE;
    } while (!validCustomsTax);
    aPlayer->customsTax = customsTax;
}


/*
 * Set sales tax for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void SetSalesTax(Player *aPlayer)
{
    char input[80];
    int  salesTax;
    bool validSalesTax;

    /* Get the new sales tax. */
    validSalesTax = FALSE;
    do
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("GIVE NEW SALES TAX (MAX=20%)? ");
        getnstr(input, sizeof(input));
        salesTax = strtol(input, NULL, 0);
        if ((salesTax >= 0) && (salesTax <= 20))
            validSalesTax = TRUE;
    } while (!validSalesTax);
    aPlayer->salesTax = salesTax;
}


/*
 * Set income tax for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void SetIncomeTax(Player *aPlayer)
{
    char input[80];
    int  incomeTax;
    bool validIncomeTax;

    /* Get the new income tax. */
    validIncomeTax = FALSE;
    do
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("GIVE NEW INCOME TAX (MAX=35%)? ");
        getnstr(input, sizeof(input));
        incomeTax = strtol(input, NULL, 0);
        if ((incomeTax >= 0) && (incomeTax <= 35))
            validIncomeTax = TRUE;
    } while (!validIncomeTax);
    aPlayer->incomeTax = incomeTax;
}


/*
 * Display tax revenues for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void DisplayTaxRevenues(Player *aPlayer)
{
    Country *country;

    /* Get the player country. */
    country = aPlayer->country;

    /* Display tax revenues. */
    printw("STATE REVENUES:    TREASURY=%6d %s\n",
           aPlayer->treasury,
           country->currency);
    printw("CUSTOMS DUTY    SALES TAX       INCOME TAX\n");
    mvprintw(2, 1, "%d %%", aPlayer->customsTax);
    mvprintw(2, 17, "%d %%", aPlayer->salesTax);
    mvprintw(2, 33, "%d %%", aPlayer->incomeTax);
    mvprintw(3, 1, "%d", aPlayer->customsTaxRevenue);
    mvprintw(3, 17, "%d", aPlayer->salesTaxRevenue);
    mvprintw(3, 33, "%d", aPlayer->incomeTaxRevenue);
}


/*------------------------------------------------------------------------------
 *
 * Buy investment functions.
 */

/*
 * Buy investments for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void BuyInvestments(Player *aPlayer)
{
    char input[80];
    bool done;

    /* Buy investments. */
    done = FALSE;
    while (!done)
    {
        /* Draw the investments screen. */
        DrawInvestmentsScreen(aPlayer);

        /* Get investment to buy. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("ANY NEW INVESTMENTS (GIVE #)? ");
        getnstr(input, sizeof(input));

        /* Parse input. */
        switch (strtol(input, NULL, 0))
        {
            case 0 :
                done = TRUE;
                break;

            case INVESTMENT_MARKETPLACE :
                BuyMarketplaces(aPlayer);
                break;

            case INVESTMENT_GRAIN_MILL :
                BuyGrainMills(aPlayer);
                break;

            case INVESTMENT_FOUNDRY :
                BuyFoundries(aPlayer);
                break;

            case INVESTMENT_SHIPYARD :
                BuyShipyards(aPlayer);
                break;

            case INVESTMENT_SOLDIER :
                BuySoldiers(aPlayer);
                break;

            case INVESTMENT_PALACE :
                BuyPalaces(aPlayer);
                break;

            default :
                break;
        }
    }
}


/*
 * Buy marketplaces for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void BuyMarketplaces(Player *aPlayer)
{
    Country *country;
    char     input[80];
    int      newMerchantCount;
    int      marketplaceCount;
    bool     validMarketplaceCount;

    /* Get the player country. */
    country = aPlayer->country;

    /* Get the number of marketplaces to buy. */
    validMarketplaceCount = FALSE;
    do
    {
        /* Get user input. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY? ");
        getnstr(input, sizeof(input));
        marketplaceCount = strtol(input, NULL, 0);

        /* Validate the number of marketplaces to buy. */
        validMarketplaceCount = ValidateInvestment(aPlayer,
                                                   INVESTMENT_MARKETPLACE,
                                                   marketplaceCount);
    } while (!validMarketplaceCount);

    /* Update marketplace count and treasury. */
    aPlayer->marketplaceCount += marketplaceCount;
    aPlayer->treasury -= marketplaceCount * 1000;

    /*
     * Merchants are gained with every marketplace purchase, even if none are
     * purchased.
     */
    newMerchantCount = RandRange(7);
    aPlayer->merchantCount += newMerchantCount;
    aPlayer->serfCount -= newMerchantCount;
}


/*
 * Buy grain mills for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void BuyGrainMills(Player *aPlayer)
{
    Country *country;
    char     input[80];
    int      grainMillCount;
    bool     validGrainMillCount;

    /* Get the player country. */
    country = aPlayer->country;

    /* Get the number of grain mills to buy. */
    validGrainMillCount = FALSE;
    do
    {
        /* Get user input. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY? ");
        getnstr(input, sizeof(input));
        grainMillCount = strtol(input, NULL, 0);

        /* Validate the number of grain mills to buy. */
        validGrainMillCount = ValidateInvestment(aPlayer,
                                                 INVESTMENT_GRAIN_MILL,
                                                 grainMillCount);
    } while (!validGrainMillCount);

    /* Update grain mill count and treasury. */
    aPlayer->grainMillCount += grainMillCount;
    aPlayer->treasury -= grainMillCount * 2000;
}


/*
 * Buy foundries for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void BuyFoundries(Player *aPlayer)
{
    Country *country;
    char     input[80];
    int      foundryCount;
    bool     validFoundryCount;

    /* Get the player country. */
    country = aPlayer->country;

    /* Get the number of foundries to buy. */
    validFoundryCount = FALSE;
    do
    {
        /* Get user input. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY? ");
        getnstr(input, sizeof(input));
        foundryCount = strtol(input, NULL, 0);

        /* Validate the number of foundries to buy. */
        validFoundryCount = ValidateInvestment(aPlayer,
                                               INVESTMENT_FOUNDRY,
                                               foundryCount);
    } while (!validFoundryCount);

    /* Update foundry count and treasury. */
    aPlayer->foundryCount += foundryCount;
    aPlayer->treasury -= foundryCount * 7000;
}


/*
 * Buy shipyards for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void BuyShipyards(Player *aPlayer)
{
    Country *country;
    char     input[80];
    int      shipyardCount;
    bool     validShipyardCount;

    /* Get the player country. */
    country = aPlayer->country;

    /* Get the number of shipyards to buy. */
    validShipyardCount = FALSE;
    do
    {
        /* Get user input. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY? ");
        getnstr(input, sizeof(input));
        shipyardCount = strtol(input, NULL, 0);

        /* Validate the number of shipyards to buy. */
        validShipyardCount = ValidateInvestment(aPlayer,
                                                INVESTMENT_SHIPYARD,
                                                shipyardCount);
    } while (!validShipyardCount);

    /* Update shipyard count and treasury. */
    aPlayer->shipyardCount += shipyardCount;
    aPlayer->treasury -= shipyardCount * 8000;
}


/*
 * Buy soldiers for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void BuySoldiers(Player *aPlayer)
{
    Country *country;
    char     input[80];
    int      soldierCount;
    bool     validSoldierCount;

    /* Get the player country. */
    country = aPlayer->country;

    /* Get the number of soldiers to buy. */
    validSoldierCount = FALSE;
    do
    {
        /* Get user input. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY? ");
        getnstr(input, sizeof(input));
        soldierCount = strtol(input, NULL, 0);

        /* Validate the number of soldiers to buy. */
        validSoldierCount = ValidateInvestment(aPlayer,
                                               INVESTMENT_SOLDIER,
                                               soldierCount);
    } while (!validSoldierCount);

    /* Update soldier count and treasury. */
    aPlayer->soldierCount += soldierCount;
    aPlayer->serfCount -= soldierCount;
    aPlayer->treasury -= soldierCount * 8;
}


/*
 * Buy palaces for the player specified by aPlayer
 *
 *   aPlayer                Player.
 */

void BuyPalaces(Player *aPlayer)
{
    Country *country;
    char     input[80];
    int      newNobleCount;
    int      palaceCount;
    bool     validPalaceCount;

    /* Get the player country. */
    country = aPlayer->country;

    /* Get the number of palaces to buy. */
    validPalaceCount = FALSE;
    do
    {
        /* Get user input. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY? ");
        getnstr(input, sizeof(input));
        palaceCount = strtol(input, NULL, 0);

        /* Validate the number of palaces to buy. */
        validPalaceCount = ValidateInvestment(aPlayer,
                                              INVESTMENT_PALACE,
                                              palaceCount);
    } while (!validPalaceCount);

    /* Update palace count and treasury. */
    aPlayer->palaceCount += palaceCount;
    aPlayer->treasury -= palaceCount * 5000;

    /*
     * Nobles are gained with every palace purchase, even if none are
     * purchased.
     */
    newNobleCount = RandRange(4);
    aPlayer->nobleCount += newNobleCount;
    aPlayer->serfCount -= newNobleCount;
}


/*
 *   Return true if the investment and investment count specified by investment
 * and investmentCount are valid for the player specified by aPlayer; return
 * false otherwise.  For example, if the investment cost times times the
 * investment count is greater than the player's treasury, this function returns
 * false.
 *
 *   aPlayer                Player.
 *   investment             Investment being purchased.
 *   investmentCount        Number of investments being purchased.
 */

bool ValidateInvestment(Player *aPlayer, int investment, int investmentCount)
{
    Country *country;
    int      totalCost;
    char     invalidMessage[128];
    bool     valid = TRUE;

    /* Clear the invalid message. */
    invalidMessage[0] = '\0';

    /* Get the player country. */
    country = aPlayer->country;

    /* Validate that the count is a non-negative number. */
    if (investmentCount < 0)
        valid = FALSE;

    /* Validate there's enough in the treasury for the purchase. */
    if (valid)
    {
        totalCost = investmentCount * investmentCost[investment - 1];
        if (totalCost > aPlayer->treasury)
        {
            valid = FALSE;
            snprintf(invalidMessage,
                     sizeof(invalidMessage),
                     "THINK AGAIN . . .YOU ONLY HAVE %d %s",
                     aPlayer->treasury,
                     country->currency);
        }
    }

    /* Validate that there are enough serfs to train for the purchase. */
    if (valid && (investmentCount > aPlayer->serfCount))
    {
        valid = FALSE;
        snprintf(invalidMessage,
                 sizeof(invalidMessage),
                 "YOU DON'T HAVE ENOUGH SERFS TO TRAIN");
    }

    /*
     * Validate if there are enough foundries and nobles for the new
     * soldiers.
     */
    if (valid && (investment == INVESTMENT_SOLDIER))
    {
        int totalSoldierCount;
        int totalPeopleCount;

        /*
         * Determine the total number of soldiers if the investment is made
         * and the total number of civilians in the country.
         */
        totalSoldierCount = aPlayer->soldierCount + investmentCount;
        totalPeopleCount =   aPlayer->serfCount
                           + aPlayer->merchantCount
                           + aPlayer->nobleCount;

        /*
         * Validate if there are enough foundries and nobles for the new
         * soldiers.
         */
        if ((((float) totalSoldierCount) / ((float) totalPeopleCount)) >
            (0.05 + 0.015*aPlayer->foundryCount))
        {
            valid = FALSE;
            snprintf(invalidMessage,
                     sizeof(invalidMessage),
                     "YOU CANNOT EQUIP AND MAINTAIN SO MANY TROOPS, %s",
                     aPlayer->title);
        }
        else if (totalSoldierCount > (20 * aPlayer->nobleCount))
        {
            valid = FALSE;
            snprintf(invalidMessage,
                     sizeof(invalidMessage),
                     "PLEASE THINK AGAIN . . .  YOU ONLY HAVE %d NOBLES\n"
                     "TO LEAD YOUR TROOPS.",
                     aPlayer->nobleCount);
        }
    }

    /* If investment is invalid, notify the player. */
    if (!valid && (strlen(invalidMessage) > 0))
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw(invalidMessage);
        refresh();
        sleep(DELAY_TIME);
    }

    return valid;
}

