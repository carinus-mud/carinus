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
      send_to_char( "This corpse is already decapitated.\r\n", ch );
      return;
   }

   if( (corpse->item_type != ITEM_CORPSE_PC ))
   {
      send_to_char( "You can only decapitate player corpses.\r\n",ch);
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


	send_to_char( "\r\n&c+===========================================================================+\n\r", ch);
        send_to_char( " &OSCORE  &Y SKILLS   &OSPELLS   FEATS   INVENTORY   EQUIPMENT   CONFIG   AFFECTS\r\n", ch);
	send_to_char( "&c+===========================================================================+\n\r", ch);
	pager_printf( ch, "                      &YPractice points remaining: &W%d\r\n", ch->practice);


	send_to_char( "&WSkills\r\n", ch);

	if( can_use_skill( ch,1, gsn_aid ) )
	{send_to_char("   &CAid -> ", ch);}
	else
	send_to_char("   &zAid -> ", ch);

	if( can_use_skill( ch,1, gsn_rescue ) )
	{send_to_char("&CRescue -> ", ch);}
	else
	send_to_char("&zRescue -> ", ch);

	if( can_use_skill( ch,1, gsn_bandage ) )
	{send_to_char("&CBandaging      ", ch);}
	else
	send_to_char("&zBandaging      ", ch);


	if( can_use_skill( ch,1, gsn_climb ) )
	{send_to_char("  &CClimb -> ", ch);}
	else
	send_to_char("  &zClimb -> ", ch);

	if( can_use_skill( ch,1, gsn_mount ) )
	{send_to_char("&CMount\r\n\r\n ", ch);}
	else
	send_to_char("&zMount\r\n\r\n ", ch);


	if( can_use_skill( ch,1, gsn_stun ) )
	{send_to_char("  &CStun -> ", ch);}
	else
	send_to_char("  &zStun -> ", ch);

	if( can_use_skill( ch,1, gsn_bash ) )
	{send_to_char("&CBash -> ", ch);}
	else
	send_to_char("&zBash -> ", ch);

	if( can_use_skill( ch,1, gsn_bashdoor ) )
	{send_to_char("&CDoorbash          ", ch);}
	else
	send_to_char("&zDoorbash          ", ch);


	if( can_use_skill( ch,1, gsn_poison_weapon ) )
	{send_to_char("&CPoison Weapon -> ", ch);}
	else
	send_to_char("&zPoison Weapon -> ", ch);

	if( can_use_skill( ch,1, gsn_backstab ) )
	{send_to_char("&CBackstab -> ", ch);}
	else
	send_to_char("&zBackstab -> ", ch);

	if( can_use_skill( ch,1, gsn_circle ) )
	{send_to_char("&CCircle\r\n\r\n ", ch);}
	else
	send_to_char("&zCircle\r\n\r\n ", ch);


	if( can_use_skill( ch,1, gsn_scan ) )
	{send_to_char("  &CScan -> ", ch);}
	else
	send_to_char("  &zScan -> ", ch);

	if( can_use_skill( ch,1, gsn_search ) )
	{send_to_char("&CSearch -> ", ch);}
	else
	send_to_char("&zSearch -> ", ch);

	if( can_use_skill( ch,1, gsn_track ) )
	{send_to_char("&CTrack         ", ch);}
	else
	send_to_char("&zTrack         ", ch);



	if( can_use_skill( ch,1, gsn_enhanced_damage ) )
	{send_to_char("  &CEnhanced Damage -> ", ch);}
	else
	send_to_char("  &zEnhanced Damage -> ", ch);

	if( can_use_skill( ch,1, gsn_dual_wield ) )
	{send_to_char("&CDual Wield -> ", ch);}
	else
	send_to_char("&zDual Wield -> ", ch);

	if( can_use_skill( ch,1, gsn_hitall ) )
	{send_to_char("&CHitall\r\n\r\n ", ch);}
	else
	send_to_char("&zHitall\r\n\r\n ", ch);



	if( can_use_skill( ch,1, gsn_grapple ) )
	{send_to_char("  &CGrapple -> ", ch);}
	else
	send_to_char("  &zGrapple -> ", ch);

	if( can_use_skill( ch,1, gsn_disarm ) )
	{send_to_char("&CDisarm -> ", ch);}
	else
	send_to_char("&zDisarm -> ", ch);

	if( can_use_skill( ch,1, gsn_grip ) )
	{send_to_char("&CGrip       ", ch);}
	else
	send_to_char("&zGrip       ", ch);



	if( can_use_skill( ch,1, gsn_second_attack ) )
	{send_to_char("  &CSecond Attack -> ", ch);}
	else
	send_to_char("  &zSecond Attack -> ", ch);

	if( can_use_skill( ch,1, gsn_third_attack ) )
	{send_to_char("&CThird -> ", ch);}
	else
	send_to_char("&zThird -> ", ch);

	if( can_use_skill( ch,1, gsn_fourth_attack ) )
	{send_to_char("&CFourth\r\n\r\n ", ch);}
	else
	send_to_char("&zFourth\r\n\r\n ", ch);



	if( can_use_skill( ch,1, gsn_parry ) )
	{send_to_char("  &CParry -> ", ch);}
	else
	send_to_char("  &zParry -> ", ch);

	if( can_use_skill( ch,1, gsn_dodge ) )
	{send_to_char("&CDodge -> ", ch);}
	else
	send_to_char("&zDodge -> ", ch);

	if( can_use_skill( ch,1, gsn_counter ) )
	{send_to_char("&CCounter       ", ch);}
	else
	send_to_char("&zCounter       ", ch);



	if( can_use_skill( ch,1, gsn_cook ) )
	{send_to_char("  &CCook -> ", ch);}
	else
	send_to_char("  &zCook -> ", ch);

	if( can_use_skill( ch,1, gsn_brew ) )
	{send_to_char("&CBrew -> ", ch);}
	else
	send_to_char("&zBrew -> ", ch);

	if( can_use_skill( ch,1, gsn_scribe ) )
	{send_to_char("&CScribe\r\n\r\n ", ch);}
	else
	send_to_char("&zScribe\r\n\r\n ", ch);



	if( can_use_skill( ch,1, gsn_pick_lock ) )
	{send_to_char("  &CPick lock -> ", ch);}
	else
	send_to_char("  &zPick lock -> ", ch);

	if( can_use_skill( ch,1, gsn_steal ) )
	{send_to_char("&CSteal -> ", ch);}
	else
	send_to_char("&zSteal -> ", ch);

	if( can_use_skill( ch,1, gsn_detrap ) )
	{send_to_char("&CDetrap    ", ch);}
	else
	send_to_char("&zDetrap    ", ch);



	if( can_use_skill( ch,1, gsn_sneak ) )
	{send_to_char("  &CSneak -> ", ch);}
	else
	send_to_char("  &zSneak -> ", ch);

	if( can_use_skill( ch,1, gsn_hide ) )
	{send_to_char("&CHide\r\n\r\n ", ch);}
	else
	send_to_char("&zHide\r\n\r\n ", ch);


	if( can_use_skill( ch,1, gsn_meditate ) )
	{send_to_char("  &CMeditate -> ", ch);}
	else
	send_to_char("  &zMeditate -> ", ch);

	if( can_use_skill( ch,1, gsn_trance ) )
	{send_to_char("&CTrance\r\n\r\n", ch);}
	else
	send_to_char("&zTrance\r\n\r\n", ch);


	send_to_char("&WStyles\r\n", ch);


	if( can_use_skill( ch,1, gsn_style_standard ) )
	{send_to_char("            &CStandard -> ", ch);}
	else
	send_to_char("            &zStandard -> ", ch);

	if( can_use_skill( ch,1, gsn_style_defensive ) )
	{send_to_char("&CDefensive -> ", ch);}
	else
	send_to_char("&zDefensive -> ", ch);

	if( can_use_skill( ch,1, gsn_style_aggressive ) )
	{send_to_char("&CAggressive-> ", ch);}
	else
	send_to_char("&zAggressive -> ", ch);

	if( can_use_skill( ch,1, gsn_style_evasive ) )
	{send_to_char("&CEvasive -> ", ch);}
	else
	send_to_char("&zEvasive -> ", ch);

	if( can_use_skill( ch,1, gsn_berserk ) )
	{send_to_char("&CBerserk\r\n\r\n", ch);}
	else
	send_to_char("&zBerserk\r\n\r\n", ch);


	send_to_char("&WWeapon Types\r\n\r\n", ch);

        if( can_use_skill( ch,1, gsn_pugilism ) )
        {send_to_char("     &CPugilism ", ch);}
        else
        send_to_char("     &zPugilism ", ch);

        if( can_use_skill( ch,1, gsn_long_blades ) )
        {send_to_char("  &CLong Blades ", ch);}
        else
        send_to_char("  &zLong Blades ", ch);

        if( can_use_skill( ch,1, gsn_short_blades ) )
        {send_to_char("  &CShort Blades ", ch);}
        else
        send_to_char("  &zShort Blades ", ch);

        if( can_use_skill( ch,1, gsn_flexible_arms ) )
        {send_to_char("  &CFlexible Arms ", ch);}
        else
        send_to_char("  &zFlexible Arms ", ch);

        if( can_use_skill( ch,1, gsn_talonous_arms ) )
        {send_to_char("  &CTalonous Arms\r\n ", ch);}
        else
        send_to_char("  &zTalonous Arms\r\n ", ch);

        if( can_use_skill( ch,1, gsn_bludgeons ) )
        {send_to_char("                       &CBludgeons ", ch);}
        else
        send_to_char("                       &zBludgeons ", ch);

        if( can_use_skill( ch,1, gsn_missile_weapons ) )
        {send_to_char("  &CMissile Weapons\r\n", ch);}
        else
        send_to_char("  &zMissile Weapons\r\n", ch);

	send_to_char("&WLanguages\r\n\r\n", ch);


        if( can_use_skill( ch,1, gsn_common ) )
        {send_to_char("&CCommon ", ch);}
        else
        send_to_char("&zCommon ", ch);

        if( can_use_skill( ch,1, gsn_elven ) )
        {send_to_char(" &CElven ", ch);}
        else
        send_to_char(" &zElven ", ch);

        if( can_use_skill( ch,1, gsn_dwarven ) )
        {send_to_char(" &CDwarven ", ch);}
        else
        send_to_char(" &zDwarven ", ch);

        if( can_use_skill( ch,1, gsn_pixie ) )
        {send_to_char(" &CPixie ", ch);}
        else
        send_to_char(" &zPixie ", ch);

        if( can_use_skill( ch,1, gsn_ogre ) )
        {send_to_char(" &COgre ", ch);}
        else
        send_to_char(" &zOgre ", ch);

        if( can_use_skill( ch,1, gsn_orcish ) )
        {send_to_char(" &COrcish ", ch);}
        else
        send_to_char(" &zOrcish ", ch);

        if( can_use_skill( ch,1, gsn_trollish ) )
        {send_to_char(" &CTrollish ", ch);}
        else
        send_to_char(" &zTrollish ", ch);

        if( can_use_skill( ch,1, gsn_goblin ) )
        {send_to_char(" &CGoblin ", ch);}
        else
        send_to_char(" &zGoblin ", ch);

        if( can_use_skill( ch,1, gsn_halfling ) )
        {send_to_char(" &CHalfling ", ch);}
        else
        send_to_char(" &zHalfling ", ch);

        if( can_use_skill( ch,1, gsn_gnomish ) )
        {send_to_char(" &CGnomish\r\n", ch);}
        else
        send_to_char(" &zGnomish\r\n", ch);



}

