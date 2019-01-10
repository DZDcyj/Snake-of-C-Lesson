#include <tchar.h>
void function_choose(void);
void load_status(void);
void CharToTchar(const char * _char, TCHAR * tchar);
bool between(int x, int min, int max);
void save(void);
void load(void);
void change_openlist(void);
void add_to_openlist(int x,int y,int dis);
void sequence(void);
void judge_neighbors(void);
void showpath(void);
void findpath(void);
void regenerate_map(int startx,int starty);
void reset_findpath(void);
void init_snake(void);