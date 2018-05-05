//宏定义是动态连接库
#define STRATEGY_API __declspec(dllexport)

#include "string.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <time.h>
//#include <Afxwin.h>

//关于一些方向上的定义
#define UP 0
#define RIGHT_UP 1
#define RIGHT 2
#define RIGHT_DOWN 3
#define DOWN 4
#define LEFT_DOWN 5
#define LEFT 6
#define LEFT_UP 7
#define RIGHTU 8 //右边线的上半部分
#define RIGHTD 9//右边线的下半部分
#define LEFTU 10//左边线的上半部分
#define LEFTD 11//左边线的上半部分


const long PLAYERS_PER_SIDE = 5;//每队有多少球员

//数据的精确度只有	0.1//真的么。。。我怎么那么不信那。。。。dyt
// gameState 游戏状态
const long FREE_BALL = 1;
const long PLACE_KICK = 2;
const long PENALTY_KICK = 3;
const long FREE_KICK = 4;
const long GOAL_KICK = 5;



// 球场边界
const double FTOP = 77.2392;				
const double FBOT = 6.3730;
const double FRIGHT = 93.4259;
const double FLEFT = 6.7118;
const double FRIGHTX = 93.4259;
const double FLEFTX = 6.8118;
const double CORRECTX = 0;			//修正  坐标转换时的  偏差//原先有一个数 2点多和6点多 但是改了以后不对 0的时候反而还好
const double CORRECTY = 0;			//感觉应该有一个特别的修正的数字。。。dyt

//左球门
const double GTOP = 49.6801;				
const double GBOT = 33.9320;				
const double GTOPY = 49.6801;				
const double GBOTY = 33.9320;				
const double GRIGHT = 85.3632;
const double GLEFT = 15.8748;
//右球门
const double GBRIGHT = 97.3632;			
const double GBLEFT = 2.8748;
const double GBTOP = 49.652424;
const double GBBOT = 33.517822;
//禁区
const double RIGHTG = 79.339401;				
const double LEFTG = 21.299461;
const double BOTG = 25.875967;
const double TOPG = 57.212448;
//自由球点
const double FREELEFT = 29.262907;			
const double FREERIGHT = 71.798508;
const double FREETOP = 64.454193;
const double FREEBOT = 18.184305;
//点球点
const double PENALTYRIGHT = 78.329132;		
const double PENALTYLEFT = 22.216028;

const long CAR = 3;							//小车边长 2.95

//角落为 边长 为5 的等腰三角形

//球能到达的边长范围

//四个角的数值
	const double RLEFT = 8.44;
	const double RRIGHT = 91.78;
	const double RTOP = 75.59;
	const double RBOT = 8.04;

	const double RRIGHTGRIT = 95.75;
	const double RRIGHTGLFT = 85.88;
	const double RLEFTGLFT = 4.52;
	const double RLEFTGRIT = 15.27;

	const double RGTOP = 53.05;
	const double RGBOT = 30.44;

	const double RJINQUTOP = 58.67;
	const double RLEFTJINQURIT = 22.89;
	const double RRIGHTJINQULFT = 77.92;
	const double RJINQUBOT = 24.33;

	const double CORNERLEFT = 13.13;
	const double CORNERRIT = 87.76;
	const double CORNERTOP = 71.24;
	const double CORNERBOT = 12.85;

	const double ROBOTWITH = 3.14;
	const double BALLWITH = 1.5;
	//球能到大的范围
	const double BLEFT = 7.77;
	const double BRIGHT = 92.46;
	const double BTOP = 76.29;
	const double BBOT = 7.35;
	

	//好像是球反弹时的参数。没太看懂 逻辑好像有点问题 by董一廷
	const double SPEED_TANGENT = 0.81;
	const double SPEED_NORMAL = 0.27;

	const double SPEED_A = 0.060;
	const double SPEED_B = 0.015222305;


	const double ANGLE_A = 0.273575;
	const double ANGLE_B = 0.534262;
	const double ANGLE_K = 0.000294678;





//以下参数 用在分区上 

const double X1= 7;		
const double X2= 21;
const double X3= 51.0;
const double X4= 80.0;
const double X5= 97.0;
const double X6= 0.0;

const double Y1= 6;
const double Y2= 25.0;
const double Y3= 40.0;
const double Y4= 55.0;
const double Y5= 77.0;
const double Y6= 0.0;
//分区参数


