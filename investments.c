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

/* Local includes. */
#include "empire.h"


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

static void DrawInvestmentsScreen(Player *aPlayer);

static void DisplayTaxRevenues(Player *aPlayer);

static void DisplayInvestments(Player *aPlayer);

static void ComputeRevenues(Player *aPlayer);


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
    char input[80];

    /* Compute revenues. */
    ComputeRevenues(aPlayer);

    /* Draw the investments screen. */
    DrawInvestmentsScreen(aPlayer);

    /* Set taxes. */
    move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
    printw("1) CUSTOMS DUTY  2) SALES TAX  3) INCOME TAX? ");
    getnstr(input, sizeof(input));
}

/*------------------------------------------------------------------------------
 *
 * Internal investments screen functions.
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


