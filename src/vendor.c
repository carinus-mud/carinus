#include <sys/types.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include "mud.h"




// grabbed from mud.h and stuck here to make life simpler :)
// changed the numerical defines to const int, or float, so debugging will be
// much easier if necessary - Andril
#define VENDOR_DIR "../vendor/"     // should be self explanatory
const float VENDOR_FEE = .06;       // how much you lose from a sale. 6%
const int COST_BUY_VENDOR = 400000; // 40k. edit as you like
const int LEVEL_BUY_VENDOR = 40;    // seems excessive to me
const int MAX_DEEDS = 3;            // Max number of deeds a player can hold at any one time
const int MOB_VNUM_VENDOR = 7;      // ideally something unused from limbo.are
const int ROOM_VNUM_VENSTOR = 1;    // send messed up vendor mobs here, may not really be necessary


/*
 may need to change char* argument to const char* argument
 depending on your particular code setup.
*/
void do_makeshop( CHAR_DATA* ch, const char* argument );
void do_say( CHAR_DATA* ch, const char* argument );
void do_remove( CHAR_DATA* ch, const char* argument );
void do_drop( CHAR_DATA* ch, const char* argument );


CHAR_DATA *	find_keeper( CHAR_DATA *ch );

/*
* This is the command to purchase a contract for a vendor.
*  + Must be used at a shopkeeper
*  Player command
*/
void do_buyvendor( CHAR_DATA* ch, const char* argument )
{
   if( IS_NPC( ch ) )
      return;

   if( ch->deeds >= MAX_DEEDS )
   {
      // fixed the check above and the line below to eliminate magic numbers - Andril
      ch_printf( ch, "Not a chance. You already have the deed for %d vendors!\r\n", MAX_DEEDS );
      return;
   }

   if( (ch->gold < COST_BUY_VENDOR ) )
   {
      send_to_char( "You are too poor!\r\n", ch );
      return;
   }

   send_to_char( "&GVery well, you may have a contract for a vendor.\r\n", ch );
   ch_printf( ch, "&YYou pay %d credits to purchase the vendor.&d\r\n", COST_BUY_VENDOR );
   ch->gold = ch->gold - COST_BUY_VENDOR;
   ++ch->deeds;
   save_char_obj(ch);
}


/*
* This will place the vendor, Once you have purchased one.
*  Player command
*/
void do_placevendor( CHAR_DATA* ch, const char* argument )
{
   CHAR_DATA *vendor;
   MOB_INDEX_DATA *temp;
   char buf [MAX_INPUT_LENGTH];

   if( find_keeper (ch) )
   {
      ch->send( "A vendor is already here!\r\n" );
      return;
   }

   if( IS_NPC( ch ) )
      return;

   if( ch->deeds <= 0 )
   {
      send_to_char( "You have not purchased a deed!\r\n", ch );
      return;
   }

   /* Make sure they're in a designated room */
   if( !xIS_SET(ch->in_room->room_flags, ROOM_VENDOR) )
   {
      send_to_char( "You may not place your vendor here!\r\n", ch );
      return;
   }

   if( (temp = get_mob_index(MOB_VNUM_VENDOR) ) == NULL )
   {
      log_string( "do_placevendor: no vendor mobile" );
      return;
   }

   --ch->deeds;

   vendor = create_mobile( temp );
   char_to_room( vendor, ch->in_room );
   vendor->owner = STRALLOC( ch->name );
   vendor->home = ch->in_room;

   sprintf( buf, vendor->long_descr, ch->name );
   vendor->long_descr = STRALLOC( buf );

   save_vendor( vendor );

   act( AT_ACTION, "A Vendor appears in the room before your very eyes.", vendor, NULL, NULL, TO_ROOM );

   /*
   * Makes the vendor a shop.. there has to be a better way to do it but hell if i know what it is!
   */
   sprintf( buf, "%d", vendor->pIndexData->vnum );
   do_makeshop ( vendor, buf );
}


