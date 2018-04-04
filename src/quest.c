/****************************************************************************
 * This is an automated questmaster snippet. Its based off the concept of   *
 * the questmaster snippet found all over the place. As such the credits are*
 * a completely disgusting tangle.                                          *
 * The nearest semblence of ownership I can determine is this comes from ROM*
 * was created by Vassago for the mud Moongate, then Vir ported it from ROM *
 * to SMAUG, then LrdElder allegedly edited it and released it for smaug 1.4*
 *                                                                          *
 * Regardless of how it got there, the one floating about is pretty crappy  *
 * And has tons of bugs in it, and since I'm sick of hearing questions about*
 * ways to fix it, therefore I've rewritten it in my own way to make it more*
 * compatible, and quite frankly workable.                                  *
 *                                                                          *
 * I myself add no terms or conditions to this code, simply because I don't *
 * care. If you're going to steal it, you will whether I tell you you can or*
 * not, likewise if you wish to do something special like mention me or the *
 * other authors or whatever, somewhere, this is also entirely up to you,   *
 * and not something I can force you to do, etc.                            *
 *                                                                          *
 * Whether or not you follow the license of the previous ways this snippet  *
 * has been affected is also up to you. Because this is entirely rewritten, *
 * I have no idea whether or not you're bound by the licenses of the first  *
 * version that exists since you're not actually using it, but as far as I  *
 * am aware, you are not constrained in any way by those licenses since this*
 * work is not a derivitive of that, nor a copy of it, merely something that*
 * was inspired by it (Aka I looked at it and said "It'd work much better if*
 * if it were like this", and so wrote my own this way learning from the    *
 * quite hideous mistakes in the other).                                    *
 *                                                                          *
 * The installation instructions are within the readme.txt, this also will  *
 * detail what the functions are, and do, as well as some other practical   *
 * usages for them. The license.txt includes the headers/licenses from the  *
 * inspirational work for those who are interested or concerned.            *
 *                                                                          *
 *                                         -Aurora                          *
 *                                          EternalEmpress@Lostprophecy.com *
 ****************************************************************************/


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* Object vnums for Quest Rewards */

#define QUEST_ITEM1 0 
#define QUEST_ITEM2 0  
#define QUEST_ITEM3 0  
#define QUEST_ITEM4 0 
#define QUEST_ITEM5 0 

/* Prices for each Quest Reward, in glory */

#define QUEST_VALUE1 0
#define QUEST_VALUE2 0
#define QUEST_VALUE3 0
#define QUEST_VALUE4 0
#define QUEST_VALUE5 0

/* Local functions */

void generate_quest	args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void quest_update	args(( void ));
bool qchance            args(( int num ));
CHAR_DATA *find_quest_mob args(( CHAR_DATA *ch));
OBJ_DATA *find_quest_obj  args(( CHAR_DATA *ch));
void quest_list           args((CHAR_DATA *ch));
void quest_buy            args((CHAR_DATA *questman, CHAR_DATA *ch, sh_int number));

bool qchance( int num )
{
 if (number_range(1,65) <= num) return TRUE;
 else return FALSE;
}

/* The main quest function */