typedef struct
{
	double x, y, z;
} Vector3D;

typedef struct
{
	long left, right, top, bottom;
} Bounds;

typedef struct
{
	Vector3D pos;
	double rotation;
	double velocityLeft, velocityRight;
} Robot;

typedef struct
{
	Vector3D pos;
	double rotation;
} OpponentRobot;

typedef struct
{
	Vector3D pos;
} Ball;

typedef struct
{
	Robot home[PLAYERS_PER_SIDE];
	OpponentRobot opponent[PLAYERS_PER_SIDE];
	Ball currentBall, lastBall, predictedBall;
	Bounds fieldBounds, goalBounds;
	long gameState;	//0,1,2,3,4,5
	long whoseBall; 
	void *userData;
} Environment;

typedef struct
{
	Robot robot[PLAYERS_PER_SIDE];			// 我方 球员
	OpponentRobot opp[PLAYERS_PER_SIDE];	// 对方 球员

	Vector3D myoldpos[PLAYERS_PER_SIDE];	//记录 我方 队员的旧坐标,方向
	Vector3D myspeed[PLAYERS_PER_SIDE];		//纪录 我方 队员的速度，跑位的方向
	Vector3D myoldvelocity[PLAYERS_PER_SIDE];		//纪录 我方 队员上次驱动力

	Vector3D opoldpos[PLAYERS_PER_SIDE];	//记录 对方 队员的旧坐标,方向
	Vector3D opspeed[PLAYERS_PER_SIDE];		//纪录 对方 队员的速度，跑位的方向

	
	Vector3D oldball;							//纪录球 过去 的坐标
	Vector3D curball;							//纪录球 现在 的坐标
	Vector3D preball;							//纪录球 预测 的坐标
	Vector3D ballspeed;							//纪录球 速度
	double BallFullSpeed;


	int mainrobot;	//主发						//发对方点球时的几个队员
	int cutrobot;	//包抄
	int slowrobot;	//轻踢
	int defentrobot;//
	//这四个没什么用 
	


   	bool mygrand;							//判断我方场地
	bool locked;							//判断场地的锁
	int ballArea;							//判断球所在分区 专用函数int checkball();
	long gameState;	//0,1,2,3,4,5			//判断比赛状态 


	//用于点球
	int penaltycount;
	int initpenaltycount;
	bool penaltyangle;

}Mydata;


extern "C" STRATEGY_API void Create ( Environment *env ); 
extern "C" STRATEGY_API void Strategy ( Environment *env );
extern "C" STRATEGY_API void Destroy ( Environment *env ); 


//以下
const	double InlitializeMyPosition_X[5]={
		10.6663,
		19.9199,
		19.7433,
		39.4618,
		39.8876
	};
const	double InlitializeMyPosition_Y[5]={
		42.3077,
		60.3647,
		22.9943,
		60.3031,
		23.1065
	};
const	double InlitializeMyPosition_Z[5]={
		90,
		0,
		0,
		0,
		0
	};
const	double InlitializeOppPosition_X[5]={
		90.4616,
		82.0921,
		81.2890,
		61.8525,
		61.4469
	};
const	double InlitializeOppPosition_Y[5]={
		42.2002,
		22.9046,
		60.4876,
		23.1527,
		60.3599
	};
const	double InlitializeOppPosition_Z[5]={
		-90,
		180,
		180,
		180,
		180
	};
const double PI = 3.1415926;

const Vector3D CONSTGATE={FRIGHT,(FTOP+FBOT)/2,0};		
const Vector3D TOPGATE={93,72,0};
const Vector3D BOTGATE={93,9,0};

void NormalGame(Environment *env);
void FreeBallGame(Environment *env);
void PlaceBallGame(Environment *env, int &placecount);
void PenaltyBallGame(Environment *env);
void FreeKickGame(Environment *env);
void GoalKickGame(Environment *env, int goalcount);
void FreeGoalie(Environment* env);
void BallAreaLook(Environment *env);
int FindNexttoRobot(Environment *env, int targrobot, Vector3D RobotPosition[4], Vector3D TargPosition);
int FindNearestRobottoBall(Environment *env, Vector3D RobotLoc[5], Vector3D BallLoc);
double dist(Environment *env, int robot);