/*
* Changes the vendors pricing
*  Player command
*/
void do_pricevendor( CHAR_DATA* ch, const char* argument )
{
   CHAR_DATA *vendor;
   OBJ_DATA *obj;
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      send_to_char( "&RSyntax: pricevendor <item> <cost>\r\n", ch );
      return;
   }

   if( ( vendor = find_keeper(ch) ) == NULL )
   {
      send_to_char( "&RThere isn't a vendor here!\r\n", ch );
      return;
   }

   if( !vendor->owner )
   {
      send_to_char( "&RThats not even a player vendor!\r\n", ch );
      return;
   }

   if( str_cmp( ch->name, vendor->owner ) )
   {
      send_to_char( "&RThis isn't your vendor!\r\n", ch );
      return;
   }

   if( ch->fighting )
   {
      send_to_char( "&RNot while you're fighting!\r\n", ch );
      return;
   }

   if( (obj = get_obj_carry( vendor, arg )) != NULL )
   {
      obj->cost = atoi( argument );
      send_to_char( "&GThe price has been changed on the item.\r\n", ch );
      save_vendor( vendor );
      return;
   }

   send_to_char( "&RYour vendor doesn't have that item!\r\n", ch );
}

/*
* Retrieve the gold from your vendor.
*  Player command.
*/
void do_collectgold( CHAR_DATA* ch, const char* argument )
{
   CHAR_DATA *vendor;
   long gold, fee;

   if( ( vendor = find_keeper (ch) ) == NULL )
   {
      send_to_char( "&RWhat Vendor?\r\n", ch );
      return;
   }

   if(vendor->owner == NULL)
   {
      send_to_char( "&RThats not a vendor!\r\n", ch );
      return;
   }

   if( str_cmp( ch->name, vendor->owner ) )
   {
      send_to_char( "&RThis is not your vendor. You can't just take other peoples money!\r\n", ch );
      return;
   }

   if( ch->fighting )
   {
      send_to_char( "&RNot while you're fighting!\r\n", ch );
      return;
   }

   if( vendor->gold <= 0 )
   {
      do_say( vendor, "Sorry, I have not earned any money yet!" );
      return;
   }

   gold = vendor->gold;
   fee = (int)(gold * VENDOR_FEE);

   send_to_char( "&GYour vendor gladly hands over his earnings, minus a small fee of course.\r\n", ch );
   ch_printf( ch, "&YYou earned %d credits, and the vendor charged you %d credits, leaving you %d credits.\r\n", gold, fee, (gold - fee) );

   act( AT_ACTION, "$n hands over some money.\r\n", vendor, NULL, NULL, TO_ROOM );

   gold -= fee;
   vendor->gold = 0;
   ch->gold += gold;

   save_vendor( vendor );
   save_char_obj( ch );
}

/*
* Sells the vendor in the room. Must be your vendor.
*  Drops all items the vendor is carrying.
*  Returns the cost of the deed minus 50%.
*  Destroys the vendor.
*  Player command
*/
void do_sellvendor( CHAR_DATA* ch, const char* argument )
{
   CHAR_DATA *vendor;
   long gold, fee;
   char buf[MAX_INPUT_LENGTH];

   if( ( vendor = find_keeper (ch) ) == NULL )
   {
      send_to_char( "&RWhat Vendor?\r\n", ch );
      return;
   }

   if(vendor->owner == NULL)
   {
      send_to_char( "&RThats not a vendor!\r\n", ch );
      return;
   }

   if( str_cmp( ch->name, vendor->owner ) )
   {
      send_to_char( "&RThis is not your vendor. You can only sell your own vendors.\r\n", ch );
      return;
   }

   if( ch->fighting )
   {
      send_to_char( "&RNot while you're fighting!\r\n", ch );
      return;
   }

   /*
   * Drop all items
   */
   do_remove( vendor, "all" );
   do_drop( vendor, "all" );

   /*
   * Return 50% of the deed value, Plus any money on the vendor.
   */
   gold = vendor->gold;
   if( gold > 0 )
   {
      fee = (int)( gold * VENDOR_FEE );
      ch_printf( ch, "&YYou collect %d credits from the vendor, minus a small fee of %d credits.\r\n", gold, fee );
      ch->gold += gold;
   }       
   ch_printf( ch, "&w&YYou collect %d credits, half of the original deed cost.\r\n", (int) (COST_BUY_VENDOR / 2) );
   ch->gold += (COST_BUY_VENDOR/2);

   /*
   * Destroy the vendor
   */
   sprintf( buf, "%s%s.%d", VENDOR_DIR, capitalize( vendor->owner ), vendor->in_room->vnum );
   bug( "Player vendor was sold. Destroying %s.", buf );
   if( !remove( buf ) )
   {
      bug("Player vendor was successfully destroyed." );
   }
   else if( errno != ENOENT )
   {
      bug("Failed to destroy vendor data! Notify an Immortal!" );
   }  
   extract_char( vendor, true );

   send_to_char( "&GYour vendor has been sold. Have a nice day.\r\n", ch );
}

