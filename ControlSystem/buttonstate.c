/*
 * buttonstate.c
 *
 * Created: 10/25/2017 11:39:52 PM
 *  Author: c golz
 */
#include <avr/io.h>
#include "buttonstate.h" 
#include "MagSwipe.h"

void getNextFloor();
uint8_t checkIfUp();
uint8_t checkIfDown();
void clearButtons();


void getNextFloor()
{
	uint8_t determined = 0;	//checks if next floor has been identified
	if (upDown == 0)	//if going up, searches for next floor upwards, if down checks for next floor downwards
	{
		if((determined = checkIfUp()) == 0)	//checks if a floor at or above current floor was requested in up direction
			if((determined = checkIfDown()) == 1)	//if no floor in up, checks if floor is requested in down direction
				upDown = 1;	//if a floor was requested in opposite direction, direction was changed
	}
	else
	{
		if((determined = checkIfDown()) == 0)//similar to above, but down floors first, then up floors
			if((determined = checkIfUp()) == 1)
				upDown = 0;
	}
	//checks if a floor was determined or if elevator should return to bottom floor
	if(determined == 1)
		return;
	else if((buttonState & ~( (1 << Butt_DoorOpen)|(1 << Butt_DoorClose)|(1 << Butt_Emergency)/*|(!datavalid << Butt_Floor3) */)) == 0) //if no floor request buttons were pushed
	{													//return elevator to 1st floor ready to go up
		upDown = 1;
		nextFloor = 1;
		nextDir = 0;
		return;
	}
}

uint8_t checkIfUp()	//determines if there is a floor requested at or above current floor
{
	uint8_t determined = 0;	//returns a 1 if a destination has been determined
	switch(floorNum)
	{	//checks from bottom to top and will move cab in the up direction
		case 1:	
		if((buttonState & ( (1<<Butt_Floor1Up)|(1 << Butt_Floor1) )) > 0)
		{	//break in if conditional to check all if statements in the next cases until one is true, or all have been checked
			nextFloor = 1;
			nextDir = 0;
			determined = 1;
			break;
		}
		
		case 2:	
		if((buttonState & ( (1<<Butt_Floor2Up)|(1 << Butt_Floor2) )) > 0)
		{
			nextFloor = 2;
			nextDir = 0;
			determined = 1;
			break;
		}
		
		case 3:	
		if((buttonState & ( (1<<Butt_Floor3Up)|(datavalid << Butt_Floor3) )) > 0)
		{
			nextFloor = 3;
			nextDir = 0;
			determined = 1;
			break;
		}
		
		case 4:	
		if((buttonState & ( (1<<Butt_Floor4Up)|(1 << Butt_Floor4) )) > 0)
		{
			nextFloor = 4;
			nextDir = 0;
			determined = 1;
			break;
		}
		
		case 5: 
		if((buttonState & ( (1<<Butt_Floor5)|(1 << Butt_Floor5Dwn) )) > 0)
		{
			nextFloor = 5;
			nextDir = 1;
			determined = 1;
			break;
		}

		default:
		if( ((buttonState & (1 << Butt_Floor4Dwn)) > 0) && (floorNum < 4) )
		{
			nextFloor = 4;
			nextDir = 1;
			determined = 1;
			break;
		}
		if( ((buttonState & (1 << Butt_Floor3Dwn)) > 0) && (floorNum < 3) )
		{
			nextFloor = 3;
			nextDir = 1;
			determined = 1;
			break;
		}
		if( ((buttonState & (1 << Butt_Floor2Dwn)) > 0) && (floorNum < 2) )
		{
			nextFloor = 2;
			nextDir = 1;
			determined = 1;
			break;
		}
		break;
	}
	return determined;
}

uint8_t checkIfDown()//determines if there is a floor requested at or below current floor
{
	uint8_t determined = 0;	//returns a 1 if a destination has been determined
	switch(floorNum)
	{	//checks from top to bottom and will move cab in the down direction
		case 5:	
		if((buttonState & ( (1<<Butt_Floor5Dwn)|(1 << Butt_Floor5) )) > 0)
		{	//break in if conditional to check all if statements in the next cases until one is true, or all have been checked
			nextFloor = 5;
			nextDir = 1;
			determined = 1;
			break;
		}
		
		case 4:	
		if((buttonState & ( (1<<Butt_Floor4Dwn)|(1 << Butt_Floor4) )) > 0)
		{
			nextFloor = 4;
			nextDir = 1;
			determined = 1;
			break;
		}
		
		case 3:	
		if((buttonState & ( (1<<Butt_Floor3Dwn)|(datavalid << Butt_Floor3) )) > 0)
		{
			nextFloor = 3;
			nextDir = 1;
			determined = 1;
			break;
		}
		
		case 2:	
		if((buttonState & ( (1<<Butt_Floor2Dwn)|(1<< Butt_Floor2) )) > 0)
		{
			nextFloor = 2;
			nextDir = 1;
			determined = 1;
			break;
		}

		case 1: 
		if((buttonState & ( (1<<Butt_Floor1)|(1 << Butt_Floor1Up) )) > 0)
		{
			nextFloor = 1;
			nextDir = 0;
			determined = 1;
			break;
		}

		default:	
		if( ((buttonState & (1 << Butt_Floor2Up)) > 0) && (floorNum > 2) )
		{
			nextFloor = 2;
			nextDir = 0;
			determined = 1;
			break;
		}
		if( ((buttonState & (1 << Butt_Floor3Up)) > 0) && (floorNum > 3) )
		{
			nextFloor = 3;
			nextDir = 0;
			determined = 1;
			break;
		}
		if( ((buttonState & (1 << Butt_Floor4Up)) > 0) && (floorNum > 4) )
		{
			nextFloor = 4;
			nextDir = 0;
			determined = 1;
			break;
		}
		break;
	}
	
	return determined;
}

void clearButtons()	//clears buttons based on floor location and direction
{
	switch(floorNum)
	{
		case 1: buttonState &= ~((1 << Butt_Floor1)|(1 << Butt_Floor1Up));	//if floor one was reached, clears both floor 1 buttons
		break;															//&~upDown ands all 1's if moving up	& upDown|0xFFFE ands all 1's if moving down
		case 2: buttonState &= ~((1 << Butt_Floor2)|(((!upDown) << Butt_Floor2Up))|((upDown << Butt_Floor2Dwn)));
		break;			//if floor 2 was reached, clears floor2 button and either floor2up if moving up or floor2dwn if moving down
		case 3: buttonState &= ~((1 << Butt_Floor3)|(((!upDown) << Butt_Floor3Up))|((upDown << Butt_Floor3Dwn)));datavalid=0;
		break;			//if floor 3 was reached, clears floor3 button and either floor3up if moving up or floor3dwn if moving down
		case 4: buttonState &= ~((1 << Butt_Floor4)|(((!upDown) << Butt_Floor4Up))|((upDown << Butt_Floor4Dwn)));
		break;			//if floor 2 was reached, clears floor4 button and either floor4up if moving up or floor4dwn if moving down
		case 5: buttonState &= ~((1 << Butt_Floor5)|(1 << Butt_Floor5Dwn));	//if floor 5 was reached, clears both floor 5 buttons
		break;
		default: break;
	}
	
	buttonled();
	
}