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
#include <ncurses.h>

/* Local includes. */
#include "empire.h"


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
    Country *country;
    char     input[80];

    /* Get the player country. */
    country = aPlayer->country;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display revenues. */
    printw("STATE REVENUES:    TREASURY=%6d %s\n",
           aPlayer->treasury,
           country->currency);

    /* Set taxes. */
    move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
    printw("1) CUSTOMS DUTY  2) SALES TAX  3) INCOME TAX? ");
    getnstr(input, sizeof(input));
}

