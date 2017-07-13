/*
ĳġ ���ε� with C
������Ʈ ���� ���� : 2017-07-10
������Ʈ ���� ���� ��ǥ : 2017-07-18
����
����� -- ���콺����, ����ȭ�� ���۸�ǥ
�Ż�ȣ -- ���� ���α׷��� server, client ����
��μ� -- �׷��� ���̺귯���� ����Ͽ� ���ӳ��� ����
������ -- ��ü���� Ʋ�� ��ħ, mysql ����
��Visual Studio 2013 �̻󿡼� ����� �����մϴ�
*/
//��ó����
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// ������� ����
//�⺻ �������
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <process.h>		//process ��Ƽ�������
//#include <iostream>
#include <stdbool.h>
#include <signal.h>
//#include <WinSock2.h>		//�������α׷���

//Ư�� ������� (���� ��ġ) 
#include "SDL/SDL.h"			//SDL - �׷��� �������
#include "SDL/SDL_image.h"

#include "mysql/mysql.h"
#define nullptr 0

// ���̺귯�� ���� ���̺귯�������� ���� �߰����ص� �˴ϴ�.
// #pragma comment �� visual studio������ ��� ���� *�ַ�� �÷����� 64��Ʈ�� ���ּ���
#pragma comment (lib, "libmysql.lib")	//mysql���̺귯��
#pragma comment (lib, "SDL2")			//�׷��� ���̺귯�� 1
#pragma comment (lib, "SDL2main")		//�׷��� ���̺귯�� 2
#pragma comment (lib, "SDL2_image")
#pragma comment (lib, "ws2_32.lib")		//����(��Ʈ��ũ)���̺귯��

#pragma warning (disable : 4101)		//������� ���� ���������Դϴ�. ��� ����
//#define ���ǹ�
#define CLS system("cls")		//ȭ�� �����
//#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�
//#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�(����)
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))		//������ X���� Y ���
#define CHOP(x) x[strlen(x) - 1] = ' '	//fgets�� ���� ���͵� �迭������. ���͸� �迭���� �������ִ°�
//������� ������� 
#define COL                   GetStdHandle(STD_OUTPUT_HANDLE)			// �ܼ�â�� �ڵ����� �ޱ�
#define BLACK                SetConsoleTextAttribute(COL, 0x0000);		// ������
#define DARK_BLUE         SetConsoleTextAttribute(COL, 0x0001);			// �Ķ���
#define GREEN                SetConsoleTextAttribute(COL, 0x0002);      // ���
#define BLUE_GREEN        SetConsoleTextAttribute(COL, 0x0003);         // û���
#define BLOOD               SetConsoleTextAttribute(COL, 0x0004);       // �˺�����
#define PURPLE               SetConsoleTextAttribute(COL, 0x0005);		// �����
#define GOLD                 SetConsoleTextAttribute(COL, 0x0006);      // �ݻ�
#define ORIGINAL            SetConsoleTextAttribute(COL, 0x0007);       // ���� ȸ�� (�⺻ �ܼ� �۾� ����)
#define GRAY                 SetConsoleTextAttribute(COL, 0x0008);      // ȸ��
#define BLUE                  SetConsoleTextAttribute(COL, 0x0009);     // �Ķ���
#define HIGH_GREEN       SetConsoleTextAttribute(COL, 0x000a);			// ���λ�
#define SKY_BLUE           SetConsoleTextAttribute(COL, 0x000b);        // �ϴû�
#define RED                   SetConsoleTextAttribute(COL, 0x000c);     // ������
#define PLUM                SetConsoleTextAttribute(COL, 0x000d);       // ���ֻ�
#define YELLOW             SetConsoleTextAttribute(COL, 0x000e);        // �����
#define WHITE                SetConsoleTextAttribute(COL, 0x000f);      // ���
//����ü ����
typedef struct {
	char name[20];
	char id[30];
	char pass[50];
}LOG;
typedef struct {
	LONG x;
	LONG y;
}MOUSEPOINT;
typedef struct {
	char roomname[30];
	char password[30];
	char ip[20];
}ROOM;
typedef struct tagPOINT *PPOINT;
typedef struct tagPOINT *LPPOINT;

//���� ������ (��� ����õ)

CRITICAL_SECTION cs;	//�̺�Ʈ
char message[100];		//���� ���α׷��� ���ڿ�
char status[4];			//���Ͽ�
char username[30];		//����� �̸�
char friendname[4][30] = {"Player 1", "Player 2", "Player 3", "Player 4"};
WSADATA wsaData;						//���� WSAStartup()�Լ��� ���̴� ����
SOCKET connect_sock, Sconnect_sock[4], listen_sock;	//���� ���Ϻ���
SOCKADDR_IN connect_addr, listen_addr;			//���� �ּ����� �����ϴ� ����
int sockaddr_in_size;
ROOM connectroom[6];
char signalmode;
bool lead = false;



//�⺻ �Լ���
void gotoxy(short x, short y);
void cur(short x, short y);
void exitsignal(void);
void signalall(void);
void ConsoleL(int x, int y);					//�ܼ�â�� ũ�⸦ �����ϴ� �Լ� x y�� �ʺ� ����
POINT MouseClick(void);							//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ� ��ȯ���� POINT�̴� (x, y)
void disablecursor(bool a);						//Ŀ�� ���̱�, �����  0 = ���̱� 1 = �����
void usermain(void);
//--------------------- ��Ʈ��ũ �Լ��� -----------------------------------
void ErrorHandling(char *Message);				//���� ���� ��� �ϴ� �Լ�
void Connect_Server(char *ServerIP);			//���� ���� ���ִ� �Լ�
void recieve(void);								//�������� ������ �޾ƿ��� ������� �Լ�
void sendall(char *message);					//�ϳ��������� ��������
void waitroom(void);							//��Ʈ��ũ ����
void Clnt_1(void);								//���� - Ŭ���̾�Ʈ 1���
void Clnt_2(void);								//���� - Ŭ���̾�Ʈ 2���
void Clnt_3(void);								//���� - Ŭ���̾�Ʈ 3���
void Clnt_4(void);								//���� - Ŭ���̾�Ʈ 4���
void makeroom(int *count);							//�游���(��Ʈ��ũ)
IN_ADDR GetDefaultMyIP(void);					//�� ip ���
//--------------------- MySQL �Լ��� --------------------------------------
int sqllogin(MYSQL *cons);						//mysql�� ����� �����͸� ���� �α����� �ϴ� �Լ�
int sqlsignup(MYSQL *cons);						//mysql�� ���������͸� �߰��ϴ� �Լ�
void loadmysql(MYSQL *cons, char mysqlip[]);	//mysql�� �����ϴ� �Լ�
char **onemysqlquery(MYSQL *cons, char *query); //mysql���ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
void writechating(MYSQL *cons);					//mysql�� ä���� �Է��ϴ� �Լ�
void readchating(MYSQL *cons);					//mysql�� ä���� �д� �Լ�
void sqlmakeroom(MYSQL *cons);					//���� ����� �Լ�
// -------------------- SDL �׷��� �Լ��� ---------------------------------
void SDL_ErrorLog(const char * msg);			//�׷��ȿ����ڵ� ��� �Լ�
void IMG_ErrorLog(const char * msg);			//�̹��������ڵ� ��� �Լ�
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // �׷��ȿ��� ó�� �Լ�
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window);						 // �̹������� ó�� �Լ�
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // �ؽ��Ŀ� �̹������� �پ��ϰ� �ε��ϴ� �Լ� ����
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h);	//�ؽ��ĸ� ����ϴ� �Լ� ����
// -------------------- ���� ���� �Լ��� ----------------------------------
void mainatitleimage(void);						//���� ����Ÿ��Ʋ ���
int maintitle(void);							//���� ����Ÿ��Ʋ ��¹� ����
void banglist(MYSQL *cons);						//���� �� ���
int bangchose(MYSQL *cons);						//���� �� ��¹� ����
int chooseroom(int roomnum);
void logintema(void);							//�α��� ������
void jointema(void);							//ȸ������ ������
LOG login(int m);								//�⺻���� �α��� �Է�
//-------------------------�ܼ� �Լ���------------------------------------
void checkword(char*nowword, char*scanword);	//�ܾ Ȯ����
void click(int *xx, int *yy);					//Ŭ���Լ� �ι�°, xx���� yy���� ��ȯ��
//--------------------------�̴ϰ��� ���ھ߱� �Լ���----------------------
void createainumber(int *ainum);
void scanfirst(int *usnum);
void scan(int(*usre)[100], int *ainum, int turn);
void aiscan(int(*aire)[100], int *usnum, int(*aiall)[10][10], int t);
void aicheck(int(*aiall)[10][10], int(*aire)[100], int t);
void numberbaseball();
//�Լ� ���� ��  �ɼ� ������ ������ �Լ��� ������ֽñ� �ٶ��ϴ�.