void do_aquest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *obj1, *obj2, *obj3, *obj4, *obj5;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!strcmp(arg1, "info"))
    {
        if (xIS_SET(ch->act, PLR_QUESTOR))
	{
	    if (ch->pcdata->questmob == -1 && ch->pcdata->questgiver->short_descr != NULL)
	    {
		sprintf(buf, "Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r",ch->pcdata->questgiver->short_descr);
		send_to_char(buf, ch);
	    }
	    else if (ch->pcdata->questobj > 0)
	    {
                questinfoobj = get_obj_index(ch->pcdata->questobj);
		if (questinfoobj != NULL)
		{
		    sprintf(buf, "You are on a quest to recover %s from %s in %s.\n\r",questinfoobj->name, ch->pcdata->questroom, ch->pcdata->questarea);
		    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	    else if (ch->pcdata->questmob > 0)
	    {
                questinfo = get_mob_index(ch->pcdata->questmob);
		if (questinfo != NULL)
		{
	            sprintf(buf, "You are on a quest to slay %s in %s in the area of %s.\n\r",questinfo->short_descr, ch->pcdata->questroom, ch->pcdata->questarea);
		    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	}
	else
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	return;
    }
    if (!strcmp(arg1, "points"))
    {
	sprintf(buf, "You have %d glory.\n\r",ch->pcdata->quest_curr);
	send_to_char(buf, ch);
	return;
    }
    else if (!strcmp(arg1, "time"))
    {
        if (!xIS_SET(ch->act, PLR_QUESTOR))
	{
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	    if (ch->pcdata->nextquest > 1)
	    {
		sprintf(buf, "There are %d minutes remaining until you can go on another quest.\n\r",ch->pcdata->nextquest);
		send_to_char(buf, ch);
	    }
	    else if (ch->pcdata->nextquest == 1)
	    {
		sprintf(buf, "There is less than a minute remaining until you can go on another quest.\n\r");
		send_to_char(buf, ch);
	    }
	}
        else if (ch->pcdata->countdown > 0)
        {
	    sprintf(buf, "Time left for current quest: %d\n\r",ch->pcdata->countdown);
	    send_to_char(buf, ch);
	}
	return;
    }

/* Check for questmaster in room. */

    for ( questman = ch->in_room->first_person; questman != NULL; questman = questman->next_in_room )
	if (IS_NPC(questman)) && xIS_SET(questman->act, ACT_QUESTMASTER))
         break;

    if (!questman)
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }

    if ( questman->fighting)
    {
	send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }

    ch->pcdata->questgiver = questman;

    obj1 = get_obj_index(QUEST_ITEM1);
    obj2 = get_obj_index(QUEST_ITEM2);
    obj3 = get_obj_index(QUEST_ITEM3);
    obj4 = get_obj_index(QUEST_ITEM4);
    obj5 = get_obj_index(QUEST_ITEM5);

    if ( !obj1 || !obj2 || !obj3 || !obj4 || !obj5 )
    {
     bug("Error loading quest objects. Char: ", ch->name);
     return;
    }

    if (!strcmp(arg1, "list"))
    {
        act(AT_PLAIN,"$n asks $N for a list of quest items.",ch,NULL,questman,TO_ROOM);
	act(AT_PLAIN,"You ask $N for a list of quest items.",ch,NULL,questman,TO_CHAR);
        quest_list(ch);
	return;
    }

    else if (!strcmp(arg1, "buy"))
    {
	if (arg2[0] == '\0')
	{
	    send_to_char("To buy an item, type 'AQUEST BUY <item #>'.\n\r",ch);
	    return;
	}
        quest_buy(questman, ch, atoi(arg2));
	return;
    }
    else if (!strcmp(arg1, "request"))
    {
        act(AT_PLAIN,"$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
	act(AT_PLAIN,"You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
        if (sysdata.DENY_NEW_PLAYERS == TRUE)
	{
            sprintf(buf, "I'm sorry. Please come back after the reboot. I cant give you a quest until then.");
	    do_say(questman, buf);
	    return;
	}
        if (xIS_SET(ch->act, PLR_QUESTOR))
	{
            sprintf(buf, "But you're already on a quest!\n\rBetter hurry up and finish it!");
	    do_say(questman, buf);
	    return;
	}
	if (ch->pcdata->nextquest > 0)
	{
	    sprintf(buf, "You're very brave, %s, but let someone else have a chance.",ch->name);
	    do_say(questman, buf);
	    sprintf(buf, "Please come back in about %d minutes.", ch->pcdata->nextquest);
	    do_say(questman, buf);
	    return;
	}

	sprintf(buf, "Very well %s...",ch->name);
	do_say(questman, buf);

	generate_quest(ch, questman);

        if (ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
	{
            ch->pcdata->countdown = number_range(10,30);
            xSET_BIT(ch->act, PLR_QUESTOR);
	    sprintf(buf, "You have %d minutes to complete this quest.",ch->pcdata->countdown);
	    do_say(questman, buf);
	    sprintf(buf, "May the gods go with you!");
	    do_say(questman, buf);
	}
	return;
    }
    else if (!strcmp(arg1, "refuse"))
    {
        act(AT_PLAIN,"$n informs $N $e has refused $s quest.", ch, NULL, questman,
		TO_ROOM);
        act(AT_PLAIN,"You inform $N you have refused the quest.",ch, NULL,
		questman, TO_CHAR);
	if (ch->pcdata->questgiver != questman)
	{
	    sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
	    do_say(questman,buf);
	    return;
	}
        if (!xIS_SET(ch->act, PLR_QUESTOR))
	{
	        ch->pcdata->questgiver = NULL;
	        ch->pcdata->countdown = 0;
	        ch->pcdata->questmob = 0;
		ch->pcdata->questobj = 0;
	        sprintf(buf, "You should at least REQUEST a quest before you refuse it!");
	        do_say(questman,buf);

	        return;
	    }

        if (xIS_SET(ch->act, PLR_QUESTOR))
	{
                xREMOVE_BIT(ch->act, PLR_QUESTOR);
	        ch->pcdata->questgiver = NULL;
	        ch->pcdata->countdown = 0;
	        ch->pcdata->questmob = 0;
		ch->pcdata->questobj = 0;
	        ch->pcdata->nextquest = 20;
	        sprintf(buf, "Alright fine, i'll put it back in the heap for someone else.");
	        do_say(questman,buf);
                sprintf(buf, "You can come back in about %d minutes.", ch->pcdata->nextquest);
                do_say(questman, buf);

	        return;
	    }
    }
    else if (!strcmp(arg1, "complete"))
    {
        act(AT_PLAIN,"$n informs $N $e has completed $s quest.", ch, NULL, questman,
		TO_ROOM);
        act(AT_PLAIN,"You inform $N you have completed $s quest.",ch, NULL,
		questman, TO_CHAR);
	if (ch->pcdata->questgiver != questman)
	{
	    sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
	    do_say(questman,buf);
	    return;
	}
        if (xIS_SET(ch->act, PLR_QUESTOR))
	{
	    if (ch->pcdata->questmob == -1 && ch->pcdata->countdown > 0)
	    {
		int reward, pointreward, pracreward;

                reward = number_range(10000,150000);
                pointreward = number_range(15,50);

		sprintf(buf, "Congratulations on completing your quest!");
		do_say(questman,buf);
		sprintf(buf,"As a reward, I am giving you %d glory, and %d gold.",pointreward,reward);
		do_say(questman,buf);
                sprintf(buf, "[INFO] %s has completed a quest!", ch->name);
                echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );  //Yzek
                if (qchance(15))
		{
                    pracreward = number_range(1,5);
		    sprintf(buf, "You gain %d practices!\n\r",pracreward);
		    send_to_char(buf, ch);
		    ch->practice += pracreward;
		}

                xREMOVE_BIT(ch->act, PLR_QUESTOR);
	        ch->pcdata->questgiver = NULL;
	        ch->pcdata->countdown = 0;
	        ch->pcdata->questmob = 0;
		ch->pcdata->questobj = 0;
	        ch->pcdata->nextquest = 20; // yzek... was 30
		ch->gold += reward;
		ch->pcdata->quest_curr += pointreward;

	        return;
	    }
	    else if (ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0)
	    {
		bool obj_found = FALSE;

                for (obj = ch->first_carrying; obj != NULL; obj=obj_next)
    		{
                    obj_next = obj->next_content;

		    if (obj != NULL && obj->pIndexData->vnum == ch->pcdata->questobj)
		    {
			obj_found = TRUE;
            	        break;
		    }
        	}
		if (obj_found == TRUE)
		{
		    int reward, pointreward, pracreward;

                    reward = number_range(5000,100000);
                    pointreward = number_range(10,50);

		    act(AT_PLAIN,"You hand $p to $N.",ch, obj, questman, TO_CHAR);
		    act(AT_PLAIN,"$n hands $p to $N.",ch, obj, questman, TO_ROOM);

	    	    sprintf(buf, "Congratulations on completing your quest!");
		    do_say(questman,buf);
		    sprintf(buf,"As a reward, I am giving you %d glory, and %d gold.",pointreward,reward);
		    do_say(questman,buf);
                    sprintf(buf, "[INFO] %s has completed a quest!", ch->name);
                    echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );  //Yzek
                    if (qchance(20))
		    {
		        pracreward = number_range(1,6);
		        sprintf(buf, "You gain %d practices!\n\r",pracreward);
		        send_to_char(buf, ch);
		        ch->practice += pracreward;
		    }

                    xREMOVE_BIT(ch->act, PLR_QUESTOR);
	            ch->pcdata->questgiver = NULL;
	            ch->pcdata->countdown = 0;
	            ch->pcdata->questmob = 0;
		    ch->pcdata->questobj = 0;
	            ch->pcdata->nextquest = 20; //Yzek ... was 30
		    ch->gold += reward;
		    ch->pcdata->quest_curr += pointreward;
		    extract_obj(obj);
		    return;
		}
		else
		{
		    sprintf(buf, "You haven't completed the quest yet, but there is still time!");
		    do_say(questman, buf);
		    return;
		}
		return;
	    }
	    else if ((ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0) && ch->pcdata->countdown > 0)
	    {
		sprintf(buf, "You haven't completed the quest yet, but there is still time!");
		do_say(questman, buf);
		return;
	    }
	}
	if (ch->pcdata->nextquest > 0)
	 sprintf(buf,"But you didn't complete your quest in time!");
	else sprintf(buf, "You have to REQUEST a quest first, %s.",ch->name);
	 do_say(questman, buf);
	return;
    }

    send_to_char("AQUEST commands: POINTS INFO TIME REQUEST REFUSE COMPLETE LIST BUY.\n\r",ch);
    send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim=NULL;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf[MAX_STRING_LENGTH];

    /*  40 % chance it will send the player on a 'recover any item' quest. */
    if (qchance(40))
    {
 
        questitem = find_quest_obj( ch);
        victim = questitem->carried_by;

        if (!victim || !questitem)
        {
	    sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
	    do_say(questman, buf);
	    sprintf(buf, "Try again later.");
	    do_say(questman, buf);
            ch->pcdata->questmob = 0;
            ch->pcdata->questobj = 0;
	    ch->pcdata->nextquest = 5;
            return;
        }

        room = victim->in_room;
	STRFREE( questitem->creator );
	questitem->creator = STRALLOC( ch->name );
	ch->pcdata->questobj = questitem->pIndexData->vnum;

        sprintf(buf, "I've been contracted to acquire %s.",questitem->short_descr);
	do_say(questman, buf);
	do_say(questman, "Using my resources I've located the present location of this item.");
	sprintf(buf, "Look in the general area of %s for %s being carried by %s, in %s.",room->area->name, questitem->short_descr, victim->short_descr, room->name);
        ch->pcdata->questroom = room->name;
        ch->pcdata->questarea = room->area->name;
	do_say(questman, buf);
	return;
    }
    /* Quest to kill a mob */
    else
    {
        victim = find_quest_mob(ch);

        if (!victim)
        {
    	    sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
	    do_say(questman, buf);
	    sprintf(buf, "Try again later.");
	    do_say(questman, buf);
            ch->pcdata->questmob = 0;
            ch->pcdata->questobj = 0;
	    ch->pcdata->nextquest = 5;
            return;
        }

        room = victim->in_room;
        switch(number_range(0,1))
        {
	    case 0:
             sprintf(buf, "%s has reneged upon a contract with me.",victim->short_descr);
             do_say(questman, buf);
             do_say(questman,"The penalty for this is death, and you are to deliver the sentence.");
	    break;

	    case 1:
	     do_say(questman,"I have a contract to carry out an assassination.");
             sprintf(buf, "You are to eliminate %s.",victim->short_descr);
	     do_say(questman, buf);
	    break;
        }

        if (room->name != NULL)
        {
            sprintf(buf, "Seek %s out somewhere in the vicinity of %s!",victim->short_descr,room->name);
            do_say(questman, buf);
	    sprintf(buf, "That location is in the general area of %s.",room->area->name);
            ch->pcdata->questroom = room->name;
            ch->pcdata->questarea = room->area->name;
	    do_say(questman, buf);
        }
        ch->pcdata->questmob = victim->pIndexData->vnum;
    }
    return;
}

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    CHAR_DATA *ch, *ch_next;

    for ( ch = first_char; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;

	if (IS_NPC(ch)) 
          continue;

	if (ch->pcdata->nextquest > 0)
	{
	    ch->pcdata->nextquest--;

	    if (ch->pcdata->nextquest == 0)
	    {
	        send_to_char("You may now quest again.\n\r",ch);
	        return;
	    }
	}
        else if (xIS_SET(ch->act,PLR_QUESTOR))
        {
	    if (--ch->pcdata->countdown <= 0)
	    {
    	        char buf [MAX_STRING_LENGTH];

	        ch->pcdata->nextquest = 20;
	        sprintf(buf, "You have run out of time for your quest!\n\rYou may quest again in %d minutes.\n\r",ch->pcdata->nextquest);
	        send_to_char(buf, ch);
                xREMOVE_BIT(ch->act, PLR_QUESTOR);
                STRFREE(ch->pcdata->questroom);
                STRFREE(ch->pcdata->questarea);
                ch->pcdata->questgiver = NULL;
                ch->pcdata->countdown = 0;
                ch->pcdata->questmob = 0;
	    }
	    if (ch->pcdata->countdown > 0 && ch->pcdata->countdown < 3)
	    {
	        send_to_char("Better hurry, you're almost out of time for your quest!\n\r",ch);
	        return;
	    }
        }
    }
    return;
}

void do_qstat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Qstat whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Mobiles cannot quest.\n\r", ch );
    return;
    }

    if (!xIS_SET(victim->act, PLR_QUESTOR))
    {
     sprintf(buf, "%s is not on a quest and may quest again in %d.\n\r",victim->name, victim->pcdata->nextquest );
     send_to_char(buf, ch);
     return;
    }
    sprintf(buf, "%s has %d minutes left for their quest.\n\r",victim->name, victim->pcdata->countdown );
    send_to_char(buf, ch);
    
    questinfoobj = get_obj_index(victim->pcdata->questobj);
    if (questinfoobj != NULL)
    {
     sprintf(buf, "%s is on a quest to find %s.\n\rObject Vnum: %d\n\rFound in %s in %s.\n\r",victim->name, questinfoobj->short_descr, questinfoobj->vnum, victim->pcdata->questroom, victim->pcdata->questarea );
     send_to_char(buf, ch);
    }

    questinfo = get_mob_index(victim->pcdata->questmob);
    if (questinfo != NULL)
    {
     sprintf(buf, "%s is on a quest to slay %s.\n\rMobile Vnum: %d\n\rFound in %s in %s.\n\r",victim->name, questinfo->short_descr, questinfo->vnum, victim->pcdata->questroom, victim->pcdata->questarea );
     send_to_char(buf, ch);
    }
   return;
}

