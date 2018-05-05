//定义类型，常量
#include "Strategy.h"

//定义动作函数
#include "vsv.h"

#include <math.h>
///正常比赛
void NormalGame(Environment *env)    //RTS模式
{
	Mydata * p;
	p = (Mydata *)env->userData;
	Vector3D pos,BallPos, begin, end;
	Vector3D RoboPos[5];
	static int flag = 1;
	int i,id,num, count;
	double alfa;
	if (flag == 1) {
		PredictBall(env, 2);
		Kick(env, 4, p->preball);
		flag++;
	}
	Goalie(env, 0);

	if (p->curball.x >= 41 && p->curball.x < 60)
	{
		BallPos = p->curball;
		for(i=0;i<5;i++)

		{
			RoboPos[i].x = p->robot[i].pos.x;
			RoboPos[i].y = p->robot[i].pos.y;
			RoboPos[i].z= p->robot[i].pos.z;
		}
		id=FindNearestRobottoBall(env,RoboPos, BallPos);
		num=FindNexttoRobot(env, id, RoboPos, p->robot[id].pos);
		Kick(env, id, p->robot[num].pos);

	}
		
	else if (p->curball.x >= 60 && p->curball.x<78.6) {
		if (p->curball.y<27.8) {
			pos.x = p->curball.x-2;
			pos.y = p->curball.y;
		}
		else if (p->curball.y <= 58.5) {
			PredictBall(env, 2);
			pos.x = p->preball.x-2;
			pos.y = p->preball.y-2;
		}
		else {
			pos.x = p->preball.x;
			pos.y = p->preball.y;
		}
		PositionAndStop(env, 4, pos);
	}
	else  if (p->curball.x>78.6 &&
		((p->curball.y<34 && p->curball.y>6.3) || (p->curball.y>48 && p->curball.y<77.2))) {
		Order(env);
		count = 120;
		while (count>0) {
			Kick(env, 4, p->slowrobot);//后期可添加防守
			count--;
		}
	}
	//根据球所在分区设置策略
	BallAreaLook(env);
}