int main(int argc, char **argv) //main�Լ� SDL������ �μ��� ������ �� ������� 
{
	//���� ����
//	InitializeCriticalSection(&cs);
	int i, j, k, v, result;
	char mainchoose = 0;
	char bangchoose;
	char chooseroomcount;
	POINT pos;								//x, y��ǥ ǥ�� )pos.x, pos.y
	MYSQL *cons = mysql_init(NULL);			//mysql �ʱ�ȭ
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	char query[400];						//mysql ���ɾ ������
	char mysqlip[30] = "10.80.161.182";		//mysql ip ����ip�Դϴ�	���� ������7����
	char *ServerIP = "10.80.162.41";		//���� ip ��ȣip��
	char data[1000][30] = { 0, };           //�ܾ����
	char nowword[30] = { 0, };              //�������� �ܾ�
	char scanword[30] = { 0, };             //���� ģ �ܾ�
	int bangnum = 0;						//���� ���� ��ȣ
	mysql_query(cons, "use catchmind");
	//���� ���� ��
	disablecursor(1);
	memset(&wsaData, 0, sizeof(wsaData));
	memset(&connect_sock, 0, sizeof(connect_sock));
	memset(&connect_addr, 0, sizeof(connect_addr));
	/*	maintitle();
		bangnum = bangchose();
		if (bangnum == 1) {
			memset(&wsaData, 0, sizeof(wsaData));
			memset(&connect_sock, 0, sizeof(connect_sock));
			memset(&connect_addr, 0, sizeof(connect_addr));
			Connect_Server(ServerIP);
		}*/
	loadmysql(cons, mysqlip);				//mysql ���� �ҷ�����

	mainchoose = maintitle();				//main ȭ��
						//
	while (1) {								//�α��� �ݺ���
		CLS;
		if (mainchoose == 1) {				//main���� ù��°�� ������
			//ConsoleL(26, 15);���콺�� �ܼ�â�� ����� �Է��� �ȵǹǷ� ��� ����				//�ܼ�ũ�⸦ �α���â�� �°�  
			disablecursor(0);               //Ŀ�� ���̰�
			if (sqllogin(cons) != 1)		//�α��ο� �������� ���ϸ� ó������
				continue;
			disablecursor(1);
			while (1) {						//�� �ݺ���
				ConsoleL(50, 20);
				bangchoose = bangchose(cons);	//���� ����	
				if (bangchoose == 0) {			//�游��⸦ Ŭ���ϸ� �游���� �̵�
					sqlmakeroom(cons);
					break;
				}
				else if (bangchoose == 1)		//�� ���� ���� -�����߰�
				{

				}
				else                            //�� ���� ����
				{
					chooseroomcount = chooseroom(bangchoose);
					if (chooseroomcount == -1)		//return -1�� �ش� ���̾�����
					{
						CLS;
						continue;
					}
					if (chooseroomcount == 0)		//return 0�� ��й�ȣ�� Ʋ����
					{
						cur(10, 1);
						printf("(��й�ȣ�� Ʋ�Ƚ��ϴ�)");
						getchar();
						continue;
					}
					if (chooseroomcount == 1)		//return 1 �� ��й�ȣ���� ������
					{
						
						Connect_Server(connectroom[bangchoose - 2].ip);
						break;
					}
				}
			}
			break;
		}

	}return 0;
}



