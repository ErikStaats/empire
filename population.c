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
#include <math.h>
#include <ncurses.h>

/* Local includes. */
#include "empire.h"


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

static void PlayerDeath(Player *aPlayer);


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
    int      population;
    int      populationGain;
    int      born;
    int      immigrated;
    int      merchantsImmigrated;
    int      noblesImmigrated;
    int      diedDisease;
    int      diedMalnutrition;
    int      diedStarvation;
    int      armyDiedStarvation;
    int      armyDeserted;

    /* Get the player country. */
    country = aPlayer->country;

    /* Reset screen. */
    clear();
    move(0, 0);

    /* Display the ruler and country. */
    printw("%s %s OF %s:\n", aPlayer->title, aPlayer->name, country->name);

    /* Display the year. */
    printw("IN YEAR %d,\n\n", year);

    /* Determine the total population. */
    population =   aPlayer->serfCount
                 + aPlayer->merchantCount
                 + aPlayer->nobleCount;

    /* Determine the number of babies born. */
    born = RandRange(((int) (((float) population) / 9.5)));

    /* Determine the number of people who died from disease. */
    diedDisease = RandRange(population / 22);

    /* Determine the number of people who died of starvation and */
    /* malnutrition.                                             */
    diedStarvation = 0;
    diedMalnutrition = 0;
    if (aPlayer->peopleGrainNeed > (2 * aPlayer->peopleGrainFeed))
    {
        diedMalnutrition = RandRange(population/12 + 1);
        diedStarvation = RandRange(population/16 + 1);
    }
    else if (aPlayer->peopleGrainNeed > aPlayer->peopleGrainFeed)
    {
        diedMalnutrition = RandRange(population/15 + 1);
    }
    aPlayer->diedStarvation = diedStarvation;

    /* Determine the number of people who immigrated. */
    if (((float) aPlayer->peopleGrainFeed) >
        (1.5 * ((float) aPlayer->peopleGrainNeed)))
    {
        immigrated =
              ((int) sqrt(aPlayer->peopleGrainFeed - aPlayer->peopleGrainNeed))
            - RandRange((int) (1.5 * ((float) aPlayer->customsTax)));
        if (immigrated > 0)
            immigrated = RandRange(((2 * immigrated) + 1));
        else
            immigrated = 0;
    }
    else
    {
        immigrated = 0;
    }
    aPlayer->immigrated = immigrated;

    /* Determine the number of merchants and nobles who immigrated. */
    merchantsImmigrated = 0;
    noblesImmigrated = 0;
    if ((immigrated / 5) > 0)
        merchantsImmigrated = RandRange(immigrated / 5);
    if ((immigrated / 25) > 0)
        noblesImmigrated = RandRange(immigrated / 25);

    /* Determine the number of soldiers who died of starvation or deserted. */
    armyDiedStarvation = 0;
    armyDeserted = 0;
    if (aPlayer->armyGrainNeed > (2 * aPlayer->armyGrainFeed))
    {
        armyDiedStarvation = RandRange(aPlayer->soldierCount/2 + 1);
        aPlayer->soldierCount -= armyDiedStarvation;
        armyDeserted = RandRange(aPlayer->soldierCount / 5);
        aPlayer->soldierCount -= armyDeserted;
    }

    /* Determine the army's efficiency. */
    aPlayer->armyEfficiency =   (10 * aPlayer->armyGrainFeed)
                              / aPlayer->armyGrainNeed;
    if (aPlayer->armyEfficiency < 5)
        aPlayer->armyEfficiency = 5;
    else if (aPlayer->armyEfficiency > 15)
        aPlayer->armyEfficiency = 15;

    /* Display the number of babies born. */
    printw(" %d BABIES WERE BORN\n", born);

    /* Display the number of people who died of disease. */
    printw(" %d PEOPLE DIED OF DISEASE\n", diedDisease);

    /* Display the number of people who immigrated. */
    if (immigrated > 0)
        printw(" %d PEOPLE IMMIGRATED INTO YOUR COUNTRY.\n", immigrated);

    /* Display the number of people who died of starvation and malnutrition. */
    if (diedMalnutrition > 0)
        printw(" %d PEOPLE DIED OF MALNUTRITION.\n", diedMalnutrition);
    if (diedStarvation > 0)
        printw(" %d PEOPLE STARVED TO DEATH.\n", diedStarvation);

    /* Display the number of soldiers who starved to death. */
    if (armyDiedStarvation > 0)
        printw(" %d SOLDIERS STARVED TO DEATH.\n", armyDiedStarvation);

    /* Display the army efficiency. */
    printw("YOUR ARMY WILL FIGHT AT %d%% EFFICIENCY.\n",
           10 * aPlayer->armyEfficiency);

    /* Display the population gain or loss. */
    populationGain =
        born + immigrated - diedDisease - diedMalnutrition - diedStarvation;
    if (populationGain >= 0)
        printw("YOUR POPULATION GAINED %d CITIZENS.\n", populationGain);
    else
        printw("YOUR POPULATION LOST %d CITIZENS.\n", -populationGain);

    /* Update population. */
    aPlayer->serfCount +=
        populationGain - merchantsImmigrated - noblesImmigrated;
    aPlayer->merchantCount += merchantsImmigrated;
    aPlayer->nobleCount += noblesImmigrated;

    /* Wait for player to be done. */
    printw("\n\n<ENTER>? ");
    getnstr(input, 80);

    /* Check if player died. */
    PlayerDeath(aPlayer);
}


/*
 * Check if any event happened that killed the player specified by aPlayer.
 *
 *   aPlayer                Player.
 */

static void PlayerDeath(Player *aPlayer)
{
    Country *country;

    /* Get the player country. */
    country = aPlayer->country;

    /* If anyone starved to death, their mother might assassinate the ruler. */
    if ((aPlayer->diedStarvation > 0) &&
        (RandRange(aPlayer->diedStarvation) > RandRange(110)))
    {
        aPlayer->dead = TRUE;
        clear();
        move(0, 0);
        printw("VERY SAD NEWS ...\n\n");
        printw("%s %s OF %s HAS BEEN ASSASSINATED\n",
            aPlayer->title,
            aPlayer->name,
            country->name);
        printw("BY A CRAZED MOTHER WHOSE CHILD HAD STARVED TO DEATH. . .\n\n");
    }

    /* Check if the player died for any other reason. */
    if (RandRange(100) == 1)
    {
        aPlayer->dead = TRUE;
        clear();
        move(0, 0);
        printw("VERY SAD NEWS ...\n\n");
        printw("%s %s ", aPlayer->title, aPlayer->name);
        switch(RandRange(4))
        {
            case 1 :
                printw("HAS BEEN ASSASSINATED BY AN AMBITIOUS\nNOBLE\n\n");
                break;

            case 2 :
                printw("HAS BEEN KILLED FROM A FALL DURING\n"
                       "THE ANNUAL FOX-HUNT.\n\n");
                break;

            case 3 :
                printw("DIED OF ACUTE FOOD POISONING.\n"
                       "THE ROYAL COOK WAS SUMMARILY EXECUTED.\n\n");
                break;

            case 4 :
            default :
                printw("PASSED AWAY THIS WINTER FROM A WEAK HEART.\n\n");
                break;
        }
    }

    /* If the player died, display the funeral. */
    if (aPlayer->dead)
    {
        printw("THE OTHER NATION-STATES HAVE SENT REPRESENTATIVES TO THE\n");
        printw("FUNERAL\n");
        refresh();
        sleep(2 * DELAY_TIME);
    }
}


