#include "ncurses-v4l2-ctl.hpp"

int main() {
	NCurses_v4l2_ctl curses;
	curses.initialize();
	char cmd;
	do{
		noecho();
		if(curses.parseCommands())
			curses.editValue();
	}while( cmd != 27);
	return 0;
}