/* setquest <victim> <mob|obj> <vnum> <location name> */

void do_setquest( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int vnum;
  bool object=TRUE;

  set_char_color( AT_IMMORT, ch );

  if ( IS_NPC(ch) )
  {
    send_to_char( "Cannot setquest as an NPC.\n\r", ch );
    return;
  }
  if ( get_trust( ch ) < LEVEL_IMMORTAL ) 
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  vnum = atoi( arg3 );
  if ( arg[0] == '\0' || arg2[0] == '\0' || vnum <= 0)
  {
    send_to_char( "Syntax: setquest <character> <mob|obj> <vnum> <quest location>\n\r", ch );
    send_to_char( "Vnum must be an existing mobile vnum.\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "There is no such player currently playing.\n\r", ch );
    return;
  }
  if ( IS_NPC( victim ) )
  {
    send_to_char( "Quests cannot be given to or taken from a mob.\n\r", ch );
    return;
  }

  set_char_color( AT_IMMORT, victim );
  if ( nifty_is_name_prefix( arg2, "mob" ) )
    object = FALSE;
  else if ( nifty_is_name_prefix( arg2, "obj" ) )
    object = TRUE;
  else
  {
    do_setquest( ch, "" );
    return;
  }


  if ( object )
  {
   if (obj_by_vnum(vnum) == NULL)
   {
    send_to_char( "There is no such object.\n\r", ch );
    return;
   }
   if (obj_by_vnum(vnum)->carried_by)
   {
    victim->pcdata->questarea = STRALLOC(obj_by_vnum(vnum)->carried_by->in_room->area->name);
    victim->pcdata->questroom = STRALLOC(obj_by_vnum(vnum)->carried_by->in_room->name);
   }
   if (obj_by_vnum(vnum)->in_room)
   {
    victim->pcdata->questarea = STRALLOC(obj_by_vnum(vnum)->in_room->area->name);
    victim->pcdata->questroom = STRALLOC(obj_by_vnum(vnum)->in_room->name);
   }

   victim->pcdata->questmob = 0;
   victim->pcdata->questobj = vnum;
   ch_printf( ch, "You've assigned %s a quest to find %s in %s in the area of %s\n\r", victim->name, obj_by_vnum(vnum)->short_descr, victim->pcdata->questroom, victim->pcdata->questarea );
   ch_printf( victim, "You've been assigned a quest to find %s in %s in the area of %s\n\r", obj_by_vnum(vnum)->short_descr, victim->pcdata->questroom, victim->pcdata->questarea );
  }
  else
  {
   if (get_mob(vnum) == NULL)
   {
    send_to_char( "There is no such mobile.\n\r", ch );
    return;
   }
   victim->pcdata->questarea = STRALLOC(get_mob(vnum)->in_room->area->name);
   victim->pcdata->questroom = STRALLOC(get_mob(vnum)->in_room->name);
   victim->pcdata->questobj = 0;
   victim->pcdata->questmob = vnum;
   ch_printf( ch, "You've assigned %s a quest to slay %s, in %s in the area of %s\n\r", victim->name, get_mob(vnum)->short_descr, victim->pcdata->questroom, victim->pcdata->questarea );
   ch_printf( victim, "You've been assigned a quest to slay %s in %s in the area of %s\n\r", get_mob(vnum)->short_descr, victim->pcdata->questroom, victim->pcdata->questarea );
  }
  xSET_BIT(victim->act, PLR_QUESTOR);
  victim->pcdata->nextquest = 0;
  victim->pcdata->countdown = 15;

  return;
}

CHAR_DATA *find_quest_mob( CHAR_DATA *ch)
{
  CHAR_DATA *victim=NULL;
  int counter, mob_vnum, level_diff;

     for (counter = 0; counter < 2000; counter ++)
     {
	mob_vnum = number_range(50, 32000); /* Raise 32000 to your highest mobile vnum */

	if ( (victim = get_mob(mob_vnum) ) != NULL )
	{
	    level_diff = victim->level - ch->level;

            if (((level_diff < questmaster->level_range && level_diff > -questmaster->level_range) 
                || (ch->level > 30 && ch->level < 40 && victim->level > 30 && victim->level < 50) 
                || (ch->level > 40 && victim->level > 40)) 
	        && victim->pIndexData->pShop == NULL 
		&& victim->pIndexData->rShop == NULL 
    		&& !xIS_SET( victim->act, ACT_PRACTICE) 
                && !xIS_SET(victim->in_room->room_flags, ROOM_SAFE)
                && in_hard_range(ch, victim->in_room->area)
                && !xIS_SET( victim->act, ACT_NOQUEST) 
                && !xIS_SET( victim->act, ACT_PET) 
    		&& !xIS_SET( victim->act, ACT_IMMORTAL))
             return victim;
	    else 
             continue;
	}
    }
  return victim;
}

OBJ_DATA *find_quest_obj( CHAR_DATA *ch)
{
  CHAR_DATA *victim=NULL;
  int counter, obj_vnum, level_diff;
  OBJ_DATA *obj=NULL;

     for (counter = 0; counter < 2000; counter ++)
     {
        OBJ_DATA *obj;

   	obj_vnum = number_range(50, 32000); /* Raise 32000 to your highest obj vnum */

   	if ( (obj = obj_by_vnum(obj_vnum) ) != NULL )
   	{

            if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE)
            || IS_OBJ_STAT(obj, ITEM_INVENTORY)
            || (obj->timer > 0)
            || !obj->carried_by
            || (obj->carried_by && !IS_NPC(obj->carried_by))
            || IS_OBJ_STAT(obj, ITEM_DEATHROT))
              continue;

            if ( (victim = obj->carried_by ) == NULL )
              continue;
            if (!victim || !IS_NPC(victim))
              continue;

	    level_diff = victim->level - ch->level;

            if (((level_diff < questmaster->level_range && level_diff > -questmaster->level_range) 
                || (ch->level > 30 && ch->level < 40 && victim->level > 30 && victim->level < 50) 
                || (ch->level > 40 && victim->level > 40)) 
	        && victim->pIndexData->pShop == NULL 
                && victim->pIndexData->rShop == NULL 
                && !xIS_SET( victim->act, ACT_PRACTICE) 
                && !xIS_SET(victim->in_room->room_flags, ROOM_SAFE)
                && in_hard_range(ch, victim->in_room->area)
                && !xIS_SET( victim->act, ACT_NOQUEST) 
                && !xIS_SET( victim->act, ACT_PET) 
                && !xIS_SET( victim->act, ACT_IMMORTAL))
             return obj;
	    else 
             continue;

   	}
     }
  return obj;
}

