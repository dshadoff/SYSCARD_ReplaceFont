/*

 *

 * Seektest2 - attempts 10 seeks for minimum and average delays
 *
 
*/


#include "huc.h"

#define TITLE_LINE       0

#define START_LINE       3
#define START_UNDER      4
#define RESULT_LINE      5

#define KANJI_START      7

#define STAT_LINE        11
#define READING_LINE     13
#define MIN_LINE         14

#define AVG_LINE         28

#define ADJUST_START     1
#define RUN_TEST         3


#define MAX_SIZE         1



#asm
; get_font(void) - references char sjis_h, char sjis_l, char * target_buf
_get_font:

    lda _sjis_h       ; SJIS code
    sta <_ah
    lda _sjis_l
    sta <_al

    lda _size         ; data read address type (LOCAL)
    sta <_dh

    lda #HIGH(_target_buf)    ; memory address
    sta <_bh
    lda #LOW(_target_buf)
    sta <_bl

    call ex_getfnt

    sta _retcode
    rts
 
#endasm 


int sjis_iter;
char sjis_h;
char sjis_l;
char retcode;
char size;

char xbase;
char ybase;
char xpos;
char ypos;
int  location;

int bat_loc1;
int bat_loc2;
int bat_loc3;
int bat_loc4;
int char_loc1;
int char_loc2;
int char_loc3;
int char_loc4;
int ptr_loc1;
int ptr_loc2;
int ptr_loc3;
int ptr_loc4;

char quad1[32]; /* top left     */
char quad2[32]; /* top right    */
char quad3[32]; /* bottom left  */
char quad4[32]; /* bottom right */

char target_buf[2048];

char pad;
char padhold;
char hex1;
char hex2;

char start_pos[3];
char result[3];


/* create array of offsets for adjusting startpos/offset: */

const char add_num[] = {
 0x10, 0x00,
 0x01, 0x00,
 0x00, 0x10,
 0x00, 0x01
};

const char page_add[] = {
 0x00, 0x80
};


char adjust;
char edit_pos;
char direction;


char i;

int j;
int j1;
int j2;
int j3;
int j4;



/* result shown in result[] array: */

add_2digit(base1, base2)
char * base1;
char * base2;
{
   int temp1, temp2;
   int temp_sum;
   int carry;

   temp1 = (*(base1+1)) & 0xff;
   temp2 = (*(base2+1)) & 0xff;

   temp_sum = temp1 + temp2;
   carry = (temp_sum) >> 8;
   result[1] = temp_sum & 0xff;

   temp1 = (*(base1)) & 0xff;
   temp2 = (*(base2)) & 0xff;

   temp_sum = temp1 + temp2 + carry;
   result[0] = temp_sum & 0xff;
}

/* result shown in result[] array: */

sub_2digit(base1, base2)
char * base1;
char * base2;
{
   int temp1, temp2;
   int temp_sum;
   int carry;

   temp1 = (*(base1+1)) & 0xff;
   temp2 = (*(base2+1)) & 0xff;

   temp_sum = temp1 - temp2;
   carry = ((temp_sum) >> 8) & 1;
   result[1] = temp_sum & 0xff;

   temp1 = (*(base1)) & 0xff;
   temp2 = (*(base2)) & 0xff;

   temp_sum = temp1 - temp2 - carry;
   result[0] = temp_sum & 0xff;
}


show_startpos()
{
   put_hex(start_pos[0], 2, 13, START_LINE);
   put_hex(start_pos[1], 2, 15, START_LINE);
   put_hex(size, 2, 28, START_LINE);
}


clear_index()
{
   put_string("      ", 13, START_UNDER);
}

put_index()
{
   if (adjust == ADJUST_START)
   {
       put_string("      ", 13, START_UNDER);
       put_string("^", 13+edit_pos, START_UNDER);
   }
}