/* Write vendor to file */
void fwrite_vendor( FILE *fp, CHAR_DATA *mob )
{
   if( !IS_NPC( mob ) || !fp )
      return;

   if(mob->gold > 0)
      fprintf (fp, "Gold     %d\n",mob->gold);
   if( mob->home )
      fprintf( fp, "Home     %d\n", mob->home->vnum );
   if(mob->owner != NULL )
      fprintf (fp, "Owner     %s~\n", mob->owner );
   fprintf( fp, "LDesc      %s~\n", mob->long_descr );
   fprintf( fp, "Position   %d\n", mob->position );
   fprintf( fp, "Flags   %d\n", mob->act );
   fprintf( fp, "END\n" );
}


/* Read vendor from file */
CHAR_DATA * fread_vendor( FILE *fp )
{
   CHAR_DATA *vendor = NULL;

   const char *word;
   bool fMatch;
   int inroom = 0;

   word = feof( fp ) ? "END" : fread_word( fp );

   for( ; ;)
   {
      word   = feof( fp ) ? "END" : fread_word( fp );
      fMatch = false;
      switch( UPPER(word[0]) )
      {
      case '*':
         fMatch = true;
         fread_to_eol( fp );
         break;
      case '#':
         if( !strcmp( word, "#OBJECT" ) )
         {
            fread_obj ( vendor, fp, OS_CARRY );
         }
         break;
      case 'E':
         if( !strcmp( word, "END" ) )
         {
            char buf [MAX_INPUT_LENGTH];
            ROOM_INDEX_DATA* dest = NULL;
            CHAR_DATA *victim;
            CHAR_DATA *vnext;

            if( inroom == 0 )
               dest = get_room_index( ROOM_VNUM_VENSTOR );
            else
               dest = get_room_index( inroom );

            for( victim = vendor->in_room->first_person; victim; victim = vnext )
            {
               vnext = victim->next_in_room;
               if(victim->home != NULL)
               {
                  extract_char( victim, true);
                  break;
               }        
            }

            char_to_room( vendor, dest );
            sprintf( buf, "%d", vendor->pIndexData->vnum );
            do_makeshop (vendor, buf );
            vendor->hit = 10000;
            vendor->max_hit = 10000;
            return vendor;
         }
         break;
      case 'F':
         KEY( "Flags", vendor->act, fread_number(fp));
         break;
      case 'G':
         KEY("Gold", vendor->gold, fread_number(fp));
         break;
      case 'H':
         KEY("Home", inroom, fread_number(fp) );
         break;
      case 'L':
         KEY( "LDesc", vendor->long_descr, fread_string(fp));
         break;
      case 'O':
         KEY ("Owner", vendor->owner, fread_string (fp) );
         break;
      case 'P':
         KEY( "Position", vendor->position, fread_number( fp ) );
         break;
      case 'S':
         KEY( "Short", vendor->short_descr, fread_string(fp));
         break;
      default:
         break;
      }

      if( !fMatch )
      {
         bug ( "fread_vendor: no match." );
         bug ( word, 0 );
      }
   }
   return NULL;
}

