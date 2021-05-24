#ifndef CmdParser_h
#define CmdParser_h

#include "StateMachine.h"

class CmdParser {
	private:
		String _cmd;
		String _token;
		int _tokenIdx;
		int _lastTokenIdx;

		StateMachine* _parser;

		State* _start;
		State* _to;
		State* _for;
		State* _up;
		State* _down;
		State* _set;
		State* _pos;
		State* _add;
		State* _sub;

		void s_token();
		void s_up();
		void s_down();
		void s_set();
		void s_pos();
		void s_add();
		void s_sub();

		bool t_head();
		bool t_feet();
		bool t_to();
		bool t_for();
		bool t_up();
		bool t_down();
		bool t_the();
		bool t_num();
		bool t_all();
		bool t_by();
		bool t_deg();
	public:
		CmdParser();
		~CmdParser();
		void exec(String cmd);
}

#endif