draw_kanji()  /* uses char xbase, char ybase, char xpos, char ypos */
{

   bat_loc1  = (((ypos * 2) + ybase) * 64) + (xpos * 2) + xbase ;
   bat_loc2  = bat_loc1 + 1;
   bat_loc3  = bat_loc1 + 64;
   bat_loc4  = bat_loc3 + 1;

   char_loc1 = (ypos * 128 * 16) + (xpos * 32) + 0x1000;
   char_loc2 = char_loc1 + 16;
   char_loc3 = char_loc1 + 1024;
   char_loc4 = char_loc3 + 16;


   /* if good, point at the kanji */
   /* if not, point at a space    */

   if (retcode == 0)
   {
      ptr_loc1 = char_loc1 >> 4;
      ptr_loc2 = ptr_loc1 + 1;
      ptr_loc3 = ptr_loc1 + 64;
      ptr_loc4 = ptr_loc3 + 1;
   }
   else
   {
      ptr_loc1 = 0xFE0 >> 4;
      ptr_loc2 = ptr_loc1 + 1;
      ptr_loc3 = ptr_loc1 + 64;
      ptr_loc4 = ptr_loc3 + 1;
   }

   for (j = 0; j < 8; j++)
   {
      j1 = j * 2;
      j2 = j1 + 1;
      j3 = j1 + 8;
      j4 = j1 + 9;

      quad1[j1] = target_buf[j1];
      quad1[j2] = 0;
      quad1[j3] = 0;
      quad1[j4] = 0;

      quad2[j1] = target_buf[j2];
      quad2[j2] = 0;
      quad2[j3] = 0;
      quad2[j4] = 0;

      quad3[j1] = target_buf[j1 + 16];
      quad3[j2] = 0;
      quad3[j3] = 0;
      quad3[j4] = 0;

      quad4[j1] = target_buf[j1 + 17];
      quad4[j2] = 0;
      quad4[j3] = 0;
      quad4[j4] = 0;
   }
   

   put_hex(bat_loc1, 4, 2, AVG_LINE);
   put_hex(char_loc1, 4, 2, AVG_LINE+1);


   /* Blit the Kanji data into the CHR VRAM area */

#asm
   lda   _char_loc1
   sta   <_di
   lda   _char_loc1+1
   sta   <_di+1
   jsr   set_write
   TIA   _quad1, video_data_l, 32

   lda   _char_loc2
   sta   <_di
   lda   _char_loc2+1
   sta   <_di+1
   jsr   set_write
   TIA   _quad2, video_data_l, 32

   lda   _char_loc3
   sta   <_di
   lda   _char_loc3+1
   sta   <_di+1
   jsr   set_write
   TIA   _quad3, video_data_l, 32

   lda   _char_loc4
   sta   <_di
   lda   _char_loc4+1
   sta   <_di+1
   jsr   set_write
   TIA   _quad4, video_data_l, 32
#endasm

   /* update the BAT with pointers to the CHR area */

#asm
   lda   _bat_loc1
   sta   <_di
   lda   _bat_loc1+1
   sta   <_di+1
   jsr   set_write

   lda   _ptr_loc1
   sta   video_data_l
   lda   _ptr_loc1+1
   sta   video_data_h

   lda   _bat_loc2
   sta   <_di
   lda   _bat_loc2+1
   sta   <_di+1
   jsr   set_write

   lda   _ptr_loc2
   sta   video_data_l
   lda   _ptr_loc2+1
   sta   video_data_h

   lda   _bat_loc3
   sta   <_di
   lda   _bat_loc3+1
   sta   <_di+1
   jsr   set_write

   lda   _ptr_loc3
   sta   video_data_l
   lda   _ptr_loc3+1
   sta   video_data_h

   lda   _bat_loc4
   sta   <_di
   lda   _bat_loc4+1
   sta   <_di+1
   jsr   set_write

   lda   _ptr_loc4
   sta   video_data_l
   lda   _ptr_loc4+1
   sta   video_data_h

#endasm

}

main()
{
   set_color(0,0);
   set_color(1,511);

   start_pos[0] = 0;
   start_pos[1] = 0;

   size = 0;

   edit_pos = 0;
   adjust = ADJUST_START;

   put_string("Kanji Test", 10, TITLE_LINE);

   put_string("SJIS Code: ", 2, START_LINE);
   put_string("sizeflag: ", 19, START_LINE);

   show_startpos();

   xbase = 0;
   ybase = KANJI_START;

/*   put_string("Change Values: direction pad", 2, INSTRUCT_LINE1);  */
/*   put_string("Button II = fwd", 2, INSTRUCT_LINE3);  */

   while(1)
   {

      put_index();

      pad = joytrg(0);

      if (pad & JOY_LEFT)
      {
         if (edit_pos > 0)
         {
            edit_pos -= 1;
            put_index();
         }
      }

      if (pad & JOY_RGHT)
      {
         if (edit_pos < 3)
         {
            edit_pos += 1;
            put_index();
         }
      }

      if (pad & JOY_SLCT)
      {
         add_2digit(&start_pos[0], &page_add[0]);

         start_pos[0] = result[0];
         start_pos[1] = result[1];
         show_startpos();

         pad = JOY_B;
      }

      if (pad & JOY_UP)
      {
         add_2digit(&start_pos[0], &add_num[(edit_pos*2)]);

         start_pos[0] = result[0];
         start_pos[1] = result[1];
         show_startpos();

      }

      if (pad & JOY_DOWN)
      {
         sub_2digit(&start_pos[0], &add_num[(edit_pos*2)]);

         start_pos[0] = result[0];
         start_pos[1] = result[1];
         show_startpos();
      }

      if (pad & JOY_A)  /* this is button I  */
      {
         size++;

         if (size > MAX_SIZE)
         {
            size = 0;
         }
         show_startpos();

      }

      else if (pad & JOY_B)  /* this is button II */
      {
         clear_index();

         /* run test */

         sjis_iter = (start_pos[0] * 256) + start_pos[1];

         for (ypos = 0; ypos < 8; ypos++)
         {
            for (xpos = 0; xpos < 16; xpos++)
            {
               sjis_h = (sjis_iter >> 8);
               sjis_l = (sjis_iter & 255);

               get_font();

               draw_kanji();
               sjis_iter++;
            }
         }
         put_string("Result: ", 2, RESULT_LINE);
         put_hex( retcode, 2, 10, RESULT_LINE);
      }

      vsync(0);
   }

   return;
}
