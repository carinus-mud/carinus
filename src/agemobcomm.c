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

Mobile Command Module
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"


void do_mp_grant( CHAR_DATA* ch, const char* argument)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int sn, max;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Mpgrant: bad syntax", ch );
      progbug( "Mpgrant - Bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      progbug( "Mpgrant: Invalid student not in room", ch );
      return;
   }

   if( ( sn = skill_lookup( arg2 ) ) < 0 )
   {
      progbug( "Mpgrant: Invalid spell/skill name", ch );
      return;
   }


   if( IS_NPC( victim ) )
   {
      send_to_char( "Mppgrant: Can't train a mob", ch );
      progbug( "Mpgrant: Can't train a mob", ch );
      return;
   }


   max = atoi( arg3 );
   if( ( max < 0 ) || ( max > 100 ) )
   {
      snprintf( log_buf, MAX_STRING_LENGTH, "mp_grant: Invalid maxpercent: %d", max );
      send_to_char( log_buf, ch );
      progbug( log_buf, ch );
      return;
   }



   /*
    * past here, victim learns something 
    */
send_to_char( "Your equipment has granted you a new skill\r\n", ch);

   victim->pcdata->learned[sn] = 100;


   return;

}




void do_mp_ungrant( CHAR_DATA* ch, const char* argument)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int sn, max;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "mpungrant: bad syntax", ch );
      progbug( "mpungrant - Bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "mpungrant: Student not in room? Invis?", ch );
      progbug( "mpungrant: Invalid student not in room", ch );
      return;
   }

   if( ( sn = skill_lookup( arg2 ) ) < 0 )
   {
      send_to_char( "mpungrant: Invalid spell/skill name", ch );
      progbug( "mpungrant: Invalid spell/skill name", ch );
      return;
   }


   if( IS_NPC( victim ) )
   {
      send_to_char( "mpungrant: Can't grant a mob", ch );
      progbug( "mpungrant: Can't grant a mob", ch );
      return;
   }


   max = atoi( arg3 );
   if( ( max < 0 ) || ( max > 100 ) )
   {
      snprintf( log_buf, MAX_STRING_LENGTH, "mp_grant: Invalid maxpercent: %d", max );
      send_to_char( log_buf, ch );
      progbug( log_buf, ch );
      return;
   }

   /*
    * past here, victim unlearns something
    */

send_to_char( "You feel less knowldgeable\r\n", ch);
   victim->pcdata->learned[sn] = 0;

   return;

}

