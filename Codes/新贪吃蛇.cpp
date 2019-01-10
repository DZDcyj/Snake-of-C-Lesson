#include "pch.h"
#include "constants.h"
#include "functions.h"
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include <graphics.h>
#include <string.h>
#pragma region definition
bool harder = false;								//钢筋铁骨是否激活
bool auto_findway = false;							//自动寻路是否开启
bool map_existence = false;							//地图(智慧草)是否存在
bool next = false;									//选择下一关
bool poison_exist = true;							//毒草是否存在
int remove_time = 0;								//清除毒草时间指标
int shine = 1;										//毒草闪烁指标
bool one_knife_dead = false;						//一刀斩定义
bool win = false;									//游戏胜利指标
int progress = 0;									//单个发电机修理进度
int bodystatus = HEALTHY;							//玩家状态(健康/受伤/死亡)
int timecount = 0;									//通知延时
int chasedtime = 0;									//游戏时间分数
int input = RIGHT;									//输入移动状态
int status = RIGHT;									//当前移动状态
int score = 0;										//总分
int length = 3;										//长度
int left_or_right = RIGHT;							//头部左右贴图决定
int randomseed = (unsigned int)time(NULL);			//随机种子
bool kit_existence = false;							//急救箱是否存在
bool hatch_existence = false;						//地窖是否存在
bool exit_existence = false;						//逃脱大门是否存在
int gamemap[25][19] = { {FREE} };					//存储地图数据，其中25和19是换算过后的格子数，一个格子是40*40(像素点)
char automove[25 * 19] = { 0 };						//自动寻路存储
int move_num = 0;									//寻路序号
int move_curr = 0;									//当前寻路序号
MOUSEMSG click;										//鼠标事件响应
IMAGE imgfood, imgpoison, imgmine, imgwall, imgheadr, imgheadl, imghatchbig, imgbody, black, repair, repairscrore, imgdead, imghealthy, imginjured, imgchase, destroyscore, imgkit, imgheal, imggenerator, imgexit_gate, imghatch, imgexit, imgcurse;
IMAGE imgagain, imgmap, imgdeadhard;
//定义一些贴图对象
int open_num = 0;//开启列表中的格子数
int close_num = 0;//关闭列表中的格子数
int findway = 0;//寻路成功与否
#pragma endregion
#pragma region struct
struct snake//蛇身结构体
{
	int x;
	int y;
	struct snake *prev;
	struct snake *next;
}*head, *tail, *body;
struct object//物体结构体
{
	int x;
	int y;
}food, poison, mine, kit, hatch, exit_gate, map;
typedef struct ASTAR //A星寻路算法所需结构体
{
	int x;
	int y;//横纵坐标
	int g;//到起点距离，即走过的路程
	int h;//到终点的距离
	int type;//该点的类型
	struct ASTAR *father;//父节点
	bool in_openlist;//在开启列表内
	bool in_closelist;//在关闭列表内
}astar, *Astar;
astar *start_node, *end_node, *curr_node;
astar a_map[25][19];//地图存储
Astar openlist[475];//开启列表
#pragma endregion
void next_level(void)
{
	int clicked = 1;
	cleardevice();
	loadimage(NULL, _T(".\\Resources\\clear.png"), 1280, 760);
	putimage(120, 600, &imgagain);
	putimage(240, 600, &imgexit);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	outtextxy(120, 680, _T("下一关卡"));
	outtextxy(240, 680, _T("退出游戏"));
	while (clicked)
	{
		click = GetMouseMsg();
		switch (click.uMsg)
		{
		case WM_LBUTTONDOWN:
			if (between(click.x, 120, 160) && between(click.y, 600, 680))
			{
				if (map_choose < 3)
					map_choose++;
				clicked = 0;
			}
			else if (between(click.x, 240, 280) && between(click.y, 600, 680))
			{
				save();
				exit(0);
			}
			break;
		default:
			break;
		}
	}
}
void loading(int waiting)//等待函数
{
	for (int j = waiting; j >= 0; j--)
	{
		TCHAR waitingtime[5];
		swprintf_s(waitingtime, _T("%d"), j);
		settextcolor(WHITE);
		settextstyle(MINSIZE * 2, MINSIZE, _T("黑体"));
		outtextxy(27 * MINSIZE, 4 * MINSIZE, waitingtime);
		Sleep(1000);
		if (j == 0)
		{
			putimage(27 * MINSIZE, 4 * MINSIZE, &black);
			putimage(27 * MINSIZE, 5 * MINSIZE, &black);
		}
		while (_kbhit())
		{
			_getch();
		}
	}
}
void save_snake(void)//存储蛇的链节
{
	snake *temp;
	temp = (snake*)malloc(sizeof(snake));
	temp = head;
	FILE *fp;
	fopen_s(&fp, "snake.txt", "w");
	while (temp != NULL)
	{
		fprintf(fp, "%d %d\n", temp->x, temp->y);
		temp = temp->next;
	}
	free(temp);
	fclose(fp);
}
void save_status(void)//存储当前状态
{
	FILE *fp;
	fopen_s(&fp, "info.txt", "w");
	fprintf(fp, "%d %d %d %d %d\n", status, left_or_right, length, score, bodystatus);//存储长度、健康等
	fclose(fp);
	FILE *fps;
	fopen_s(&fps, "status.txt", "w");
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if ((i*MINSIZE) == head->x && (j*MINSIZE) == head->y)
				fprintf(fps, "9");//以9代替头部
			else if (gamemap[i][j] == MAP)
				fprintf(fps, "0");//地图会在保存时刷掉
			else
				fprintf(fps, "%d", gamemap[i][j]);
		}
		fprintf(fps, "\n");
	}
	fclose(fps);
}
void cleanscore(void)//清除临时计分通知
{
	putimage(25 * MINSIZE, 15 * MINSIZE, &black);
	putimage(26 * MINSIZE, 15 * MINSIZE, &black);
	putimage(27 * MINSIZE, 15 * MINSIZE, &black);
	putimage(28 * MINSIZE, 15 * MINSIZE, &black);
	putimage(29 * MINSIZE, 15 * MINSIZE, &black);
	putimage(30 * MINSIZE, 15 * MINSIZE, &black);
}
void generate_map(void)//地图生成
{
	srand(randomseed);
	do
	{
		map.x = rand() % 24 * MINSIZE + MINSIZE;
		map.y = rand() % 18 * MINSIZE + MINSIZE;
	} while (gamemap[map.x / MINSIZE][map.y / MINSIZE] != FREE);
	putimage(map.x, map.y, &imgmap);
	gamemap[map.x / MINSIZE][map.y / MINSIZE] = MAP;
	randomseed = rand();
}
void generate_food(void)//食物生成
{
	srand(randomseed);
	do
	{
		food.x = rand() % 24 * MINSIZE + MINSIZE;
		food.y = rand() % 18 * MINSIZE + MINSIZE;
	} while (gamemap[food.x / MINSIZE][food.y / MINSIZE] != FREE);
	putimage(food.x, food.y, &imgfood);
	gamemap[food.x / MINSIZE][food.y / MINSIZE] = FOOD;
	randomseed = rand();
}
void generate_poison(void)//毒草生成
{
	srand(randomseed);
	do
	{
		poison.x = rand() % 24 * MINSIZE + MINSIZE;
		poison.y = rand() % 18 * MINSIZE + MINSIZE;
	} while (gamemap[poison.x / MINSIZE][poison.y / MINSIZE] != FREE);
	putimage(poison.x, poison.y, &imgpoison);
	gamemap[poison.x / MINSIZE][poison.y / MINSIZE] = POISON;
	randomseed = rand();
}
void generate_mine(void)//地雷生成
{
	srand(randomseed);
	do
	{
		mine.x = rand() % 24 * MINSIZE + MINSIZE;
		mine.y = rand() % 18 * MINSIZE + MINSIZE;
	} while (gamemap[mine.x / MINSIZE][mine.y / MINSIZE] != FREE);
	putimage(mine.x, mine.y, &imgmine);
	gamemap[mine.x / MINSIZE][mine.y / MINSIZE] = MINE;
	randomseed = rand();
}
void generate_kit(void)//急救包生成
{
	srand(randomseed);
	do
	{
		kit.x = rand() % 24 * MINSIZE + MINSIZE;
		kit.y = rand() % 18 * MINSIZE + MINSIZE;
	} while (gamemap[kit.x / MINSIZE][kit.y / MINSIZE] != FREE);
	putimage(kit.x, kit.y, &imgkit);
	gamemap[kit.x / MINSIZE][kit.y / MINSIZE] = KIT;
	randomseed = rand();
}
void generate_exit_gate(void)//大门生成
{
	srand(randomseed);
	do
	{
		exit_gate.x = rand() % 24 * MINSIZE + MINSIZE;
		exit_gate.y = rand() % 18 * MINSIZE + MINSIZE;
	} while (gamemap[exit_gate.x / MINSIZE][exit_gate.y / MINSIZE] != FREE);
	putimage(exit_gate.x, exit_gate.y, &imgexit_gate);
	gamemap[exit_gate.x / MINSIZE][exit_gate.y / MINSIZE] = EXIT;
	randomseed = rand();
}
void generate_hatch(void)//地窖生成
{
	srand(randomseed);
	do
	{
		hatch.x = rand() % 24 * MINSIZE + MINSIZE;
		hatch.y = rand() % 18 * MINSIZE + MINSIZE;
	} while (gamemap[hatch.x / MINSIZE][hatch.y / MINSIZE] != FREE);
	putimage(hatch.x, hatch.y, &imghatch);
	gamemap[hatch.x / MINSIZE][hatch.y / MINSIZE] = HATCH;
	randomseed = rand();
}
void display_status(int sta)//更新状态显示
{
	IMAGE imgsta;
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	switch (sta)
	{
	case HEALTHY:imgsta = imghealthy;
		outtextxy(25 * MINSIZE, 13 * MINSIZE, _T("当前状态"));
		outtextxy(25 * MINSIZE, 14 * MINSIZE, _T("  健康")); break;
	case INJURED:imgsta = imginjured;
		outtextxy(25 * MINSIZE, 13 * MINSIZE, _T("当前状态"));
		outtextxy(25 * MINSIZE, 14 * MINSIZE, _T("  受伤")); break;
	case DEAD:imgsta = imgdead;
		outtextxy(25 * MINSIZE, 13 * MINSIZE, _T("当前状态"));
		outtextxy(25 * MINSIZE, 14 * MINSIZE, _T("  死亡")); break;
	default:break;
	}
	putimage(27 * MINSIZE, 13 * MINSIZE, &imgsta);
}
void initmap(int choice)//初始化地图
{
	FILE *fp;
	switch (choice)
	{
	case 1:fopen_s(&fp, "map1.txt", "r"); break;
	case 2:fopen_s(&fp, "map2.txt", "r"); break;
	case 3:fopen_s(&fp, "map3.txt", "r"); break;
	default:fopen_s(&fp, "map1.txt", "r"); break;
	}
	char in;
	int i = 0, j = 0;
	while ((in = fgetc(fp)) != EOF)
	{
		if (in == '\n')
			continue;
		in -= '0';
		gamemap[i][j] = in;
		j++;
		if (j == 19)
		{
			j = 0;
			i++;
		}
		if (i == 25)
			break;
	}
	fclose(fp);
}
void loadmap(void)//加载地图
{
	for (int i = 0; i < MAP_WIDTH; i += MINSIZE)
	{
		for (int j = 0; j < MAP_HEIGHT; j += MINSIZE)
		{
			switch (gamemap[i / MINSIZE][j / MINSIZE])
			{
			case FREE:break;
			case BARRIER:putimage(i, j, &imgwall); break;
			case FOOD:putimage(i, j, &imgfood); break;
			case POISON:putimage(i, j, &imgpoison); break;
			case MINE:putimage(i, j, &imgmine); break;
			case SNAKE:putimage(i, j, &imgbody); break;
			case 9:
				if (left_or_right == LEFT)
					putimage(i, j, &imgheadl);
				else
					putimage(i, j, &imgheadr);
				break;
			default:break;
			}
		}
	}
}
void loadgame(void)//游戏初始界面
{
	cleardevice();
	loadimage(NULL, _T(".\\Resources\\background.jpg"), WINDOW_WIDTH, MAP_HEIGHT);
	settextstyle(MINSIZE, MINSIZE / 2, _T("黑体"));
	outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 6, MINSIZE, _T("Dead by Daylight Snake"));
	int red = 255;
	int dred = -5;
	while (true)
	{
		settextcolor(RGB(red, 0, 0));
		red += dred;
		settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
		outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 3, MAP_HEIGHT - MINSIZE, _T("Press Any Key to Start"));
		if (_kbhit())
			break;
		if (red == 0)
			dred = 5;
		else if (red == 255)
			dred = -5;
		Sleep(10);
	}
	_getch();
	cleardevice();
}
void one_kill(void)//一刀斩
{
	one_knife_dead = true;
	settextcolor(RED);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	putimage(25 * MINSIZE, 11 * MINSIZE, &imgcurse);
	outtextxy(27 * MINSIZE, 12 * MINSIZE, _T("恶咒:难逃一死"));
}
int opposite(int Status)//输出反向方位
{
	switch (Status)
	{
	case 'W':case 'w':return 's';
	case 'S':case 's':return 'w';
	case 'A':case 'a':return 'd';
	case 'D':case 'd':return 'a';
	default:return 0; break;
	}
}
void loadsource(void)//载入资源
{
	loadimage(&imgmap, _T(".\\Resources\\RainbowMap.png"), MINSIZE, MINSIZE);
	loadimage(&imgagain, _T(".\\Resources\\destroyscore.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imghatchbig, _T(".\\Resources\\hatch.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imgfood, _T(".\\Resources\\food.png"), MINSIZE, MINSIZE);
	loadimage(&imggenerator, _T(".\\Resources\\food.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imgpoison, _T(".\\Resources\\poison.png"), MINSIZE, MINSIZE);
	loadimage(&imgmine, _T(".\\Resources\\mine.png"), MINSIZE, MINSIZE);
	loadimage(&imgwall, _T(".\\Resources\\wall.png"), MINSIZE, MINSIZE);
	loadimage(&imgheadr, _T(".\\Resources\\head_right.png"), MINSIZE, MINSIZE);
	loadimage(&imgheadl, _T(".\\Resources\\head_left.png"), MINSIZE, MINSIZE);
	loadimage(&imgbody, _T(".\\Resources\\body.png"), MINSIZE, MINSIZE);
	loadimage(&black, _T(".\\Resources\\black.png"), MINSIZE, MINSIZE);
	loadimage(&repair, _T(".\\Resources\\repair_icon.png"), MINSIZE, MINSIZE);
	loadimage(&repairscrore, _T(".\\Resources\\repair_score.png"), MINSIZE, MINSIZE);
	loadimage(&imghealthy, _T(".\\Resources\\healthy.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imginjured, _T(".\\Resources\\injured.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imgdead, _T(".\\Resources\\dead.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imgchase, _T(".\\Resources\\exit_score.png"), MINSIZE, MINSIZE);
	loadimage(&destroyscore, _T(".\\Resources\\destroyscore.png"), MINSIZE, MINSIZE);
	loadimage(&imgkit, _T(".\\Resources\\recover.png"), MINSIZE, MINSIZE);
	loadimage(&imgheal, _T(".\\Resources\\cure_score.png"), MINSIZE, MINSIZE);
	loadimage(&imgexit_gate, _T(".\\Resources\\exit.png"), MINSIZE, MINSIZE);
	loadimage(&imghatch, _T(".\\Resources\\hatch.png"), MINSIZE, MINSIZE);
	loadimage(&imgexit, _T(".\\Resources\\exit.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imgcurse, _T(".\\Resources\\kill.png"), MINSIZE * 2, MINSIZE * 2);
	loadimage(&imgdeadhard, _T(".\\Resources\\DeadHard.png"), MINSIZE * 2, MINSIZE * 2);
}
void load_snake(void)
{
	snake *tmphead;
	tmphead = (snake*)malloc(sizeof(snake));
	FILE *fp;
	errno_t err;
	err = fopen_s(&fp, "snake.txt", "r");
	if (err == 0)
	{
		snake *tmp;
		bool first = true;
		int tmpx, tmpy;
		tmp = (snake*)malloc(sizeof(snake));
		while (fscanf_s(fp, "%d %d\n", &tmpx, &tmpy) != EOF)
		{
			tmp->x = tmpx;
			tmp->y = tmpy;
			tmp->next = (snake*)malloc(sizeof(snake));
			tmp->next->prev = tmp;
			if (first)
			{
				tmphead->prev = NULL;
				tmphead = tmp;
				first = false;
			}
			tmp = tmp->next;
			tmp->next = NULL;
		}
		tmp = NULL;
		tmp = tmphead;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tail = tmp->prev;
		head = tmphead;
		free(tmp);
		fclose(fp);
	}
	else
		init_snake();
}
void remove_poison(void)
{
	for (int i = 0; i < MAP_WIDTH; i += MINSIZE)
	{
		for (int j = 0; j < MAP_HEIGHT; j += MINSIZE)
		{
			if (gamemap[i / MINSIZE][j / MINSIZE] == POISON)
			{
				putimage(i, j, &black);
				gamemap[i / MINSIZE][j / MINSIZE] = FREE;
			}
		}
	}
}
void init_snake(void)//初始化蛇体
{
	head = (snake*)malloc(sizeof(snake));
	head->x = 12 * MINSIZE;
	head->y = 8 * MINSIZE;
	head->prev = NULL;
	body = (snake*)malloc(sizeof(snake));
	head->next = body;
	body->prev = head;
	body->x = 440;
	body->y = 320;
	tail = (snake*)malloc(sizeof(snake));
	body->next = tail;
	tail->prev = body;
	tail->x = 400;
	tail->y = 320;
	putimage(head->x, head->y, &imgheadr);
	putimage(body->x, body->y, &imgbody);
	putimage(tail->x, tail->y, &imgbody);
	gamemap[head->x / MINSIZE][head->y / MINSIZE] = SNAKE;
	gamemap[body->x / MINSIZE][body->y / MINSIZE] = SNAKE;
	gamemap[tail->x / MINSIZE][tail->y / MINSIZE] = SNAKE;
}
void gameover(void)//游戏结束函数
{
	cleanscore();
	if (chasedtime == 3001)
		chasedtime--;
	score += chasedtime;
	TCHAR chase[10], finalscore[10];
	swprintf_s(chase, _T("%d"), chasedtime);
	swprintf_s(finalscore, _T("%d"), score);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(RED);
	outtextxy(MAP_WIDTH / 2, MAP_HEIGHT / 2, _T("GAME OVER"));
	outtextxy(MAP_WIDTH / 2 - MINSIZE, MAP_HEIGHT / 2 + MINSIZE, _T("总得分"));
	outtextxy(MAP_WIDTH / 2 + 2 * MINSIZE, MAP_HEIGHT / 2 + MINSIZE, finalscore);
	putimage(25 * MINSIZE, 15 * MINSIZE, &imgchase);
	settextcolor(WHITE);
	outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("被追杀 +"));
	outtextxy(28 * MINSIZE, 15 * MINSIZE, chase);
	Sleep(3000);
}
void gameclear(int ways)//游戏胜利函数
{
	cleanscore();
	if (chasedtime == 3001)
		chasedtime--;
	score += chasedtime;
	if (ways == EXIT)
		score += 2000;
	else if (ways == HATCH)
		score += 3000;
	TCHAR chase[10], finalscore[10];
	swprintf_s(chase, _T("%d"), chasedtime);
	swprintf_s(finalscore, _T("%d"), score);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(RED);
	outtextxy(MAP_WIDTH / 2, MAP_HEIGHT / 2, _T("脱逃成功"));
	outtextxy(MAP_WIDTH / 2 - MINSIZE, MAP_HEIGHT / 2 + MINSIZE, _T("总得分"));
	outtextxy(MAP_WIDTH / 2 + 2 * MINSIZE, MAP_HEIGHT / 2 + MINSIZE, finalscore);
	putimage(25 * MINSIZE, 15 * MINSIZE, &imgchase);
	settextcolor(WHITE);
	putimage(27 * MINSIZE, 13 * MINSIZE, &imgexit);
	outtextxy(25 * MINSIZE, 13 * MINSIZE, _T("当前状态"));
	outtextxy(25 * MINSIZE, 14 * MINSIZE, _T("  逃脱"));
	outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("被追杀 +"));
	outtextxy(28 * MINSIZE, 15 * MINSIZE, chase);
	Sleep(2000);
	cleanscore();
	if (ways == EXIT)
	{
		putimage(25 * MINSIZE, 15 * MINSIZE, &destroyscore);
		outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("逃出生天 +2000"));
	}
	else if (ways == HATCH)
	{
		putimage(25 * MINSIZE, 15 * MINSIZE, &destroyscore);
		outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("秘密通道逃脱 +3000"));
	}
	Sleep(3000);
	win = true;
	//next_level();
}
void update_score_food(void)//食物得分
{
	cleanscore();
	putimage(25 * MINSIZE, 15 * MINSIZE, &repairscrore);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("完成修理 +200"));
	timecount = 10;
}
void update_score_map(void)//地图得分
{
	cleanscore();
	putimage(25 * MINSIZE, 15 * MINSIZE, &repairscrore);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("地图哨兵 +300"));
	timecount = 10;
}
void update_score_poison(void)//毒草得分
{
	cleanscore();
	putimage(25 * MINSIZE, 15 * MINSIZE, &destroyscore);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("摆脱杀手 +500"));
	timecount = 10;
}
void update_score_mine(void)//地雷得分
{
	cleanscore();
	putimage(25 * MINSIZE, 15 * MINSIZE, &destroyscore);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("摆脱半数杀手 +1000"));
	timecount = 10;
}
void update_score_kit(void)//急救包得分
{
	cleanscore();
	putimage(25 * MINSIZE, 15 * MINSIZE, &imgheal);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("自我治疗 +150"));
	timecount = 10;
}
void hurt(void)
{
	if (harder == false)//在钢筋铁骨没有生效时，正常受伤/死亡
	{
		if (bodystatus == HEALTHY)
		{
			if (one_knife_dead)
				bodystatus = DEAD;
			else
				bodystatus = INJURED;
		}
		else
			bodystatus = DEAD;
	}
	if (harder&&length < 2)//即使是钢筋铁骨状态，长度小于2也会死亡
		bodystatus = DEAD;
}
void harder_active(void)//钢筋铁骨启动
{
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	settextcolor(WHITE);
	putimage(25 * MINSIZE, 4 * MINSIZE, &imgdeadhard);
	outtextxy(27 * MINSIZE, 4 * MINSIZE, _T("钢筋铁骨激活"));
}
void harder_deactive(void)//钢筋铁骨失效
{
	putimage(25 * MINSIZE, 4 * MINSIZE, &black);
	putimage(25 * MINSIZE, 5 * MINSIZE, &black);
	putimage(26 * MINSIZE, 4 * MINSIZE, &black);
	putimage(26 * MINSIZE, 5 * MINSIZE, &black);
	putimage(27 * MINSIZE, 4 * MINSIZE, &black);
	putimage(28 * MINSIZE, 4 * MINSIZE, &black);
	putimage(29 * MINSIZE, 4 * MINSIZE, &black);
	putimage(30 * MINSIZE, 4 * MINSIZE, &black);
}
void move(void)//移动函数
{
	if (length < 1)
		bodystatus = DEAD;
	if (bodystatus != DEAD)
	{
		int dx = 0, dy = 0;
		if (auto_findway&&findway)
		{
			input = automove[move_curr];
			move_curr++;
			while (_kbhit())
				_getch();
			if (move_curr > move_num)
			{
				reset_findpath();
			}
		}
		else
		{
			if (_kbhit())
			{
				input = _getch();
				if (input == 'r')
				{
					save_status();
					save_snake();
					input = status;
				}
				else if (input == 224)					//在输入功能键时，第一个返回值是224
				{
					input = _getch();
					switch (input)						//接受功能键，下面的72,80,75,77分别对应功能键的↑↓←→
					{
					case 72:
						if (status != DOWN)
							input = UP;
						else
							input = status;
						break;
					case 80:
						if (status != UP)
							input = DOWN;
						else
							input = status;
						break;
					case 75:
						if (status != RIGHT)
							input = LEFT;
						else
							input = status;
						break;
					case 77:
						if (status != LEFT)
							input = RIGHT;
						else
							input = status;
						break;
					default:break;
					}
				}
				else
					input = status;
			}
			else
				input = status;
			if (input == opposite(status))//防止倒车入库
				input = status;
		}
		switch (input)						//接受输入的指令进行改变
		{
		case UP:
			if (status != DOWN)
			{
				dy = -MINSIZE;
				status = UP;
			} break;
		case DOWN:
			if (status != UP)
			{
				dy = MINSIZE;
				status = DOWN;
			} break;
		case LEFT:
			if (status != RIGHT)
			{
				dx = -MINSIZE;
				status = LEFT;
				left_or_right = LEFT;
			}break;
		case RIGHT:
			if (status != LEFT)
			{
				dx = MINSIZE;
				status = RIGHT;
				left_or_right = RIGHT;
			}break;
		default:break;
		}
		snake *nexthead;
		nexthead = (snake*)malloc(sizeof(snake));
		nexthead->x = head->x + dx;
		nexthead->y = head->y + dy;
		nexthead->next = head;
		nexthead->prev = NULL;
		head->prev = nexthead;
		if (left_or_right == RIGHT)
			putimage(nexthead->x, nexthead->y, &imgheadr);
		else
			putimage(nexthead->x, nexthead->y, &imgheadl);
		putimage(head->x, head->y, &imgbody);
		tail = tail->prev;
		putimage(tail->x, tail->y, &imgbody);
		if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == SNAKE)//自噬检测
		{
			hurt();
			display_status(bodystatus);
		}
		if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == BARRIER)//撞墙检测
		{
			bodystatus = DEAD;
			display_status(bodystatus);
		}
		if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == FOOD)//食物设定
		{
			srand(randomseed);

			if (auto_findway)
			{
				reset_findpath();
				harder_deactive();//钢筋铁骨在修理进行后失效
				harder = false;
				auto_findway = false;
			}
			if (rand() % 10 == 9 && bodystatus == INJURED && kit_existence == false)
			{
				generate_kit();
				kit_existence = true;
				randomseed = rand();
			}
			tail = tail->next;
			putimage(tail->x, tail->y, &imgbody);
			score += 200;
			update_score_food();
			if (progress < 100)
				progress += 10;
			if (progress == 100)
			{
				progress = 0;
				generators_left--;
			}
			putimage(25 * MINSIZE, 18 * MINSIZE, &black);
			putimage(26 * MINSIZE, 18 * MINSHORT, &black);
			putimage(27 * MINSIZE, 18 * MINSHORT, &black);
			putimage(28 * MINSIZE, 18 * MINSHORT, &black);
			putimage(29 * MINSIZE, 18 * MINSHORT, &black);
			putimage(30 * MINSIZE, 18 * MINSHORT, &black);
			settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
			settextcolor(WHITE);
			TCHAR progress_main[10], generators[10];
			if (progress == 0)
				swprintf_s(progress_main, _T(" %d%%"), progress);
			else
				swprintf_s(progress_main, _T("%d%%"), progress);
			swprintf_s(generators, _T(" %d"), generators_left);
			outtextxy(26 * MINSIZE, 18 * MINSIZE, progress_main);
			settextstyle(MINSIZE * 2, MINSIZE, _T("黑体"));
			outtextxy(27 * MINSIZE, 0, generators);
			length++;
			if (generators_left == hatch_generate && hatch_existence == false)//当第X台电机被点亮时，地图上生成地窖并启动一刀斩状态
			{
				settextcolor(WHITE);
				settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
				putimage(25 * MINSIZE, 9 * MINSIZE, &imghatchbig);
				outtextxy(27 * MINSIZE, 10 * MINSIZE, _T("地窖已开启"));
				one_kill();
				generate_hatch();
				hatch_existence = true;
			}
			else if (generators_left == 0 && exit_existence == false)//当最后一台电机被点亮，逃生大门刷出，同时不再刷出电机
			{
				settextcolor(WHITE);
				settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
				putimage(25 * MINSIZE, 7 * MINSIZE, &imgexit);
				outtextxy(27 * MINSIZE, 8 * MINSIZE, _T("逃生大门已开启"));
				generate_exit_gate();
				exit_existence = true;
			}
			if (exit_existence == false)//刷电机的条件是没有逃生大门
				generate_food();
		}
		else if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == POISON)//毒草设定
		{
			putimage(tail->next->x, tail->next->y, &black);
			gamemap[tail->next->x / MINSIZE][tail->next->y / MINSIZE] = FREE;
			free(tail->next);
			tail->next = NULL;
			tail = tail->prev;
			putimage(tail->next->x, tail->next->y, &black);
			gamemap[tail->next->x / MINSIZE][tail->next->y / MINSIZE] = FREE;
			free(tail->next);
			tail->next = NULL;
			hurt();
			display_status(bodystatus);
			length--;
			if (length <= 1)
				bodystatus = DEAD;
			if (bodystatus != DEAD)
			{
				generate_poison();
				score += 500;
				update_score_poison();
			}
		}
		else if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == MINE)//地雷设定
		{
			for (int i = 1; i <= length / 2; i++)
			{
				putimage(tail->next->x, tail->next->y, &black);
				gamemap[tail->next->x / MINSIZE][tail->next->y / MINSIZE] = FREE;
				free(tail->next);
				tail->next = NULL;
				tail = tail->prev;
			}
			putimage(tail->next->x, tail->next->y, &black);
			gamemap[tail->next->x / MINSIZE][tail->next->y / MINSIZE] = FREE;
			hurt();
			display_status(bodystatus);
			if (length % 2 != 0)
				length = (length - 1) / 2;
			else
				length /= 2;
			if (length <= 0)
				bodystatus = DEAD;
			if (bodystatus != DEAD)
			{
				generate_mine();
				score += 1000;
				update_score_mine();
			}
		}
		else if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == HATCH)//地窖设定
		{
			if (rand() % 10 <= 6)
			{
				bodystatus = DEAD;
				display_status(bodystatus);
			}
			else
				gameclear(HATCH);
		}
		else if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == EXIT)//逃生大门设定
			gameclear(EXIT);
		else
		{
			if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == KIT)//急救包设定
			{
				bodystatus = HEALTHY;
				display_status(bodystatus);
				kit_existence = false;
				score += 150;
				update_score_kit();
				gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] = SNAKE;
			}
			if (gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] == MAP)//地图设定
			{
				auto_findway = true;
				regenerate_map(nexthead->x, nexthead->y);
				showpath();
				harder_active();
				score += 300;
				harder = true;
				map_existence = false;
				update_score_map();
				gamemap[nexthead->x / MINSIZE][nexthead->y / MINSIZE] = SNAKE;
			}
			putimage(tail->next->x, tail->next->y, &black);
			gamemap[tail->next->x / MINSIZE][tail->next->y / MINSIZE] = FREE;
			free(tail->next);
			tail->next = NULL;
		}
		head = nexthead;
		gamemap[head->x / MINSIZE][head->y / MINSIZE] = SNAKE;
	}
}
void reset(void)//重置游戏
{
	auto_findway = false;
	harder = false;
	map_existence = false;
	chasedtime = 0;
	remove_time = 0;
	poison_exist = true;
	generators_left = hatch_generate * 2 - 1;
	resume_game = false;
	one_knife_dead = false;
	win = false;
	progress = 0;
	bodystatus = HEALTHY;
	timecount = 0;
	input = RIGHT;
	status = RIGHT;
	score = 0;
	length = 3;
	left_or_right = RIGHT;
	randomseed = (unsigned int)time(NULL);
	kit_existence = false;
	hatch_existence = false;
	exit_existence = false;
}
void save_score(void)//存储得分
{
	cleardevice();
	loadimage(NULL, _T(".\\Resources\\save_score.png"));
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	char name[11] = { '\0' };
	char inp;
	int i = 0;
	int pos = 520;
	while (i <= 9)
	{
		inp = _getch();
		if (inp == '\b'&&i != 0)
		{
			i--;
			pos -= 10;
			outtextxy(pos, 380, _T(" "));
			name[i] = '\0';
		}
		else if (i == 0 && inp == '\b');
		else if (inp == 13)//13是回车键的键值
			break;
		else
		{
			outtextxy(pos, 380, inp);
			pos += 10;
			name[i] = inp;
			i++;
		}
	}
	if (i == 0)
		strcpy_s(name, "noname");
	if (score < 5000)
		outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 3, 480, _T("您的得分太低,没有保存的必要！"));
	else
	{
		outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 3, 480, _T("正在保存中……"));
		FILE *fp;
		fopen_s(&fp, "ScoreList.txt", "a+");
		char diff[10];
		switch (difficulty)
		{
		case 0:strcpy_s(diff, "Easy"); break;
		case 1:strcpy_s(diff, "Normal"); break;
		case 2:strcpy_s(diff, "Hard"); break;
		default:break;
		}
		fprintf(fp, "Name:%s Score:%d Difficulty:%s Map:%d\n", name, score, diff, map_choose);
		fclose(fp);
	}
	Sleep(2000);
}
void load_status(void)//读取保存时的地图状态
{
	cleardevice();
	load_snake();
	errno_t err;
	FILE *fp;
	err = fopen_s(&fp, "status.txt", "r");
	if (err == 0)
	{
		char ins;
		int i = 0, j = 0;
		while ((ins = fgetc(fp)) != EOF)
		{
			if (ins == '\n')
				continue;
			ins -= '0';
			gamemap[i][j] = ins;
			j++;
			if (j == 19)
			{
				j = 0;
				i++;
			}
			if (i == 24 && j == 19)
				break;
		}
		fclose(fp);
	}
	else
	{
		initmap(1);
		generate_food();
		for (int i = 0; i < generate_num; i++)
		{
			generate_poison();
			generate_mine();
		}
	}
	errno_t err2;
	FILE *fp2;
	err2 = fopen_s(&fp2, "info.txt", "r");
	if (err2 == 0)
	{
		fscanf_s(fp2, "%d %d %d %d %d\n", &status, &left_or_right, &length, &score, &bodystatus);
		fclose(fp2);
	}
	loadmap();
}
void poison_shine(void)//毒草闪烁函数
{
	for (int i = 0; i < MAP_WIDTH; i += MINSIZE)
	{
		for (int j = 0; j < MAP_HEIGHT; j += MINSIZE)
		{
			if (gamemap[i / MINSIZE][j / MINSIZE] == POISON)
			{
				if (shine % 2 == 0)
					putimage(i, j, &black);
				else
					putimage(i, j, &imgpoison);
			}
		}
	}
	shine++;
}
void save(void)//存储关卡进度
{
	FILE *fp;
	fopen_s(&fp, "level.txt", "w");
	fprintf(fp, "%d %d %d", map_choose, difficulty, speed);
	fclose(fp);
}
void load(void)//读取关卡进度
{
	FILE *fp;
	errno_t err;
	err = fopen_s(&fp, "level.txt", "r");
	if (err == 0)//如果文件存在，则读取文件内容。否则以默认为准
	{
		fscanf_s(fp, "%d %d %d", &map_choose, &difficulty, &speed);
		fclose(fp);
		switch (difficulty)
		{
		case 0:
			generate_num = 3;
			hatch_generate = 2;
			generators_left = 3; break;
		case 1:
			generate_num = 5;
			hatch_generate = 3;
			generators_left = 5;
			break;
		case 2:
			generate_num = 7;
			hatch_generate = 4;
			generators_left = 7;
			break;
		default:break;
		}
	}
}
void change_openlist(void)//移动开启列表的内容

