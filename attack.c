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
#include <string.h>

/* Local includes. */
#include "empire.h"


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

/*
 * Attack prototypes.
 */

static void Attack(Player *aPlayer, Player *aTargetPlayer);

static void GetSoldiersToAttack(Battle *aBattle);

static void RunBattle(Battle *aBattle);

static void DisplayBattleResults(Battle *aBattle);

static void Sack(Player *aTargetPlayer);

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
    int     maxAttacks;
    bool    done;

    /* Determine the maximum number of attacks per year. */
    maxAttacks = (aPlayer->nobleCount / 4) + 1;

    /* Attack other countries. */
    aPlayer->attackCount = 0;
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
            targetPlayer = NULL;
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
                continue;
            }
        }

        /* Attack. */
        if (aPlayer->attackCount < maxAttacks)
        {
            Attack(aPlayer, targetPlayer);
        }
        else
        {
            move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
            printw("DUE TO A SHORTAGE OF NOBLES , YOU ARE LIMITED TO ONLY\n");
            printw(" %d ATTACKS PER YEAR", maxAttacks);
            refresh();
            sleep(DELAY_TIME);
        }
    }
}


/*------------------------------------------------------------------------------
 *
 * Attack functions.
 */

/*
 *   Attack the player specified by aTargetPlayer for the player specified by
 * aPlayer.  If aTargetPlayer is NULL, attack barbarians.
 *
 *   aPlayer                Player.
 *   aTargetPlayer          Player to attack.
 */

static void Attack(Player *aPlayer, Player *aTargetPlayer)
{
    Battle battle;

    /* Can't attack other players until the third year. */
    if ((aTargetPlayer != NULL) && (year < 3))
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("DUE TO INTERNATIONAL TREATY, YOU CANNOT ATTACK OTHER\n"
               "NATIONS UNTIL THE THIRD YEAR.");
        refresh();
        sleep(DELAY_TIME);
        return;
    }

    /* If attacking the barbarians, make sure they have land. */
    if ((aTargetPlayer == NULL) && (barbarianLand == 0))
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("ALL BARBARIAN LANDS HAVE BEEN SEIZED\n");
        refresh();
        sleep(DELAY_TIME);
        return;
    }

    /* Initialize the battle information. */
    memset(&battle, 0, sizeof(battle));

    /* Set the player battle information. */
    battle.player = aPlayer;
    battle.soldierEfficiency = aPlayer->armyEfficiency;
    snprintf(battle.soldierLabel,
             sizeof(battle.soldierLabel),
             "%s %s OF %s",
             aPlayer->title,
             aPlayer->name,
             aPlayer->country->name);
    GetSoldiersToAttack(&battle);

    /* Set the target battle information. */
    if (aTargetPlayer != NULL)
    {
        battle.targetPlayer = aTargetPlayer;
        battle.targetLand = aTargetPlayer->land;
        snprintf(battle.targetSoldierLabel,
                 sizeof(battle.targetSoldierLabel),
                 "%s %s OF %s",
                 aTargetPlayer->title,
                 aTargetPlayer->name,
                 aTargetPlayer->country->name);
        if (aTargetPlayer->soldierCount > 0)
        {
            battle.targetSoldierCount = aTargetPlayer->soldierCount;
            battle.targetSoldierEfficiency = aTargetPlayer->armyEfficiency;
        }
        else
        {
            battle.targetSerfs = TRUE;
            battle.targetSoldierCount = aTargetPlayer->serfCount;
            battle.targetSoldierEfficiency = SERF_EFFICIENCY;
        }
    }
    else
    {
        battle.targetLand = barbarianLand;
        snprintf(battle.targetSoldierLabel,
                 sizeof(battle.targetSoldierLabel),
                 "PAGAN BARBARIANS");
        battle.targetSoldierCount =
              RandRange(3 * RandRange(battle.soldierCount))
            + RandRange(RandRange(3 * battle.soldierCount / 2));
        battle.targetSoldierEfficiency = 9;
    }

    /* Battle. */
    RunBattle(&battle);
    DisplayBattleResults(&battle);

    /* Update soldiers, land, etc. */
    aPlayer->attackCount++;
    aPlayer->soldierCount -=   battle.soldiersToAttackCount
                             - battle.soldierCount;
    if (aTargetPlayer != NULL)
    {
        if (battle.targetSerfs)
            aTargetPlayer->serfCount = battle.targetSoldierCount;
        else
            aTargetPlayer->soldierCount = battle.targetSoldierCount;
    }
    aPlayer->land += battle.landCaptured;
    if (aTargetPlayer != NULL)
        aTargetPlayer->land -= battle.landCaptured;
    else
        barbarianLand -= battle.landCaptured;
    if ((aTargetPlayer != NULL) && battle.targetOverrun)
    {
        aPlayer->serfCount += aTargetPlayer->serfCount;
        aTargetPlayer->dead = TRUE;
    }
}


