#include "pch.h"
#include "functions.h"
#include "constants.h"
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <Windows.h>
struct score_list//得分链表
{
	int score;
	char name[20];
	int map_num;
	char diff[10];
	struct score_list *next;
	struct score_list *prev;
};
#pragma region definitions
bool choice_change = false;							//更改设置后的刷新指标
int generate_num = 5;								//默认难度下的毒草地雷生成数量
int hatch_generate = 3;								//地窖生成指标
int generators_left = 5;							//总发电机剩余数量
int speed = 100;									//默认速度
int difficulty = 1;									//默认难度(0-简单 1-中等 2-困难)
int map_choose = 1;									//默认地图
bool startgame = false;								//开始游戏指标
bool resume_game = false;							//继续游戏指标
IMAGE backgroumd, start, resume, setting, rule, scorelist, exit_button, generators, hooks, traps;//开始界面贴图
MOUSEMSG clickpoint;//鼠标消息
#pragma endregion
void loadbutton(void)//加载开始界面的按钮
{
	loadimage(&start, _T(".\\Resources\\start_button.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&resume, _T(".\\Resources\\resume_button.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&setting, _T(".\\Resources\\setting_button.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&rule, _T(".\\Resources\\rules_button.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&scorelist, _T(".\\Resources\\scorelist_button.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&exit_button, _T(".\\Resources\\exit.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&generators, _T(".\\Resources\\exit.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&traps, _T(".\\Resources\\exit.png"), 4 * MINSIZE, 4 * MINSIZE);
	loadimage(&hooks, _T(".\\Resources\\exit.png"), 4 * MINSIZE, 4 * MINSIZE);
}
bool between(int x, int min, int max)//范围判断
{
	if (x >= min && x <= max)
		return true;
	else
		return false;
}
void display_rules(void)//规则展示
{
	cleardevice();
	loadimage(NULL, _T(".\\Resources\\rules.png"));//规则已经用画图直接写好了，直接载入图片即可
	settextcolor(WHITE);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 4, MAP_HEIGHT - MINSIZE, _T("Press Any Key to Go back Menu"));
	outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 2, 0, _T("游戏规则释义"));
	_getch();
}
void settings(void)//设置函数
{
	TCHAR gene[5], _hatch[5], gene_num[5];
	cleardevice();
	setlinecolor(RED);
	setlinestyle(PS_SOLID, 3);
	loadimage(NULL, _T(".\\Resources\\settings.png"), 1280, 760);
	swprintf_s(gene, _T("%d"), generators_left);
	swprintf_s(_hatch, _T("%d"), hatch_generate);
	swprintf_s(gene_num, _T("%d"), generate_num);
	outtextxy(240, 330, gene_num);
	outtextxy(200, 230, gene);
	outtextxy(200, 270, _hatch);
	while (true)
	{
		clickpoint = GetMouseMsg();
		switch (map_choose)
		{
		case 1:rectangle(120, 80, 200, 120); break;
		case 2:rectangle(240, 80, 320, 120); break;
		case 3:rectangle(360, 80, 440, 120); break;
		default:break;
		}
		switch (clickpoint.uMsg)//地图选择响应
		{
		case WM_LBUTTONDOWN:
			if (between(clickpoint.x, 120, 200) && between(clickpoint.y, 80, 120))
			{
				map_choose = 1;
				choice_change = true;
			}
			else if (between(clickpoint.x, 240, 320) && between(clickpoint.y, 80, 120))
			{
				map_choose = 2;
				choice_change = true;
			}
			else if (between(clickpoint.x, 360, 440) && between(clickpoint.y, 80, 120))
			{
				map_choose = 3;
				choice_change = true;
			}
		default:break;
		}
		switch (map_choose)
		{
		case 1:rectangle(120, 80, 200, 120); break;
		case 2:rectangle(240, 80, 320, 120); break;
		case 3:rectangle(360, 80, 440, 120); break;
		default:break;
		}
		switch (difficulty)
		{
		case 0:rectangle(120, 130, 200, 170);
			generate_num = 3;
			hatch_generate = 2;
			generators_left = 3; break;
		case 1:rectangle(240, 130, 320, 170);
			generate_num = 5;
			hatch_generate = 3;
			generators_left = 5;
			break;
		case 2:rectangle(360, 130, 440, 170);
			generate_num = 7;
			hatch_generate = 4;
			generators_left = 7;
			break;
		default:break;
		}
		switch (speed)
		{
		case 200:rectangle(120, 180, 170, 220); break;
		case 150:rectangle(240, 180, 290, 220); break;
		case 100:rectangle(360, 180, 410, 220); break;
		}
		switch (clickpoint.uMsg)//难度选择响应
		{
		case WM_LBUTTONDOWN:
			if (between(clickpoint.x, 120, 200) && between(clickpoint.y, 130, 170))
			{
				difficulty = 0;
				choice_change = true;
			}
			else if (between(clickpoint.x, 240, 320) && between(clickpoint.y, 130, 170))
			{
				difficulty = 1;

				choice_change = true;
			}
			else if (between(clickpoint.x, 360, 440) && between(clickpoint.y, 130, 170))
			{
				difficulty = 2;

				choice_change = true;
			}
		default:break;
		}
		switch (clickpoint.uMsg)//速度选择响应
		{
		case WM_LBUTTONDOWN:
			if (between(clickpoint.x, 120, 160) && between(clickpoint.y, 180, 220))
			{
				speed = 200;
				choice_change = true;
			}
			else if (between(clickpoint.x, 240, 280) && between(clickpoint.y, 180, 220))
			{
				speed = 150;
				choice_change = true;
			}
			else if (between(clickpoint.x, 360, 400) && between(clickpoint.y, 180, 220))
			{
				speed = 100;
				choice_change = true;
			}
		default:break;
		}
		swprintf_s(gene, _T("%d"), generators_left);
		swprintf_s(_hatch, _T("%d"), hatch_generate);
		swprintf_s(gene_num, _T("%d"), generate_num);
		outtextxy(240, 330, gene_num);
		outtextxy(200, 230, gene);
		outtextxy(200, 270, _hatch);
		if (choice_change)
		{
			loadimage(NULL, _T(".\\Resources\\settings.png"), 1280, 760);
			choice_change = false;
		}
		if (_kbhit())
			break;
	}
	settextcolor(WHITE);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 4, MAP_HEIGHT - MINSIZE, _T("Press Any Key to Go back Menu"));
	_getch();
}
void CharToTchar(const char * _char, TCHAR * tchar)//char转换成TCHAR
{
	int iLength;
	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}
void loadscore(void)//排行榜展示
{
	outtextxy(WINDOW_WIDTH / 2 - 2 * MINSIZE, 0, _T("游戏风云榜"));
	score_list *tmp, *head;
	head = (score_list*)malloc(sizeof(score_list));
	tmp = (score_list*)malloc(sizeof(score_list));
	bool first = true;
	FILE *fp;
	int tmp_score;
	char tmp_name[20];
	char tmp_diff[20];
	int tmp_map;
	errno_t err;
	err=fopen_s(&fp, "ScoreList.txt", "r");
	if (err == 0)
	{
		int num = 0;//记录数据个数
		while (fscanf_s(fp, "Name:%s Score:%d Difficulty:%s Map:%d\n", tmp_name, 20, &tmp_score, tmp_diff, 20, &tmp_map) != EOF)//逐行读取文件中得分情况
		{
			tmp->score = tmp_score;
			strcpy_s(tmp->name, tmp_name);
			strcpy_s(tmp->diff, tmp_diff);
			tmp->map_num = tmp_map;
			if (first)
			{
				head->prev = NULL;
				head = tmp;
				first = false;
			}
			tmp->next = (score_list*)malloc(sizeof(score_list));
			tmp->next->prev = tmp;
			tmp = tmp->next;
			tmp->next = NULL;
			num++;
		}
		tmp = NULL;
		fclose(fp);
		tmp = head;
		int i = 0;
		while (i < num)//采用冒泡排序进行排序
		{
			while (tmp != NULL)
			{
				if (tmp->next == NULL)
					break;
				if (tmp->score < tmp->next->score)//交换两个链表的数据而非交换链表
				{
					score_list *temp;
					temp = (score_list*)malloc(sizeof(score_list));
					temp->score = tmp->score;
					strcpy_s(temp->name, tmp->name);
					tmp->score = tmp->next->score;
					strcpy_s(tmp->name, tmp->next->name);
					tmp->next->score = temp->score;
					strcpy_s(tmp->next->name, temp->name);
					temp->map_num = tmp->map_num;
					tmp->map_num = tmp->next->map_num;
					tmp->next->map_num = temp->map_num;
					strcpy_s(temp->diff, tmp->diff);
					strcpy_s(tmp->diff, tmp->next->diff);
					strcpy_s(tmp->next->diff, temp->diff);
					free(temp);
				}
				tmp = tmp->next;
			}
			tmp = head;
			i++;
		}
		tmp = head;
		int pos = 20;//纵坐标
		while (tmp->next != NULL)//对分数进行输出
		{
			TCHAR NAME[11];
			CharToTchar(tmp->name, NAME);
			TCHAR info[100] = _T("Name:");
			lstrcat(info, NAME);
			TCHAR strscore[30] = _T(" Score:");
			char cscore[20];
			sprintf_s(cscore, "%d", tmp->score);
			TCHAR tscore[20];
			CharToTchar(cscore, tscore);
			char diff_and_map[30];
			sprintf_s(diff_and_map, " Difficulty:%s Map:%d", tmp->diff, tmp->map_num);
			TCHAR t_diff[30];
			CharToTchar(diff_and_map, t_diff);
			lstrcat(strscore, tscore);
			lstrcat(info, strscore);
			lstrcat(info, t_diff);
			outtextxy(WINDOW_WIDTH / 2 - 6 * MINSIZE, pos, info);
			pos += 20;
			tmp = tmp->next;
		}
		free(tmp);
	}
	else
		outtextxy(WINDOW_WIDTH / 2 - 2 * MINSIZE, MINSIZE, _T("无数据显示"));
}
void display_scorelist(void)//显示排行榜
{
	cleardevice();
	settextcolor(WHITE);
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 4, MAP_HEIGHT - MINSIZE, _T("Press Any Key to Go back Menu"));
	loadscore();
	_getch();
}
void choose_exit(void)//退出
{
	save();
	exit(0);
}
void function_choose(void)//功能界面主函数
{
begin:
	loadbutton();
	loadimage(NULL, _T(".\\Resources\\choose_background.png"), 1280, 760);
	settextcolor(WHITE);
	settextstyle(MINSIZE, MINSIZE / 2, _T("黑体"));
	outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 6, 320, _T("Dead by Daylight Snake"));
	settextstyle(MINSIZE / 2, MINSIZE / 4, _T("黑体"));
	putimage(160, 400, &start);
	outtextxy(200, 560, _T("开始游戏"));
	putimage(320, 400, &resume);
	outtextxy(360, 560, _T("继续游戏"));
	putimage(480, 400, &setting);
	outtextxy(520, 560, _T("游戏设置"));
	putimage(640, 400, &rule);
	outtextxy(680, 560, _T("听听规则"));
	putimage(800, 400, &scorelist);
	outtextxy(840, 560, _T("游戏排行"));
	putimage(960, 400, &exit_button);
	outtextxy(1000, 560, _T("退出游戏"));
	outtextxy(WINDOW_WIDTH / 2 - MINSIZE * 2, MAP_HEIGHT - MINSIZE, _T("Made by ChenYongji"));
	while (!(startgame || resume_game))
	{
		clickpoint = GetMouseMsg();
		switch (clickpoint.uMsg)
		{
		case WM_LBUTTONDOWN:
			if (between(clickpoint.x, 160, 320) && between(clickpoint.y, 400, 560))
				startgame = true;
			else if (between(clickpoint.x, 320, 480) && between(clickpoint.y, 400, 560))
				resume_game = true;
			else if (between(clickpoint.x, 480, 640) && between(clickpoint.y, 400, 560))
			{
				settings();
				goto begin;
			}
			else if (between(clickpoint.x, 640, 800) && between(clickpoint.y, 400, 560))
			{
				display_rules();
				goto begin;
			}
			else if (between(clickpoint.x, 800, 960) && between(clickpoint.y, 400, 560))
			{
				display_scorelist();
				goto begin;
			}
			else if (between(clickpoint.x, 960, 1120) && between(clickpoint.y, 400, 560))
				choose_exit();
			break;
		default:break;
		}
		while (_kbhit())
			_getch();
	}
	cleardevice();
}