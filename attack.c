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

static void AttackBarbarians(Player *aPlayer);

static void AttackPlayer(Player *aPlayer, Player *aTargetPlayer);

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
    Player *targetPlayer;
    char    input[80];
    int     country;
    bool    done;

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
        {
            done = TRUE;
        }
        else if (country == 1)
        {
            AttackBarbarians(aPlayer);
        }
        else if ((country >= 2) && (country <= (COUNTRY_COUNT + 1)))
        {
            targetPlayer = &(playerList[country - 2]);
            if (targetPlayer == aPlayer)
            {
                mvprintw(15,
                         0,
                         "%s, PLEASE THINK AGAIN.  YOU ARE # %d!",
                         aPlayer->title,
                         country);
                refresh();
                sleep(DELAY_TIME);
            }
            else
            {
                AttackPlayer(aPlayer, &(playerList[country - 2]));
            }
        }
    }
}


/*------------------------------------------------------------------------------
 *
 * Attack functions.
 */

/*
 * Attack barbarians for the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

static void AttackBarbarians(Player *aPlayer)
{
}


/*
 *   Attack the player specified by aTargetPlayer for the player specified by
 * aPlayer.
 *
 *   aPlayer                Player.
 */

static void AttackPlayer(Player *aPlayer, Player *aTargetPlayer)
{
    char input[80];
    int  soldierCount;
    int  soldierEfficiency;
    int  targetSoldierCount;
    int  targetSoldierEfficiency;
    int  soldierKillCount;
    int  landCaptured;
    bool soldierCountValid;
    bool targetSerfs = FALSE;
    bool targetOverrun = FALSE;
    bool battleDone;

    /* Can't attack other players until the third year. */
    if (year < 3)
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("DUE TO INTERNATIONAL TREATY, YOU CANNOT ATTACK OTHER\n"
               "NATIONS UNTIL THE THIRD YEAR.");
        refresh();
        sleep(DELAY_TIME);
        return;
    }

    /* Get the number of soldiers with which to attack. */
    soldierCountValid = FALSE;
    while (!soldierCountValid)
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY SOLDIERS DO YOU WISH TO SEND? ");
        getnstr(input, sizeof(input));
        soldierCount = strtol(input, NULL, 0);
        if (soldierCount > aPlayer->soldierCount)
        {
            move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
            printw("THINK AGAIN... YOU HAVE ONLY %d SOLDIERS",
                   aPlayer->soldierCount);
            refresh();
            sleep(DELAY_TIME);
        }
        else
        {
            soldierCountValid = TRUE;
        }
    }
    aPlayer->soldierCount -= soldierCount;

    /* Get soldier information. */
    soldierEfficiency = aPlayer->armyEfficiency;
    if (aTargetPlayer->soldierCount > 0)
    {
        targetSoldierCount = aTargetPlayer->soldierCount;
        targetSoldierEfficiency = aTargetPlayer->armyEfficiency;
    }
    else
    {
        targetSerfs = TRUE;
        targetSoldierCount = aTargetPlayer->serfCount;
        targetSoldierEfficiency = SERF_EFFICIENCY;
    }

    /* Battle. */
    landCaptured = 0;
    battleDone = FALSE;
    while (!battleDone)
    {
        /* Show soldiers remaining. */
        clear();
        mvprintw(2, 41, "SOLDIERS REMAINING:");
        mvprintw(4, 13, "%s %s OF %s:",
                 aPlayer->title, aPlayer->name, aPlayer->country->name);
        mvprintw(5, 13, "%s %s OF %s:",
                 aTargetPlayer->title,
                 aTargetPlayer->name,
                 aTargetPlayer->country->name);
        mvprintw(4, 51, "%d", soldierCount);
        mvprintw(5, 51, "%d", targetSoldierCount);
        if (targetSerfs)
        {
            mvprintw(8, 0, "%s'S SERFS ARE FORCED TO DEFEND THEIR COUNTRY!",
                     aTargetPlayer->country->name);
        }
        refresh();
        usleep(250000);

        /*
         * Determine how many soldiers were killed in this round, who won the
         * round, and how much land was captured.
         */
        soldierKillCount = (soldierCount / 15) + 1;
        if (RandRange(soldierEfficiency) < RandRange(targetSoldierEfficiency))
        {
            /* Player lost. */
            soldierCount -= soldierKillCount;
            if (soldierCount < 0)
                soldierCount = 0;

            /* Battle is done if all target land has been captured. */
            if (landCaptured >= aTargetPlayer->land)
                battleDone = TRUE;
        }
        else
        {
            /* Player won. */
            landCaptured +=   RandRange(26 * soldierKillCount)
                            - RandRange(soldierKillCount + 5);
            if (landCaptured < 0)
                landCaptured = 0;
            else if (landCaptured > aTargetPlayer->land)
                landCaptured = aTargetPlayer->land;
            targetSoldierCount -= soldierKillCount;
            if (targetSoldierCount < 0)
                targetSoldierCount = 0;
        }

        /* Keep battling until one army is defeated. */
        if ((soldierCount == 0) || (targetSoldierCount == 0))
            battleDone = TRUE;
    }

    /* Display battle results. */
    clear();
    mvprintw(1, 23, "BATTLE OVER\n\n");
    if (   (soldierCount > 0)
        && (targetSerfs || (landCaptured >= aTargetPlayer->land)))
    {
        /* Target player overrun. */
        targetOverrun = TRUE;
        printw("THE COUNTRY OF %s WAS OVERUN!\n", aTargetPlayer->country->name);
        printw("ALL ENEMY NOBLES WERE SUMMARILY EXECUTED!\n\n\n");
        printw("THE REMAINING ENEMY SOLDIERS "
               "WERE IMPRISONED. ALL ENEMY SERFS\n");
        printw("HAVE PLEDGED OATHS OF FEALTY TO "
               "YOU, AND SHOULD NOW BE CONSID-\n");
        printw("ERED TO BE YOUR PEOPLE TOO. ALL "
               "ENEMY MERCHANTS FLED THE COUN-\n");
        printw("TRY. UNFORTUNATELY, ALL ENEMY "
               "ASSETS WERE SACKED AND DESTROYED\n");
        printw("BY YOUR REVENGEFUL ARMY IN A "
               "DRUNKEN RIOT FOLLOWING THE VICTORY\n");
        printw("CELEBRATION.\n");
    }
    else if (soldierCount > 0)
    {
        /* Player won. */
        printw("THE FORCES OF %s %s WERE VICTORIOUS.\n",
               aPlayer->title,
               aPlayer->name);
        printw(" %d ACRES WERE SEIZED.\n", landCaptured);
    }
    else
    {
        /* Player lost. */
        printw("%s %s WAS DEFEATED.\n", aPlayer->title, aPlayer->name);
        if (landCaptured > 2)
        {
            landCaptured /= RandRange(3);
            printw("IN YOUR DEFEAT YOU NEVERTHELESS "
                   "MANAGED TO CAPTURE %d ACRES.\n",
                   landCaptured);
        }
        else
        {
            landCaptured = 0;
            printw(" 0 ACRES WERE SEIZED.\n");
        }
    }
    printw("<ENTER>? ");
    getnstr(input, sizeof(input));

    /* Update soldiers, land, etc. */
    aPlayer->soldierCount += soldierCount;
    if (targetSerfs)
        aTargetPlayer->serfCount = targetSoldierCount;
    else
        aTargetPlayer->soldierCount = targetSoldierCount;
    aPlayer->land += landCaptured;
    aTargetPlayer->land -= landCaptured;
    if (targetOverrun)
    {
        aPlayer->serfCount += aTargetPlayer->serfCount;
        aTargetPlayer->dead = TRUE;
    }
}


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
        /* Get the country name and player land.  Don't display dead players. */
        if (i == 0)
        {
            countryName = "BARBARIANS";
            playerLand = barbarianLand;
        }
        else
        {
            player = &(playerList[i - 1]);
            if (player->dead)
                continue;
            country = player->country;
            countryName = country->name;
            playerLand = player->land;
        }

        /* Display land holdings. */
        printw(" %d)  %-11s %d\n", i + 1, countryName, playerLand);
    }
}


