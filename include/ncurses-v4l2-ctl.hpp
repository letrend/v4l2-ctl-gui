#pragma once

#include <stdio.h>
#include <ncurses.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <regex>
#include <menu.h>

using namespace std;

enum COLORS{
	CYAN=1,
	RED,
	GREEN,
};

char welcomestring[] = "commandline tool for v4l device control";
char choosedevicestring[] = "choose video device";
char nodevicesstring[] = "could not find any video devices";
char invalidstring[] = "invalid";
char donestring[] = "done";
char valuestring[] = "value?  ";
char byebyestring[] = "BYE BYE!";

class NCurses_v4l2_ctl{
public:
	NCurses_v4l2_ctl(){
		//! start ncurses mode
		initscr();
		//! Start color functionality
		start_color();
		init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
		init_pair(RED, COLOR_RED, COLOR_BLACK);
		init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
		//! get the size of the terminal window
		getmaxyx(stdscr,rows,cols);
		//! enable special keys
		keypad(stdscr, TRUE);

		print(0,0,cols,"-");
		printMessage(1,0,welcomestring);
		print(2,0,cols,"-");
	}

	~NCurses_v4l2_ctl(){
		clearAll(0);
		printMessage(rows/2,cols/2-strlen(byebyestring)/2,byebyestring);
		refresh();
		usleep(1000000);
		endwin();
	}

	void initialize(){
		if(getDevices()){
			printMessage(3,0,choosedevicestring,CYAN);
			menu(devices,chosenDevice,4,0);
		}else{
			printMessage(4,0,nodevicesstring,RED);
		}
	}

	bool parseCommands(){
		commands.clear();
		commandVerbose.clear();
		char cmd[100];
		vector<string> out;
		sprintf(cmd,"v4l2-ctl -d %s -l", chosenDevice.second.c_str());
		if(!executeSystemCommand(cmd, out))
			return false;
		for(auto str:out){
			vector<string> singleWords = parseForWords(str);
			vector<string> command;
			command.push_back(singleWords[0]);
			command.insert(command.end(), singleWords.begin()+3, singleWords.end());
			commands.push_back(command);
			char s[200];
			strncpy(s,str.c_str(),str.size()-1);
			commandVerbose.push_back(s);
		}
		return !commands.empty();
	}

	void editValue(){
		menu(commandVerbose,chosenCmd,3,0);
		echo();
		printMessage(3,0,valuestring,CYAN);
		mvgetnstr(3,strlen(valuestring),inputstring,30);
		print(3,0,cols," ");
		int pos = atoi(inputstring);
		vector<string> out;
		char cmd[100];
		sprintf(cmd,"v4l2-ctl -d %s -c %s=%d", chosenDevice.second.c_str(), commands[chosenCmd.first][0].c_str(), pos);
		executeSystemCommand(cmd, out);
		if(!out.empty()){
			print(3,0,cols," ");
			printMessage(3,0,invalidstring,RED);
		}else{
			print(3,0,cols," ");
			printMessage(3,0,donestring,GREEN);
		}
		usleep(500000);
		print(3,0,cols," ");
	}

private:
	bool executeSystemCommand(const char* cmd, vector<string> &out){
		out.clear();
		FILE *in;
		if(!(in = popen(cmd, "r"))){
			return false;
		}
		char buffer[512];
		while(fgets(buffer, sizeof(buffer), in)!=NULL){
			out.push_back(buffer);
		};
		pclose(in);
		return true;
	}

	bool getDevices(){
		vector<string> out;
		if(!executeSystemCommand("v4l2-ctl --list-devices", out))
			return false;
		smatch m;
		regex r("/dev/video\\d");
		for(uint i=0; i<out.size(); i++) {
			if (regex_search(out[i], m, r)) {
				devices.push_back(make_pair(m[0], out[i-1]));
//				cout << devices.back().first << " " << devices.back().second << endl;
			}
		}
		return !devices.empty();
	}

	vector<string> parseForWords(string str){
		vector<string> words;
		regex rgx("(\\w+|-?\\d+)");
		regex_iterator<string::iterator> it(str.begin(), str.end(), rgx);
		regex_iterator<string::iterator> end;
		for (; it != end; ++it)
		{
			words.push_back(it->str());
		}
		return words;
	}

