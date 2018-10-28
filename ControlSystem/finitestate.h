/*
 * finitestate.h
 *
 * Created: 10/25/2017 8:52:41 PM
 *  Author: Gulaa
 */ 


#ifndef FINITESTATE_H_
#define FINITESTATE_H_



typedef enum state{
	IDLE,
	PROCESS,
	MOVING,
	TRAN,
	EMERGENCY
	
	}STATE;

volatile STATE curr_state;
void statetransition(STATE next);

#endif /* FINITESTATE_H_ */