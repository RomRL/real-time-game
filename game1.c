/* game1.c - main, prntr */

//#include <conf.h>
//#include <kernel.h>
//#include <io.h>
//#include <bios.h>

#include <dos.h>
#include <stdio.h>
 

#define ARROW_NUMBER 30
#define TARGET_NUMBER 4
#define ARRSIZE 1000

void  interrupt (*old_int9)(void);

char entered_ascii_codes[ARRSIZE];
int tail = -1;
char display[2001];

char ch_arr[ARRSIZE];

int front = -1;
int rear = -1;

int point_in_cycle;
int gcycle_length;
int gno_of_pids;

int initial_run = 1;
  int gun_position;           
  int no_of_arrows;
  int target_disp = 80/TARGET_NUMBER;
  char ch;

  int no_of_targets;

void my_halt()
{
   setvect(9, old_int9);
   asm {CLI}
   exit();
   
} // my_halt()

void  interrupt new_int9(void)
{
 char result = 0;
 int scan = 0;
  int ascii = 0;

(*old_int9)();


asm {
  MOV AH,1
  INT 16h
  JZ Skip1
  MOV AH,0
  INT 16h
  MOV BYTE PTR scan,AH
  MOV BYTE PTR ascii,AL
 } //asm

 ascii = 0;
 if (scan == 75)
   ascii = 'a';
 else
   if (scan == 72)
     ascii = 'w';
   else
   if (scan == 77)
      ascii = 'd';
// if ((scan == 46)&& (ascii == 3)) // Ctrl-C?
   if (scan == 1) // Esc?
     my_halt(); // terminate program

  if ((ascii != 0) && (tail < ARRSIZE))
   {
      entered_ascii_codes[++tail] = ascii;
   } // if

Skip1:

} // new_int9



typedef struct position
{
  int x;
  int y;

}  POSITION;



void displayer( void )
{
	//while (1)
         //{               
               printf(display);
         //} //while
} // prntr

void receiver()
{
    char temp;
    while( tail > -1)
    {
      temp = entered_ascii_codes[tail];
      rear++;
      tail--;
      if (rear < ARRSIZE)
          ch_arr[rear] = temp;
      if (front == -1)
         front = 0;
   } // while

} //  receiver


char display_draft[25][80];
POSITION target_pos[TARGET_NUMBER];
POSITION arrow_pos[ARROW_NUMBER];


void updater()
{

  int i,j;

  if (initial_run == 1)
  {
    initial_run = 0;
    no_of_arrows = 0;

    no_of_targets = TARGET_NUMBER;

    gun_position = 39;



    target_pos[0].x = 3;
    target_pos[0].y = 0; 


    for(i=1; i < TARGET_NUMBER; i++)
    {
      target_pos[i].x = i*target_disp;
      target_pos[i].y = 0; 

    } // for
    for(i=0; i < ARROW_NUMBER; i++)
         arrow_pos[i].x =  arrow_pos[i].y = -1;

   } // if (initial_run ==1)

   while(front != -1)
   {
     ch = ch_arr[front];
     if(front != rear)
       front++;
     else
       front = rear = -1;

     if ( (ch == 'a') || (ch == 'A') )
       if (gun_position >= 2 )
              gun_position--;
       else;
     else if ( (ch == 'd') || (ch == 'D') )
       if (gun_position <= 78 )
         gun_position++;
       else;
     else if ( (ch == 'w') || (ch == 'W') )
       if (no_of_arrows < ARROW_NUMBER)
       {
         arrow_pos[no_of_arrows].x = gun_position;
         arrow_pos[no_of_arrows].y = 23;
         no_of_arrows++;

       } // if
   } // while(front != -1)

     ch = 0;
     for(i=0; i < 25; i++)
        for(j=0; j < 80; j++)
            display_draft[i][j] = ' ';  // blank

    display_draft[22][gun_position] = '^';
    display_draft[23][gun_position-1] = '/';
    display_draft[23][gun_position] = '|';
    display_draft[23][gun_position+1] = '\\';
    display_draft[24][gun_position] = '|';

    for(i=0; i < ARROW_NUMBER; i++)
       if (arrow_pos[i].x != -1)
       {
         if (arrow_pos[i].y > 0)
           arrow_pos[i].y--;
           display_draft[arrow_pos[i].y][arrow_pos[i].x] = '^';
           display_draft[arrow_pos[i].y+1][arrow_pos[i].x] = '|';

       } // if

    for(i=0; i < TARGET_NUMBER; i++)
       if (target_pos[i].x != -1)
        {
         if (target_pos[i].y < 22)
              target_pos[i].y++;
         display_draft[target_pos[i].y][target_pos[i].x] = '*';
        } // if

    for(i=0; i < 25; i++)
      for(j=0; j < 80; j++)
        display[i*80+j] = display_draft[i][j];
    display[2000] = '\0';


} // updater 



main()
{
        int uppid, dispid, recvpid;


  old_int9 = getvect(9);
  setvect(9, new_int9);

  while(1)
  {
     receiver();
     updater();
     displayer();
   
 //      delay(2700);
     sleep(1);

  } // while

} // main