void BallAreaLook(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	Vector3D pos, BallPos, begin, end;
	Vector3D RoboPos;
	static int flag = 1;
	int i, id, num, count;
	double alfa;
	switch (p->ballArea) {

	case 1:
		Order(env);
		RoboPos.x = 12.0;
		RoboPos.y = 32.2;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos.x = 30.0;
		RoboPos.y = 41.7;
		PositionAndStop(env, 2, RoboPos);
		RoboPos = p->curball;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.x = 22.0;
		RoboPos.y = 25.8;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 45.3;
		RoboPos.y = 41.7;
		PositionAndStop(env, 4, RoboPos);
		break;
	case 2:
		Order(env);
		RoboPos.x = 12.0;
		RoboPos.y = 32.2;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 17.5;
		RoboPos.y = 40.5;
		PositionAndStop(env, 2, RoboPos);
		RoboPos.x = 23.1;
		RoboPos.y = 26.2;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.x = 38.1;
		RoboPos.y = 41.2;
		PositionAndStop(env, 4, RoboPos);
		break;

	case 3:
		Order(env);
		RoboPos.x = 7.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.y = p->curball.y;
		RoboPos.x = 17.1;
		PositionAndStop(env, 2, RoboPos);
		RoboPos.x = 17.3;
		RoboPos.y = 36.4;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.x = 38.1;
		RoboPos.y = 41.2;
		PositionAndStop(env, 4, RoboPos);
		break;
	case 4:
		Order(env);
		RoboPos.x = 7.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.y = p->curball.y;
		RoboPos.x = 17.1;
		PositionAndStop(env, 2, RoboPos);
		RoboPos.x = 40.1;
		RoboPos.y = 35.4;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.x = 30.1;
		RoboPos.y = 42.2;
		PositionAndStop(env, 4, RoboPos);
		break;
	case 5:
		Order(env);
		RoboPos.x =9.2;
		RoboPos.y = 33.2;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.y =25.0;
		RoboPos.x = 21.1;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 23.0;
		RoboPos.y = 26.4;
		PositionAndStop(env, 2, RoboPos);
		RoboPos.x = 47.2;
		RoboPos.y = 24.0;
		PositionAndStop(env, 4, RoboPos);
		break;
	case 6:
		Order(env);
		RoboPos.x = 12.0;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.y = 41.1;
		RoboPos.x = 32.2;
		PositionAndStop(env, 2, RoboPos);
		RoboPos.x = 21.0;
		RoboPos.y = 34.4;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 50.2;
		RoboPos.y = 42.2;
		PositionAndStop(env, 4, RoboPos);

		break;
	case 7:
		Order(env);
		RoboPos.x = 11.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 2, RoboPos);
		RoboPos.y = 47.0;
		RoboPos.x = 20.0;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 22.0;
		RoboPos.y = 40.4;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.x = 40.0;
		RoboPos.y = 41.1;
		PositionAndStop(env, 4, RoboPos);

		break;
	case 8:
		Order(env);
		RoboPos.x =12.3;
		RoboPos.y = 49.1;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 2, RoboPos);
		RoboPos.y = 21.1;
		RoboPos.x = 55.0;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 23.1;
		RoboPos.y = 51.4;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.x = 42.1;
		RoboPos.y = 41.2;
		PositionAndStop(env, 4, RoboPos);
		break;
	case 9:
		Order(env);
		RoboPos.x = 12.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 3, RoboPos);
		RoboPos.x = p->robot[3].pos.x + 3;
		RoboPos.y = p->robot[3].pos.y - 3;
		PositionAndStop(env, 4,RoboPos );																										
		RoboPos.x = 63.0;
		RoboPos.y = 41.2;
		//
		PositionAndStop(env, 2, RoboPos);
		Kick(env, 3, 2);
		RoboPos.x = 40.3;
		RoboPos.y = 25.2;
		PositionAndStop(env, 3, RoboPos);
		shoot(env, 2);
		RoboPos.x = 41.3;
		RoboPos.y = 41.2;
		PositionAndStop(env, 1, RoboPos);
		break;
	case 10:
		Order(env);
		RoboPos.x = 12.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 4, RoboPos);
		PositionAndStop(env, 3, p->robot[4].pos);
		shoot(env, 4);
		RoboPos.y = 60.1;
		RoboPos.x =41.0;
		PositionAndStop(env, 2, RoboPos);
		if (dist(env, 2) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 2, RoboPos);
			shoot(env, 2);
		}
		RoboPos.x = 42.1;
		RoboPos.y = 41.2;
		PositionAndStop(env, 1, RoboPos);
		break;
	case 11:
		Order(env);
		RoboPos.x = 12.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 4, RoboPos);
		PositionAndStop(env, 2, p->robot[4].pos);
		shoot(env, 4);
		RoboPos.y = 60.1;
		RoboPos.x = 41.0;
		PositionAndStop(env, 3, RoboPos);
		if (dist(env, 3) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 3, RoboPos);
			shoot(env, 3);
		}
		RoboPos.x = 42.1;
		RoboPos.y = 41.2;
		PositionAndStop(env, 1, RoboPos);
		break;
	case 12:
		Order(env);
		RoboPos.x = 12.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 2, RoboPos);
		PositionAndStop(env, 4, p->robot[3].pos);
		RoboPos.x = 63.0;
		RoboPos.y = 41.2;
		//
		PositionAndStop(env, 3, RoboPos);
		Kick(env, 2, 3);
		RoboPos.x = 40.3;
		RoboPos.y = 25.2;
		PositionAndStop(env, 2, RoboPos);
		shoot(env, 3);
		RoboPos.x = 41.3;
		RoboPos.y = 41.2;
		PositionAndStop(env, 1, RoboPos);
		break;
	case 13:
		Order(env);
		RoboPos.x = 11.3;
		RoboPos.y =p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 3, RoboPos);
		PositionAndStop(env, 4, p->robot[3].pos);
		RoboPos.x = 78.1;
		RoboPos.y = 39.0;
		PositionAndStop(env, 2,RoboPos);
		Kick(env, 3, 2);
		shoot(env, 2);
		RoboPos.y = 40.0;
		RoboPos.x = 49.0;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.y = 24.0;
		RoboPos.x = 50.0;
		PositionAndStop(env, 3, RoboPos);
		if (dist(env, 4) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 4, RoboPos);
			shoot(env, 4);
		}
		if (dist(env, 2) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 2, RoboPos);
			shoot(env, 2);
		break;
	case 14:
		Order(env);
		RoboPos.x = 12.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 4, RoboPos);
		PositionAndStop(env, 3, p->robot[4].pos);
		shoot(env, 4);
		RoboPos.y = 38.1;
		RoboPos.x = 50.0;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 60.1;
		RoboPos.y = 40.2;
		PositionAndStop(env, 2, RoboPos);
		if (dist(env, 3) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 3, RoboPos);
			shoot(env, 3);
		}
		
		break;
	case 15:
		Order(env);
		RoboPos.x = 12.3;
		RoboPos.y = p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 4, RoboPos);
		PositionAndStop(env, 2, p->robot[4].pos);
		shoot(env, 4);
		RoboPos.y = 48.1;
		RoboPos.x = 49.1;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.x = 52.1;
		RoboPos.y = 35.2;
		PositionAndStop(env, 3, RoboPos);
		if (dist(env, 2) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 2, RoboPos);
			shoot(env, 2);
		}
		break;
	case 16:
		Order(env);
		RoboPos.x = 11.3;
		RoboPos.y =p->curball.y;
		PositionAndStop(env, 0, RoboPos);//门将
		RoboPos = p->curball;
		PositionAndStop(env, 2, RoboPos);
		PositionAndStop(env, 4, p->robot[3].pos);
		RoboPos.x = 78.1;
		RoboPos.y = 38.0;
		PositionAndStop(env, 3,RoboPos);
		Kick(env, 2, 3);
		shoot(env, 3);
		RoboPos.y = 35.1;
		RoboPos.x = 49.0;
		PositionAndStop(env, 1, RoboPos);
		RoboPos.y = 56.0;
		RoboPos.x = 49.0;
		PositionAndStop(env, 2, RoboPos);
		if (dist(env, 4) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 4, RoboPos);
			shoot(env, 4);
		}
		if (dist(env, 3) < 3)
		{
			RoboPos = p->curball;
			PositionAndStop(env, 3, RoboPos);
			shoot(env, 3);
		}
		break;
	}
}
}
//争球
void FreeBallGame(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	PositionAndStop(env, 2, p->curball);
	NormalGame(env);
}