void quest_list(CHAR_DATA *ch)
{
  sh_int i, count=0;

  ch_printf( ch, "&Y[&W%2s&Y] [&W%-35s&Y] [&W%6s&Y] \n\r", "#", "Item", "Cost" );
  for(i = 0; i <= MAX_QDATA; i++)
  {
   if (questmaster->award_vnum[i] && questmaster->award_value[i])
   {
     ++count;
     ch_printf( ch, "&Y[&W%2d&Y] [&g%-35s&Y] [&W%6s&Y] \n\r", count, get_obj_index(questmaster->award_vnum[i])->short_descr, num_punct(questmaster->award_value[i]) );
   }
  }
  ch_printf( ch, "&Y[&W%2d&Y] [%s Gold                    ] [&W%6s&Y]\n\r", count+1, num_punct(questmaster->gold_amt), num_punct(questmaster->gold_cost));
  ch_printf( ch, "&Y[&W%2d&Y] [%d Practices                       ] [&W%6s&Y]\n\r", count+2, questmaster->practice_amt, num_punct(questmaster->practice_cost) );
  return;
}

void quest_buy(CHAR_DATA *questman, CHAR_DATA *ch, sh_int number)
{
  sh_int i, count=0;
  OBJ_DATA *obj;

  for(i = 0; i <= MAX_QDATA; i++)
  {
   if (questmaster->award_vnum[i] > 0 && questmaster->award_value[i] > 0)
   {
     ++count;
     if (count == number)
      break;
   }
  }
  if (number > count+2 || number < 1)
  {
    do_say(questman, "I don't know what you're trying to buy. Check the list again.");
    return;
  }

  if (number == (count+1)) /* Gold */
  {
   if (ch->pcdata->quest_curr >= questmaster->gold_cost)
   {
    ch->pcdata->quest_curr -= questmaster->gold_cost;
    ch->gold += questmaster->gold_amt;
    act(AT_MAGIC,"$N gives a sack of gold pieces to $n.", ch, NULL, questman, TO_ROOM );
    act(AT_MAGIC,"$N hands you your gold.", ch, NULL, questman, TO_CHAR );
    return;
   }
   else
   {
    do_say(questman, "You're not glorious enough to purchase that.");
    return;
   }
  }
  else if (number == (count+2)) /* Practices */
  {
   if (ch->pcdata->quest_curr >= questmaster->practice_cost)
   {
    ch->pcdata->quest_curr -= questmaster->practice_cost;
    ch->practice += questmaster->practice_amt;
    act(AT_MAGIC,"$N expands $n's mind to recieve greater knowledge.", ch, NULL, questman, TO_ROOM );
    act(AT_MAGIC,"$N opens your mind to greater possibilities of learning.", ch, NULL, questman, TO_CHAR );
    return;
   }
   else
   {
    do_say(questman, "You're not glorious enough to purchase that.");
    return;
   }
  }
  else if (ch->pcdata->quest_curr >= questmaster->award_value[i] && (questmaster->award_value[i] != 0))
  {
   ch->pcdata->quest_curr -= questmaster->award_value[i];
   obj = create_object(get_obj_index(questmaster->award_vnum[i]), ch->level);
   act(AT_PLAIN,"$N gives $p to $n.", ch, obj, questman, TO_ROOM );
   act(AT_PLAIN,"$N gives you $p.",   ch, obj, questman, TO_CHAR );
   obj_to_char(obj, ch);
  }
  else
  {
   do_say(questman, "You're not glorious enough to purchase that.");
   return;
  }
  return;
}


