/**********************************************************************

               AAA                  GGGGGGGGGGGGGEEEEEEEEEEEEEEEEEEEEEE
              A:::A              GGG::::::::::::GE::::::::::::::::::::E
             A:::::A           GG:::::::::::::::GE::::::::::::::::::::E
            A:::::::A         G:::::GGGGGGGG::::GEE::::::EEEEEEEEE::::E
           A:::::::::A       G:::::G       GGGGGG  E:::::E       EEEEEE
          A:::::A:::::A     G:::::G                E:::::E             
         A:::::A A:::::A    G:::::G                E::::::EEEEEEEEEE   
        A:::::A   A:::::A   G:::::G    GGGGGGGGGG  E:::::::::::::::E   
       A:::::A     A:::::A  G:::::G    G::::::::G  E:::::::::::::::E   
      A:::::AAAAAAAAA:::::A G:::::G    GGGGG::::G  E::::::EEEEEEEEEE   
     A:::::::::::::::::::::AG:::::G        G::::G  E:::::E             
    A:::::AAAAAAAAAAAAA:::::AG:::::G       G::::G  E:::::E       EEEEEE
   A:::::A             A:::::AG:::::GGGGGGGG::::GEE::::::EEEEEEEE:::::E
  A:::::A               A:::::AGG:::::::::::::::GE::::::::::::::::::::E
 A:::::A                 A:::::A GGG::::::GGG:::GE::::::::::::::::::::E
AAAAAAA                   AAAAAAA   GGGGGG   GGGGEEEEEEEEEEEEEEEEEEEEEE

Asylum Gaming Engine
by Joseph Fogas

Command Module
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"


void do_nohelps(CHAR_DATA *ch, const char *argument)
{
  CMDTYPE *command;
  AREA_DATA *tArea;
  char arg[MAX_STRING_LENGTH];
  int hash, col=0, sn=0;
  
   argument = one_argument( argument, arg );

   if(!IS_IMMORTAL(ch) || IS_NPC(ch) )
    {
	send_to_char("Huh?\n\r", ch);
	return;
    }

   if ( arg[0] == '\0' || !str_cmp(arg, "all") )
   {
      do_nohelps(ch, "commands");
      send_to_char( "\n\r", ch);
      do_nohelps(ch, "skills");
      send_to_char( "\n\r", ch);
      do_nohelps(ch, "areas");
      send_to_char( "\n\r", ch);
      return;
   }
  
  if(!str_cmp(arg, "commands") )
  {
    send_to_char_color("&C&YCommands for which there are no help files:\n\r\n\r", ch);

    for ( hash = 0; hash < 126; hash++ )
     {
       for ( command = command_hash[hash]; command; command = command->next )
	{
	      	if(!get_help(ch, command->name) )
	      	{
	   	  ch_printf_color(ch, "&W%-15s", command->name);
	   	  if ( ++col % 5 == 0 )
	   	      {
			send_to_char( "\n\r", ch );
		      }
	      	}
	}
      }

	send_to_char("\n\r", ch);
	return;
   }

   if(!str_cmp(arg, "skills") || !str_cmp(arg, "spells") )
   {
     send_to_char_color("&CSkills/Spells for which there are no help files:\n\r\n\r", ch);
    for ( sn = 0; sn < 999 && skill_table[sn] && skill_table[sn]->name; sn++ )
     {
   	if(!get_help(ch, skill_table[sn]->name))
   	{
   	   ch_printf_color(ch, "&W%-20s", skill_table[sn]->name);
   	   if ( ++col % 4 == 0 )
   	     {
   	     	send_to_char("\n\r", ch);
   	     }
   	}
     }
       
	send_to_char("\n\r", ch);	
	return;
   }

   if(!str_cmp(arg, "areas") )
   {
        send_to_char_color("&GAreas for which there are no help files:\n\r\n\r", ch);
   
        for (tArea = first_area; tArea;tArea = tArea->next)
        {
    	  if(!get_help(ch, tArea->name) )
    	   {
    	      ch_printf_color(ch, "&W%-35s", tArea->name);
    	      if ( ++col % 2 == 0 )
    	       {
    	     	send_to_char("\n\r", ch);
    	       }
    	   }
        }

	   send_to_char( "\n\r", ch);
	   return;
    }

	send_to_char("Syntax:  nohelps <all|areas|commands|skills>\n\r", ch);
	return;
}




void do_decapitate( CHAR_DATA* ch, const char* argument)
{
   OBJ_INDEX_DATA *korps;
   OBJ_DATA *corpse;
   OBJ_DATA *obj;
   OBJ_DATA *head;
   const char *name;
   char buf[MAX_STRING_LENGTH];

   if( !IS_PKILL( ch ) && !IS_IMMORTAL( ch ) )
   {
      send_to_char( "Leave the hideous defilings to the killers!\n", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Whose corpse do you wish to behead?\r\n", ch );
      return;
   }

   if( ( corpse = get_obj_here( ch, argument ) ) == NULL )
   {
      send_to_char( "You cannot find that here.\r\n", ch );
      return;
   }

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You have no weapon with which to perform this deed.\r\n", ch );
      return;
   }

   if( ( obj->value[3] != 1 && obj->value[3] != 2 && obj->value[3] != 3 && obj->value[3] != 11 ) || IS_OBJ_STAT( corpse, ITEM_DECAPITATED ) )
   {
      send_to_char( "There is nothing you can do with this corpse.\r\n", ch );
      return;
   }
   if( !IS_IMMORTAL( ch ) )
   {
      if( IS_OBJ_STAT( corpse, ITEM_CLANCORPSE ) )
      {
         if( corpse->action_desc && is_name( ch->name, corpse->action_desc ) );
         else
         {
            send_to_char( "There is nothing you can do with this corpse.\r\n", ch );
            return;
         }
      }
      else
      {
         send_to_char( "There is nothing you can do with this corpse.\r\n", ch );
         return;
      }
   }

   if( get_obj_index( OBJ_VNUM_HEAD ) == NULL )
   {
      bug( "Vnum %d (OBJ_VNUM_HEAD) not found for do_decapitate!", OBJ_VNUM_HEAD );
      return;
   }

   if( !( korps = get_obj_index( OBJ_VNUM_CORPSE_PC ) ) )
   {
      bug( "Vnum %d (OBJ_VNUM_CORPSE_PC) not found for %s!", OBJ_VNUM_CORPSE_PC, __func__ );
      return;
   }

   head = create_object( get_obj_index( OBJ_VNUM_HEAD ), 0 );
   name = IS_NPC( ch ) ? korps->short_descr : corpse->short_descr;
   snprintf( buf, MAX_STRING_LENGTH, head->short_descr, name );
   STRFREE( head->short_descr );
   head->short_descr = STRALLOC( buf );
   snprintf( buf, MAX_STRING_LENGTH, head->description, name );
   STRFREE( head->description );
   head->description = STRALLOC( buf );
   act( AT_BLOOD, "$n takes the head of $p.", ch, corpse, NULL, TO_ROOM );
   act( AT_BLOOD, "You take the head of $p.", ch, corpse, NULL, TO_CHAR );
   separate_obj( corpse );
   xSET_BIT( corpse->extra_flags, ITEM_DECAPITATED );
   obj_to_char( head, ch );
   return;
}



void do_skills( CHAR_DATA* ch, const char* )
{
   int sn;



      int col;
      short lasttype, cnt;
      bool is_ok;

      col = cnt = 0;
      lasttype = SKILL_SPELL;
      set_pager_color( AT_MAGIC, ch );

      for( sn = 0; sn < num_skills; ++sn )
      {
         const SKILLTYPE *skill;
         int normalSn;

         // the first num_sorted_skills are sorted by type, so we don't want
         // to index into skill_table -- that is sorted alphabetically -- so
         // we need to do the following dance to check if we are in the
         // sorted section or the unsorted section.
         if( sn < num_sorted_skills )
         {
            skill = skill_table_bytype[sn];

            // we are looping over the skills sorted by type.
            // So, we need to get the normal sn as well.
            normalSn = skill_lookup( skill->name );
         }
         else
         {
            skill = skill_table[sn];
            normalSn = sn;
         }

         if( !skill || !skill->name || skill->name[0] == '\0' )
            continue;

		if (IS_IMMORTAL(ch) || !CAN_CAST(ch))
         if( strcmp( skill->name, "reserved" ) == 0 && ( IS_IMMORTAL( ch ) || !CAN_CAST( ch ) ) )
         {
            if( col % 3 != 0 )
               send_to_pager( "\r\n", ch );
            send_to_pager_color( "\r\n &R[&WSpells&R]&z\r\n",
                                 ch );
            col = 0;

         }
         if( skill->type != lasttype )
         {
            if( !cnt )
               send_to_pager( "\r\n", ch );
            else if( col % 3 != 0 )
               send_to_pager( "\r\n", ch );
            pager_printf_color( ch,
                                " &R[&W%ss&R]&z\r\n",
                                skill_tname[skill->type] );
            col = cnt = 0;
         }
         lasttype = skill->type;

      if( !IS_IMMORTAL( ch )
             && ( skill->guild != CLASS_NONE && ( !IS_GUILDED( ch ) || ( ch->pcdata->clan->Class != skill->guild ) ) ) )
            continue;

         {
            is_ok = FALSE;

            if( ch->level >= skill->skill_level[ch->Class] )
               is_ok = TRUE;
            if( ch->level >= skill->race_level[ch->race] )
               is_ok = TRUE;

            if( !is_ok )
               continue;
         }

         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill, SF_SECRETSKILL ) )
            continue;
// skip spells we haven't learned
if ( ch->pcdata->learned[normalSn] <= 0 )
  continue;
         ++cnt;
         pager_printf( ch, "%18.18s", skill->name );
	if (ch->pcdata->learned[normalSn] > 0)  
       pager_printf( ch, " %3d%% ", ch->pcdata->learned[normalSn] );

//         if( ch->pcdata->learned[normalSn] > 99 )
//        send_to_char( " &W[&GX&W]&z", ch);
//else
//         if( ch->pcdata->learned[normalSn] < 99 )
//        send_to_char( " &W[ ]&z", ch);
         if( ++col % 3 == 0 )
            send_to_pager( "\r\n", ch );
      }

      if( col % 3 != 0 )
         send_to_pager( "\r\n", ch );

}



/*
void do_soulfeed( CHAR_DATA* ch, const char* )
{
   OBJ_DATA *obj;

  if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL  )
   {
      send_to_char( "You must be holding the weapon you wish to feed.\r\n", ch );
      return;
   }
  if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL && ( obj->value[4] >= 4 ))
   {
      send_to_char( "You have fed this weapon as much as you can.\r\n", ch );
      return;
   }
  if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL && ( obj->value[4] <= 3 ))
   {

        if ( ch->pcdata->souls <= 1999)
        {
                send_to_char( "You do not have the souls\r\n", ch );
                return;
        }
        else
        obj->value[4] += 1;
        ch->pcdata->souls -= 2000;
      send_to_char( "The souls of the vanquished fuse into this weapon.\r\n", ch );
      return;
   }


}
*/
void do_cpose( CHAR_DATA* ch, const char* argument)
{
   if( IS_NPC( ch ) )
      return;

   set_char_color( AT_SCORE, ch );

   if( IS_SET( ch->pcdata->flags, PCFLAG_NOCPOSE ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The Gods prohibit you from changing your cpose.\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Change your cpose to what?\r\n", ch );
      return;
   }

 if( !str_cmp( argument, "none" ) )

   {
      send_to_char( "Cpose cleared.\r\n", ch );
      ch->pcdata->cpose = str_dup( " is here." );
      return;
   }

   char cpose[50];
   mudstrlcpy(cpose, argument, 50);

   set_cpose( ch, cpose );
   send_to_char( "Ok.\r\n", ch );
}


void set_cpose( CHAR_DATA * ch, const char *cpose )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
   {
      bug( "%s: NPC.", __func__ );
      return;
   }

   if( isalpha( cpose[0] ) || isdigit( cpose[0] ) )
   {
      buf[0] = ' ';
      mudstrlcpy( buf + 1, cpose, MAX_STRING_LENGTH - 1 );
   }
   else
      mudstrlcpy( buf, cpose, MAX_STRING_LENGTH );

   ch->pcdata->cpose = STRALLOC( buf );
}



