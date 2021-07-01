#include <LinkedList.h>
#include "State.h"

#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

class StateMachine {
	public:
		// Methods
		StateMachine();
		~StateMachine();
		void init();
		void run();
		void exec();

		// When a stated is added we pass the function that represents
		// that state logic
		State* addState(void (*functionPointer)());
		State* transitionTo(State* s);
		int transitionTo(int i);

		// Attributes
		LinkedList<State*> *stateList;
		bool executeOnce = true; // transition to a different state has occurred
		int currentState = 0;    // the current state number
};



#endif
