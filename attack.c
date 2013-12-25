/*------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 *
 * TRS-80 Empire game attack screen source file.
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

/*
 * Attack prototypes.
 */

static void DrawAttackScreen(Player *aPlayer);


/*------------------------------------------------------------------------------
 *
 * External attack screen functions.
 */

/*
 * Manage attacking for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

void AttackScreen(Player *aPlayer)
{
    char input[80];
    int  country;
    bool done;

    /* Attack other countries. */
    done = FALSE;
    while (!done)
    {
        /* Draw the attack screen. */
        DrawAttackScreen(aPlayer);

        /* Get country to attack. */
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("WHO DO YOU WISH TO ATTACK (GIVE #)? ");
        getnstr(input, sizeof(input));
        country = strtol(input, NULL, 0);

        /* Parse input. */
        if (country == 0)
            done = TRUE;
    }
}


/*------------------------------------------------------------------------------
 *
 * Attack functions.
 */

/*
 * Draw the attack screen for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

static void DrawAttackScreen(Player *aPlayer)
{
    Player     *player;
    Country    *country;
    const char *countryName;
    int         playerLand;
    int         i;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display land holdings. */
    printw("LAND HOLDINGS:\n\n");
    for (i = 0; i < COUNTRY_COUNT + 1; i++)
    {
        /* Get the country name and player land. */
        if (i == 0)
	{
            countryName = "BARBARIANS";
            playerLand = barbarianLand;
        }
        else
	{
            player = &(playerList[i - 1]);
            country = player->country;
            countryName = country->name;
            playerLand = player->land;
        }

        /* Display land holdings. */
        printw(" %d)  %-11s %d\n", i + 1, countryName, playerLand);
    }
}


