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
 */

#define COUNTRY_COUNT       6
#define TITLE_COUNT         4
#define DELAY_TIME          3


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
 *   defeated               If true, player has been defeated.
 *   land                   Land in acres.
 *   grain                  Grain reserves in bushels.
 *   treasury               Currency treasury.
 *   serfCount              Count of number of serfs.
 *   soldierCount           Count of number of soldiers.
 *   nobleCount             Count of number of nobles.
 *   merchantCount          Count of number of merchants.
 *   armyEfficiency         Efficiency of army.
 *   customsTax             Customs tax.
 *   salesTax               Sales tax.
 *   incomeTax              Income tax.
 *   foundryCount           Count of number of foundries.
 *   grainForSale           Bushels of grain for sale.
 *   grainPrice             Grain price.
 *   ratPct                 Percent of grain eaten by rats.
 *   grainHarvest           Grain harvest for year.
 */

typedef struct
{
    char                    name[80];
    int                     number;
    Country                *country;
    int                     level;
    char                    title[80];
    bool                    defeated;
    int                     land;
    int                     grain;
    int                     treasury;
    int                     serfCount;
    int                     soldierCount;
    int                     nobleCount;
    int                     merchantCount;
    int                     armyEfficiency;
    int                     customsTax;
    int                     salesTax;
    int                     incomeTax;
    int                     foundryCount;
    int                     grainForSale;
    float                   grainPrice;
    int                     ratPct;
    int                     grainHarvest;
} Player;


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
 * Macros.
 */

/*
 * Return the length of the array specified by aArray.
 *
 *   aArray                 Array to get length.
 */

#define ArraySize(aArray) (sizeof((aArray)) / sizeof((aArray)[0]))


#endif /* __EMPIRE_H__ */

