// SpaceInvaders.c

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "stdlib.h"
#include "time.h"
void PortF_Init(void);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long Semaphore;


void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0   
      
}





// *************************** Images ***************************
// cars that starts at the top of the screen (arms/mouth closed)
const unsigned char car[] ={
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF

};

// *************************** Capture image dimensions out of BMP**********
#define carW        ((unsigned char)car[18])
#define carH        ((unsigned char)car[22])

/*#define BUNKERW     ((unsigned char)Bunker0[18])
#define BUNKERH     ((unsigned char)Bunker0[22])
#define ENEMY30W    ((unsigned char)SmallEnemy30PointA[18])
#define ENEMY30H    ((unsigned char)SmallEnemy30PointA[22])
#define ENEMY20W    ((unsigned char)SmallEnemy20PointA[18])
#define ENEMY20H    ((unsigned char)SmallEnemy20PointA[22])
#define ENEMY10W    ((unsigned char)SmallEnemy10PointA[18])
#define ENEMY10H    ((unsigned char)SmallEnemy10PointA[22])
#define ENEMYBONUSW ((unsigned char)SmallEnemyBonus0[18])
#define ENEMYBONUSH ((unsigned char)SmallEnemyBonus0[22])
#define LASERW      ((unsigned char)Laser0[18])
#define LASERH      ((unsigned char)Laser0[22])
#define MISSILEW    ((unsigned char)Missile0[18])
#define MISSILEH    ((unsigned char)Missile0[22])
#define PLAYERW     ((unsigned char)PlayerShip0[18])
#define PLAYERH     ((unsigned char)PlayerShip0[22])*/


//int screenW =84, screenH = 47;						//screen diminssions
int player_x, player_y ;	                  //player position(x, y)
int enemy_y, enemy_x;	                      //enemies position(x,y)
int score;
int tracks[3] = {12, 29, 46};					      
int flag1 = 0, flag2 = 0;							  		//flags



int main(void){ 
	PortF_Init();
	//enemy_y = rand()%3;
	player_x =68;
	player_y =29;	           	 //The first position for the player
	score = 0;
	
	 TExaS_Init(NoLCD_NoScope);
//  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
 // you cannot use both the Scope and the virtual Nokia (both need UART0)
  Random_Init(1);
  Nokia5110_Init();
  EnableInterrupts(); // virtual Nokia uses UART0 interrupts
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer
	
	
	
  // ***************************** Welcome screen *************************** //
	Nokia5110_Clear();
	Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("Aya Mahfouz");
  Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("Racing Car");
  Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("For Playing");
  Nokia5110_SetCursor(1, 4);
  Nokia5110_OutString("Press SW1");
 /* Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("GAME OVER");
  Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("Nice try,");
	Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("Earthling!");
  Nokia5110_SetCursor(2, 4);
  Nokia5110_OutUDec(1234);
  Nokia5110_SetCursor(0, 0); // renders screen
	*/
  while(1){
		if((GPIO_PORTF_DATA_R&0x10) == 0)         
			break;                            //For Playing Press SW1
	}
	        
	Nokia5110_ClearBuffer();
	Nokia5110_PrintBMP(player_x, player_y, car, 0);
	Nokia5110_DisplayBuffer();
	Delay100ms(70);	
	
	while(1){
		Nokia5110_ClearBuffer();		
		
								           /////// player Movement 
      if(!((GPIO_PORTF_DATA_R&0x11) == 0) ){	          //check on the two swiches
			if((GPIO_PORTF_DATA_R&0x10) == 0)	{              //if sw1 is bressed
				if((player_y > 12) && (flag1 == 0)){
					player_y -= 17;                              //move up
					flag1 = 1;
				}
			}
			else{
				flag1 = 0;}
 
			if((GPIO_PORTF_DATA_R&0x1) == 0){            //if sw2 is bressed
				if((player_y < 46) && (flag2 == 0)){
					player_y += 17;                         //move down  
					flag2 = 1;
				 }
				
			}
			else{
				flag2 = 0;}
		}
			Nokia5110_PrintBMP(player_x, player_y, car, 0);
		
		          ///////// enemys movement 
	
			if(enemy_x == 68){
				score++;					       //number of passed cars
				enemy_y = rand()%3;
        enemy_x = 12;  			
			}	
		
			enemy_x++;
		Nokia5110_PrintBMP(enemy_x, tracks[enemy_y],car, 0);
		Nokia5110_DisplayBuffer();                   // draw buffer
		Delay100ms(3);
			
			if(enemy_x >= 52 && player_y == tracks[enemy_y] ){     //If the enemy car hit the player's car
			break;
		}
	}
	
	Delay100ms(50);			      	//Delay befor gameover 

	 // Gameover 
  Nokia5110_Clear();
  Nokia5110_SetCursor(1, 0);
  Nokia5110_OutString("GAME OVER");
	Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("your score");
	Nokia5110_SetCursor(4, 4);
  Nokia5110_OutString("is");
	Nokia5110_SetCursor(2, 5);
  Nokia5110_OutUDec(score);
  while(1){
  }

}



// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Handler(void){ 	
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  TimerCount++;
	
	Semaphore = 1;
	
	 // trigger
}
void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240/80;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