void do_spells( CHAR_DATA* ch, const char* )
{
   int sn;


send_to_char( "\r\n&c+===========================================================================+\n\r", ch);
        send_to_char( " &OSCORE   SKILLS   &YSPELLS   &OFEATS   INVENTORY   EQUIPMENT   CONFIG   AFFECTS\r\n", ch);
send_to_char( "&c+===========================================================================+\n\r", ch);

   if( IS_NPC( ch ) )
      return;


      int col;
      short lasttype, cnt;

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
         if( skill->type != SKILL_SPELL )
            continue;

         if( strcmp( skill->name, "reserved" ) == 0 && ( IS_IMMORTAL( ch ) || CAN_CAST( ch ) ) )
         {
            if( col % 3 != 0 )
               send_to_pager( "\r\n", ch );
            col = 0;
         }

         lasttype = skill->type;

         if( !IS_IMMORTAL( ch )
             && ( skill->guild != CLASS_NONE && ( !IS_GUILDED( ch ) || ( ch->pcdata->clan->Class != skill->guild ) ) ) )
            continue;


         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill, SF_SECRETSKILL ) )
            continue;
	if ( ch->pcdata->learned[normalSn] <= 0 )
	    continue;

         ++cnt;
         set_pager_color( AT_MAGIC, ch );
         pager_printf( ch, "&W%20.20s", skill->name );
         if( ch->pcdata->learned[normalSn] > 0 )
            set_pager_color( AT_SCORE, ch );
         pager_printf( ch, " %3d%% ", ch->pcdata->learned[normalSn] );
         if( ++col % 3 == 0 )
            send_to_pager( "\r\n", ch );
      }

      if( col % 3 != 0 )
         send_to_pager( "\r\n", ch );
   }


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


