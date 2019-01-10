/*
This head file includes some constants.
*/
#pragma region definition
#define LEFT  'A'
#define RIGHT 'D'
#define UP    'W'
#define DOWN  'S'				//���������
#define MAP_WIDTH 1000			//��ͼ���
#define WINDOW_WIDTH 1280		//���ڿ��
#define MAP_HEIGHT 760			//��ͼ�߶�
#define FREE 0					//�յ�
#define BARRIER 1				//�ϰ�
#define FOOD 2					//ʳ��
#define POISON 3				//����
#define MINE 4					//����
#define SNAKE 5					//�߱���
#define EXIT 6					//����
#define HATCH 7					//�ؽ�
#define KIT 8					//���Ȱ�
#define MAP 9					//��ͼ
#define MINSIZE 40				//���ػ�������
#define HEALTHY 50				//����״̬
#define INJURED 60				//����״̬
#define DEAD	70				//����״̬
extern bool startgame;			//��ʼ��Ϸָ��
extern bool resume_game;		//������Ϸָ��
extern int map_choose;			//��ͼѡ��ָ��
extern int difficulty;			//�Ѷ�ָ��
extern int speed;				//�ٶ�ָ��
extern int generators_left;		//�ܷ������ָ��
extern int hatch_generate;		//�ؽѿ���ָ��
extern int generate_num;		//���ɶ��ݵ�������
#pragma endregion