void do_customclass( CHAR_DATA* ch, const char* argument)
{
   if( IS_NPC( ch ) )
      return;

   set_char_color( AT_SCORE, ch );


   if( argument[0] == '\0' )
   {
      send_to_char( "Change your class to what?\r\n", ch );
      return;
   }

 if( !str_cmp( argument, "none" ) )

   {
      send_to_char( "Class set to Adventurer.\r\n", ch );
      ch->pcdata->customclass = str_dup( "Adventurer" );
      return;
   }

   char customclass[15];
   mudstrlcpy(customclass, argument, 15);

   set_customclass( ch, customclass );
   send_to_char( "Ok.\r\n", ch );
}


void set_customclass( CHAR_DATA * ch, const char *customclass )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
   {
      bug( "%s: NPC.", __func__ );
      return;
   }

   if( isalpha( customclass[0] ) || isdigit( customclass[0] ) )
   {
      buf[0] = ' ';
      mudstrlcpy( buf + 1, customclass, MAX_STRING_LENGTH - 1 );
   }
   else
      mudstrlcpy( buf, customclass, MAX_STRING_LENGTH );

   ch->pcdata->customclass = STRALLOC( buf );
}


void do_newscore( CHAR_DATA* ch, const char* argument )
{

	if (IS_NPC(ch))
	{
	send_to_char("Mobiles do not have score sheets. Use Mstat.\r\n", ch);
	return;
	}

   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
//   CLAN_DATA *pClan = NULL;

//   AFFECT_DATA *paf;

send_to_char( "\r\n&r+===========================================================================+\n\r", ch);
pager_printf( ch, "&r|&W %-15s Level %-2d %-12s Hometown: %-19s       &r|\r\n",
        ch->name, ch->level, capitalize( get_race(ch)), ch->pcdata->hometown );
send_to_char( "&r+===========================================================================+\n\r", ch);
pager_printf( ch, "&r| &CFaction: &W%-15s     &CTier: &W%d           &CBleed Counters: &R%-3d        &r|\r\n",
        ch->pcdata->clan_name, ch->pcdata->tier, ch->pcdata->condition[COND_BLEEDING]);
pager_printf( ch, "&r| &CRank:    &W%-15s    &CAlign: &W%-5d       &CAP: &W%-5d                  &r|\r\n",
	ch->pcdata->rank, ch->alignment, ch->pcdata->ap);
pager_printf( ch, "&r| &CWorships: &W%-15s   &CFavor: &W%-5d       &CArmor: &W%-4d                &r|\r\n",
        ch->pcdata->deity ? ch->pcdata->deity->name : "(none)", ch->pcdata->favor, GET_AC(ch));
pager_printf( ch, "&r| &CHitroll: &W%-3d              &CDamroll: &W%-3d                                    &r|\r\n",
	GET_HITROLL(ch), GET_DAMROLL(ch));
send_to_char( "&r+=============================+=============================================+\n\r", ch);
pager_printf( ch, "&r| &CStrength:      &W%2d(%2d)       &r|             &CHealth: &W%-5d/%-5d             &r|\r\n",
        get_curr_str( ch ), ch->perm_str, ch->hit, ch->max_hit);
pager_printf( ch, "&r| &CDexterity:     &W%2d(%2d)       &r|         ", get_curr_dex(ch), ch->perm_dex);

send_to_char( "&W[",ch);
int start;
float value = 25.0 * ((float) ch->hit / (float) ch->max_hit) ;
                          for (start = 1; start <= 25; start++ )

                                {
                          if (start <=  value )
                                        send_to_char("&G|",ch);
                                  else
                                        send_to_char(" ",ch);
                                }
                    send_to_char("&W]&r         |\n\r",ch);
pager_printf( ch, "&r| &CConstitution:  &W%2d(%2d)       &r|              &CMana:  &W%-5d/%-5d             &r|\r\n",
	get_curr_con(ch), ch->perm_con, ch->mana, ch->max_mana); 
pager_printf(ch, "&r| &CIntelligence:  &W%2d(%2d)       &r|         ", get_curr_int(ch), ch->perm_int);

send_to_char("&W[",ch);
value = 25.0 * ((float) ch->mana / (float) ch->max_mana) ;
                          for (start = 1; start <= 25; start++ )

                                {
                          if (start <=  value )
                                        send_to_char("&C|",ch);
                                  else
                                        send_to_char(" ",ch);
                                }
                    send_to_char("&W]&r         |\n\r",ch);

pager_printf( ch, "&r| &CWisdom:        &W%2d(%2d)       &r|            &CMovement:  &W%-5d/%-5d           &r|\r\n",
        get_curr_wis( ch ), ch->perm_wis, ch->move, ch->max_move);
pager_printf(ch, "&r| &CCharisma:      &W%2d(%2d)       &r|         ", get_curr_cha(ch), ch->perm_cha);
send_to_char("&W[",ch);
value = 25.0 * ((float) ch->move / (float) ch->max_move) ;
                          for (start = 1; start <= 25; start++ )
                                {
                          if (start <=  value )
                                        send_to_char("&P|",ch);
                                  else
                                        send_to_char(" ",ch);
                                }
                    send_to_char("&W]&r         &r|\n\r",ch);

pager_printf( ch, "&r| &CLuck:          &W%2d(%2d)       &r|            &CExp TNL: &W%-15s         &r|\r\n",
        get_curr_lck( ch ), ch->perm_lck,  num_punct( exp_level( ch, ch->level + 1 ) - ch->exp ));
pager_printf( ch, "&r|                             &r|         ");
send_to_char("&W[",ch);
value = 25.0 * (((float) ch->exp )/((float) exp_level( ch, ch->level + 1 ) + ch->exp )) ;
                          for (start = 1; start <= 25; start++ )

                                {
                          if (start <=  value )
                                        send_to_char("&w|",ch);
                                  else
                                        send_to_char(" ",ch);
                                }
                    send_to_char("&W]&r         |\n\r",ch);
pager_printf( ch, "&r| &CPractices:     &W%-5d        &r|                                             |  \r\n", ch->practice);
send_to_char( "&r+=============================+=============================================+\n\r", ch);
send_to_char( "&r|&CFeats:&r                                                                     |\n\r", ch);
send_to_char( "&r|                                                                           |\n\r", ch);
pager_printf( ch, "&r|          &YDeftness: &W%d             &YMight: &W%d         &YOmniscience: &W%d          &r|\n\r"
		,ch->pcdata->deftness, ch->pcdata->might, ch->pcdata->omniscience);
pager_printf( ch, "&r|         &YIngenuity: &W%d             &YKarma: &W%d              &YCombat: &W%d          &r|\n\r"
		,ch->pcdata->ingenuity, ch->pcdata->karma, ch->pcdata->combat);
pager_printf( ch, "&r|       &zBlack Magic: &W%d         &RRed Magic: &W%d         &GGreen Magic: &W%d          &r|\n\r"
		,ch->pcdata->blackmagic, ch->pcdata->redmagic, ch->pcdata->greenmagic);
pager_printf( ch, "&r|        &CBlue Magic: &W%d       &WWhite Magic: &W%d        &PArcana Magic: &W%d          &r|\n\r"
		,ch->pcdata->bluemagic, ch->pcdata->whitemagic, ch->pcdata->arcanamagic);



send_to_char( "&r|                                                                           |\n\r", ch);
send_to_char( "&r+===========================================================================+\n\r", ch);

   switch ( ch->position )
   {
      case POS_DEAD:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "slowly decomposing" );
         break;
      case POS_MORTAL:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "mortally wounded" );
         break;
      case POS_INCAP:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "incapacitated" );
         break;
      case POS_STUNNED:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "stunned" );
         break;
      case POS_SLEEPING:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "sleeping" );
         break;
      case POS_RESTING:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "resting" );
         break;
      case POS_STANDING:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "standing" );
         break;
      case POS_FIGHTING:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "fighting" );
         break;
      case POS_EVASIVE:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "fighting (evasive)" );   /* Fighting style support -haus */
         break;
      case POS_DEFENSIVE:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "fighting (defensive)" );
         break;
      case POS_AGGRESSIVE:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "fighting (aggressive)" );
         break;
      case POS_BERSERK:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "fighting (berserk)" );
         break;
      case POS_MOUNTED:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "mounted" );
         break;
      case POS_SITTING:
         snprintf( buf, MAX_STRING_LENGTH, "%s", "sitting" );
         break;
   }



   switch ( ch->style )
   {
      case STYLE_EVASIVE:
         snprintf( buf2, MAX_STRING_LENGTH, "%s", "evasive" );
         break;
      case STYLE_DEFENSIVE:
         snprintf( buf2, MAX_STRING_LENGTH, "%s", "defensive" );
         break;
      case STYLE_AGGRESSIVE:
         snprintf( buf2, MAX_STRING_LENGTH, "%s", "aggressive" );
         break;
      case STYLE_BERSERK:
         snprintf( buf2, MAX_STRING_LENGTH, "%s", "berserk" );
         break;
      default:
         snprintf( buf2, MAX_STRING_LENGTH, "%s", "standard" );
         break;
   }
   pager_printf( ch, "&r| &CPosition:&W %-15s          &CStance:&W   %-15s              &r|\r\n", buf, buf2 );

   pager_printf( ch, "&r| &CYour Cpose is set to: &W%-40s            &r|\r\n", ch->pcdata->cpose );