//点球
void PenaltyBallGame(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	shoot(env, 4);
	NormalGame(env);   //若重写此函数请删除此内容
}

//任意球
void FreeKickGame(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	shoot(env, 4);
	NormalGame(env);   //若重写此函数请删除此内容
}

//门球
void GoalKickGame(Environment *env, int goalcount)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	Kick(env, 0, 4);
	NormalGame(env);   //若重写此函数请删除此内容
}

//防点球
void PenaltyGoalie(Environment *env, Robot *robot)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	Vector3D RoboPos;
	RoboPos.x = 86.5;
	RoboPos.y = p->curball.y;
	PositionAndStop(env, 0, RoboPos);
	if (dist(env, 0) < 3)
	{
		RoboPos = p->curball;
		PositionAndStop(env, 0, RoboPos);
		Kick(env, 0,4);
	}
	NormalGame(env);   //若重写此函数请删除此内容
}

//防任意球
void FreeGoalie(Environment* env)
{
	Mydata* p;
	p = (Mydata*)env->userData;
	Vector3D RoboPos;
	RoboPos.x = 86.5;
	RoboPos.y = p->curball.y;
	if (dist(env, 0) < 3)
	{
		RoboPos = p->curball;
		PositionAndStop(env, 0, RoboPos);
		Kick(env, 0,4);
	}
	NormalGame(env);   //若重写此函数请删除此内容
}