{
	int i = 0;
	if (open_num != 1)
	{
		for (i = 0; i < open_num - 1; i++)
			openlist[i] = openlist[i + 1];
		openlist[i] = NULL;
	}
}
void sequence(void)//排序算法
{
	int j;
	for (j = 1; j <= open_num - 1; j++)
	{
		Astar a = openlist[j];
		int b = j;
		while (b - 1 >= 0 && (a->g + a->h) < (openlist[b - 1]->g + openlist[b - 1]->h))
		{
			openlist[b] = openlist[b - 1];
			b--;
		}
		openlist[b] = a;
	}
}
void judge_neighbors(void)//判断相邻格子
{
	int x = curr_node->y;
	int y = curr_node->x;
	if (x + 1 < 25)
		add_to_openlist(x + 1, y, 10);
	if (x - 1 > 0)
		add_to_openlist(x - 1, y, 10);
	if (y - 1 > 0)
		add_to_openlist(x, y - 1, 10);
	if (y + 1 < 19)
		add_to_openlist(x, y + 1, 10);
}
void add_to_openlist(int x, int y, int dis)//将某点加入到开启列表
{
	int i;
	if (a_map[x][y].type != BARRIER && a_map[x][y].type != POISON && a_map[x][y].type != MINE && a_map[x][y].type != SNAKE && a_map[x][y].type != HATCH)
	{
		if (!a_map[x][y].in_closelist)
		{
			if (a_map[x][y].in_openlist)
			{
				if (a_map[x][y].g > curr_node->g + dis)
				{
					a_map[x][y].g = curr_node->g + dis;
					a_map[x][y].father = curr_node;
					for (i = 0; i < open_num; i++)
					{
						if (openlist[i]->x == a_map[x][y].x&&openlist[i]->y == a_map[x][y].y)
						{
							break;
						}
					}
				}
			}
			else
			{
				a_map[x][y].g = curr_node->g + dis;
				a_map[x][y].h = abs(end_node->x - a_map[x][y].x) + abs(end_node->y - a_map[x][y].y);
				a_map[x][y].in_openlist = 1;
				a_map[x][y].father = curr_node;
				openlist[open_num] = &(a_map[x][y]);
				open_num++;
			}
		}
	}
}
void reset_findpath(void)//重置寻路参数
{
	for (int a = 0; a < 475; a++)
	{
		openlist[a] = NULL;
		automove[a] = 0;
	}
	move_curr = 0;
	move_num = 0;
	open_num = 0;
	close_num = 0;
	findway = 0;
	start_node = NULL;
	end_node = NULL;
	curr_node = NULL;
}
void regenerate_map(int startx, int starty)//再生成地图
{
	start_node = (ASTAR*)malloc(sizeof(ASTAR));
	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			a_map[x][y].g = 0;
			a_map[x][y].h = 0;
			a_map[x][y].x = y;
			a_map[x][y].y = x;
			a_map[x][y].in_closelist = 0;
			a_map[x][y].in_openlist = 0;
			a_map[x][y].father = NULL;
			a_map[x][y].type = gamemap[x][y];
			if ((x*MINSIZE) == startx && (y*MINSIZE) == starty)
				end_node = &(a_map[x][y]);//定义终点(蛇头)
			if (gamemap[x][y] == FOOD || gamemap[x][y] == EXIT)
				start_node = &(a_map[x][y]);	//定义起点(电机/大门)
		}
	}
}
void findpath(void)//寻路
{
	openlist[open_num] = start_node;
	start_node->h = abs(end_node->x - start_node->x) + abs(end_node->y - start_node->y);
	start_node->g = 0;
	start_node->in_openlist = 1;
	open_num++;
	while (true)
	{
		curr_node = openlist[0];
		change_openlist();
		open_num--;
		curr_node->in_closelist = 1;
		if (curr_node->x == end_node->x&&curr_node->y == end_node->y)
		{
			findway = 1;
			break;
		}
		judge_neighbors();
		if (open_num == 0)
		{
			findway = 0;
			break;
		}
	}
}
void showpath(void)//显示路线
{
	findpath();
	IMAGE img;
	loadimage(&img, _T(".\\Resources\\red.png"), MINSIZE, MINSIZE);
	if (findway)
	{
		curr_node = end_node;
		while (curr_node->father != NULL)
		{
			putimage(curr_node->y*MINSIZE, curr_node->x*MINSIZE, &img);
			if (curr_node->y > curr_node->father->y)
				automove[move_num] = LEFT;
			else if (curr_node->y < curr_node->father->y)
				automove[move_num] = RIGHT;
			else if (curr_node->x < curr_node->father->x)
				automove[move_num] = DOWN;
			else if (curr_node->x > curr_node->father->x)
				automove[move_num] = UP;
			move_num++;
			curr_node = curr_node->father;
		}
	}
	else
	{
		findway = 0;
		automove[0] = status;
	}
}
void regenerate_mine(void)
{
	for (int i = 0; i < MAP_WIDTH; i += MINSIZE)
	{
		for (int j = 0; j < MAP_HEIGHT; j += MINSIZE)
		{
			if (gamemap[i / MINSIZE][j / MINSIZE] == MINE)
			{
				putimage(i, j, &black);
				gamemap[i / MINSIZE][j / MINSIZE] = FREE;
			}
		}
	}
	for (int a = 0; a < generate_num; a++)
		generate_mine();
}
void refresh_generators(void)//某些意外情况下会将电机产生到蛇的位置，利用此函数进行刷新
{
	bool existence = false;
	for (int i = 0; i < MAP_WIDTH; i += MINSIZE)
	{
		for (int j = 0; j < MAP_HEIGHT; j += MINSIZE)
		{
			if (gamemap[i / MINSIZE][j / MINSIZE] == FOOD)
			{
				existence = true;
				break;
			}
		}
	}
	if (existence == false && exit_existence == false)
		generate_food();
}
int main(void)//主函数
{
	load();
	initgraph(1280, 760);//初始化绘图界面
	loadsource();//加载资源
	loadgame();//加载开始界面
beginning:
	function_choose();//打开功能界面
	if (resume_game)
	{
		load_status();
		load_snake();
	}
	else
	{
		init_snake();
		initmap(map_choose);
	}
	loadmap();					//根据上面对gamemap的赋值来载入地图
#pragma region init
	putimage(25 * MINSIZE, 17 * MINSIZE, &repair);
	settextstyle(MINSIZE * 2, MINSIZE, _T("黑体"));
	settextcolor(WHITE);
	putimage(25 * MINSIZE, 0, &imggenerator);
	char gene_left[5];
	sprintf_s(gene_left, " %d", generators_left);
	TCHAR gene_left_t[5];
	CharToTchar(gene_left, gene_left_t);
	outtextxy(27 * MINSIZE, 0, gene_left_t);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	outtextxy(26 * MINSIZE, 17 * MINSIZE, _T("修理(当前电机)"));
	outtextxy(26 * MINSIZE, 18 * MINSIZE, _T("0%"));//初始化一些贴图文字
	if (resume_game == false)
	{
		generate_food();
		for (int i = 0; i < generate_num; i++)
		{
			generate_poison();
			generate_mine();
		}
	}
#pragma endregion
	display_status(bodystatus);
	loading(3);
	while (true)
	{
		if (map_existence == false && rand() % 100 == 1)
		{
			generate_map();
			map_existence = true;
		}
		remove_time++;
		if (remove_time == 50)
		{
			refresh_generators();
			remove_poison();
			poison_exist = false;
		}
		else if (remove_time == 100)
		{
			for (int i = 0; i < generate_num; i++)
				generate_poison();
			regenerate_mine();
			remove_time = 0;
			poison_exist = true;
		}
		if (poison_exist)
			poison_shine();
		if (bodystatus == DEAD)
		{
			gameover();
			startgame = false;
			break;
		}
		if (win)
			break;
		move();
		if (timecount > 0)
			timecount--;
		else if (timecount == 0)
			cleanscore();
		Sleep(speed);
		if (chasedtime < 3000)
			chasedtime += 1;
		else if (chasedtime == 3000)
		{
			cleanscore();
			putimage(25 * MINSIZE, 15 * MINSIZE, &imgchase);
			settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
			settextcolor(WHITE);
			outtextxy(26 * MINSIZE, 15 * MINSIZE, _T("已达到脱逃等级最高分"));
			chasedtime++;
			timecount = 10;
		}
	}
	save_score();
	if (win)
	{
		next_level();
		startgame = true;
	}
	reset();
	reset_findpath();
	goto beginning;
	return 0;
}