send_to_char( "&r+===========================================================================+\n\r", ch);


      pager_printf( ch, "&r| &CPKILL DATA:  Pkills &W(%3.3d)     &CIllegal Pkills &W(%3.3d)     &CPdeaths &W(%3.3d)      &r|\r\n",
                    ch->pcdata->pkills, ch->pcdata->illegal_pk, ch->pcdata->pdeaths );
send_to_char( "&r+===========================================================================+\n\r", ch);



   if( IS_IMMORTAL( ch ) )
   {
      pager_printf( ch, "&r| &YIMMORTAL DATA:  Wizinvis [%s]  Wizlevel (%-2d)                               &r|\r\n",
                    xIS_SET( ch->act, PLR_WIZINVIS ) ? "X" : " ", ch->pcdata->wizinvis );

      pager_printf( ch, "&r| &YBamfin:  %-60s     &r|\r\n", ( ch->pcdata->bamfin[0] != '\0' )
                    ? ch->pcdata->bamfin : "An immortal appears." );
      pager_printf( ch, "&r| &YBamfout: %-60s     &r|\r\n", ( ch->pcdata->bamfout[0] != '\0' )
                    ? ch->pcdata->bamfout : "The immortal disappears." );
send_to_char( "&r+===========================================================================+\n\r", ch);

  }
      if( ch->pcdata->area )
      {
         pager_printf( ch, "Vnums:   Room (%-5.5d - %-5.5d)   Object (%-5.5d - %-5.5d)   Mob (%-5.5d - %-5.5d)\r\n",
                       ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
                       ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum,
                       ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum );
         pager_printf( ch, "Area Loaded [%s]\r\n", ( IS_SET( ch->pcdata->area->status, AREA_LOADED ) ) ? "yes" : "no" );
      }


}