//�Լ� �����		���� �ּ�ȭ Ctrl + M + O  ���� ���̱� Ctrl + M + L
void sqlmakeroom(MYSQL *cons) {
	while (1) {
		CLS;
		int count = 0;
		int i = 0;
		IN_ADDR addr;

		addr = GetDefaultMyIP();	//����Ʈ IPv4 �ּ� ������
		char * myip = inet_ntoa(addr);
		ROOM myroom = { 0, 0, 0 };
		disablecursor(1);
		printf("��������������������������\n");
		printf("��                                              ��\n");
		printf("��            ĳġ���ε� �� �����              ��\n");
		printf("��          �� ip :  %s              ��\n", myip);
		printf("��������������������������\n");
		printf("��    ����    ��                              ���\n");
		printf("��������������������������\n");
		printf("��  Password  ��                              ���\n");
		printf("��������������������������\n");
		cur(16, 5);
		scanf("%[^\n]s", myroom.roomname);
		getchar();
		cur(16, 7);
		while (1) {

			if (_kbhit()) {
				myroom.password[i] = _getch();
				if (myroom.password[i] == 8) {
					if (i == 0) {
						myroom.password[0] = 0;
						continue;
					}
					printf("\b \b");
					myroom.password[i - 1] = 0;
					myroom.password[i--] = 0;
				}
				else if (myroom.password[i] == 13 && i > 3) {
					myroom.password[i] = 0;
					break;
				}
				else if (myroom.password[i] == 13) {
					myroom.password[i] = 0;
				}
				else if (i >= 15) {
					continue;
				}
				else if (!((myroom.password[i] >= '0' && myroom.password[i] <= '9') || (myroom.password[i] >= 'a' && myroom.password[i] <= 'z') || (myroom.password[i] >= 'A' && myroom.password[i] <= 'Z'))) {
					myroom.password[i] = 0;
				}
				else {
					printf("*");
					i++;
				}
			}
		}
		char query[100];
		sprintf(query, "insert into catchmind.room (ip, name, password) values ('%s', '%s', '%s')", myip, myroom.roomname, myroom.password);
		if (!(mysql_query(cons, query)))
		{
			cur(10, 1);
			printf("(�˼� ���� ���� �ߺ��� �ȵǰ� ����� �ּ���)");
		}
		disablecursor(1);
		CLS;
		printf("�� ������....");
		_beginthreadex(NULL, 0, (_beginthreadex_proc_type)makeroom, &count, 0, 0);
		while (1) {
			if (count == 1)
			{
				lead = true;
				CLS;
				Connect_Server(myip);
				return;
			}
			Sleep(1000);
		}
		break;
	}
}
void waitroom(void)
{
	int xx = 0, yy = 0;
	ConsoleL(100, 50);
	while (1) { //�޾ƿ� ������ ó��
		
		gotoxy(0, 3);
		WHITE
		printf("      �������������������������������������������������\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��   [ STATUS ] : ");
		cur(11, 5);
		printf("[ %s ]", friendname[0]);
		if (status[0] == 0) {

			GRAY
				gotoxy(24, 7);
			printf("                   ");
			gotoxy(24, 7);
			printf("DISCONNECTED");
			WHITE
		}
		else if (status[0] == 1) {
			HIGH_GREEN
				gotoxy(24, 7);
			printf("                   ");
			gotoxy(24, 7);
			printf("JOIN           ");
			WHITE
		}
		if (status[0] == 2) {
			SKY_BLUE
				gotoxy(24, 7);
			printf("                   ");
			gotoxy(24, 7);
			printf("READY");
			WHITE
		}
		gotoxy(100, 7);
		printf("��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      �������������������������������������������������\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��   [ STATUS ] : ");
		cur(11, 14);
		printf("[ %s ]", friendname[1]);
		if (status[1] == 0) {
			GRAY
				gotoxy(24, 16);
			printf("                   ");
			gotoxy(24, 16);
			printf("DISCONNECTED");
			WHITE
		}
		else if (status[1] == 1) {
			HIGH_GREEN
				gotoxy(24, 16);
			printf("                   ");
			gotoxy(24, 16);
			printf("JOIN");
			WHITE
		}
		if (status[1] == 2) {
			SKY_BLUE
				gotoxy(24, 16);
			printf("                   ");
			gotoxy(24, 16);
			printf("READY");
			WHITE
		}
		gotoxy(100, 16);
		printf("��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      �������������������������������������������������\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��   [ STATUS ] : ");
		cur(11, 23);
		printf("[ %s ]", friendname[2]);
		if (status[2] == 0) {
			GRAY
				gotoxy(24, 25);
			printf("                   ");
			gotoxy(24, 25);
			printf("DISCONNECTED");
			WHITE
		}
		else if (status[2] == 1) {
			HIGH_GREEN
				gotoxy(24, 25);
			printf("                   ");
			gotoxy(24, 25);
			printf("JOIN");
			WHITE
		}
		if (status[2] == 2) {
			SKY_BLUE
				gotoxy(24, 25);
			printf("                   ");
			gotoxy(24, 25);
			printf("READY");
			WHITE
		}
		gotoxy(100, 25);
		printf("��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      �������������������������������������������������\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n"); //92
		printf("      ��   [ STATUS ] : ");
		cur(11, 32);
		printf("[ %s ]", friendname[3]);
		if (status[3] == 0) {
			GRAY
				gotoxy(24, 34);
			printf("                   ");
			gotoxy(24, 34);
			printf("DISCONNECTED");
			WHITE
		}
		else if (status[3] == 1) {
			HIGH_GREEN
				gotoxy(24, 34);
			printf("                   ");
			gotoxy(24, 34);
			printf("JOIN");
			WHITE
		}
		if (status[3] == 2) {
			SKY_BLUE
				gotoxy(24, 34);
			printf("                   ");
			gotoxy(24, 34);
			printf("READY");
			WHITE
		}
		gotoxy(100, 34);
		printf("��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      ��                                                                                            ��\n");
		printf("      �������������������������������������������������\n");
		printf("      ��                ��                                                        ��                ��\n");		// 4, 40		//11, 40		//42, 40		49, 40
		if (lead == true && status[0] == 2 && status[1] == 2 && status[2] == 2 && status[3] == 2)
			printf("      ��     ready      ��                       Start!                           ��     exit       ��\n");
		else
			printf("      ��     ready      ��                                                        ��     exit       ��\n");
		printf("      ��                ��                                                        ��                ��\n");		// 4, 42		//11, 42		//42, 42		49, 42
		printf("      �������������������������������������������������\n");
		click(&xx, &yy);
		if (xx > 3 && xx < 12 && yy < 43 && yy > 39) {
			cur(0, 0);
			printf("��%d  ", xx);
			send(connect_sock, "player ready", 40, 0);
		}
		Sleep(100);
		
	}
}
void usermain(void) {
#ifdef SANGHO
	makeroom();
#endif
#ifdef SOOHAN
#endif
#ifdef SANGHIE


#endif
#ifdef MINSUK
#endif
	exit(1);
}
LOG login(int m) { // 1�̸� �α��� 2�̸� ȸ������ �ʼ�!!
				   //���� ���� �ڵ�ϱ� ȸ�������̶� �α��ο� �� �����ñ�

	int n = 0;
restart:
	POINT a;
	gotoxy(0, 0);
	if (m == 1)
		logintema();
	else if (m == 2)
		jointema();
	LOG user = { 0, 0 };
	int i = 0;
	int cnt = 0;
	int xx = 0, yy = 0;
	/*�г��� ����*/
	gotoxy(16, 3);
	if (m == 2) {
		while (1) {
			if (_kbhit()) {

				user.name[i] = _getch();
				if (user.name[i] == 8) {
					if (i == 0) {
						user.name[0] = 0;
						continue;
					}
					printf("\b \b");
					user.name[i - 1] = 0;
					user.name[i--] = 0;
				}
				else if ((user.name[i] == 9 || user.name[i] == 13)) {
					user.name[i] = 0;
					break;
				}
				else if (i >= 15) {
					continue;
				}
				else if (!((user.name[i] >= '0' && user.name[i] <= '9') || (user.name[i] >= 'a' && user.name[i] <= 'z') || (user.name[i] >= 'A' && user.name[i] <= 'Z'))) {
					user.name[i] = 0;
				}
				else
					putchar(user.name[i++]);
			}

			GetCursorPos(&a);
			SetCursorPos(a.x, a.y);
			click(&xx, &yy);
			//gotoxy(20,20);
			//printf("%3d %3d", xx, yy); //login 19~23 5~7      ������ ����Ʈ 1~7 9~11    ȸ������ 9~15      �ʱ�ȭ 17~23

			if (9 <= yy && yy <= 11) {
				if (1 <= xx && xx <= 7) {
					system("start https://blog.naver.com/dgsw102");
					system("start https://blog.naver.com/soohan530");
				}
				else if (n < 1)
					n++;
				else if (9 <= xx && xx <= 15) {
					for (i = 0; i <= 15; i++) {
						user.name[i] = 0;
					}
					return user; //�Լ��� ������ ���ΰ��� �����ϸ� �˾Ƽ� �ɷ�����
				}
				else if (17 <= xx && xx <= 23) {
					goto restart;
				}
			}
			Sleep(20);
		}
	}
	i = 0;
	gotoxy(16, 5);
	while (1) {
		if (_kbhit()) {

			user.id[i] = _getch();
			if (user.id[i] == 8) {
				if (i == 0) {
					user.id[0] = 0;
					continue;
				}
				printf("\b \b");
				user.id[i - 1] = 0;
				user.id[i--] = 0;
			}
			else if ((user.id[i] == 9 || user.id[i] == 13) && i > 3) {
				user.id[i] = 0;
				break;
			}
			else if (user.id[i] == 13) {
				user.id[i] = 0;
			}
			else if (i >= 15) {
				continue;
			}
			else if (!((user.id[i] >= '0' && user.id[i] <= '9') || (user.id[i] >= 'a' && user.id[i] <= 'z') || (user.id[i] >= 'A' && user.id[i] <= 'Z'))) {
				user.id[i] = 0;
			}
			else
				putchar(user.id[i++]);
		}
		GetCursorPos(&a);
		SetCursorPos(a.x, a.y);
		click(&xx, &yy);
		//gotoxy(20,20);
		//printf("%3d %3d", xx, yy); //login 19~23 5~7      ������ ����Ʈ 1~7 9~11    ȸ������ 9~15      �ʱ�ȭ 17~23

		if (9 <= yy && yy <= 11) {
			if (1 <= xx && xx <= 7) {
				system("start https://blog.naver.com/dgsw102");
				system("start https://blog.naver.com/soohan530");
			}
			else if (n < 1)
				n++;
			else if (9 <= xx && xx <= 15) {
				for (i = 0; i <= 15; i++) {
					user.id[i] = 0;
					user.pass[i] = 0;
					user.name[i] = 0;
				}
				return user; //�Լ��� ������ ���ΰ��� �����ϸ� �˾Ƽ� �ɷ�����
			}
			else if (17 <= xx && xx <= 23) {
				goto restart;
			}
		}
		Sleep(20);
	}
	i = 0;

	/*��й�ȣ ��ȣȭ ����*/
	gotoxy(16, 7);
	while (1) {

		if (_kbhit()) {
			user.pass[i] = _getch();
			if (user.pass[i] == 8) {
				if (i == 0) {
					user.pass[0] = 0;
					continue;
				}
				printf("\b \b");
				user.pass[i - 1] = 0;
				user.pass[i--] = 0;
			}
			else if (user.pass[i] == 13 && i > 3) {
				user.pass[i] = 0;
				break;
			}
			else if (user.pass[i] == 13) {
				user.pass[i] = 0;
			}
			else if (i >= 15) {
				continue;
			}
			else if (!((user.pass[i] >= '0' && user.pass[i] <= '9') || (user.pass[i] >= 'a' && user.pass[i] <= 'z') || (user.pass[i] >= 'A' && user.pass[i] <= 'Z'))) {
				user.pass[i] = 0;
			}
			else {
				printf("*");
				i++;
			}
		}
		GetCursorPos(&a);
		SetCursorPos(a.x, a.y);
		click(&xx, &yy);

		if (9 <= yy && yy <= 11) {
			if (1 <= xx && xx <= 7) {
				system("start https://blog.naver.com/dgsw102");
				system("start https://blog.naver.com/soohan530");
			}
			else if (9 <= xx && xx <= 15) {
				for (i = 0; i <= 15; i++) {
					user.id[i] = 0;
					user.pass[i] = 0;
					user.name[i] = 0;
				}
				break;  //�Լ��� ������ ���ΰ��� �����ϸ� �˾Ƽ� �ɷ�����
			}
			else if (17 <= xx && xx <= 23) {
				goto restart;
			}
		}
		else if (m == 1 && 19 <= xx && xx <= 23 && 5 <= yy && yy <= 7) {
			break;
		}
		else if (m == 2 && 19 <= xx && xx <= 23 && 3 <= yy && yy <= 7) {
			break;
		}


		Sleep(20);
	}
	return user;
}
void checkword(char*nowword, char*scanword) {

	int cnt = 0;
	int i = 0;
	while (cnt != i - 1) { //�ܾ ������ ���� ����

		cnt = 0;

		for (i = 0; scanword[i] != 0; i++) //�ʱ�ȭ
			scanword[i] = 0;

		fgets(scanword, sizeof(scanword), stdin); // ���� ä�ü����� ����

		for (i = 0; nowword[i] != 0; i++) //��
			if (nowword[i] == scanword[i])
				cnt++;

		if (scanword[i] != 0) //��(��������)
			cnt = 0;

	}
	printf("��");
} //�˾Ƽ� ������
void writechating(MYSQL *cons)
{
	char query[300];
	int i = 0;
	TCHAR buff;
	char buff2 = 0;
	TCHAR buffer[100] = { 0, };
	while (1) {

		while (1)
		{
			if (_kbhit())
			{


				buff = _getch();
				if (buff == 13)
					break;
				else if (buff == 8 && i != 0) {
					CLS;
					buffer[i--] = 0;
				}
				else
				{
					if (buff == buff2)
						i++;
					buffer[i++] = buff;
					buffer[i + 1] = 0;
					buff = buff2;
				}
				buff = 0;

			}

			EnterCriticalSection(&cs);
			cur(40, 20);
			printf("%ls", buffer);
			cur(0, 0);
			LeaveCriticalSection(&cs);
			Sleep(10);
		}
		CLS;
		i = 0;
		sprintf(query, "insert into catchmind.chating (name, mean) values ('������', '%S')", buffer);
		mysql_query(cons, query);
		memset(buffer, 0, sizeof(buffer));
	}
}
void readchating(MYSQL *cons) {
	int v = 0;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	while (1) {

		v = 0;
		mysql_query(cons, "select * from catchmind.chating order by time desc limit 10");
		sql_result = mysql_store_result(cons);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			EnterCriticalSection(&cs);
			cur(10, 20 - (short)v);
			printf("%s : %s", sql_row[2], sql_row[3]);
			v++;
			LeaveCriticalSection(&cs);

		}
		Sleep(500);
	}

}
void loadmysql(MYSQL *cons, char mysqlip[])	//MYSQL ���� �ҷ�����
{
	CLS;
	printf("�����ͺ��̽� �ҷ����� �õ���...");
	if (cons == NULL) //cons�� �ʱ�ȭ�� ��������
	{
		fprintf(stderr, "%s\n", mysql_error(cons));		//���� ����Ʈ, printf("%s")�� ����
		printf("ä�� ���� �ҷ����� ���� (�ʱ�ȭ ����)\n");
		Sleep(1000);
		exit(1);
	}
	else
		printf("1");
	if (mysql_real_connect(cons, mysqlip, "root", "tbvjaos15", NULL, 0, NULL, 0) == NULL) //mysql������ ����
	{
		printf("\b����... \n������ �������� �ʽ��ϴ�.\n");
		fprintf(stderr, "%s\n", mysql_error(cons));
		printf("���ο� ip�� ������ �ּ���.\n->");
		scanf("%s", mysqlip);
		loadmysql(cons, mysqlip);					//����Լ� ȣ��
	}
	else {
		printf("\b2");
		printf("\n����");
		mysql_set_character_set(cons, "euckr");
		CLS;
	}

	return;
}
char **onemysqlquery(MYSQL *cons, char *query) {		//mysql ���ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
	mysql_query(cons, query);
	return mysql_fetch_row(mysql_store_result(cons));

}
POINT MouseClick(void)			//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ�
{
	POINT pos;
	HANDLE       hIn, hOut;
	DWORD        dwNOER;
	INPUT_RECORD rec;

	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	while (TRUE) {
		ReadConsoleInput(hIn, &rec, 1, &dwNOER);

		if (rec.EventType == MOUSE_EVENT) {
			if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
				pos.x = rec.Event.MouseEvent.dwMousePosition.X;
				pos.y = rec.Event.MouseEvent.dwMousePosition.Y;
				return pos;
			}
		}
	}
}
void ConsoleL(int x, int y) {			//�ܼ�â�� ũ�⸦ �������ִ� �Լ�
	char buff[50];
	sprintf(buff, "mode con cols=%d lines=%d", x * 2, y);
	system(buff);
}
void disablecursor(bool a) {

	CONSOLE_CURSOR_INFO ConsoleCursor;			// �ܼ�Ŀ�� ���� ����ü
	if (a == true) {
		ConsoleCursor.bVisible = false;				// true ���� , false �Ⱥ���
		ConsoleCursor.dwSize = 1;
	}// Ŀ�� ������
	else
	{
		ConsoleCursor.bVisible = true;				// true ���� , false �Ⱥ���
		ConsoleCursor.dwSize = 10;
	}
	SetConsoleCursorInfo(COL, &ConsoleCursor);	// ����
}
void ErrorHandling(char *Message) {
	system("cls");
	fputs(Message, stderr);
	fputc('\n', stderr);
	exit(1);
}
void SDL_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	printf("%s Error: %s\n", msg, SDL_GetError());
}
void IMG_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	printf("%s Error: %s\n", msg, IMG_GetError());
	return;
}
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step) {// ���� ó�� �Լ�
	SDL_ErrorLog(msg);//�����ڵ� ���
	switch (step) {
	case 3:
		SDL_DestroyRenderer(Renderer);// SDL ������ �ı�
	case 2:
		SDL_DestroyWindow(Window);//SDL ������ �ı�
	case 1:
		SDL_Quit();//SDL ����
		getchar();//�����ڵ� Ȯ���ϱ� ���� �ܼ�â ���
		return;
	}
}
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window) {
	SDL_DestroyRenderer(Renderer);// SDL ������ �ı�
	SDL_DestroyWindow(Window);//SDL ������ �ı�
	SDL_Quit();//SDL ����
	getchar();//�����ڵ� Ȯ���ϱ� ���� �ܼ�â ���
	return;
}
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file) { // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;// JPG���ϰ� PNG���� �ε� ����
	if (IMG_Init(imgFlags) != imgFlags) {//IMG �ʱ�ȭ�ϰ� �ʱ�ȭ �ȵǸ� if�� ����
		IMG_ErrorLog("IMG_Init");
		return nullptr;//�������� ��ȯ
	}
	SDL_Surface* Surface = IMG_Load(file);//���ǽ��� �̹����ε�
	if (Surface == nullptr) {//���ǽ��� �̹����ε尡 �ȵǸ� 
		IMG_ErrorLog("IMG_Load");
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);//���ǽ��κ��� �ؽ��� ����
	SDL_FreeSurface(Surface);// ���ǽ� �޸�����
	if (Texture == nullptr) {// �ؽ��� ���� ���н� if������
		SDL_ErrorLog("SDL_CreateTextureFromSurface");// ���� �ڵ� ���
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	IMG_Quit();// IMG ����
	return Texture;// Texture������ ��ȯ
}
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip) { // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;// JPG���ϰ� PNG���� �ε� ����
	if (IMG_Init(imgFlags) != imgFlags) {//IMG �ʱ�ȭ�ϰ� �ʱ�ȭ �ȵǸ� if�� ����
		IMG_ErrorLog("IMG_Init");
		return nullptr;//�������� ��ȯ
	}
	SDL_Surface* Surface = IMG_Load(file);//���ǽ��� �̹����ε�
	if (Surface == nullptr) {//���ǽ��� �̹����ε尡 �ȵǸ� 
		IMG_ErrorLog("IMG_Load");
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	SDL_SetColorKey(Surface, 1, SDL_MapRGB(Surface->format, r, g, b));// r,g,b���� �ش��ϴ� ������ ����� �Լ���
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);//���ǽ��κ��� �ؽ��� ����
	SDL_FreeSurface(Surface);// ���ǽ� �޸�����
	if (Texture == nullptr) {// �ؽ��� ���� ���н� if������
		SDL_ErrorLog("SDL_CreateTextureFromSurface");// ���� �ڵ� ���
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	IMG_Quit();// IMG ����
	return Texture;// Texture������ ��ȯ
}
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h) {//�ؽ��ĸ� ����ϴ� �Լ� ����
	SDL_Rect Src;// ���簢�� ����
	Src.x = 0;// ���簢���� ������ �������� x��ǥ�ʱ�ȭ
	Src.y = 0;// ���簢���� ������ �������� y��ǥ�ʱ�ȭ
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h); // Texture�� �ʺ�� ���� ������ Src.w, Src.h�� ����
	SDL_Rect Dst;
	Dst.x = x;//�Ű�����x�� ������ �������� x��ǥ�� ����
	Dst.y = y;//�Ű�����y�� ������ �������� y��ǥ�� ����
	Dst.w = w;//�Ű�����w�� ���簢���� �ʺ� ����
	Dst.h = h;//�Ű�����h�� ���簢���� ���̿� ����
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst);//Src�� ������ ������ �ִ� Texture�� Dst�� ������ ���� Texture �� ��ȯ�Ͽ� �������� ����
}
void Connect_Server(char *ServerIP) { //���� ���� ���ִ� �Լ�
	char query[100];
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock������ ���� �Ҵ�
	connect_addr.sin_family = AF_INET;				//������ ������ �ּ� ����
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //���� IP
	connect_addr.sin_port = htons(5555);					 //���� ��Ʈ
	if (connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) //������ ����
		ErrorHandling("connect() error");
	_beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, NULL, 0, NULL); //�������� �����͸� �޾ƿ��� ������ ����
	CLS;
	sprintf(query, "player   connect %s" , username);
	send(connect_sock, query, 30, 0);
	waitroom();
}
void recieve(void) { //�������� ������ �޾ƿ��� ������� �Լ�
	char message[50] = { 0, };
	Sleep(1000);
	while (1) {

		if (recv(connect_sock, message, 40, 0) > 0) { //�������� �����͸� �޾ƿ� message������ ����
			if (strncmp("player 1 connect", message, 15) == 0) {
				sscanf(message, "player 1 connect %s", friendname[0]);
				status[0] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 connect", message, 15) == 0) {
				sscanf(message, "player 2 connect %s", friendname[1]);
				status[1] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 connect", message, 15) == 0) {
				sscanf(message, "player 3 connect %s", friendname[2]);
				status[2] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 connect", message, 15) == 0) {
				sscanf(message, "player 4 connect %s", friendname[3]);
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 1 ready") == 0) {
				status[0] = 2;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 2 ready") == 0) {
				status[1] = 2;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 3 ready") == 0) {
				status[2] = 2;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 4 ready") == 0) {
				status[3] = 2;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 1 not ready") == 0) {
				status[0] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 2 not ready") == 0) {
				status[1] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 3 not ready") == 0) {
				status[2] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "player 4 not ready") == 0) {
				status[3] = 1;
				ZeroMemory(message, sizeof(message));
			}
			Sleep(100);
		}

	}
}
int sqllogin(MYSQL *cons) {
	char query[100];
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	LOG user;								//����� ���� ����ü
	while (1) {

		int check = 0;

		user = login(1);							//login �Լ��� ���
		if (user.id[0] == 0)
		{
			fflush(stdin);
			check = sqlsignup(cons);
			gotoxy(2, 3);
			if (check == 1)
				printf("               (ȸ������ ����)           ");
			else if (check == -1)
				printf("             (���̵� �ߺ��˴ϴ�)              ");
			else
				printf("               (ȸ������ ����)          ");
		}
		else {
			sprintf(query, "select * from catchmind.login where id = '%s'", user.id);	//id�� DB���� ã��
			mysql_query(cons, query);
			sql_result = mysql_store_result(cons);
			if (mysql_fetch_row(sql_result) == NULL)									//�؈� id�� ������ 
			{
				gotoxy(2, 3);
				printf("           ���̵� �������� �ʽ��ϴ�     ");

			}
			else {

				sprintf(query, "select * from catchmind.login where password = password('%s')", user.pass);
				mysql_query(cons, query);	//password�� DB�� ��ȣȭ�Ǿ� �־ ���� ���Ҷ��� ���� ��ȣȭ�ؼ� �񱳸���
				sql_result = mysql_store_result(cons);



				if ((sql_row = mysql_fetch_row(sql_result)) == NULL)
				{
					gotoxy(2, 3);
					printf("              ��й�ȣ�� Ʋ�Ƚ��ϴ�        ");
				}
				else {
					strcpy(username, sql_row[1]);
					sprintf(query, "title %s�� ĳġ���ε忡 ���Ű��� ȯ���մϴ�!", username);
					system(query);
					return 1; //�α��� ����
				}
			}
		}


	}


}
int sqlsignup(MYSQL *cons) {
	LOG user;
	char query[100];
	char query2[100];
	user = login(2);
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����


	if (user.name[0] == 0)
		return 0;

	sprintf(query2, "select * from catchmind.login where id = '%s'", user.id);	//id�� DB���� ã��
	mysql_query(cons, query2);
	sql_result = mysql_store_result(cons);
	if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		if (sql_row[0][1] != 0)
			return -1;
	}

	sprintf(query, "insert into catchmind.login (name, id, password) values ('%s', '%s', password('%s'))", user.name, user.id, user.pass);

	if (!(mysql_query(cons, query)))											//		 password�� mysql���� �����ϴ� ��ȣȭ ������.
		return 1; //����
	else
		return 0; //����

}
void mainatitleimage(void) {
	gotoxy(6, 3);
	printf("������  ��  ��      ����    ��    ������  ��        ����    ��      ������                                           ����");
	gotoxy(6, 4);
	printf("        ��  ��  ��                ��    ��      ��  ��      ��      ��  ��      ��                                                 ��      ��");
	gotoxy(6, 5);
	printf("  �����  ��  ��    ������  ��    ��      ��  ��      ��      ��  ��      ��              ��      ��   ��    ��    ��      ��");
	gotoxy(6, 6);
	printf("      ��    ����        ��      ��    ��      ��  ���      ����    ��      ������      ��      ��       ����  ��      ��");
	gotoxy(6, 7);
	printf("    ��      ��  ��      ��  ��    ��    ��      ��  ��                                          ��  ��  ��   ��    ��    ���    ��");
	gotoxy(6, 8);
	printf("  ��        ��  ��    ��      ��  ��    ��      ��  ��        ��                                ��  ��  ��   ��    ��    ��  ��    ��      ��");
	gotoxy(6, 9);
	printf("��          ��  ��    ��      ��  ��    ������  ��        �������    ��������      ��  ��     ��    ��    ��  ��      ����");
	gotoxy(12, 20);
	printf("����������                      ����������                      ����������");
	gotoxy(12, 21);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 22);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 23);
	printf("��  ���� ����   ��                      ��              ��                      ��              ��");
	gotoxy(12, 24);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 25);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 26);
	printf("����������                      ����������                      ����������");
}
int maintitle(void) { //���� ����Ÿ��Ʋ ���
	ConsoleL(100, 60);
	disablecursor(true);
	int xx = 0, yy = 0;
	mainatitleimage();
	while (1) {
		printf("%3d %3d\n", xx, yy);

		click(&xx, &yy);

		if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25)
			return 1;

		gotoxy(0, 0);
	}
	CLS;
}
void click(int *xx, int *yy) {//���콺���� 2�� �������� �޴´�

	HANDLE       hIn, hOut;
	DWORD        dwNOER;
	INPUT_RECORD rec;


	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER); // �ܼ�â �Է��� �޾Ƶ���.



	if (rec.EventType == MOUSE_EVENT) {// ���콺 �̺�Ʈ�� ���

		if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { // ���� ��ư�� Ŭ���Ǿ��� ���
			int mouse_x = rec.Event.MouseEvent.dwMousePosition.X; // X�� �޾ƿ�
			int mouse_y = rec.Event.MouseEvent.dwMousePosition.Y; // Y�� �޾ƿ�

			*xx = mouse_x / 2;
			*yy = mouse_y;
			Sleep(100);
		}
		else {
			*xx = 0;
			*yy = 0;
		}
	}

}
void banglist(MYSQL *cons) {
	CLS;
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	short i = 0;
	printf("\n"); //��ǥ�������� ��ĭ �о
	printf("                ��������������������������������\n"); //�游��� 9 ~ 22 , 2
	printf("                ��          �游���          ��          ��������          ��\n"); //�������� 24 ~ 37 , 2
	printf("                ��������������������������������\n");
	printf("                ��                          ����                          ��\n"); //���� 9 ~ 37, 4
	printf("                ��������������������������������\n");
	printf("                ��                            ��                            ��\n"); //��1 9 ~ 22 , 6 ~ 8 
	printf("                ��                            ��                            ��\n"); //��2 24 ~ 37 , 6 ~ 8
	printf("                ��                            ��                            ��\n");
	printf("                ��������������������������������\n");
	printf("                ��                            ��                            ��\n"); //��3 9 ~ 22 , 10 ~ 12  
	printf("                ��                            ��                            ��\n"); //��4 24 ~ 37 ,10 ~ 12 
	printf("                ��                            ��                            ��\n");
	printf("                ��������������������������������\n");
	printf("                ��                            ��                            ��\n"); //��5 9 ~ 22 , 14 ~ 16
	printf("                ��                            ��                            ��\n"); //��6 24 ~ 37 , 14 ~ 16
	printf("                ��                            ��                            ��\n");
	printf("                ��������������������������������\n");

	mysql_query(cons, "select ip, name, password from catchmind.room");
	sql_result = mysql_store_result(cons);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{

		if (i % 2 == 0)
			cur(25, 6 + (i * 2));
		else
			cur(55, 6 + ((i / 2) * 4));
		printf("%s", sql_row[0]);

		/*for (short j = 0; sql_row[1][j] != 0; j++) {
			if (i % 2 == 0 && j < 10)
				cur(25 + j, 7 + (i * 2));
			else if (i % 2 == 0)
				cur(25 + j - 10, 8 + (i * 2));
			else if (j < 10)
				cur(55 + j, 7 + (i / 2) * 4);
			else
				cur(55 + j - 10, 8 + (i / 2) * 4);
			printf("%c", sql_row[1][j]);
		}*/
		if (i % 2 == 0)
			cur(25, 7 + (i * 2));
		else
			cur(55, 7 + ((i / 2) * 4));
		printf("%-7s", sql_row[1]);
		strcpy(connectroom[i].ip, sql_row[0]);
		strcpy(connectroom[i].roomname, sql_row[1]);
		strcpy(connectroom[i].password, sql_row[2]);
		i++;

	}

	i = 0;

}
int bangchose(MYSQL *cons) {

	int xx = 0, yy = 0;

	while (1) {
		banglist(cons);
		gotoxy(0, 0);
		printf("%3d %3d\n", xx, yy);

		click(&xx, &yy);

		if (9 <= xx && xx <= 22 && 2 == yy)			//�游���
			return 0;
		else if (24 <= xx && xx <= 37 && 2 == yy)		//��������
			return 1;
		else if (9 <= xx && xx <= 22 && 6 <= yy && yy <= 8)	//�� 1
			return 2;
		else if (24 <= xx && xx <= 34 && 6 <= yy && yy <= 8)	//�� 2
			return 3;
		else if (9 <= xx && xx <= 22 && 10 <= yy && yy <= 12)	//�� 3
			return 4;
		else if (24 <= xx && xx <= 34 && 10 <= yy && yy <= 12)	//�� 4
			return 5;
		else if (9 <= xx && xx <= 22 && 14 <= yy && yy <= 16)	//�� 5
			return 6;
		else if (24 <= xx && xx <= 34 && 14 <= yy && yy <= 16)	//�� 6
			return 7;

		Sleep(50);

	}

}
int chooseroom(int roomnum) {
	roomnum -= 2;
	char roompassword[30] = { 0, };
	int i = 0;
	if (connectroom[roomnum].ip[0] == 0)
		return -1;
	CLS;

	printf("��������������������������\n");
	printf("��                                              ��\n");
	printf("��              ĳġ���ε� �� ����              ��\n");
	printf("��          ���� ip :  %s            ��\n", connectroom[roomnum].ip);
	printf("��������������������������\n");
	printf("��  �� ����   ��                              ���\n");			// x = 17 y = 5
	printf("��������������������������\n");
	printf("��  Password  ��                              ���\n");
	printf("��������������������������\n");
	cur(17, 5);
	printf("%s", connectroom[roomnum].roomname);
	cur(17, 7);
	while (1) {

		if (_kbhit()) {
			roompassword[i] = _getch();
			if (roompassword[i] == 8) {
				if (i == 0) {
					roompassword[0] = 0;
					continue;
				}
				printf("\b \b");
				roompassword[i - 1] = 0;
				roompassword[i--] = 0;
			}
			else if (roompassword[i] == 13 && i > 3) {
				roompassword[i] = 0;
				break;
			}
			else if (roompassword[i] == 13) {
				roompassword[i] = 0;
			}
			else if (i >= 15) {
				continue;
			}
			else if (!((roompassword[i] >= '0' && roompassword[i] <= '9') || (roompassword[i] >= 'a' && roompassword[i] <= 'z') || (roompassword[i] >= 'A' && roompassword[i] <= 'Z'))) {
				roompassword[i] = 0;
			}
			else {
				printf("*");
				i++;
			}
		}
	}
	if (!(strcmp(connectroom[roomnum].password, roompassword)))
	{
		return 1;
	}
	else
		return 0;
}
void numberbaseball(void) {

	srand((unsigned int)time(NULL));

	int i, j, k;

	int aire[5][100] = { 0, }; //3��° : s 4��° : b 5��° ����
	int aiall[10][10][10] = { 0, };
	for (i = 0; i < 10; i++)
		for (j = 0; j < 10; j++)
			for (k = 0; k < 10; k++)
				if (i == j || j == k || i == k)
					aiall[i][j][k] = -1;
	int ainum[10] = { 0, };

	int usre[5][100] = { 0, };
	int usnum[3] = { 0, };

	int turn = 0;

	createainumber(ainum);
	scanfirst(usnum);


	while (aire[3][turn - 1] != 3) {

		scan(usre, ainum, turn);
		aiscan(aire, usnum, aiall, turn);
		aicheck(aiall, aire, turn);

		turn++;
	}



	return;

}
void createainumber(int *ainum) {

	int i;

	for (i = 0; i < 3; i++) {

		ainum[i] = rand() % 10;

		if (i == 1 && ainum[i] == ainum[0])
			ainum[i--] = 0;
		else if (i == 2 && (ainum[i] == ainum[1] || ainum[i] == ainum[0]))
			ainum[i--] = 0;
		//printf("%d", ainum[i]);
	}
	printf("\n");

}
void scanfirst(int *usnum) {
	int i;
	printf("����� ���ڸ� ������ �ּ��� : ");

	for (i = 0; i < 3; i++) {
		usnum[i] = _getch() - '0';

		if (usnum[i] < 0 || usnum[i] > 9)
			usnum[i--] = 0;
		else if (i == 1 && usnum[i] == usnum[0])
			i--;
		else if (i == 2 && (usnum[i] == usnum[1] || usnum[i] == usnum[0])) {
			i--;
		}
		else
			printf("%d", usnum[i]);

	}

	printf("\n\n    ��                    AI\n\n\n");
}
void scan(int(*usre)[100], int *ainum, int t) {

	int i, s = 0, b = 0;
	int tmp1 = 0, tmp2 = 0;

	for (i = 0; i < 3; i++) {

		usre[i][t] = _getch() - '0';

		if (usre[i][t] == 8 - '0') {
			usre[i - 1][t] = 0;
			i -= 2;
			printf("\b \b");
		}
		else if (usre[i][t] < 0 || usre[i][t] > 9)
			usre[i--][t] = 0;
		else if (i == 1 && usre[i][t] == usre[0][t])
			usre[i--][t] = 0;
		else if (i == 2 && (usre[i][t] == usre[0][t] || usre[i][t] == usre[1][t])) {
			usre[i--][t] = 0;
		}
		else
			printf("%d", usre[i][t]);

	}

	for (i = 0; i < 3; i++) {

		tmp1 = (i + 1) % 3;
		tmp2 = (i + 2) % 3;
		if (usre[i][t] == ainum[i])
			s++;
		else if (usre[i][t] == ainum[tmp1])
			b++;
		else if (usre[i][t] == ainum[tmp2])
			b++;
	}

	usre[3][t] = s;
	usre[4][t] = b;
	printf("    %ds %db        ", s, b);

}
void aiscan(int(*aire)[100], int *usnum, int(*aiall)[10][10], int t) {

	int i, s = 0, b = 0;
	int x = 0, y = 0, z = 0;
	int tmp1, tmp2;

	while (aiall[x][y][z] == -1) {
		x = y = z = 0;

		for (i = 0; i < 3; i++)
			aire[i][t] = rand() % 10;

		x = aire[0][t];
		y = aire[1][t];
		z = aire[2][t];

	}

	for (i = 0; i < 3; i++) {

		tmp1 = (i + 1) % 3;
		tmp2 = (i + 2) % 3;
		if (aire[i][t] == usnum[i])
			s++;
		else if (aire[i][t] == usnum[tmp1])
			b++;
		else if (aire[i][t] == usnum[tmp2])
			b++;
	}

	aire[3][t] = s;
	aire[4][t] = b;

	printf("%d %d %d    %ds %db\n", aire[0][t], aire[1][t], aire[2][t], s, b);
}
void aicheck(int(*aiall)[10][10], int(*aire)[100], int t) {

	int i, j, k;
	int s = 0, b = 0;
	int t0 = aire[0][t], t1 = aire[1][t], t2 = aire[2][t];

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			for (k = 0; k < 10; k++) {

				s = 0, b = 0;

				if (i == aire[0][t])
					s++;
				else if (i == aire[1][t])
					b++;
				else if (i == aire[2][t])
					b++;

				if (j == aire[1][t])
					s++;
				else if (j == aire[0][t])
					b++;
				else if (j == aire[2][t])
					b++;

				if (k == aire[2][t])
					s++;
				else if (k == aire[0][t])
					b++;
				else if (k == aire[1][t])
					b++;

				if (s != aire[3][t] || b != aire[4][t])
					aiall[i][j][k] = -1;
			}
		}
	}
}
void logintema(void) {
	printf("��������������������������\n");
	printf("��                                              ��\n");
	printf("��            ĳġ���ε� ������ �α���          ��\n");
	printf("��\n");
	printf("��������������������������\n");
	printf("��     ID     ��                    ��          ��\n");
	printf("��������������������   login  ��\n");
	printf("��  Password  ��                    ��          ��\n");
	printf("��������������������������\n");
	printf("��              ��              ��              ��\n");
	printf("�� �����ڻ���Ʈ ��   ȸ������   ��    �ʱ�ȭ    ��\n");
	printf("��              ��              ��              ��\n");
	printf("��������������������������\n");
	gotoxy(48, 3);
	printf("��");
}
void jointema(void) {
	printf("��������������������������\n");
	printf("��  ȸ������ : �г����� ���� Ư������ �ȵ˴ϴ�  ��\n");
	printf("��������������������������\n");
	printf("��    �г���  ��                    ��          ��\n");
	printf("��������������������          ��\n");
	printf("��     ID     ��                    �� ȸ������ ��\n");
	printf("��������������������          ��\n");
	printf("��  Password  ��                    ��          ��\n");
	printf("��������������������������\n");
	printf("��              ��              ��              ��\n");
	printf("�� �����ڻ���Ʈ ��    ������    ��    �ʱ�ȭ    ��\n");
	printf("��              ��              ��              ��\n");
	printf("��������������������������\n");
}
void sendall(char *message) {
	while (1) {
		send(Sconnect_sock[0], message, 40, 0);
	//	printf("Client 1 <- Server : %s\n", message);
		if (Sconnect_sock[1] != 0) {
			send(Sconnect_sock[1], message, 40, 0);
	//		printf("Client 2 <- Server : %s\n", message);
		}
		if (Sconnect_sock[2] != 0) {
			send(Sconnect_sock[2], message, 40, 0);
	//		printf("Client 3 <- Server : %s\n", message);
		}
		if (Sconnect_sock[3] != 0) {
			send(Sconnect_sock[3], message, 40, 0);
	//		printf("Client 4 <- Server : %s\n", message);
		}
		Sleep(300);
	}
}
void Clnt_1(void) {
	char message[100];
//	printf("hello\n");
	while (1) {
		if (recv(Sconnect_sock[0], message, 40, 0) > 0) {
		//	printf("Client 1 -> Server : %s\n", message);
			if (strncmp(message, "player   connect", 16) == 0) {

				message[7] = '1';
			}
			else if (strcmp(message, "player ready") == 0) {
				ZeroMemory(message, sizeof(message));
				strcpy(message, "player 1 ready");
			}
			sendall(message);
		}
	}
}
void Clnt_2(void) {
	char message[100];
//	printf("hello\n");
	while (1) {

		if (recv(Sconnect_sock[1], message, 40, 0) > 0) {
			//printf("Client 2 -> Server : %s\n", message);
			if (strncmp(message, "player   connect", 16) == 0) {

				message[7] = '2';
			}
			else if (strcmp(message, "player ready") == 0) {
				ZeroMemory(message, sizeof(message));
				strcpy(message, "player 2 ready");
			}
			sendall(message);
		}
	}
}
void Clnt_3(void) {
	char message[100];
//	printf("hello\n");
	while (1) {

		if (recv(Sconnect_sock[2], message, 40, 0) > 0) {
	//		printf("Client 3 -> Server : %s\n", message);
			if (strncmp(message, "player   connect", 16) == 0) {

				message[7] = '3';
			}
			else if (strcmp(message, "player ready") == 0) {
				ZeroMemory(message, sizeof(message));
				strcpy(message, "player 3 ready");
			}
			sendall(message);
		}
	}
}
void Clnt_4(void) {
	char message[100];
//	printf("hello\n");
	while (1) {

		if (recv(Sconnect_sock[3], message, 20, 0) > 0) {
	//		printf("Client 4 -> Server : %s\n", message);
			if (strncmp(message, "player   connect", 16) == 0) {

				message[7] = '4';
			}
			else if (strcmp(message, "player ready") == 0) {
				ZeroMemory(message, sizeof(message));
				strcpy(message, "player 4 ready");
			}
			sendall(message);
		}
	}
}
void makeroom(int *count) {
	int i = 0;
	char message[100];
	IN_ADDR serverip = GetDefaultMyIP();
	listen_sock = socket(PF_INET, SOCK_STREAM, 0); // ���� ���� �� ���Ͽ� ���� ������ listen_sock������ ����					
	if (listen_sock == INVALID_SOCKET)
		ErrorHandling("socket() error");
	printf("���� ���� �Ϸ�!\n");
	memset(&listen_addr, 0, sizeof(listen_addr)); // ������ �ּ� �ʱ�ȭ
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // ������ �ּҸ� �� �ּҷ� (�Ƹ���)
	listen_addr.sin_port = htons(5555); // ���� ��Ʈ 
	printf("�ּ� ���� �Ϸ�!\n");
	if (bind(listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR) // ���ݱ��� ������ �ּҸ� listen_sock�� bind()�� ����
		ErrorHandling("bind() error");
	printf("bind() �Ϸ�!\n");
	if (listen(listen_sock, 5) == SOCKET_ERROR)	// Ŭ���̾�Ʈ�� �����Ҷ� ���� ��ٸ�
		ErrorHandling("listen() error");
	printf("listen() �Ϸ�!\n");
	sockaddr_in_size = sizeof(connect_addr);
	*count = 1;
	while (1) {
		Sconnect_sock[i] = accept(listen_sock, (SOCKADDR*)&connect_addr, &sockaddr_in_size); // �����ϸ� accept() ����
		if (Sconnect_sock[i] != 0) {
			switch (i) {
			case 0:_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_1, NULL, 0, NULL); break;
			case 1:_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_2, NULL, 0, NULL); break;
			case 2:_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_3, NULL, 0, NULL); break;
			case 3:_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_4, NULL, 0, NULL); break;
			default: break;
			}
			i++;
		}
	}
}
IN_ADDR GetDefaultMyIP()
{

	char localhostname[MAX_PATH];
	IN_ADDR addr = { 0, };

	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR)//ȣ��Ʈ �̸� ������
	{
		return addr;
	}
	HOSTENT *ptr = gethostbyname(localhostname);//ȣ��Ʈ ��Ʈ�� ������
	while (ptr && ptr->h_name)
	{
		if (ptr->h_addrtype == PF_INET)//IPv4 �ּ� Ÿ���� ��
		{
			memcpy(&addr, ptr->h_addr_list[0], ptr->h_length);//�޸� ����
			break;//�ݺ��� Ż��
		}
		ptr++;
	}
	return addr;
}
void signalall(void)
{
	signal(SIGINT, (_crt_signal_t)exitsignal);
	signal(SIGBREAK, (_crt_signal_t)exitsignal);
	signal(SIGILL, (_crt_signal_t)exitsignal);
	signal(SIGFPE, (_crt_signal_t)exitsignal);
	signal(SIGSEGV, (_crt_signal_t)exitsignal);
}
void exitsignal(void)
{

}
void gotoxy(short x, short y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
void cur(short x, short y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}