void do_score( CHAR_DATA* ch, const char* argument )
{

	if (IS_NPC(ch))
	{
	send_to_char("Mobiles do not have score sheets. Use Mstat.\r\n", ch);
	return;
	}

   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];

send_to_char( "\r\n&c+===========================================================================+\n\r", ch);
        send_to_char( " &YSCORE&O   SKILLS   SPELLS   FEATS   INVENTORY   EQUIPMENT   CONFIG   AFFECTS\r\n", ch);
send_to_char( "&c+===========================================================================+\n\r", ch);
pager_printf( ch, "&r&W %s %-30s &cHometown: &W%-13s &cRace: &W%-9s\r\n",
        ch->name, ch->pcdata->title, ch->pcdata->hometown,  capitalize( get_race(ch)) );
	send_to_char( "&c+===========================================================================+\n\r", ch);
pager_printf( ch, " &CLevel: &W%2d         &CTier: &W%-3d       &CAlign: &W%-6d   &CFavor:&W %-5d \r\n",
        ch->level, ch->pcdata->tier, ch->alignment, ch->pcdata->favor);

pager_printf( ch, " &CSTR:&W%2d(%2d)     &CHitroll: &W%-3d    &CBleeding: &W%-6d  &CHunger: &W%d\r\n",
        get_curr_str(ch), ch->perm_str, GET_HITROLL(ch), ch->pcdata->condition[COND_BLEEDING], ch->pcdata->condition[COND_FULL]);

