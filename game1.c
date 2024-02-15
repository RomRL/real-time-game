/* game1.c - main, prntr */

// #include <conf.h>
// #include <kernel.h>
// #include <io.h>
// #include <bios.h>

#include <dos.h>
#include <stdio.h>

#define ARROW_NUMBER 3000
#define TARGET_NUMBER 2
#define ARRSIZE 1000

void interrupt (*old_int9)(void);
void interrupt (*old_int8)(void); // For timer interrupt
volatile int ticks = 0;
void my_sleep(float seconds);
int counter_hit = 0;
float sleep_time = 1;
int restart_game = 0;
int level = 1;

void interrupt new_int8(void)
{
  ticks++;
  (*old_int8)();
}

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
int target_disp = 80 / TARGET_NUMBER;
char ch;

int no_of_targets;

void my_halt()
{
  setvect(9, old_int9);
  setvect(8, old_int8); // Restore the original timer interrupt handler
  asm {CLI}
  exit();

} // my_halt()
  // srconds 1==18 ticks
void my_sleep(float seconds)
{
  float target_ticks = ticks + seconds * 18; // 18.2 ticks per second
  while (ticks < target_ticks);

}

void interrupt new_int9(void)
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
  } // asm

  ascii = 0;
  if (scan == 75)
    ascii = 'a';
  else if (scan == 72)
    ascii = 'w';
  else if (scan == 77)
    ascii = 'd';
  // if ((scan == 46)&& (ascii == 3)) // Ctrl-C?
  if ((scan == 46) && (ascii == 3)) // Ctrl-C?
    my_halt();                      // terminate program
  if (scan == 1)                    // Esc?
    my_halt();                      // terminate program

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

} POSITION;

void displayer(void)
{
  // while (1)
  //{
  if (counter_hit != TARGET_NUMBER)
    printf(display);
  //} //while
} // prntr

void receiver()
{
  char temp;
  while (tail > -1)
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

  int i, j;

  if (initial_run == 1)
  {
    counter_hit = 0; // Reset the counter

    initial_run = 0;
    no_of_arrows = 0;

    no_of_targets = TARGET_NUMBER;

    gun_position = 39;

    target_pos[0].x = 3;
    target_pos[0].y = 0;

    for (i = 1; i < TARGET_NUMBER; i++)
    {
      target_pos[i].x = i * target_disp;
      target_pos[i].y = 0;

    } // for
    for (i = 0; i < ARROW_NUMBER; i++)
      arrow_pos[i].x = arrow_pos[i].y = -1;

  } // if (initial_run ==1)

  // Check if any target has reached the bottom
  for (i = 0; i < TARGET_NUMBER; i++)
  {
    if (target_pos[i].x != -1 && target_pos[i].y >= 22)
    {
      // Target reached the bottom, game over
      printf("\n\t\t\tGame Over! Targets reached the bottom.\n");
      my_halt();
    }
  }

  while (front != -1)
  {
    ch = ch_arr[front];
    if (front != rear)
      front++;
    else
      front = rear = -1;

    if ((ch == 'a') || (ch == 'A'))
      if (gun_position >= 2)
        gun_position--;
      else
        ;
    else if ((ch == 'd') || (ch == 'D'))
      if (gun_position <= 78)
        gun_position++;
      else
        ;
    else if ((ch == 'w') || (ch == 'W'))
      if (no_of_arrows < ARROW_NUMBER)
      {
        arrow_pos[no_of_arrows].x = gun_position;
        arrow_pos[no_of_arrows].y = 23;
        no_of_arrows++;

      } // if
  }     // while(front != -1)

  ch = 0;
  for (i = 0; i < 25; i++)
    for (j = 0; j < 80; j++)
      display_draft[i][j] = ' '; // blank

  display_draft[22][gun_position] = '^';
  display_draft[23][gun_position - 1] = '/';
  display_draft[23][gun_position] = '|';
  display_draft[23][gun_position + 1] = '\\';
  display_draft[24][gun_position] = '|';
  //print the level in the top right corner
  display_draft[0][81] = 'L';
  display_draft[0][82] = 'e';
  display_draft[0][83] = 'v';
  display_draft[0][84] = 'e';
  display_draft[0][85] = 'l';
  display_draft[0][86] = ':';
  display_draft[0][87] = level + '0';
  

  for (i = 0; i < ARROW_NUMBER; i++) // SHOOTED ARROWS
  {
    if (arrow_pos[i].x != -1) // If arrow is in the air
    {
      if (arrow_pos[i].y > 0)
      {
        arrow_pos[i].y--;
        display_draft[arrow_pos[i].y][arrow_pos[i].x] = '^';
        display_draft[arrow_pos[i].y + 1][arrow_pos[i].x] = '|';
      }

    } // if
  }

  for (i = 0; i < TARGET_NUMBER; i++)
    if (target_pos[i].x != -1)
    {
      if (target_pos[i].y < 22)
        target_pos[i].y++;
      display_draft[target_pos[i].y][target_pos[i].x] = '*';
    } // if
  // check if any arrow has hit any target
  // Check if any arrow has hit any target
  // Check if any arrow has hit any target
for (i = 0; i < ARROW_NUMBER; i++)
{
    if (arrow_pos[i].x != -1) // If arrow is in the air
    {
        for (j = 0; j < TARGET_NUMBER; j++)
        {
            if (target_pos[j].x != -1) // If target is still there
            {
                if (arrow_pos[i].x == target_pos[j].x && arrow_pos[i].y <= target_pos[j].y)
                {
                    counter_hit++;
                    arrow_pos[i].x = arrow_pos[i].y = -1;
                    target_pos[j].x = target_pos[j].y = -1;
                    // If you want to break only from the inner loop
                    // break; // exit the inner loop
                } // if
            }     // if
        }         // for
    }             // if
}

  for (i = 0; i < 25; i++)
    for (j = 0; j < 80; j++)
      display[i * 80 + j] = display_draft[i][j];
      
  display[2000] = '\0';
  if (counter_hit == TARGET_NUMBER)
  {
    initial_run = 1;
    printf("\n\t\t\tCongratulations! You completed Level %d\n\t\t\tGet Ready For Faster Game !!\n", level);
    my_sleep(2);
    sleep_time -= 0.05;
    level++;
  }

} // updater

main()
{
  int uppid, dispid, recvpid;
  old_int8 = getvect(8);
  old_int9 = getvect(9);
  setvect(9, new_int9);
  setvect(8, new_int8);

  while (1)
  {
    receiver();
    updater();
    displayer();

    //      delay(2700);
    my_sleep(sleep_time);

  } // while

} // main