void do_apbuy( CHAR_DATA* ch, const char* argument )

{
  int AP = ch->pcdata->ap;
 


 if( !str_cmp( argument, "\0" ) )
	{
		send_to_char ("Usage:  apbuy <item> \rItems can be any of the following:\r\n\r\n", ch);
		send_to_char ("Strength Agility Intelligence Luck HP Mana Stamina\r\n\r\n", ch);

		send_to_char ( "Costs for stats currently are:\r", ch);
		send_to_char ( "Strength     - 30AP/pt\r", ch);
		send_to_char ( "Agility      - 30AP/pt\r", ch);
		send_to_char ( "Intelligence - 20AP/pt\r", ch);
		send_to_char ( "Luck         - 30AP/pt\r", ch);
		send_to_char ( "HP           - 50AP/30pts\r", ch);
		send_to_char ( "Mana         - 50AP/20pts\r", ch);
		send_to_char ( "Stamina      - 30AP/15pts\r", ch);


	}



 if( !str_cmp( argument, "strength" ) )
	{
			if (ch->perm_str >= 25)
			{
			send_to_char( "Your stat is already maxed\r\n", ch);
			return;
			}
			
		if (AP < 30)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 30)
			{
			send_to_char( "Raising Strength by 1!", ch);
			ch->perm_str++;
			ch->pcdata->ap = AP-30;
			return;
			}
	}
 if( !str_cmp( argument, "intelligence" ) )
	{
			if (ch->perm_int >= 25)
			{
			send_to_char( "Your stat is already maxed\r\n", ch);
			return;
			}
			
		if (AP < 30)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 30)
			{
			send_to_char( "Raising Strength by 1!", ch);
			ch->perm_int++;
			ch->pcdata->ap = AP-30;
			return;
			}
	}

 if( !str_cmp( argument, "wisdom" ) )
	{
			if (ch->perm_wis >= 25)
			{
			send_to_char( "Your stat is already maxed\r\n", ch);
			return;
			}
			
		if (AP < 30)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 30)
			{
			send_to_char( "Raising Strength by 1!", ch);
			ch->perm_wis++;
			ch->pcdata->ap = AP-30;
			return;
			}
	}
 if( !str_cmp( argument, "luck" ) )
	{
			if (ch->perm_lck >= 25)
			{
			send_to_char( "Your stat is already maxed\r\n", ch);
			return;
			}
			
		if (AP < 30)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 30)
			{
			send_to_char( "Raising Strength by 1!", ch);
			ch->perm_lck++;
			ch->pcdata->ap = AP-30;
			return;
			}
	}
 if( !str_cmp( argument, "constitution" ) )
	{
			if (ch->perm_con >= 25)
			{
			send_to_char( "Your stat is already maxed\r\n", ch);
			return;
			}
			
		if (AP < 30)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 30)
			{
			send_to_char( "Raising Strength by 1!", ch);
			ch->perm_con++;
			ch->pcdata->ap = AP-30;
			return;
			}
	}

 if( !str_cmp( argument, "charisma" ) )
	{
			if (ch->perm_cha >= 25)
			{
			send_to_char( "Your stat is already maxed\r\n", ch);
			return;
			}
			
		if (AP < 30)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 30)
			{
			send_to_char( "Raising Strength by 1!", ch);
			ch->perm_cha++;
			ch->pcdata->ap = AP-30;
			return;
			}
	}


 if( !str_cmp( argument, "hp" ) )
	{
		if (AP < 50)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 50)
			{
			send_to_char( "Raising Hitpoints by 30!", ch);
			ch->max_hit += 30;
			ch->pcdata->ap = AP-50;
			return;
			}
	}

 if( !str_cmp( argument, "mana" ) )
	{
		if (AP < 50)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 50)
			{
			send_to_char( "Raising Mana by 30!", ch);
			ch->max_mana += 20;
			ch->pcdata->ap = AP-50;
			return;
			}
	}
 if( !str_cmp( argument, "stamina" ) )
	{
		if (AP < 30)
			{
			send_to_char( "Not enough AP\r", ch);
			return;
			}
		if (AP >= 30)
			{
			send_to_char( "Raising Stamina by 15!", ch);
			ch->max_move += 15;
			ch->pcdata->ap = AP-30;
			return;
			}

	send_to_char( "You cannot buy that\r\n", ch);
	}





}


