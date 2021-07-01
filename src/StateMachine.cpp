// https://github.com/knickers/StateMachine

#include "StateMachine.h"

StateMachine::StateMachine() {
	stateList = new LinkedList<State*>();
};

StateMachine::~StateMachine() {};

/*
 * Main execution of the machine occurs here in run
 * The current state is executed and it's transitions are evaluated
 * to determine the next state.
 *
 * By design, only one state is executed in one loop() cycle.
 */
void StateMachine::run() {
	//Serial.println("StateMachine::run()");
	// Early exit, no states are defined
	if (stateList->size() == 0) return;

	int initialState = currentState;

	// Execute state logic and return transitioned
	// to state number. Remember the current state then check
	// if it wasnt't changed in state logic. If it was, we
	// should ignore predefined transitions.
	int next = stateList->get(currentState)->execute();

	if (initialState == currentState) {
		executeOnce = currentState != next;

		if (next > -1 && next < stateList->size()) {
			currentState = next;
		}
	}
}

/*
 * Execute the entire state machine in one call.
 * Returns when a state is reached that has no possible transitions
 */
void StateMachine::exec() {
	// Early exit, no states are defined
	if (stateList->size() == 0) return;

	// Initial condition
	int initialState = currentState = 0;
	int next = 0;

	do {
		initialState = currentState;

		// Execute state logic and return transitioned
		// to state number. Remember the current state then check
		// if it wasnt't changed in state logic. If it was, we
		// should ignore predefined transitions.
		next = stateList->get(currentState)->execute();

		if (initialState == currentState) {
			executeOnce = currentState != next;
			currentState = next;
		}
	} while (next > -1 && next < stateList->size());
}

/*
 * Adds a state to the machine
 * It adds the state in sequential order.
 */
State* StateMachine::addState(void(*functionPointer)()) {
	State* s = new State();
	s->stateLogic = functionPointer;
	stateList->add(s);
	s->index = stateList->size()-1;
	return s;
}

/*
 * Jump to a state
 * given by a pointer to that state.
 */
State* StateMachine::transitionTo(State* s) {
	this->currentState = s->index;
	this->executeOnce = true;
	return s;
}

/*
 * Jump to a state
 * given by a state index number.
 */
int StateMachine::transitionTo(int i) {
	if (i < stateList->size()) {
		this->currentState = i;
		this->executeOnce = true;
		return i;
	}
	return currentState;
}