//中场开球
void PlaceBallGame(Environment *env, int &placecount)
{
	Mydata* p;
	p = (Mydata*)env->userData;
	Vector3D RoboPos;
	PositionAndStop(env, 4, p->curball);
	Kick(env, 4, 2);
	NormalGame(env);   //若重写此函数请删除此内容
}

//门将也不是很强 建议重写
void Goalie(Environment *env, int robot)
{//先校正姿态，再去拦球
	Mydata * p;
	p = (Mydata *)env->userData;

	Vector3D go;

	double OX = FLEFT - (GTOP - GBOT);	// 该点为球门中心 向后移动半个球门
	double OY = (FTOP + FBOT) / 2;			//球门中心	
	double ballx = p->curball.x;
	double bally = p->curball.y;
	double gx = p->robot[robot].pos.x;
	double gx_outline = FLEFT + 2.2;		//对x坐标的限定，防止离球门线太远了
	double gx_inline = FLEFT + 1.8;
	double gy = p->robot[robot].pos.y;		//跑位点,初值为当前位置
	double goalline = FLEFT + 3;
	bool   notout = true;	//不出击
	bool   standby = true;	//限制x 坐标
	bool   XX = false;	//是否旋转

	if (XX && ballx < gx + 0.5)
	{//旋转
		if (bally < gy && bally > gy - 4)
		{
			Velocity(env, robot, -125, 125);
			return;
		}
		else if (bally > gy && bally < gy + 4)
		{
			Velocity(env, robot, 125, -125);
			return;
		}
	}

	gy = OY + (goalline - OX) * (bally - OY) / (ballx - OX);

	if (notout)
	{
		if (gy > GTOP + 3)
			gy = GTOP + 3;
		else if (gy < GBOT - 3)
			gy = GBOT - 3;
	}
	go.y = gy;

	if (standby)
	{
		if (gx > gx_outline)
			gx = gx_outline;
		else if (gx < gx_inline)
			gx = gx_inline;
	}

	go.x = gx;
	GoaliePosition(env, robot, go, 90, 1.5);

}
int FindNearestRobottoBall(Environment *env, Vector3D RobotLoc[5], Vector3D BallLoc)  //搜索距球最近的机器人，返回其标号
{
	int robot;
	Mydata * p;
	p = (Mydata *)env->userData;
	double dist_x,dist_y,dist[5];
	double mindist;
	int id;
	for(robot=0;robot<5;robot++)
	{
		dist_x = RobotLoc[robot].x - BallLoc.x;
		dist_y = RobotLoc[robot].y - BallLoc.y;
		dist[robot] = sqrt(dist_x *dist_x + dist_y * dist_y);
	}
	mindist = dist[0];
	for (robot = 0; robot < 5; robot++)
	{
		if (dist[robot] < mindist)
		{
			mindist = dist[robot];
			id = robot;
		}
	}
	return id;
}

int FindNexttoRobot(Environment *env, int targrobot, Vector3D RobotPosition[4], Vector3D TargPosition) //查找距目标机器人最近的另一个机器人
{
	double dist_x, dist_y, dist[5];
	double mindist;
	int robot,num;
	for (robot = 0; robot<5; robot++)
	{
		dist_x = RobotPosition[robot].x - TargPosition.x;
		dist_y = RobotPosition[robot].y - TargPosition.y;
		dist[robot] = sqrt(dist_x *dist_x + dist_y * dist_y);
	}
	mindist = dist[0];
	for (robot = 0; robot < 5; robot++)
	{
		if ((dist[robot] < mindist)&&dist[robot]!=0)
		{
			mindist = dist[robot];
			num = robot;
		}
	}
	return num;
}
double dist(Environment *env, int robot)//计算机器人与球的距离
{
	Mydata * p;
	p = (Mydata *)env->userData;
	Vector3D ball, probot;
	double distx, disty;
	double dist;
	ball = p->curball;
	probot = p->robot[robot].pos;
	distx = ball.x - probot.x;
	disty = ball.y - probot.y;
	dist = sqrt(distx*distx + disty * disty);
	return dist;
}