/*
 *   Get the number of player soldiers to attack in the battle specified by
 * aBattle.
 *
 *   aBattle                Battle for which to get attacking soldiers.
 */

static void GetSoldiersToAttack(Battle *aBattle)
{
    char    input[80];
    Player *player;
    int     soldiersToAttackCount;
    bool    soldiersToAttackCountValid;

    /* Get battle information. */
    player = aBattle->player;

    /* Get the number of soldiers with which to attack. */
    soldiersToAttackCountValid = FALSE;
    while (!soldiersToAttackCountValid)
    {
        move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
        printw("HOW MANY SOLDIERS DO YOU WISH TO SEND? ");
        getnstr(input, sizeof(input));
        soldiersToAttackCount = strtol(input, NULL, 0);
        if (soldiersToAttackCount > player->soldierCount)
        {
            move(14, 0); clrtoeol(); move(15, 0); clrtoeol(); move(14, 0);
            printw("THINK AGAIN... YOU HAVE ONLY %d SOLDIERS",
                   player->soldierCount);
            refresh();
            sleep(DELAY_TIME);
        }
        else
        {
            soldiersToAttackCountValid = TRUE;
        }
    }

    /* Update battle information. */
    aBattle->soldiersToAttackCount = soldiersToAttackCount;
    aBattle->soldierCount = soldiersToAttackCount;
}


/*
 * Run the battle specified by aBattle.
 *
 *   aBattle                Battle to run.
 */

static void RunBattle(Battle *aBattle)
{
    Player *player;
    Player *targetPlayer;
    int     soldierCount;
    int     soldierEfficiency;
    int     targetSoldierCount;
    int     targetSoldierEfficiency;
    int     targetLand;
    int     soldierKillCount;
    int     landCaptured = 0;
    bool    targetSerfs = FALSE;
    bool    targetOverrun = FALSE;
    bool    battleDone;

    /* Get battle information. */
    player = aBattle->player;
    targetPlayer = aBattle->targetPlayer;
    soldierCount = aBattle->soldierCount;
    soldierEfficiency = aBattle->soldierEfficiency;
    targetSoldierCount = aBattle->targetSoldierCount;
    targetSoldierEfficiency = aBattle->targetSoldierEfficiency;
    targetLand = aBattle->targetLand;
    targetSerfs = aBattle->targetSerfs;

    /* Battle. */
    landCaptured = 0;
    battleDone = FALSE;
    while (!battleDone)
    {
        /* Show soldiers remaining. */
        clear();
        mvprintw(2, 41, "SOLDIERS REMAINING:");
        mvprintw(4, 13, "%s:", aBattle->soldierLabel);
        mvprintw(5, 13, "%s:", aBattle->targetSoldierLabel);
        mvprintw(4, 51, "%d", soldierCount);
        mvprintw(5, 51, "%d", targetSoldierCount);
        if (targetSerfs)
        {
            mvprintw(8, 0, "%s'S SERFS ARE FORCED TO DEFEND THEIR COUNTRY!",
                     targetPlayer->country->name);
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
            if (landCaptured >= targetLand)
                battleDone = TRUE;
        }
        else
        {
            /* Player won. */
            landCaptured +=   RandRange(26 * soldierKillCount)
                            - RandRange(soldierKillCount + 5);
            if (landCaptured < 0)
                landCaptured = 0;
            else if (landCaptured > targetLand)
                landCaptured = targetLand;
            targetSoldierCount -= soldierKillCount;
            if (targetSoldierCount < 0)
                targetSoldierCount = 0;
        }

        /* Keep battling until one army is defeated. */
        if ((soldierCount == 0) || (targetSoldierCount == 0))
            battleDone = TRUE;
    }

    /* Update battle information. */
    aBattle->soldierCount = soldierCount;
    aBattle->targetSoldierCount = targetSoldierCount;
    aBattle->landCaptured = landCaptured;
    if (soldierCount > 0)
    {
        aBattle->targetDefeated = TRUE;
        if (targetSerfs || (landCaptured >= targetLand))
            aBattle->targetOverrun = TRUE;
    }
}