pager_printf( ch, " &CDEX:&W%2d(%2d)     &CDamroll: &W%-3d      &CAP pts: &W%-6d  &CThirst: &W%d\r\n",
        get_curr_dex(ch), ch->perm_dex, GET_DAMROLL(ch), ch->pcdata->ap, ch->pcdata->condition[COND_THIRST]);

pager_printf( ch, " &CCON:&W%2d(%2d)     &C  Armor: &W%-4d   &C   Glory: &W%-6d  &CMental: &W%d\r\n",
        get_curr_con(ch), ch->perm_con, GET_AC(ch), ch->pcdata->quest_curr, ch->mental_state);

pager_printf( ch, " &CINT:&W%2d(%2d)\r\n",
        get_curr_int(ch), ch->perm_int);

pager_printf( ch, " &CWIS:&W%2d(%2d)    &CEXP to next level: &W%s\r\n",
        get_curr_wis(ch), ch->perm_wis, num_punct( exp_level( ch, ch->level + 1 ) - ch->exp ));

pager_printf( ch, " &CCHA:&W%2d(%2d)    &CMemberships: &W[%s] [%s]\r\n",
        get_curr_cha(ch), ch->perm_cha, ch->pcdata->clan_name, ch->pcdata->council_name);

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
	pager_printf( ch, " &CLCK:&W%2d(%2d)    &CPosition: &WYou are %s and your stance is %s\r\n",
        get_curr_lck(ch), ch->perm_lck, buf, buf2 );

	pager_printf( ch, "               &CWorships: &W%s\r\n",
         ch->pcdata->deity ? ch->pcdata->deity->name : "(none)" );
       pager_printf( ch, "               &CCpose: &W%s\r\n\r\n", ch->pcdata->cpose );


	pager_printf(ch,  "  &CHealth: &W%-5d/%-5d        &CMana:&W %-5d/%-5d       &CMovement: &W%-5d/%-5d\r\n", 
		ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);

int start;
send_to_char_color(" &W[",ch);
float value = 20.0 * ((float) ch->hit / (float) ch->max_hit) ;
                          for (start = 1; start <= 20; start++ )

                                {
                          if (start <=  value )
                                        send_to_char("&G|",ch);
                                  else
                                        send_to_char(" ",ch);
                                }
                    send_to_char("&W]   ",ch);

send_to_char_color("&W[",ch);
value = 20.0 * ((float) ch->mana / (float) ch->max_mana) ;
                          for (start = 1; start <= 20; start++ )

                                {
                          if (start <=  value )
                                        send_to_char("&C|",ch);
                                  else
                                        send_to_char(" ",ch);
                                }
                    send_to_char("&W]   ",ch);

