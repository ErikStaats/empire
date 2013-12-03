/*------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 *
 * TRS-80 Empire game population screen source file.
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
 * Prototypes.
 */


/*------------------------------------------------------------------------------
 *
 * External population screen functions.
 */

/*
 * Manage population for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void PopulationScreen(Player *aPlayer)
{
    char     input[80];
    Country *country;

    /* Get the player country. */
    country = aPlayer->country;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display the ruler and country. */
    printw("%s %s OF %s:\n", aPlayer->title, aPlayer->name, country->name);

    /* Display the year. */
    printw("IN YEAR %d,\n", year);

    /* Wait for player to be done. */
    printw("<ENTER>? ");
    getnstr(input, 80);
}