void save_vendor( CHAR_DATA *ch )
{
   char strsave[MAX_INPUT_LENGTH];
   /* char strback[MAX_INPUT_LENGTH]; */
   FILE *fp;
   int vnum=0;

   if( !ch )
   {
      bug( "Save_char_obj: null ch!", 0 );
      return;
   }

   de_equip_char( ch );

   if( ch->in_room )
      vnum = ch->in_room->vnum;

   sprintf( strsave, "%s%s.%d", VENDOR_DIR, capitalize( ch->owner ), vnum );

   // bug( "Save_Vendor: Vendor %s saving...", strsave );

   if( ( fp = fopen( strsave, "w" ) ) == NULL )
   {
      perror( strsave );
      bug( "Save_vendor: fopen" );
   }
   else
   {
/*
      bool ferr;

      fchmod(fileno(fp), S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH);
*/
      fprintf( fp, "#VENDOR\n"		);
      fwrite_vendor( fp );

      if( ch->first_carrying )
         fwrite_obj( ch, ch->last_carrying, fp, 0, OS_CARRY, false );

      fprintf(fp, "#END\n" );
/*
      ferr = ferror(fp);
      fclose( fp );
      if(ferr)
      {
         perror(strsave);
         bug("Error writing temp file for %s -- not copying", strsave);
      }
      else
         rename(TEMP_FILE, strsave);
*/
   }

   re_equip_char( ch );
}

/*
* Syntax: NAMESHOP <new name>
*  Allows you to change the name of the shop
*  Player command
*/
void do_nameshop( CHAR_DATA* ch, char* argument )
{
   ROOM_INDEX_DATA *room;
   CHAR_DATA *vendor;

   if( IS_NPC( ch ) )
      return;

   if( ( vendor = find_keeper (ch) ) == NULL )
   {
      send_to_char( "&RThis is not an owned shop.\r\n", ch );
      return;
   }

   if(vendor->owner == NULL)
   {
      send_to_char( "&RThat's not a player-owned vendor!\r\n", ch );
      return;
   }

   if( str_cmp( ch->name, vendor->owner ) )
   {
      send_to_char( "&RThis is not your store. NAMESHOP only works inside *YOUR* shops.\r\n", ch );
      return;
   }

   room = ch->in_room;

   if( !xIS_SET( room->room_flags, ROOM_VENDOR ) )
   {
      send_to_char( "&RYou're not inside a shop! Contact an Immortal. This guy is roaming.\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "&RSyntax: NAMESHOP <shop name>\r\n", ch );
      return;
   }

   STRFREE( room->name );
   room->name = STRALLOC( argument );

   send_to_char( "You have successfully renamed your store.\r\n", ch );

   fold_area( room->area, room->area->filename, false );
}


void slay_vendors( const char * name )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA * ch;
   CHAR_DATA * tch;
   int cnt=0;

   for( ch = first_char; ch; ch = tch )
   {
      tch = ch->next;

      if( !ch->owner )
         continue;

      if( IS_NPC( ch ) )
      {
         if( !str_cmp( ch->owner, name ) )
         {
            raw_kill( ch );
            cnt++;
         }
      }
   }

   sprintf( buf, "Slay_vendors: Destroyed %d of %s's vendors.\r\n", cnt, name );
   if( cnt > 0 )
      bug( buf );
}


void load_vendors()
{
   DIR *dp;
   CHAR_DATA *mob;
   struct dirent *de;

   if( !(dp = opendir(VENDOR_DIR)) )
   {
      bug( "Load_vendors: can't open VENDOR_DIR" );
      perror(VENDOR_DIR);
      return;
   }

   falling = 1;
   while( (de = readdir(dp)) != NULL )
   {
      if( de->d_name[0] != '.' )
      {
         sprintf(strArea, "%s%s", VENDOR_DIR, de->d_name );
         /*fprintf(stderr, "Vendor -> %s\n", strArea);*/
         if( !(fpArea = fopen(strArea, "r")) )
         {
            perror(strArea);
            continue;
         }
         for( ; ; )
         {
            char letter;
            char *word;

            letter = fread_letter( fpArea );
            if( letter == '*' )
            {
               fread_to_eol(fpArea);
               continue;
            }
            if( letter != '#' )
            {
               bug( "Load_vendor: # not found.", 0 );
               break;
            }
            word = fread_word( fpArea );
            if( !strcmp(word, "VENDOR" ) )
               mob = fread_vendor( fpArea );
            else if( !strcmp(word, "OBJECT" ) )
               fread_obj( mob, fpArea, OS_CARRY );
            else if( !strcmp( word, "END" ) )
               break;
         }
         fclose(fpArea);
      }
   }
   fpArea = NULL;
   strcpy(strArea, "$");
   closedir(dp);
   falling = 0;
   return;
}