send_to_char_color("&W[",ch);
value = 20.0 * ((float) ch->move / (float) ch->max_move) ;
                          for (start = 1; start <= 20; start++ )
                                {
                          if (start <=  value )
                                        send_to_char("&P|",ch);
                                  else
                                        send_to_char(" ",ch);
                                }
                    send_to_char("&W]\n\r\n\r",ch);

	send_to_char( "&c+===========================================================================+\n\r", ch);

	if IS_PKILL(ch)
	{

      pager_printf( ch, "  &CPKILL DATA:  Pkills &W(%3.3d)     &CIllegal Pkills &W(%3.3d)     &CPdeaths &W(%3.3d)\r\n",
                    ch->pcdata->pkills, ch->pcdata->illegal_pk, ch->pcdata->pdeaths );
	send_to_char( "&c+===========================================================================+\n\r", ch);
	}

   if( IS_IMMORTAL( ch ) )
   {
      pager_printf( ch, "  &YIMMORTAL DATA:  Wizinvis [%s]  Wizlevel (%-2d)  ",
                    xIS_SET( ch->act, PLR_WIZINVIS ) ? "X" : " ", ch->pcdata->wizinvis );

	if(ch->pcdata->area)
		{
         pager_printf( ch, " &YArea Loaded &W[%s]\r\n", ( IS_SET( ch->pcdata->area->status, AREA_LOADED ) ) ? "yes" : "no" );
		}
		else
	send_to_char("\r\n",ch);

      pager_printf( ch, "  &YBamfin:  %s\r\n", ( ch->pcdata->bamfin[0] != '\0' )
                    ? ch->pcdata->bamfin : "An immortal appears." );
      pager_printf( ch, "  &YBamfout: %s \r\n", ( ch->pcdata->bamfout[0] != '\0' )
                    ? ch->pcdata->bamfout : "The immortal disappears." );
	send_to_char( "&c+===========================================================================+\n\r", ch);

  }


      if( ch->pcdata->area )
      {
         pager_printf( ch, "  &CVnums: Room &W(%-5.5d - %-5.5d)  &CObject &W(%-5.5d - %-5.5d)  &CMob &W(%-5.5d - %-5.5d)\r\n",
                       ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
                       ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum,
                       ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum );
	send_to_char( "&c+===========================================================================+\n\r", ch);

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


void do_skin( CHAR_DATA* ch, const char* argument)
{
   OBJ_INDEX_DATA *korps;
   OBJ_DATA *corpse;
   OBJ_DATA *obj;
   OBJ_DATA *skin;
   const char *name;
   char buf[MAX_STRING_LENGTH];

   if( argument[0] == '\0' )
   {
      send_to_char( "What corpse do you wish to skin?\r\n", ch );
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
      if( (corpse->item_type != ITEM_CORPSE_NPC ))
   {
      send_to_char( "You can only skin animal corpses.\r\n",ch);
      return;
   }

  if( corpse->value[0] != 2)
	{
	send_to_char( "You can only skin animal corpses.\r\n", ch);
	return;
	}
   if( obj->value[3] != 1 && obj->value[3] != 2 && obj->value[3] != 3 && obj->value[3] != 11 )
   {
      send_to_char( "There is nothing you can do with this corpse.\r\n", ch );
      return;
   }
   if( get_obj_index( OBJ_VNUM_SKIN ) == NULL )
   {
      bug( "Vnum %d (OBJ_VNUM_SKIN) not found for do_skin!", OBJ_VNUM_SKIN );
      return;
   }
   if( !( korps = get_obj_index( OBJ_VNUM_CORPSE_PC ) ) )
   {
      bug( "Vnum %d (OBJ_VNUM_CORPSE_PC) not found for %s!", OBJ_VNUM_CORPSE_PC, __FUNCTION__ );
      return;
   }

   skin = create_object( get_obj_index( OBJ_VNUM_SKIN ), 0 );
   name = IS_NPC( ch ) ? korps->short_descr : corpse->short_descr;
   snprintf( buf, MAX_STRING_LENGTH, skin->short_descr, name );
   STRFREE( skin->short_descr );
   skin->short_descr = STRALLOC( buf );
   snprintf( buf, MAX_STRING_LENGTH, skin->description, name );
   STRFREE( skin->description );
   skin->description = STRALLOC( buf );
   act( AT_BLOOD, "$n strips the skin from $p.", ch, corpse, NULL, TO_ROOM );
   act( AT_BLOOD, "You strip the skin from $p.", ch, corpse, NULL, TO_CHAR );
   obj_to_char( skin, ch );
   return;
}

