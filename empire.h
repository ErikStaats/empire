/*------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 *
 * TRS-80 Empire game header file.
 *
 *------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/

#ifndef __EMPIRE_H__
#define __EMPIRE_H__

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
 *   SERF_EFFICIENCY        Serf fighting efficiency (10 = 100%).
 */

#define COUNTRY_COUNT       6
#define TITLE_COUNT         4
#define DELAY_TIME          3
#define SERF_EFFICIENCY     5


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
 *   dead                   If true, player is dead.
 *   land                   Land in acres.
 *   grain                  Grain reserves in bushels.
 *   treasury               Currency treasury.
 *   serfCount              Count of number of serfs.
 *   soldierCount           Count of number of soldiers.
 *   soldierRevenue         Revenue from soldiers.
 *   nobleCount             Count of number of nobles.
 *   merchantCount          Count of number of merchants.
 *   immigrated             Number of people who immigrated into country.
 *   armyEfficiency         Efficiency of army.
 *   customsTax             Customs tax.
 *   customsTaxRevenue      Revenue from customs tax.
 *   salesTax               Sales tax.
 *   salesTaxRevenue        Revenue from sales tax.
 *   incomeTax              Income tax.
 *   incomeTaxRevenue       Revenue from income tax.
 *   marketplaceCount       Count of number of marketplaces.
 *   marketplaceRevenue     Revenue from marketplaces.
 *   grainMillCount         Count of number of grain mills.
 *   grainMillRevenue       Revenue from grain mills.
 *   foundryCount           Count of number of foundries.
 *   foundryRevenue         Revenue from foundries.
 *   shipyardCount          Count of number of shipyards.
 *   shipyardRevenue        Revenue from shipyards.
 *   palaceCount            Count of work done on palace.
 *   grainForSale           Bushels of grain for sale.
 *   grainPrice             Grain price.
 *   ratPct                 Percent of grain eaten by rats.
 *   grainHarvest           Grain harvest for year.
 *   peopleGrainNeed        How much grain people need for year.
 *   peopleGrainFeed        How much grain to feed people for year.
 *   armyGrainNeed          How much grain army needs for year.
 *   armyGrainFeed          How much grain to feed army for year.
 *   diedStarvation         How many people died of starvation.
 */

typedef struct
{
    char                    name[80];
    int                     number;
    Country                *country;
    int                     level;
    char                    title[80];
    bool                    dead;
    int                     land;
    int                     grain;
    int                     treasury;
    int                     serfCount;
    int                     soldierCount;
    int                     soldierRevenue;
    int                     nobleCount;
    int                     merchantCount;
    int                     immigrated;
    int                     armyEfficiency;
    int                     customsTax;
    int                     customsTaxRevenue;
    int                     salesTax;
    int                     salesTaxRevenue;
    int                     incomeTax;
    int                     incomeTaxRevenue;
    int                     marketplaceCount;
    int                     marketplaceRevenue;
    int                     grainMillCount;
    int                     grainMillRevenue;
    int                     foundryCount;
    int                     foundryRevenue;
    int                     shipyardCount;
    int                     shipyardRevenue;
    int                     palaceCount;
    int                     grainForSale;
    float                   grainPrice;
    int                     ratPct;
    int                     grainHarvest;
    int                     peopleGrainNeed;
    int                     peopleGrainFeed;
    int                     armyGrainNeed;
    int                     armyGrainFeed;
    int                     diedStarvation;
} Player;


/*
 * This structure contains fields used to managed a battle.
 *
 *   player                 Player initiating the battle.
 *   soldiersToAttackCount  Number of player soldiers to attack.
 *   soldierCount           Remaining number of player soldiers.
 *   soldierEfficiency      Efficiency of player soldiers.
 *   soldierLabel           Label to use to show remaining player soldiers.
 *   targetPlayer           Target players being attacked.
 *   targetSoldierCount     Number of target soldiers being attacked.
 *   targetSoldierEfficiency
 *                          Efficiency of target soldiers.
 *   targetSoldierLabel     Label to use to show remaining target soldiers.
 *   targetLand             Acres of land of target.
 *   targetSerfs            If true, target is defending with serfs.
 *   targetDefeated         If true, target has been defeated.
 *   targetOverrun          If true, target has been overrun.
 */

typedef struct
{
    Player                 *player;
    int                     soldiersToAttackCount;
    int                     soldierCount;
    int                     soldierEfficiency;
    char                    soldierLabel[80];
    Player                 *targetPlayer;
    int                     targetSoldierCount;
    int                     targetSoldierEfficiency;
    char                    targetSoldierLabel[80];
    int                     targetLand;
    bool                    targetSerfs;
    int                     landCaptured;
    bool                    targetDefeated;
    bool                    targetOverrun;
} Battle;


/*------------------------------------------------------------------------------
 *
 * Globals.
 */

/*
 * Country  list.
 */

extern Country countryList[COUNTRY_COUNT];


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

extern Player playerList[COUNTRY_COUNT];
extern int playerCount;
extern int year;
extern int weather;
extern int barbarianLand;
extern int done;


/*------------------------------------------------------------------------------
 *
 * Prototypes.
 */

int RandRange(int range);

void InvestmentsScreen(Player *aPlayer);

void AttackScreen(Player *aPlayer);


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


#endif /* __EMPIRE_H__ */