void do_invade( CHAR_DATA *ch , const char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    AREA_DATA *tarea;
    int count, created;
    bool found=FALSE;
    MOB_INDEX_DATA *pMobIndex;
    ROOM_INDEX_DATA *location;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    count = atoi( arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Invade <area> <# of invaders> <mob vnum>\n\r", ch );
	return;
    }
    for ( tarea = first_area; tarea; tarea = tarea->next )
	if ( !str_cmp( tarea->filename, arg1 ) )
	{
	  found = TRUE;
	  break;
	}
    if ( !found )
    {
	send_to_char( "Area not found.\n\r", ch );
	return;
    }
    if ( count > 300)
    {
	send_to_char( "Whoa...Less than 300 please.\n\r", ch );
	return;
    }
    if ( ( pMobIndex = get_mob_index( atoi(arg3) ) ) == NULL )
    {
	send_to_char( "No mobile has that vnum.\n\r", ch );
	return;
    }

    for ( created=0; created < count; created++ )
    {
	if ( (location = get_room_index(number_range(tarea->low_r_vnum, tarea->hi_r_vnum ))) == NULL )
        {
          --created;
	  continue;
        }
	if (xIS_SET( location->room_flags, ROOM_SAFE))
        {
          --created;
	  continue;
        }
        victim = create_mobile( pMobIndex );
        char_to_room( victim, location );
        act( AT_IMMORT, "$N appears as part of an invasion force!", ch, NULL, victim, TO_ROOM );
    }
	send_to_char( "The invasion was successful!\n\r", ch );

 return;
}