/*
 * Display results of the battle specified by aBattle.
 *
 *   aBattle                Battle for which to display results.
 */

static void DisplayBattleResults(Battle *aBattle)
{
    char    input[80];
    Player *player;
    Player *targetPlayer;
    int     soldierCount;
    int     targetLand;
    int     landCaptured;
    bool    targetSerfs;

    /* Get battle information. */
    player = aBattle->player;
    targetPlayer = aBattle->targetPlayer;
    soldierCount = aBattle->soldierCount;
    targetLand = aBattle->targetLand;
    targetSerfs = aBattle->targetSerfs;
    landCaptured = aBattle->landCaptured;

    /* Display battle results. */
    clear();
    mvprintw(1, 23, "BATTLE OVER\n\n");
    if ((targetPlayer != NULL) && aBattle->targetOverrun)
    {
        /* Target player overrun. */
        printw("THE COUNTRY OF %s WAS OVERUN!\n", targetPlayer->country->name);
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
    else if ((targetPlayer == NULL) && aBattle->targetOverrun)
    {
        printw("ALL BARBARIAN LANDS HAVE BEEN SEIZED\n");
        printw("THE REMAINING BARBARIANS FLED\n");
    }
    else if (aBattle->targetDefeated)
    {
        /* Player won. */
        printw("THE FORCES OF %s %s WERE VICTORIOUS.\n",
               player->title,
               player->name);
        printw(" %d ACRES WERE SEIZED.\n", landCaptured);
    }
    else
    {
        /* Player lost. */
        printw("%s %s WAS DEFEATED.\n", player->title, player->name);
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

    /* Check for sacking. */
    if (   (targetPlayer != NULL)
        && !aBattle->targetOverrun
        && (landCaptured > (targetLand / 3)))
    {
        Sack(targetPlayer);
    }

    /* Wait for player. */
    printw("<ENTER>? ");
    getnstr(input, sizeof(input));

    /* Update battle information. */
    aBattle->landCaptured = landCaptured;
}


/*
 * Sack the player specified by aTargetPlayer.
 *
 *   aTargetPlayer          Player to sack.
 */

static void Sack(Player *aTargetPlayer)
{
    int  sackCount;

    /* Sack serfs. */
    if (aTargetPlayer->serfCount > 0)
    {
        sackCount = RandRange(aTargetPlayer->serfCount);
        aTargetPlayer->serfCount -= sackCount;
        printw(" %d ENEMY SERFS WERE BEATEN AND MURDERED BY YOUR TROOPS!\n",
               sackCount);
    }

    /* Sack marketplaces. */
    if (aTargetPlayer->marketplaceCount > 0)
    {
        sackCount = RandRange(aTargetPlayer->marketplaceCount);
        aTargetPlayer->marketplaceCount -= sackCount;
        printw(" %d ENEMY MARKETPLACES WERE DESTROYED\n", sackCount);
    }

    /* Sack grain. */
    if (aTargetPlayer->grain > 0)
    {
        sackCount = RandRange(aTargetPlayer->grain);
        aTargetPlayer->grain -= sackCount;
        printw(" %d BUSHELS OF ENEMY GRAIN WERE BURNED\n", sackCount);
    }

    /* Sack grain mills. */
    if (aTargetPlayer->grainMillCount > 0)
    {
        sackCount = RandRange(aTargetPlayer->grainMillCount);
        aTargetPlayer->grainMillCount -= sackCount;
        printw(" %d ENEMY GRAIN MILLS WERE SABOTAGED\n", sackCount);
    }

    /* Sack foundries. */
    if (aTargetPlayer->foundryCount > 0)
    {
        sackCount = RandRange(aTargetPlayer->foundryCount);
        aTargetPlayer->foundryCount -= sackCount;
        printw(" %d ENEMY FOUNDRIES WERE LEVELED\n", sackCount);
    }

    /* Sack shipyards. */
    if (aTargetPlayer->shipyardCount > 0)
    {
        sackCount = RandRange(aTargetPlayer->shipyardCount);
        aTargetPlayer->shipyardCount -= sackCount;
        printw(" %d ENEMY SHIPYARDS WERE OVER-RUN\n", sackCount);
    }

    /* Sack nobles. */
    if (aTargetPlayer->nobleCount > 2)
    {
        sackCount = RandRange(aTargetPlayer->nobleCount / 2);
        aTargetPlayer->nobleCount -= sackCount;
        printw(" %d ENEMY NOBLES WERE SUMMARILY EXECUTED\n", sackCount);
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