void fwrite_questmaster()
{
    FILE *fp;
    char filename[256];
    int counter;

    sprintf( filename, "%s", QUEST_FILE );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "fwrite_questmaster: fopen", 0 );
    	perror( filename );
    }
    fprintf( fp, "Lrange %d\n", questmaster->level_range);
    fprintf( fp, "Pamt %d\n",	questmaster->practice_amt);
    fprintf( fp, "Pcost %d\n",	questmaster->practice_cost);
    fprintf( fp, "Gamt %d\n",	questmaster->gold_amt	);
    fprintf( fp, "Gcost %d\n",	questmaster->gold_cost	);
    for(counter = 0; counter <= MAX_QDATA; counter++)
     if (questmaster->award_value[counter] && questmaster->award_vnum[counter])
	fprintf( fp, "Object %d %d\n", questmaster->award_vnum[counter], questmaster->award_value[counter] );
     else
        continue;
    fprintf( fp, "End\n"						);
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				      field = value;			\
				      fMatch = TRUE;			\
				      break;				\
				}


void fread_questmaster()
{
    char filename[256];
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int vnum=-1, value=-1, counter;

    sprintf( filename, "%s", QUEST_FILE );

    if ( ( fp = fopen( filename, "r" ) ) == NULL )
    {
    	bug( "fread_questmaster: fopen", 0 );
    	perror( filename );
    }
    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;
	case 'E':
	    if ( !str_cmp( word, "End" ) )
            {
                fclose( fp );
                for(counter = vnum; counter != MAX_QDATA; counter++)
                {
		 questmaster->award_vnum[++vnum] = 0;
		 questmaster->award_value[++value] = 0;
                }
		return;
            }
	    break;
	case 'G':
	    KEY( "Gamt",	questmaster->gold_amt,	fread_number( fp ) );
	    KEY( "Gcost",	questmaster->gold_cost,	fread_number( fp ) );
	    break;
	case 'L':
	    KEY( "Lrange",	questmaster->level_range, fread_number( fp ) );
	    break;
	case 'P':
	    KEY( "Pamt",	questmaster->practice_amt,	fread_number( fp ) );
	    KEY( "Pcost",	questmaster->practice_cost,	fread_number( fp ) );
	    break;
	case 'O':
	    if ( !str_cmp( word, "Object" ) )
            {
                if (vnum >= MAX_QDATA)
                 break;
		questmaster->award_vnum[++vnum]	= fread_number( fp );
		questmaster->award_value[++value] = fread_number( fp );
		fMatch = TRUE;
                break;
            }
	    break;
	}
	if ( !fMatch )
	{
	    sprintf( buf, "fread_questmaster: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void do_queststat( CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  char arg4[MIL];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  argument = one_argument( argument, arg4 );

  if (arg1[0] == '\0')
  {
    sh_int i, count=0;

    for(i = 0; i != MAX_QDATA; i++)
    {
       ch_printf( ch, "&p[&w%2d&p] [&g%-35s&p] [&w%6s&p] [&w%6d&p] \n\r", count, questmaster->award_vnum[i] ? get_obj_index(questmaster->award_vnum[i])->short_descr : "Empty", num_punct(questmaster->award_value[i]), questmaster->award_vnum[i] );
       ++count;
    }
    ch_printf( ch, "&p[&w**&p] [&Y%s Gold%-20s&p] [&w%6s&p] \n\r", num_punct(questmaster->gold_amt), "", num_punct(questmaster->gold_cost));
    ch_printf( ch, "&p[&w**&p] [&Y%d Practices%-23s&p] [&w%6s&p] \n\r", questmaster->practice_amt, "", num_punct(questmaster->practice_cost)  );
    ch_printf( ch, "&p[&w**&p] [%-37s&p] [&w%6d&p]\n\r", "&YLevel Range", questmaster->level_range  );
    return;
  }
  if (!str_cmp(arg1, "Object"))
  {
   int seek=atoi(arg2), vnum=atoi(arg3), value=atoi(arg4);
   sh_int i, count=0;

   if (vnum < 0|| seek < 0|| value < 0)
   {
    send_to_char( "Syntax: queststat object # vnum value\n\r", ch );
    return;
   }
   
   for(i = 0; i != MAX_QDATA; i++)
   {
     if (count == seek)
      break;
     ++count;
   }

   if (seek != count)
   {
    send_to_char( "# Not found.\n\r", ch );
    send_to_char( "Syntax: queststat object # vnum value\n\r", ch );
    return;
   }
   else
   {
    questmaster->award_vnum[i] = vnum;
    questmaster->award_value[i] = value;
   }
   fwrite_questmaster();
   send_to_char( "Done.\n\r", ch );
   return;
  }
  if (!str_cmp(arg1, "Gold"))
  {
   int amount=atoi(arg2), value=atoi(arg3);

   if (!amount || !value)
   {
    send_to_char( "Syntax: queststat gold amount value\n\r", ch );
    return;
   }
   questmaster->gold_amt = amount;
   questmaster->gold_cost = value;
   fwrite_questmaster();
   send_to_char( "Done.\n\r", ch );
   return;
  }
  if (!str_cmp(arg1, "Practices"))
  {
   int amount=atoi(arg2), value=atoi(arg3);

   if (!amount || !value)
   {
    send_to_char( "Syntax: queststat practices amount value\n\r", ch );
    return;
   }
   questmaster->practice_amt = amount;
   questmaster->practice_cost = value;
   fwrite_questmaster();
   send_to_char( "Done.\n\r", ch );
   return;
  }
  if (!str_cmp(arg1, "Range"))
  {
   int value=atoi(arg2);

   if (!value)
   {
    send_to_char( "Syntax: queststat range value\n\r", ch );
    return;
   }
   questmaster->level_range = value;
   fwrite_questmaster();
   send_to_char( "Done.\n\r", ch );
   return;
  }
  do_queststat(ch, "");
  return;
}