	void printMessage(uint row, uint col, char* msg){
		mvprintw(row,col,"%s", msg);
		refresh();
	}
	void printMessage(uint row, uint col, char* msg, uint color){
		mvprintw(row,col,"%s", msg);
		mvchgat(row, col, strlen(msg), A_BOLD, color, NULL);
		refresh();
	}
	void print(uint row, uint startcol, uint length, const char* s){
		for (uint i=startcol;i<startcol+length;i++){
			mvprintw(row,i,"%s",s);
		}
		refresh();
	}
	void clearAll(uint row){
		for(uint i=row;i<rows;i++){
			print(i,0,cols," ");
		}
		refresh();
	}
	bool menu(vector<pair<string,string>> options, pair<uint, string> &choose, int row, int col){
		bool success = false;
		ITEM *cur_item;
		cbreak();
		noecho();
		//! enable special keys
		keypad(stdscr, TRUE);

		ITEM **items = (ITEM **)calloc(options.size()+1, sizeof(ITEM *));
		uint i=0;
		vector<uint> sources;
		for(auto it=options.begin();it!=options.end();it++) {
			items[i] = new_item(it->first.c_str(), it->second.c_str());
			i++;
		}
		items[options.size()] = (ITEM *)NULL;
		MENU *menu = new_menu(items);
		WINDOW * win;
		win = newwin(options.size()+2, 30, row, col);
		set_menu_win (menu, win);
		set_menu_sub (menu, derwin(win, options.size(), 28, 1, 1));
		box(win, 0, 0);

		post_menu(menu);
		refresh();
		wrefresh(win);

		int c;
		uint j=0;
		while((c = getch()) != 27)
		{   switch(c)
			{	case KEY_DOWN:
					menu_driver(menu, REQ_DOWN_ITEM);
					if(j<options.size())
						j++;
					break;
				case KEY_UP:
					menu_driver(menu, REQ_UP_ITEM);
					if(j>0)
						j--;
					break;
				case 10:
					menu_driver(menu, REQ_TOGGLE_ITEM);
					choose=make_pair(j,menu->curitem->name.str);
					success = true;
					break;
			}
			if(success) break;
			refresh();
			wrefresh(win);
		}
		unpost_menu(menu);
		free_menu(menu);
		free_item(items[0]);
		free_item(items[1]);
		wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window
		delwin(win); // and delete
		wrefresh(win);
		clearAll(row-1);
		return success;
	}

	bool menu(vector<vector<string>> options, pair<uint, string> &choose, int row, int col){
		bool success = false;
		ITEM *cur_item;
		cbreak();
		noecho();
		//! enable special keys
		keypad(stdscr, TRUE);

		ITEM **items = (ITEM **)calloc(options.size()+1, sizeof(ITEM *));
		uint i=0;
		vector<uint> sources;
		for(auto it=options.begin();it!=options.end();it++) {
			items[i] = new_item((*it)[0].c_str(), " ");
			i++;
		}
		items[options.size()] = (ITEM *)NULL;
		MENU *menu = new_menu(items);
		WINDOW * win;
		win = newwin(options.size()+2, 30, row, col);
		set_menu_win (menu, win);
		set_menu_sub (menu, derwin(win, options.size(), 28, 1, 1));
		box(win, 0, 0);

		post_menu(menu);
		refresh();
		wrefresh(win);

		int c;
		uint j=0;
		while((c = getch()) != 27)
		{   switch(c)
			{	case KEY_DOWN:
					menu_driver(menu, REQ_DOWN_ITEM);
					if(j<options.size())
						j++;
					break;
				case KEY_UP:
					menu_driver(menu, REQ_UP_ITEM);
					if(j>0)
						j--;
					break;
				case 10:
					menu_driver(menu, REQ_TOGGLE_ITEM);
					choose=make_pair(j,menu->curitem->name.str);
					success = true;
					break;
			}
			if(success) break;
			refresh();
			wrefresh(win);
		}
		unpost_menu(menu);
		free_menu(menu);
		free_item(items[0]);
		free_item(items[1]);
		wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window
		delwin(win); // and delete
		wrefresh(win);
		clearAll(row-1);
		return success;
	}

	bool menu(vector<string> options, pair<uint, string> &choose, int row, int col){
		bool success = false;
		ITEM *cur_item;
		cbreak();
		noecho();
		//! enable special keys
		keypad(stdscr, TRUE);

		ITEM **items = (ITEM **)calloc(options.size()+1, sizeof(ITEM *));
		uint i=0;
		vector<uint> sources;
		for(auto it=options.begin();it!=options.end();it++) {
			items[i] = new_item(it->c_str(), " ");
			i++;
		}
		items[options.size()] = (ITEM *)NULL;
		MENU *menu = new_menu(items);
		WINDOW * win;
		win = newwin(options.size()+2, 130, row, col);
		set_menu_win (menu, win);
		set_menu_sub (menu, derwin(win, options.size(), 128, 1, 1));
		box(win, 0, 0);

		post_menu(menu);
		refresh();
		wrefresh(win);

		int c;
		uint j=0;
		while((c = getch()) != 27)
		{   switch(c)
			{	case KEY_DOWN:
					menu_driver(menu, REQ_DOWN_ITEM);
					if(j<options.size())
						j++;
					break;
				case KEY_UP:
					menu_driver(menu, REQ_UP_ITEM);
					if(j>0)
						j--;
					break;
				case 10:
					menu_driver(menu, REQ_TOGGLE_ITEM);
					choose=make_pair(j,menu->curitem->name.str);
					success = true;
					break;
			}
			refresh();
			wrefresh(win);
			if(success) break;
		}
		unpost_menu(menu);
		free_menu(menu);
		free_item(items[0]);
		free_item(items[1]);
		wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window
		delwin(win); // and delete
		wrefresh(win);
		clearAll(row-1);
		return success;
	}

	uint rows, cols;
	char inputstring[30];
	vector<pair<string, string>> devices;
	pair<uint, string> chosenDevice, chosenCmd;
	vector<vector<string>> commands;
	vector<string> commandVerbose;
};