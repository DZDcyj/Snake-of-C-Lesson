/*
This head file includes some constants.
*/
#pragma region definition
#define LEFT  'A'
#define RIGHT 'D'
#define UP    'W'
#define DOWN  'S'				//方向键定义
#define MAP_WIDTH 1000			//地图宽度
#define WINDOW_WIDTH 1280		//窗口宽度
#define MAP_HEIGHT 760			//地图高度
#define FREE 0					//空地
#define BARRIER 1				//障碍
#define FOOD 2					//食物
#define POISON 3				//毒草
#define MINE 4					//地雷
#define SNAKE 5					//蛇本身
#define EXIT 6					//大门
#define HATCH 7					//地窖
#define KIT 8					//急救包
#define MAP 9					//地图
#define MINSIZE 40				//像素换算因子
#define HEALTHY 50				//健康状态
#define INJURED 60				//受伤状态
#define DEAD	70				//死亡状态
extern bool startgame;			//开始游戏指标
extern bool resume_game;		//继续游戏指标
extern int map_choose;			//地图选择指标
extern int difficulty;			//难度指标
extern int speed;				//速度指标
extern int generators_left;		//总发电机数指标
extern int hatch_generate;		//地窖开启指标
extern int generate_num;		//生成毒草地雷数量
#pragma endregion