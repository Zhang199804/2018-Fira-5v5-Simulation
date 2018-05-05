#include "strategy.h"
#include "vsv.h"
#include"iostream"
#include"tchar.h"

//不用理会 dllmain dll函数
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

//只是如果你在策略之间如果有数据需要赋初值可以在Create 中进行，可以不理会
extern "C" STRATEGY_API void Create(Environment *env)
{
	//初始化数据
	env->userData = (void*) new Mydata;
	
	Mydata* p;
	p = (Mydata*)env->userData;


	//初始化机器人信息
	for (int i = 0; i < 5; i++)
	{
	//我方--------------------------------------------------------------------------------------------------

		//我方现在的位置初始化
		p->robot[i].pos.x = InlitializeMyPosition_X[i];
		p->robot[i].pos.y = InlitializeMyPosition_Y[i];
		p->robot[i].pos.z = InlitializeMyPosition_Z[i];			//Z方向好像没用
		//我方上一帧的位置初始化
		p->myoldpos[i].x = InlitializeMyPosition_X[i];
		p->myoldpos[i].y = InlitializeMyPosition_Y[i];
		p->myoldpos[i].z = InlitializeMyPosition_Z[i];			//Z方向是上一帧小车所朝向的方向
		//我方通过上一帧与现在的位置计算出的速度
		p->myspeed[i].x = 0;
		p->myspeed[i].y = 0;
		p->myspeed[i].z = 0;
		//我方上一帧所提交的车轮驱动力
		p->myoldvelocity[i].x = 0;
		p->myoldvelocity[i].y = 0;
		p->myoldvelocity[i].z = 0;


	//对方--------------------------------------------------------------------------------------------------

		//对方现在的位置初始化
		p->opp[i].pos.x = InlitializeOppPosition_X[i];
		p->opp[i].pos.y = InlitializeOppPosition_Y[i];
		p->opp[i].pos.z = InlitializeOppPosition_Z[i];			//Z方向好像没用

		//对方上一帧的位置初始化
		p->opoldpos[i].x = InlitializeOppPosition_X[i];
		p->opoldpos[i].y = InlitializeOppPosition_Y[i];
		p->opoldpos[i].z = InlitializeOppPosition_Z[i];			//Z方向是上一帧小车所朝向的方向
	
		//对方通过上一帧与现在的位置计算出的速度
		p->opspeed[0].x = 0;
		p->opspeed[0].y = 0;
		p->opspeed[0].z = 0;

	}
	PlayerInit(env,1 );  //设置阵型初始位置，根据阵型选择。函数原型：void PlayerInit(Environment *env,int Method)
	p->locked = false;				// 场地锁初始化
	p->mygrand = true;				// 场地赋初值 假设是黄队 之后会变更

	//小球--------------------------------------------------------------------------------------------------

	//小球上一帧的位置初始化
	p->oldball.x = (FLEFT + FRIGHT) / 2.0;
	p->oldball.y = (FTOP + FBOT) / 2.0;
	p->oldball.z = 0;                                           
	
    //小球现在的位置初始化
	p->curball.x = (FLEFT + FRIGHT) / 2.0;
	p->curball.y = (FTOP + FBOT) / 2.0;
	p->curball.z = 0;											
	
    //小球通过球速预测以后出现的位置
	p->preball.x = (FLEFT + FRIGHT) / 2.0;
	p->preball.y = (FTOP + FBOT) / 2.0;
	p->preball.z = 0;										//z轴复用为小车及球的速度方向信息存储	
	
	//小球通过上一帧与现在计算出的球速
	p->ballspeed.x = 0;
	p->ballspeed.y = 0;
	p->ballspeed.z = 0;											

	p->penaltycount=0;
	p->initpenaltycount=0;
	p->penaltyangle = false;

	//严禁修改
	HWND hwnd = GetActiveWindow();		            //获取当前程序句柄
	SetWindowTextA(hwnd, "bjut2018fira5v5仿真校赛");//更改窗口标题为“”

}

/*Stategy  策略的入口，该函数的形式不能修改，否则会出错
参数 Environment *env 用来获取当前的比赛信息。
并且通过修改 Environment.home.velocityLeft, Environment.home.velocityRight的值来控制机器人的运动
具体工作由
	See(env);			// 预处理
	Action(env);		//策略
	End ( env );		//后期处理
	分担了
*/
extern "C" STRATEGY_API void Strategy ( Environment *env )
{
	Mydata * p;
	p=(Mydata *)env->userData;

	if(!p->locked)		//判断场地的锁
	{//确定区域
		if( env->home[0].pos.x < 50.0 )
			p->mygrand=true; // 是黄队
		else
			p->mygrand=false;//是蓝队
		p->locked=true;//锁住
	}
			// 预处理	
	See(env);

			 //策略
	Action(env);
	
			 //后期处理
	End ( env );		
}

//比赛结束时销毁数据 处理等 不必理会
extern "C" STRATEGY_API void Destroy(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;

	if (env->userData != NULL)	delete (Mydata *)env->userData;
}


//See函数每一帧都执行 进行每一帧预处理计算 进行坐标变换 求出一些策略中经常用到的量，如每个队员的速率和角速度等
void See ( Environment *env )
{
	Mydata * p;
	p=(Mydata *)env->userData;

	p->ballArea = CheckBall(env);//检查场地
	
//处理位置----------------------------------------------------------------------------------------------------
	int i = 0;
	if(p->mygrand)//如果，我方是黄队的话
	{

		p->gameState = env->gameState ;				//从系统接收游戏状态
		p->curball.x = env->currentBall.pos.x;		//从系统接收小球坐标
		p->curball.y = env->currentBall.pos.y;

		//从系统接收小车现在坐标与角度
		for(i=0;i<5;i++)
		{
			//我方
			p->robot[i].pos.x = env->home[i].pos.x ;	
			p->robot[i].pos.y = env->home[i].pos.y ;
			p->robot[i].rotation= env->home[i].rotation;
			RegulateAngle(p->opp[i].rotation);//规范角度 
			//对方
			p->opp[i].pos.x =env->opponent[i].pos.x;	
			p->opp[i].pos.y =env->opponent[i].pos.y;
			p->opp[i].rotation =env->opponent[i].rotation;
			RegulateAngle(p->opp[i].rotation);//规范角度 

		}
	}//原坐标不变，这是符合左边（黄队）情况的。

	else//如果是蓝队
	{
		p->gameState = env->gameState;				//从系统接收游戏状态

		
		//从系统接收小球坐标 因为蓝队在右场 需要特殊的转换 转换以后 策略只需要按照黄队写 蓝黄通用
		
		p->curball.x =FLEFT+FRIGHT + CORRECTX - env->currentBall.pos.x;		//球坐标变化
		p->curball.y =FBOT+FTOP + CORRECTY - env->currentBall.pos.y;
		//不必有Z轴

		
		//小车同理
		for(i=0;i<5;i++)
		{
			//我方
			p->robot[i].pos.x =FLEFT+FRIGHT + CORRECTX - env->home[i].pos.x ;	//我方队员坐标变换
			p->robot[i].pos.y =FBOT+FTOP + CORRECTY - env->home[i].pos.y ;
			p->robot[i].rotation= 180.0 + env->home[i].rotation;
			RegulateAngle(p->robot[i].rotation);
			//对方
			p->opp[i].pos.x = FLEFT+FRIGHT + CORRECTX- env->opponent[i].pos.x;	//对方坐标变换
			p->opp[i].pos.y = FBOT+FTOP + CORRECTY - env->opponent[i].pos.y;
			p->opp[i].rotation =  180 + env->opponent[i].rotation;
			RegulateAngle(p->robot[i].rotation);

			//可能的话可以计算小车的角速度（捂脸）


		}
	}

//计算速度----------------------------------------------------------------------------------------------------
	
	//小车速度
	for(i=0;i<5;i++)
	{
		//我方
		p->myspeed[i].x = ( p->robot[i].pos.x - p->myoldpos[i].x);	
		p->myspeed[i].y = ( p->robot[i].pos.y - p->myoldpos[i].y);
		p->myspeed[i].z = Atan(p->myspeed[i].y,p->myspeed[i].x);	//小车运动方向
		//对方
		p->opspeed[i].x = ( p->opp[i].pos.x - p->opoldpos[i].x);
		p->opspeed[i].y = ( p->opp[i].pos.y - p->opoldpos[i].y);
		p->opspeed[i].z = Atan(p->opspeed[i].y,p->opspeed[i].x);	//小车运动方向

		
	}


	//小球速度
	p->ballspeed.x = p->curball.x - p->oldball.x;
	p->ballspeed.y = p->curball.y - p->oldball.y;
	p->ballspeed.z = Atan( p->ballspeed.y , p->ballspeed.x );   //小球运动方向

	//=================================================================================================
	do {
		//向后预测一帧。

		//我也不知道为什么要向后预测，把它删掉以后小车会乱抖 谁要是能把它改好就改。。。by 董一廷
		double v, a, b, c, omiga, angle;
		for (i = 0; i < 5; i++)
		{

			omiga = p->robot[i].rotation - p->myoldpos[i].z;
			RegulateAngle(omiga);
			omiga = AngleOne(omiga, p->myoldvelocity[i].x, p->myoldvelocity[i].y);
			c = p->robot[i].rotation;
			p->robot[i].rotation += omiga;
			RegulateAngle(p->robot[i].rotation);

			v = sqrt((p->myspeed[i].x * p->myspeed[i].x) + (p->myspeed[i].y * p->myspeed[i].y));
			angle = p->robot[i].rotation - p->myspeed[i].z;
			RegulateAngle(angle);
			if (angle > -90 && angle < 90)
				v = v;
			else
				v = -v;

			v = VelocityOne(v, p->myoldvelocity[i].x, p->myoldvelocity[i].y);
			a = p->robot[i].pos.x;
			b = p->robot[i].pos.y;

			p->robot[i].pos.x += v*cos(p->robot[i].rotation * PI / 180);
			p->robot[i].pos.y += v*sin(p->robot[i].rotation * PI / 180);
			///处理撞墙
			//不处理最好

			////处理撞墙		
			p->myoldpos[i].x = a;
			p->myoldpos[i].y = b;
			p->myoldpos[i].z = c;

			p->myspeed[i].x = (p->robot[i].pos.x - p->myoldpos[i].x);
			p->myspeed[i].y = (p->robot[i].pos.y - p->myoldpos[i].y);
			p->myspeed[i].z = Atan(p->myspeed[i].y, p->myspeed[i].x);
		}

		double x, y;
		x = p->curball.x;
		y = p->curball.y;


		PredictBall(env);		//求到现在球的位置
		p->curball = p->preball;

		p->oldball.x = x;
		p->oldball.y = y;

		PredictBall(env);		//预测下一步球的位置





		p->ballspeed.x = p->curball.x - p->oldball.x;
		p->ballspeed.y = p->curball.y - p->oldball.y;
		p->ballspeed.z = Atan(p->ballspeed.y, p->ballspeed.x);

	

	} while (0);
}


//Action函数每一帧都执行 对不同的GameState 进入不同游戏状态
void Action(Environment *env)			
{
	Mydata * p;
	p = (Mydata *)env->userData;

	static bool ourpenalty = false;			//点球
	static bool Freekick = false;			//任意球
	static bool Freeball = false;			//争球
	static bool Placeball = false;			//中场开球
	static bool Goalkick = false;			//门球



	static clock_t count = 0;	//用于排除第一帧“抽风”的问题 加一个定时器//宋和田
	static clock_t start = count;

	static bool choosepenalty = false;
	static bool panel1 = false, panel2 = false, panel3 = false, panel4 = false;

	static int goalcount = 0;
	static int freecount = 0;
	static int placecount = 0;
	static int penaltycount = 0;
	switch (p->gameState)				//各状态判定目前都是自己方控球 比如对面点球的时候就没有判定了 直接是normalgame
	{
	case FREE_KICK:
		//简单阐述原理 时间为动量 如果切换gamestate的话必然需要花费时间去摆位置 每一帧循环的时候都对start赋个时
		//如果这帧的start比上帧的start （在最后让count=start-5,其实就是让count=start）大于5000毫秒，也就是5秒（5可改），那么我们可以说这是一个“新的开始”
		//即点球是点球 任意球是任意球 门球是门球 因为该软件是不按按钮不改状态 所以你点球踢飞后仍然是点球状态，但是你要去执行“Normalgame”
		//备注：1s执行60次 也就是1s有60帧  sht
		start = clock();
		if (start - count >= 5000 || 0 == count)
		{
			See(env);   //规避第一帧数据异常 
			count = start;
			freecount = 0;//选择点球手的开关开启
		}
		count = start - 5;  //5是随便写的

		if (
			p->curball.x < 78 + 3 && p->curball.x>78 - 3 && p->curball.y > 41 - 3 && p->curball.y < 41 + 3
			&& p->robot[1].pos.x < p->curball.x &&p->robot[2].pos.x < p->curball.x &&p->robot[3].pos.x < p->curball.x &&p->robot[4].pos.x < p->curball.x
			&&freecount == 0
			)// 球在点球点 1-4号车小于球的坐标
		{
			Freekick = true;
		}
		if (freecount>30)//判定退出条件			大于80帧		
		{
			Freekick = false;
		}
		if (Freekick)
		{
			FreeKickGame(env);
			freecount++;
		}
		else if (!Freekick)
			NormalGame(env);
		break;




	case FREE_BALL:	// Free ball tm这是争球
		start = clock();
		if (start - count >= 5000 || 0 == count)
		{
			See(env);   //规避第一帧数据异常 
			count = start;
		}
		count = start - 5;  //5是随便写的
		if (p->curball.x > 71 - 3 && p->curball.x < 71 + 3)//限制坐标x							//1 2 在左边半场，4号在争球区域的下半场   
		{//右方
			if (p->curball.y > 65 - 3 && p->curball.y < 65 + 3)//限制坐标y
			{//右上方开球点
				if ((p->robot[3].pos.x > 60 - 3 && p->robot[3].pos.x < 60 + 3)
					&& (p->robot[3].pos.y > 65 - 3 && p->robot[3].pos.y < 65 + 3)
					&& p->robot[1].pos.x < 50.1158	&&	p->robot[2].pos.x < 50.1158  &&  p->robot[4].pos.y < 41.7038
					)
					//检测robot坐标
					Freeball = true;
			}
		else if (p->curball.y > 18 - 3 && p->curball.y < 18 + 3)//限制坐标y							  //争球的时候4号抢球	
			{//右下方开球点																				  //1，2在左边半场，3号在争球区域的上半场	上半场的时候稍微判定松些？
				if ((p->robot[2].pos.x > 60 - 3 && p->robot[2].pos.x < 60 + 3)
					&& (p->robot[2].pos.y > 18 - 3 && p->robot[2].pos.y < 18 + 3)
					&& p->robot[1].pos.x < 50.1158	&&	p->robot[2].pos.x<50.1158  &&  p->robot[3].pos.y>41.7038
					)
					Freeball = true;
			}
		}
		else if (p->curball.x > 29 - 3 && p->curball.x < 29 + 3)
		{//左方																							  //争球的时候1号争球
			if (p->curball.y > 65 - 3 && p->curball.y < 65 + 3)//限制坐标y								  //2 4 号在征求半场下方 对3号不做判定要求
			{//左上方开球点
				if ((p->robot[1].pos.x > 18 - 3 && p->robot[1].pos.x < 18 + 3)
					&& (p->robot[1].pos.y > 65 - 3 && p->robot[1].pos.y < 65 + 3)
					&& p->robot[2].pos.y < 41.7038	&&p->robot[4].pos.y < 41.7038
					)
					Freeball = true;
			}
			else if (p->curball.y > 18 - 3 && p->curball.y < 18 + 3)//限制坐标y							//征球的时候2号征求
			{//左下方开球点
				if ((p->robot[2].pos.x > 18 - 3 && p->robot[2].pos.x < 18 + 3)							//1 3 在半场上方 对4号不做要求
					&& (p->robot[2].pos.y > 19 - 3 && p->robot[2].pos.y < 19 + 3) && (p->robot[1].pos.x < p->robot[3].pos.x)
					&& p->robot[1].pos.y > 41.7038	&&p->robot[3].pos.y > 41.7038
					)
					Freeball = true;
			}
		}
		if (sqrt(pow(p->ballspeed.x, 2) + pow(p->ballspeed.y, 2)) >0.3)  //退出判定 应该不准确 0.3可以改
		{
			Freeball = false;
		}
		if (Freeball)
		{
			FreeBallGame(env);
		}
		else if (!Freeball)
			NormalGame(env);
		break;

	case PLACE_KICK:		//Place kick(Kickoff)
		start = clock();
		if (start - count >= 5000 || 0 == count)
		{
			See(env);   //规避第一帧数据异常 
			count = start;
			placecount = 0;
		}
		count = start - 5;  //5是随便写的

		if (p->curball.x < 50.1158 + 3 && p->curball.x>50.1158 - 3 && p->curball.y > 41.7038 - 3 && p->curball.y < 41.7038 + 3 && placecount == 0)
			// 球在中心点 1 2 3 号在己方半场 4 号在对面半场开球 防守的时候全在己方半场 具体攻守分配以及策略在PlaceBallGame函数中分配
		{
			Placeball = true;
		}
		if (placecount>35)//判定退出条件			大于80帧		
		{
			Placeball = false;
		}
		if (Placeball)
		{
			PlaceBallGame(env, placecount);
			placecount++;
		}
		else if (!Placeball)
			NormalGame(env);
		break;




	case PENALTY_KICK:
		start = clock();
		if (start - count >= 5000 || 0 == count)
		{
			See(env);   //规避第一帧数据异常 
			count = start;
			choosepenalty = true;  //选择点球手的开关开启
			penaltycount = 0;
		}
		count = start - 5;  //5是随便写的

		if (p->curball.x < 78 + 3 && p->curball.x>78 - 3 && p->curball.y > 41 - 3 && p->curball.y < 41 + 3)
		{
			int robotnum = 0;
			for (int i = 0; i < 5; i++)
				if (p->robot[i].pos.x <= 51)  //应该为中场线数据 放宽点
					robotnum++;
			if (robotnum >= 4 && choosepenalty == true)
			{
				ourpenalty = true;
				if (p->robot[1].pos.x >= 50.3)//开关：1号站在点球位
					panel1 = true;
				else if (p->robot[2].pos.x >= 50.3)//开关：2号站在点球位
					panel2 = true;
				else if (p->robot[3].pos.x >= 50.3)//开关：3号站在点球位
					panel3 = true;
				else if (p->robot[4].pos.x >= 50.3)//开关：4号站在点球位
					panel4 = true;
				choosepenalty = false;
			}
		}
		else if (p->curball.x > 93.4259 || (p->curball.x >= 93.4259 - 5 && p->ballspeed.x < 0.1) || p->ballspeed.x<-0.1)//goal or our ball was blocked by the enemy
		{

			penaltycount = 0;
			ourpenalty = false;//不再是我方点球阶段	

		}
		if (ourpenalty)
		{
			//有多个点球策略的时候这里可以用
			if (panel1)		PenaltyBallGame(env);
			if (panel2)		PenaltyBallGame(env);
			if (panel3)		PenaltyBallGame(env);
			if (panel4)		PenaltyBallGame(env);
			penaltycount++;
		}
		else if (!ourpenalty)
		{
			NormalGame(env);
		}
		break;


	case GOAL_KICK:	//5:
					//Goal kick

		start = clock();
		if (start - count >= 1000 || 0 == count)
		{
			See(env);   //规避第一帧数据异常 
			count = start;
			goalcount = 0;
		}
		count = start - 5;  //5是随便写的

							//门球判定 1球在禁区 对面球员在对面半场 自己小车速度都很低 门区只有自己一个守门员
		if (p->curball.x > 6.8118&&p->curball.x<13.5753&& p->curball.y>31.6176&&p->curball.y < 51.51125
			&&goalcount == 0
			&& OnlyGoalieInMenqu(env) == true)//判定进入条件
		{
			Goalkick = true;
		}
		//29.26 x   18.18 y
		if (goalcount>30)//判定退出条件			大于80帧		
		{
			Goalkick = false;
		}
		if (Goalkick)
		{
			GoalKickGame(env, goalcount);
			goalcount++;
		}
		else if (!Goalkick)
			NormalGame(env);
		break;

	default:
		NormalGame(env);

		break;

	}

}


//End函数每一帧都执行 提交对队员轮速的修改 保存本周期的状态供下次计算使用

void End(Environment *env)
{
	///做一些清扫的工作
	///做一些记录整理工作

	Mydata * p;
	p = (Mydata *)env->userData;
	int i = 0;

	//提交轮速
	for (i = 0; i < 5; i++)
	{
		env->home[i].velocityLeft = p->robot[i].velocityLeft;
		env->home[i].velocityRight = p->robot[i].velocityRight;
	}

	//记录我方轮速
	for (i = 0; i < 5; i++)
	{
		p->myoldvelocity[i].x = p->robot[i].velocityLeft;
		p->myoldvelocity[i].y = p->robot[i].velocityRight;
	}

	//跟上面一样我也不知道为什么预测一步，导致下面的信息要从env里重新获取。。。dyt
	if (p->mygrand)//我是黄方
	{
		p->oldball.x = env->currentBall.pos.x;		//球坐标变化
		p->oldball.y = env->currentBall.pos.y;


		for (i = 0; i<5; i++)
		{
			p->myoldpos[i].x = env->home[i].pos.x;	//我方队员坐标变换
			p->myoldpos[i].y = env->home[i].pos.y;
			p->myoldpos[i].z = env->home[i].rotation;
			RegulateAngle(p->opoldpos[i].z);

			p->opoldpos[i].x = env->opponent[i].pos.x;	//对方坐标变换
			p->opoldpos[i].y = env->opponent[i].pos.y;
			p->opoldpos[i].z = env->opponent[i].rotation;
			RegulateAngle(p->opoldpos[i].z);

		}
	}

	else//我是蓝队
	{
		p->oldball.x = FLEFT + FRIGHT + CORRECTX - env->currentBall.pos.x;		//球坐标变化
		p->oldball.y = FBOT + FTOP + CORRECTY - env->currentBall.pos.y;

		for (i = 0; i<5; i++)
		{
			p->myoldpos[i].x = FLEFT + FRIGHT + CORRECTX - env->home[i].pos.x;	//我方队员坐标变换
			p->myoldpos[i].y = FBOT + FTOP + CORRECTY - env->home[i].pos.y;
			p->myoldpos[i].z = 180.0 + env->home[i].rotation;
			RegulateAngle(p->myoldpos[i].z);

			p->opoldpos[i].x = FLEFT + FRIGHT + CORRECTX - env->opponent[i].pos.x;	//对方坐标变换
			p->opoldpos[i].y = FBOT + FTOP + CORRECTY - env->opponent[i].pos.y;
			p->opoldpos[i].z = 180 + env->opponent[i].rotation;
			RegulateAngle(p->opoldpos[i].z);
		}
	}

}


// CheckBall	返回球所在的分区号
int CheckBall(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	int  ballArea;

	double x1 = 21.3;//30
	double x2 = 50.1;
	double x3 = 78.7;//70

	double y1 = 25.1;
	double y2 = 41.8;
	double y3 = 58.6;

	Vector3D ball;
	ball.x = p->preball.x;
	ball.y = p->preball.y;

	if (ball.x <= x1&&ball.y <= y1)
		ballArea = 1;
	else if (ball.x <= x1&&ball.y <= y2)
		ballArea = 2;
	else if (ball.x <= x1&&ball.y <= y3)
		ballArea = 3;
	else if (ball.x <= x1&&ball.y>y3)
		ballArea = 4;
	else if (ball.x <= x2&&ball.y <= y1)
		ballArea = 5;
	else if (ball.x <= x2&&ball.y <= y2)
		ballArea = 6;
	else if (ball.x <= x2&&ball.y <= y3)
		ballArea = 7;
	else if (ball.x <= x2&&ball.y>y3)
		ballArea = 8;
	else if (ball.x<x3&&ball.y <= y1)
		ballArea = 9;
	else if (ball.x<x3&&ball.y <= y2)
		ballArea = 10;
	else if (ball.x<x3&&ball.y <= y3)
		ballArea = 11;
	else if (ball.x<x3&&ball.y>y3)
		ballArea = 12;
	else if (ball.x >= x3&&ball.y <= y1)
		ballArea = 13;
	else if (ball.x >= x3&&ball.y <= y2)
		ballArea = 14;
	else if (ball.x >= x3&&ball.y<y3)
		ballArea = 15;
	else  ballArea = 16;
	return ballArea;

}

//工具函数=======================================================================


//RegulateAngle 规范angle的大小在（-180，+180）之间
void RegulateAngle(double &angle)
{
	while (angle >= 180.0)angle -= 360.0;
	while (angle < -180.0)angle += 360.0;
}


//toint 转换成整数 （四舍五入）
int toint(double num)
{
	return num * 10 < (int)num * 10 + 5 ? (int)num : (int)num + 1;
}
 
 
//Atan 求y/x正切值对应的角度（-180，+180）之间
double Atan(double y, double x)
{
	if (x != 0.0 || y != 0.0)
		return 180 * atan2(y, x) / PI;
	else return 0.0;
}


//Atan (end-begin)矢量的角度，（-180，+180）之间
double Atan(Vector3D begin, Vector3D end)
{
	double y, x;
	y = end.y - begin.y;
	x = end.x - begin.x;
	return Atan(y, x);
}


//Distance  两点之间距离
double Distance(Vector3D pos1, Vector3D pos2)
{
	return sqrt((pos1.x - pos2.x)*(pos1.x - pos2.x) + (pos1.y - pos2.y)*(pos1.y - pos2.y));
}

//AngleOne 计算在当前角速度omiga的基础上以左右轮速vl,vr控制，下一个周期达到的角速度
double AngleOne(double omiga, double vl, double vr)
{
	//		omiga = p->robot[i].rotation - p->myoldpos[i].z ;
	//		RegulateAngle(omiga);
	if (vl>125)vl = 125;
	if (vl<-125)vl = -125;
	if (vr>125)vr = 125;
	if (vr<-125)vr = -125;
	double angle = (vr - vl) / 2;

	RegulateAngle(omiga);		//应该没有大于180 的角速度罢

	omiga += ANGLE_A*(ANGLE_B* angle - omiga);

	if (vr > vl)
	{
		if (vl >= 0 || vr <= 0)
		{
			omiga -= 4 * ANGLE_K * angle * angle;
		}
	}
	else if (vr < vl)
	{
		if (vr >= 0 || vl <= 0)
		{
			omiga += 4 * ANGLE_K * angle * angle;
		}
	}

	RegulateAngle(omiga);		//应该没有大于180 的角速度罢

	return omiga;

}


//VelocityOne 计算在当前速度speed的基础上以左右轮速vl,vr控制，下一个周期达到的速度
double VelocityOne(double speed, double vl, double vr)
{
	if (vl>125)vl = 125;
	if (vl<-125)vl = -125;
	if (vr>125)vr = 125;
	if (vr<-125)vr = -125;

	if (speed > 3 || speed < -3)
		speed = 0;

	if (vl == 0 && vr == 0)
		speed += -SPEED_ODD * speed;
	else
		speed += SPEED_A*(SPEED_B*(vl + vr) / 2 - speed);
	return speed;
}


//Count 计算robot跑道pos位置的周期数
int Count(Environment *env, int robot, Vector3D pos)
{
	Mydata * p;
	p = (Mydata *)env->userData;

	double v = 0, s = 0;
	double  distance = 0;
	int n = 0;
	double turnangle = 0, vangle = 0;
	double posangle = 0, sangle = 0;
	double dx, dy;

	dx = pos.x - p->robot[robot].pos.x;
	dy = pos.y - p->robot[robot].pos.y;

	distance = Distance(p->robot[robot].pos, pos);
	posangle = Atan(dy, dx);

	//omiga
	turnangle = posangle - p->robot[robot].rotation;
	RegulateAngle(turnangle);

	if (turnangle > 90)
	{
		turnangle -= 180;
	}
	else if (turnangle < -90)
	{
		turnangle += 180;
	}

	double omiga = p->robot[robot].rotation - p->myoldpos[robot].z;
	RegulateAngle(omiga);

	if (turnangle <0)
	{
		turnangle = -turnangle;
		if (omiga < 0)
			omiga = -omiga;
	}

	do {
		omiga = AngleOne(omiga, -120, 120);
		sangle += omiga;
		n++;
	} while (sangle < turnangle);
	n--;
	//omiga	
	v = sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	vangle = p->myspeed[robot].z - posangle;
	RegulateAngle(vangle);
	if (vangle <-90 || vangle > 90)
		v = -v;
	do {//跑位的步数
		//还没有考虑反向 是怎么办呢  
		v = VelocityOne(v, 110, 110);
		s += v;
		n++;
	} while (s < distance);
	n--;
	return n;
}


//将球速的像素每帧换算为像素每秒
double BallSpeed(Environment *env, int steps)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	PredictBall(env, steps);
	double vx = 0, vy = 0;
	vx = (p->preball.x - p->curball.x) * 60;
	vy = (p->preball.y - p->curball.y) * 60;
	return sqrt(pow(vx, 2) + pow(vy, 2));
}


//Meetball_p	求出robot追到球的最佳位置
Vector3D Meetball_p(Environment *env, int robot)
{//求出robot追到球的位置
	Mydata * p;
	p = (Mydata *)env->userData;

	Vector3D meetpoint = { 0,0,-1 };
	double dis = Distance(p->curball, p->robot[robot].pos);

	double t = 0;
	double vb = 0;
	double v = 1.9;		//按照最大速度计算
	double pos_angle, b_sp_angle;

	pos_angle = Atan(p->curball.y - p->robot[robot].pos.y, p->curball.x - p->robot[robot].pos.x);
	b_sp_angle = p->ballspeed.z;
	vb = (p->ballspeed.y * p->ballspeed.y + p->ballspeed.x * p->ballspeed.x);
	t = sin((b_sp_angle - pos_angle) * PI / 180);
	t = vb* t*t;
	v = v*v;
	if (v > t)
	{
		v = sqrt(v - t) + sqrt(vb) * cos((b_sp_angle - pos_angle) * PI / 180);
		if (v > 0.1)
		{
			t = dis / v;	//得到步数
			meetpoint.x = p->ballspeed.x *t + p->curball.x;
			meetpoint.y = p->ballspeed.y *t + p->curball.y;
			meetpoint.z = t;
		}
	}

	return meetpoint;

}


//Meetball_y	求出该队员和求相遇的 y 坐标 maxn用于限制追到球的最长的时间（周期数）
Vector3D Meetball_y(Environment *env, double X, double maxn)
{//求出该队员和求相遇的 y 坐标

	Mydata * p;
	p = (Mydata *)env->userData;
	double dx = X - p->curball.x;

	Vector3D meetpoint = { 0,0,-1 };


	if (dx > 0)
	{//球在  左方,目标在右边
		if (p->ballspeed.x > 0.1)
			meetpoint.z = dx / p->ballspeed.x;
	}
	else
	{//球在 右边 ,目标在左方	//dx < 0 
		if (p->ballspeed.x < -0.1)
			meetpoint.z = dx / p->ballspeed.x;
	}

	if (meetpoint.z > 0)
	{
		meetpoint.x = X;
		meetpoint.y = p->ballspeed.y * meetpoint.z + p->curball.y;
	}
	return meetpoint;

}


//Meetball_x	求出该队员和求相遇的 x 坐标 maxn用于限制追到球的最长的时间（周期数）
Vector3D Meetball_x(Environment *env, double Y, double maxn)
{//求出该队员和求相遇的 x 坐标

	Mydata * p;
	p = (Mydata *)env->userData;
	double dy = Y - p->curball.y;

	Vector3D meetpoint = { 0,0,-1 };


	if (dy > 0)
	{//球在  下方,目标在上
		if (p->ballspeed.y > 0.01)
			meetpoint.z = dy / p->ballspeed.y;
	}
	else
	{//球在 上方 ,目标在下	//dx < 0 
		if (p->ballspeed.y < -0.01)
			meetpoint.z = dy / p->ballspeed.y;
	}

	if (meetpoint.z > 0)
	{
		meetpoint.y = Y;
		meetpoint.x = p->ballspeed.x * meetpoint.z + p->curball.x;
	}
	return meetpoint;

}


//Within  判断robot队员和球的距离是否再LENGTH规定的范围内
bool Within(Environment* env, int robot, double LENGTH)
{
	Mydata* p;
	p = (Mydata*)env->userData;

	const double steps = 50;
	int who = robot;
	double dis;

	Vector3D ballgo = { 0,0,0 };
	Vector3D robotgo = { 0,0,0 };
	Vector3D ball = p->curball;


	ballgo.x = ball.x + steps * p->myspeed[who].x;
	ballgo.y = ball.y + steps * p->myspeed[who].y;

	dis = Distance(ballgo, p->robot[robot].pos);

	if (dis < LENGTH)
	{
		return true;
	}
	return false;

}


//只有门将在门区内
bool OnlyGoalieInMenqu(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	int i;
	for (i = 1; i < 5; i++)
	{
		if (p->robot[i].pos.x > 6.8118&&p->robot[i].pos.x<13.5753&&p->robot[i].pos.y>25.87&&p->robot[i].pos.y < 51.5112)
			return false;
		if (p->opp[i].pos.x > 6.8118&&p->opp[i].pos.x<13.5753&&p->opp[i].pos.y>25.87&&p->opp[i].pos.y < 51.5112)
			return false;
	}
	return true;
}


//己方球员在对方门区内有几个人
int opareanum(Environment* env)
{
	Mydata* p;
	p = (Mydata*)env->userData;
	int i = 0;
	for (int j = 1; j < 5; j++)
		if (p->robot[j].pos.x > 79 && p->robot[j].pos.y<57.5&& p->robot[j].pos.y>25.5)
			i++;
	return i;
}


//给的normalgame函数中所用到的队列排序
void Order(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;
	int i, j, k, a[4];
	double dis[4], b[4], temple;
	double dy = 0;
	static int record = 2;

	b[0] = dis[0] = Distance(p->robot[1].pos, p->curball);
	b[1] = dis[1] = Distance(p->robot[2].pos, p->curball);
	b[2] = dis[2] = Distance(p->robot[3].pos, p->curball);
	b[3] = dis[3] = Distance(p->robot[4].pos, p->curball);

	switch (p->ballArea) {
	case 1:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		for (i = 0; i<3; i++)
			for (j = i + 1; j<4; j++) {
				if (b[i]>b[j]) {
					temple = b[i];
					b[i] = b[j];
					b[j] = temple;
				}
			}
		for (i = 0; i<4; i++)
			for (j = 0; j<4; j++)
				if (dis[j] == b[i])
					a[i] = j + 1;

		p->mainrobot = a[0];
		p->slowrobot = a[1];
		p->cutrobot = a[2];
		p->defentrobot = a[3];
		break;
	case 2:
	case 3:
		if (p->curball.y>p->robot[0].pos.y) {
			i = p->mainrobot;
			p->mainrobot = p->cutrobot;
			p->cutrobot = i;
		}
		break;
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:

		if (dis[1]<dis[2]) {

			if (dis[2] <= dis[3]) {
				i = 2;
				j = 3;
				k = 4;
			}
			else if (dis[1] <= dis[3]) {
				i = 2;
				j = 4;
				k = 3;
			}
			else {
				i = 4;
				j = 2;
				k = 3;
			}
		}
		else {

			if (dis[1] <= dis[3]) {
				i = 3;
				j = 2;
				k = 4;
			}
			else if (dis[2] <= dis[3]) {
				i = 3;
				j = 4;
				k = 2;
			}
			else {
				i = 4;
				j = 3;
				k = 2;
			}
		}

		p->mainrobot = i;
		p->slowrobot = j;
		p->cutrobot = k;
		record = p->cutrobot;
		break;
	case 14:
		if (!Within(env, record, 1))
			p->cutrobot = record;

		else {
			if (p->ballspeed.z<0) {
				dy = 100;
				for (i = 2; i<5; i++)
					if (p->robot[i].pos.y<dy) {
						dy = p->robot[i].pos.y;
						record = i;
					}
				p->mainrobot = p->cutrobot;
				p->cutrobot = record;
			}
			else {
				dy = 0;
				for (i = 2; i<5; i++)
					if (p->robot[i].pos.y>dy) {
						dy = p->robot[i].pos.y;
						record = i;
					}
				p->mainrobot = p->cutrobot;
				p->cutrobot = record;
			}
		}

		for (i = 2; i<5; i++)
			if (i != p->cutrobot&&i != p->mainrobot)
				p->slowrobot = i;

		break;
	case 15:

		if (!Within(env, record, 1))
			p->cutrobot = record;

		else {
			if (p->ballspeed.z<0) {
				dy = 100;
				for (i = 2; i<5; i++)
					if (p->robot[i].pos.y<dy) {
						dy = p->robot[i].pos.y;
						record = i;
					}
				p->mainrobot = p->cutrobot;
				p->cutrobot = record;
			}
			else {
				dy = 0;
				for (i = 2; i<5; i++)
					if (p->robot[i].pos.y>dy) {
						dy = p->robot[i].pos.y;
						record = i;
					}
				p->mainrobot = p->cutrobot;
				p->cutrobot = record;
			}
		}
		for (i = 2; i<5; i++)
			if (i != p->cutrobot&&i != p->mainrobot)
				p->slowrobot = i;

		break;
	case 16:

		if (dis[1]<dis[2]) {

			if (dis[2] <= dis[3]) {
				i = 2;
				j = 3;
				k = 4;
			}
			else if (dis[1] <= dis[3]) {
				i = 2;
				j = 4;
				k = 3;
			}
			else {
				i = 4;
				j = 2;
				k = 3;
			}
		}
		else {

			if (dis[1] <= dis[3]) {
				i = 3;
				j = 2;
				k = 4;
			}
			else if (dis[2] <= dis[3]) {
				i = 3;
				j = 4;
				k = 2;
			}
			else {
				i = 4;
				j = 3;
				k = 2;
			}
		}

		p->mainrobot = i;
		p->slowrobot = j;
		p->cutrobot = k;
		record = p->cutrobot;
		break;
	}

}


//动作函数====================================================================

//Velocity 修改robot的左右轮速vl,vr
void Velocity(Environment *env, int robot, double vl, double vr)
{
	Mydata * p;
	p = (Mydata *)env->userData;

	//vl,vr都有取值范围的!!!
	if (vl>125)vl = 125;
	if (vl<-125)vl = -125;
	if (vr>125)vr = 125;
	if (vr<-125)vr = -125;

	if (true)
	{//速度的特别控制
		if (vl == 0 && vr != 0)
			vl = 0.00001;
		if (vr == 0 && vl != 0)
			vr = 0.00001;
	}
	p->robot[robot].velocityLeft = vl;
	p->robot[robot].velocityRight = vr;
}


//自旋函数     正数为顺时针旋转 
void Spin(Environment *env, int Number, double Rotate)
{
	env->home[Number].velocityLeft = Rotate;
	env->home[Number].velocityRight = -Rotate;
};


//Angle 让robot转到angle的方向
void Angle( Environment *env, int robot,double angle)
{

	Mydata * p;
	p=(Mydata *)env->userData;

	double speed = 0;		//和pangle接轨
	double accuracy=1;
	double turnangle=0,nextangle=0;
	double FF=125;		//最大减速度

	turnangle = angle -p->robot[robot].rotation;
	RegulateAngle(turnangle);
if(turnangle < 1 && turnangle >-1)
{
	Velocity(env,robot,0,0);
	return ;
}
else if(turnangle < 2 && turnangle >-2)
	FF=10;
else if( turnangle >-3 && turnangle < 3)
	FF=15;
else if( turnangle >-5 && turnangle < 5)
	FF=30;

	double v=p->robot[robot].rotation - p->myoldpos[robot].z ;
	RegulateAngle(v);
	double v1=v;
	double f=0;	//相当于减速时,右轮速度，
//	int n=0;
	bool turnleft=true;			//判断小车是否是该向左转
	double a=ANGLE_A;
	double b=ANGLE_B;

	if(turnangle > 90)
	{
		turnleft=false;
		turnangle-=180;
	}
	else if(turnangle >0)
	{
		turnleft=true;	
	}
	else if(turnangle > -90)
	{
		turnleft=false;	
	}
	else 
	{
		turnleft=true;
		turnangle+=180;		
	}

	if(turnleft)
	{//
		f=-FF;

		v1=AngleOne(v1,speed+f,speed-f);		//v1+=a *( -b *f-v1);
		nextangle+=v1;

		do{//whether to reduce
			//收敛!!
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 > 0  );
		
		nextangle-=v1;

		if(nextangle < turnangle)
		{//不满足减速条件 所以 f 取相反数
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
	
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 < 0 )
			{
				do{//该降低功率了
					f++;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 < turnangle && f <FF);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}
	else
	{//
		f=FF;
		v1=AngleOne(v1,speed+f,speed-f);		//v1+=a *( -b *f-v1);
		nextangle+=v1;
		
		do{//whether to reduce
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 < 0 );
		
		nextangle-=v1;

		if(nextangle > turnangle)
		{//不满足减速条件 所以 f 取相反数
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 > 0 )
			{
				do{//该降低功率了
					f--;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 > turnangle && f >-FF);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}

}


//Angle 让robot转到正对pos的方向
void Angle( Environment *env, int robot,Vector3D pos)
{

	Mydata * p;
	p=(Mydata *)env->userData;

	double speed = 0;		//和pangle接轨
	double accuracy=1;
	double turnangle=0,nextangle=0;
	double FF=125;		//最大减速度
	double angle=0;
	angle = Atan(p->robot[robot].pos , pos);

	turnangle = angle -p->robot[robot].rotation;
	RegulateAngle(turnangle);
if(turnangle < 1 && turnangle >-1)
{
	Velocity(env,robot,0,0);
	return ;
}
else if(turnangle < 2 && turnangle >-2)
	FF=10;
else if( turnangle >-3 && turnangle < 3)
	FF=15;
else if( turnangle >-5 && turnangle < 5)
	FF=30;

	double v=p->robot[robot].rotation - p->myoldpos[robot].z ;
	RegulateAngle(v);
	double v1=v;
	double f=0;	//相当于减速时,右轮速度，
//	int n=0;
	bool turnleft=true;			//判断小车是否是该向左转
	double a=ANGLE_A;
	double b=ANGLE_B;

	if(turnangle > 90)
	{
		turnleft=false;
		turnangle-=180;
	}
	else if(turnangle >0)
	{
		turnleft=true;	
	}
	else if(turnangle > -90)
	{
		turnleft=false;	
	}
	else 
	{
		turnleft=true;
		turnangle+=180;		
	}

	if(turnleft)
	{//
		f=-FF;

		v1=AngleOne(v1,speed+f,speed-f);		//v1+=a *( -b *f-v1);
		nextangle+=v1;

		do{//whether to reduce
			//收敛!!
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 > 0  );
		
		nextangle-=v1;

		if(nextangle < turnangle)
		{//不满足减速条件 所以 f 取相反数
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
	
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 < 0 )
			{
				do{//该降低功率了
					f++;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 < turnangle && f <FF);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}
	else
	{//
		f=FF;
		v1=AngleOne(v1,speed+f,speed-f);		//v1+=a *( -b *f-v1);
		nextangle+=v1;
		
		do{//whether to reduce
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 < 0 );
		
		nextangle-=v1;

		if(nextangle > turnangle)
		{//不满足减速条件 所以 f 取相反数
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 > 0 )
			{
				do{//该降低功率了
					f--;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 > turnangle && f >-FF);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}

}


//PAngle 让robot朝angle的方向跑，并且speed控制它的最大轮速
void PAngle( Environment *env, int robot,double angle,double speed)
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	double accuracy=1;
	double turnangle=0,nextangle=0;
	turnangle = angle -p->robot[robot].rotation;
	RegulateAngle(turnangle);
	double v=p->robot[robot].rotation - p->myoldpos[robot].z ;
	RegulateAngle(v);
	double v1=v;
	double FF=125;		//最大减速度
	double f=0;	//相当于减速时,右轮速度，
//	int n=0;
	bool turnleft=true;			//判断小车是否是该向左转
	double a=ANGLE_A;
	double b=ANGLE_B;
	
	bool face ;
	if(  turnangle < 90 && turnangle > -90 )	//检查是否正面跑位
	{
		face = true;
		speed = speed;
	}
	else
	{
		face = false;
		speed = -speed;
	}



	if(turnangle > 90)
	{
		turnleft=false;
		turnangle-=180;
	}
	else if(turnangle >0)
	{
		turnleft=true;	
	}
	else if(turnangle > -90)
	{
		turnleft=false;	
	}
	else 
	{
		turnleft=true;
		turnangle+=180;		
	}




	if(turnleft)
	{//
		f=-FF;

		v1=AngleOne(v1,speed+f,speed-f);		//v1+=a *( -b *f-v1);
		nextangle+=v1;

		do{//whether to reduce
			//收敛!!
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 > 0  );
		
		nextangle-=v1;

		if(nextangle < turnangle)
		{//不满足减速条件 所以 f 取相反数
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
	
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 < 0 )
			{
				do{//该降低功率了
					f++;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 < turnangle && f <125);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}
	else
	{//
		f=FF;
		v1=AngleOne(v1,speed+f,speed-f);		//v1+=a *( -b *f-v1);
		nextangle+=v1;
		
		do{//whether to reduce
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 < 0 );
		
		nextangle-=v1;

		if(nextangle > turnangle)
		{//不满足减速条件 所以 f 取相反数
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 > 0 )
			{
				do{//该降低功率了
					f--;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 > turnangle && f >-125);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}
}


//PositionAndStop 让robot 跑到pos附近控制在limit的距离内，并且面向bestangle停下来
void PositionAndStop(Environment *env,int  robot,Vector3D pos ,double bestangle,double limit) 
{	//考虑到可能的	急停和 急快速加速
	//特别作了优化
	//还有就是 被碰转后的转角过程 不能耽搁时间!!!
	//转角是最危险的过程

	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//控制转交速度的变量
	double vmax=125;			//默认的跑位加速度
	double Limitedangle=2;		//默认减速范围

	if( limit < 0.5 )
		limit =0.5;
	double Limiteddis=limit;	//减速范围有一个下限，保证不会来回跑动
	
	double  distance;			//robot和目标点的距离
	double turnangle,posangle,vangle;	//转动角度 ，目标点相对robot的角度，速度的绝对角度
	double dx,dy;				//pos  和robot的坐标差
	double a=SPEED_A;			//参数
	double b=SPEED_B;
	double v,v1;				//临时保存速度的大小!!!
	double f=vmax;				//加速度变量
	double s=0;					//预测的减速位移(路程)
	int n=0;					//跑位的步数
	bool face=true;			//判断小车是否是正面前进

	v= sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	//临时保存速度的大小!!!
	dx = pos.x - p->robot[robot].pos.x ;		//pos  和robot的坐标差
	dy = pos.y - p->robot[robot].pos.y ;
	
	distance = Distance(p->robot[robot].pos , pos);
	posangle = Atan(dy,dx);

	turnangle = p->robot[robot].rotation - posangle;		//转动角度 
	RegulateAngle(turnangle);

	if(turnangle > 90)
	{//判断小车是否是正面前进
		face=false;
		turnangle-=180;
	}
	else if(turnangle < -90)
	{
		face=false;
		turnangle+=180;
	}
	else 
	{
		face=true;
	}

	vangle = p->myspeed[robot].z - p->robot[robot].rotation;		//速度的方向和robot正面的夹角
	RegulateAngle(vangle);					//主要用于最后控制减速度的大小
	if( vangle <-90 || vangle > 90 )		//同时判断v的正负
		v=-v;

	if(face)
	{//forward	跑位，如果后退的话  就v=0
		//设vl,vr=0 还是vl,vr=125 有一个条件有一个临界条件那就是 
		//v = SPEED_ZERO
		if(v < -SPEED_ZERO)
		{
			Velocity(env,robot,0,0);
			return ;
		}
	}
	else if(v > SPEED_ZERO)
	{//back		跑位，如果后退的话  就v=0
		Velocity(env,robot,0,0);
		return ;
	}

	v1=v;	//v1 is changing while program running 
			//whlie, v is not

	if(distance > Limiteddis )
	{//it is too early to count the steps
		//but the Limiteddis should be tested!!	to do...
		if(turnangle > Limitedangle || turnangle < -Limitedangle)
		{//adjust angle
			/////////////////测试这一段
			//对于goalie这一段应该特别注意
			//发生变向	1.knock the robot,especially the opponent
			//	2.knock the wall
			// so the anglespeedmax is allowed ++ more!!
			if(turnangle > 20  || turnangle < -20)
				anglespeedmax = 0;
			else if(turnangle > 10  || turnangle < -10)
				anglespeedmax = 125;
			else if(turnangle > 5  || turnangle < -5)
				anglespeedmax = 180;
			else 
				anglespeedmax = 200;
			///////////////测试这一段
			PAngle(env,robot,posangle,anglespeedmax);
		}
		else
		{
			if(face)
				Velocity(env,robot,f,f);
			else
				Velocity(env,robot,-f,-f);
		}//it is time to rush
	}
	else
	{
		if(distance > 1)
		{		//调整角度	return!!!!!!
			//radious of robot is about 1.5 ,so the distance is very short
			if(turnangle > Limitedangle || turnangle < -Limitedangle)
			{	
				Angle(env,robot,posangle);
				return ;
			}
		}

		if(distance < 0.4)
		{	//停止并转向		return!!!!!!
			//radious of robot is about 1.5 ,so the distance is very short
			if( v<0.1 && v>-0.1)
			{	//the range of v shoud be tested 
				if(bestangle == 0)
					Velocity(env,robot,0,0);
				else
					Angle(env,robot,bestangle);
				return	;
			}
		}

		if(true)
		{
			vmax=125;
			if(face)
			{
				f=-vmax;		//减速度  为  0000000
				v1=VelocityOne(v1,-f,-f);		//加速一步
				s=v1;
				do{//whether to reduce
					if(v1 > SPEED_ZERO)	//as i said,this is limited
						v1=VelocityOne(v1,0,0);
					else 
						v1=VelocityOne(v1,f,f);
					s+=v1;
				}while( v1 > 0 );

				s-=v1;

				if(s < distance)
				{//不满足减速条件加速
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v > SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//减速一步
						if( v1 < 0 )
						{
							do{//该降低功率了
								f++;		//f=-vmax;
								v1 = VelocityOne(v,f,f);
							}while( v1 < distance && f < vmax);	
						}
						Velocity(env,robot,f,f);
					}
				}
			}			
			else
			{
				f=vmax;		//减速度!!!!!
				v1=VelocityOne(v1,-f,-f);
				s=v1;
				do{//whether to reduce
					if(v1 < -SPEED_ZERO)	//as i said,this is limited
						v1=VelocityOne(v1,0,0);
					else 
						v1=VelocityOne(v1,f,f);
					s+=v1;
				}while( v1 < -0.1 );

				s-=v1;

				if(s > -distance)
				{//不满足减速条件加速
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v < -SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//减速一步
						if( v1 > 0 )
						{
							do{//该降低功率了
								f--;		//f=-vmax;
								v1 = VelocityOne(v,f,f);
							}while( v1 > -distance && f > -vmax);	
						}
						Velocity(env,robot,f,f);
					}
				}
			}	
		}
	}
}


//PositionAndStop 让robot 跑到pos附近控制在limit的距离内，并且面向face停下来
void PositionAndStop(Environment *env, int  robot, Vector3D pos, Vector3D face, double limit)
{
	PositionAndStop(env, robot, pos, Atan(pos, face), limit);
}


//GoaliePosition 让robot  跑到pos附近控制在limit的距离内，并且面向bestangle停下来 针对守门员写的
void GoaliePosition(Environment *env,int  robot,Vector3D pos ,double bestangle,double limit) 
{	//考虑到可能的	急停和 急快速加速
	//特别作了优化
	//还有就是 被碰转后的转角过程 不能耽搁时间!!!
	//转角是最危险的过程

	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//控制转交速度的变量
	double vmax=125;			//默认的跑位加速度
	double Limitedangle=2;		//默认减速范围

	if( limit < 0.5 )
		limit =0.5;
	double Limiteddis=limit;	//减速范围有一个下限，保证不会来回跑动
	
	double  distance;			//robot和目标点的距离
	double turnangle,posangle,vangle;	//转动角度 ，目标点相对robot的角度，速度的绝对角度
	double dx,dy;				//pos  和robot的坐标差
	double a=SPEED_A;			//参数
	double b=SPEED_B;
	double v,v1;				//临时保存速度的大小!!!
	double f=vmax;				//加速度变量
	double s=0;					//预测的减速位移(路程)
	int n=0;					//跑位的步数
	bool face=true;			//判断小车是否是正面前进

	v= sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	//临时保存速度的大小!!!
	dx = pos.x - p->robot[robot].pos.x ;		//pos  和robot的坐标差
	dy = pos.y - p->robot[robot].pos.y ;
	
	distance = Distance(p->robot[robot].pos , pos);
	posangle = Atan(dy,dx);

	turnangle = p->robot[robot].rotation - posangle;		//转动角度 
	RegulateAngle(turnangle);

	if(turnangle > 90)
	{//判断小车是否是正面前进
		face=false;
		turnangle-=180;
	}
	else if(turnangle < -90)
	{
		face=false;
		turnangle+=180;
	}
	else 
	{
		face=true;
	}

	vangle = p->myspeed[robot].z - p->robot[robot].rotation;		//速度的方向和robot正面的夹角
	RegulateAngle(vangle);					//主要用于最后控制减速度的大小
	if( vangle <-90 || vangle > 90 )		//同时判断v的正负
		v=-v;

	if(face)
	{//forward	跑位，如果后退的话  就v=0
		//设vl,vr=0 还是vl,vr=125 有一个条件有一个临界条件那就是 
		//v = SPEED_ZERO
		if(v < -SPEED_ZERO)
		{
			Velocity(env,robot,0,0);
			return ;
		}
	}
	else if(v > SPEED_ZERO)
	{//back		跑位，如果后退的话  就v=0
		Velocity(env,robot,0,0);
		return ;
	}

	v1=v;	//v1 is changing while program running 
			//whlie, v is not

	if(distance > Limiteddis )
	{//it is too early to count the steps
		//but the Limiteddis should be tested!!	to do...
		if(turnangle > Limitedangle || turnangle < -Limitedangle)
		{//adjust angle
			/////////////////测试这一段
			//对于goalie这一段应该特别注意
			//发生变向	1.knock the robot,especially the opponent
			//	2.knock the wall
			// so the anglespeedmax is allowed ++ more!!
			if(turnangle > 50  || turnangle < -50)
				anglespeedmax = 0;
			else if(turnangle > 30  || turnangle < -30)
				anglespeedmax = 80;
			else if(turnangle > 10  || turnangle < -10)
				anglespeedmax = 125;
			else if(turnangle > 5  || turnangle < -5)
				anglespeedmax = 180;
			else 
				anglespeedmax = 200;
			///////////////测试这一段

			PAngle(env,robot,posangle,anglespeedmax);
		}
		else
		{
			if(face)
				Velocity(env,robot,f,f);
			else
				Velocity(env,robot,-f,-f);
		}//it is time to rush
	}
	else
	{
		if(distance > 1)
		{		//调整角度	return!!!!!!
			//radious of robot is about 1.5 ,so the distance is very short
			if(turnangle > Limitedangle || turnangle < -Limitedangle)
			{	
				Angle(env,robot,posangle);
				return ;
			}
		}

		if(distance < 0.4)
		{	//停止并转向		return!!!!!!
			//radious of robot is about 1.5 ,so the distance is very short

			if( v<0.1 && v>-0.1)
			{	//the range of v shoud be tested 
				Angle(env,robot,bestangle);
				return	;
			}
		}

		if(true)
		{
			vmax=125;
			if(face)
			{
				f=-vmax;		//减速度  为  0000000
				v1=VelocityOne(v1,-f,-f);		//加速一步
				s=v1;
				do{//whether to reduce
					if(v1 > SPEED_ZERO)	//as i said,this is limited
						v1=VelocityOne(v1,0,0);
					else 
						v1=VelocityOne(v1,f,f);
					s+=v1;
				}while( v1 > 0 );

				s-=v1;

				if(s < distance)
				{//不满足减速条件加速
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v > SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//减速一步
						if( v1 < 0 )
						{
							do{//该降低功率了
								f++;		//f=-vmax;
								v1 = VelocityOne(v,f,f);
							}while( v1 < distance && f < vmax);	
						}
						Velocity(env,robot,f,f);
					}
				}
			}			
			else
			{
				f=vmax;		//减速度!!!!!
				v1=VelocityOne(v1,-f,-f);
				s=v1;
				do{//whether to reduce
					if(v1 < -SPEED_ZERO)	//as i said,this is limited
						v1=VelocityOne(v1,0,0);
					else 
						v1=VelocityOne(v1,f,f);
					s+=v1;
				}while( v1 < -0.1 );

				s-=v1;

				if(s > -distance)
				{//不满足减速条件加速
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v < -SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//减速一步
						if( v1 > 0 )
						{
							do{//该降低功率了
								f--;		//f=-vmax;
								v1 = VelocityOne(v,f,f);
							}while( v1 > -distance && f > -vmax);	
						}
						Velocity(env,robot,f,f);
					}
				}
			}	
		}
	}
}


//PositionAndStopX  让robot  跑到pos附近控制在limit的距离内，并且用Xangle的角速度旋转
void PositionAndStopX(Environment *env,int  robot,Vector3D pos ,double Xangle,double limit) 
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//控制转交速度的变量
	double vmax=125;
	double Limitedangle=2;

	if( limit < 2 )
		limit =2;
	double Limiteddis=limit;
	
	double  distance;
	double turnangle,posangle,vangle;
	double dx,dy;
	double a=SPEED_A;
	double b=SPEED_B;
	double v,v1;
	double f=vmax;
	double s=0;
	int n=0;
	bool face=true;			//判断小车是否是正面前进

	v= sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	
	dx = pos.x - p->robot[robot].pos.x ;
	dy = pos.y - p->robot[robot].pos.y ;
	
	distance = Distance(p->robot[robot].pos , pos);
	posangle = Atan(dy,dx);

	turnangle = p->robot[robot].rotation - posangle;		//think more!!
	RegulateAngle(turnangle);

	if(turnangle > 90)
	{
		face=false;
		turnangle-=180;
	}
	else if(turnangle < -90)
	{
		face=false;
		turnangle+=180;
	}
	else 
	{
		face=true;
	}

	vangle = p->myspeed[robot].z - p->robot[robot].rotation;
	RegulateAngle(vangle);
	if( vangle <-90 || vangle > 90 )
		v=-v;
	v1=v;

	if(distance > Limiteddis )
	{//it is too early to count the steps
		if(turnangle > Limitedangle || turnangle < -Limitedangle)
		{//adjust angle
			/////////////////测试这一段
			if(turnangle > 20  || turnangle < -20)
				anglespeedmax = 0;
			else if(turnangle > 10  || turnangle < -10)
				anglespeedmax = 125;
			else if(turnangle > 5  || turnangle < -5)
				anglespeedmax = 180;
			else 
				anglespeedmax = 200;
			///////////////测试这一段
			PAngle(env,robot,posangle,anglespeedmax);

		}
		else
		{
			if(face)
				Velocity(env,robot,f,f);
			else
				Velocity(env,robot,-f,-f);
		}//it is time to rush
	}
	else
	{
		if(distance > 1)
		{		//调整角度	return!!!!!!
			if(turnangle > Limitedangle || turnangle < -Limitedangle)
			{	
				Angle(env,robot,posangle);
				return ;
			}

		}

		if(distance < 1)
		{	//停止并转向		return!!!!!!
			if( v<0.5 && v>-0.5)
			{
				Velocity(env,robot,-Xangle,Xangle);
				return	;
			}
		}

		if(true)
		{
			vmax=125;
			if(face)
			{
				f=-vmax;		//减速度  为  0000000
				v1=VelocityOne(v1,-f,-f);		//加速一步
				s=v1;
				do{//whether to reduce
					if(v1 > SPEED_ZERO)	//as i said,this is limited
						v1=VelocityOne(v1,0,0);
					else 
						v1=VelocityOne(v1,f,f);
					s+=v1;
				}while( v1 > 0 );

				s-=v1;

				if(s < distance)
				{//不满足减速条件加速
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v > SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//减速一步
						if( v1 < 0 )
						{
							do{//该降低功率了
								f++;		//f=-vmax;
								v1 = VelocityOne(v,f,f);
							}while( v1 < distance && f < vmax);	
						}
						Velocity(env,robot,f,f);
					}
				}
			}			
			else
			{
				f=vmax;		//减速度!!!!!
				v1=VelocityOne(v1,-f,-f);
				s=v1;
				do{//whether to reduce
					if(v1 < -SPEED_ZERO)	//as i said,this is limited
						v1=VelocityOne(v1,0,0);
					else 
						v1=VelocityOne(v1,f,f);
					s+=v1;
				}while( v1 < -0.1 );

				s-=v1;

				if(s > -distance)
				{//不满足减速条件加速
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v < -SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//减速一步
						if( v1 > 0 )
						{
							do{//该降低功率了
								f--;		//f=-vmax;
								v1 = VelocityOne(v,f,f);
							}while( v1 > -distance && f > -vmax);	
						}
						Velocity(env,robot,f,f);
					}
				}
			}	
		}
	}
}


//PositionBallX  让robot  跑到pos附近 如果球在小车limit的距离内用Xangle的角速度旋转
void PositionBallX(Environment *env,int  robot,Vector3D pos ,double Xangle,double limit) 
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//控制转交速度的变量
	double vmax=125;
	double Limitedangle=2;

	if( limit <2.8 )
		limit =2.8;
	double Limiteddis=limit;
	
	double  distance;
	double turnangle,posangle,vangle;
	double dx,dy;
	double a=SPEED_A;
	double b=SPEED_B;
	double v;
	double f=vmax;
	bool face=true;			//判断小车是否是正面前进
	bool turnornot=false ;	//是否旋转,临时变量

	v= sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	
	dx = pos.x - p->robot[robot].pos.x ;
	dy = pos.y - p->robot[robot].pos.y ;
	
	distance = Distance(p->robot[robot].pos , pos);
	posangle = Atan(dy,dx);

	turnangle = p->robot[robot].rotation - posangle;		//think more!!
	RegulateAngle(turnangle);

	if(turnangle > 90)
	{
		face=false;
		turnangle-=180;
	}
	else if(turnangle < -90)
	{
		face=false;
		turnangle+=180;
	}
	else 
	{
		face=true;
	}

	vangle = p->myspeed[robot].z - p->robot[robot].rotation;
	RegulateAngle(vangle);


	if(distance <  3.2)
		turnornot = true;
	else if(distance < 3.5 && v > 0.5 )
		turnornot = true ;
	else if(distance < 4.5 && v > 0.8)
		turnornot = true ;

	if(distance > Limiteddis  )	//不在旋转范围内  则不转
		turnornot = false ;
	
	if(turnornot)
	{//满足条件 转!!!
		Velocity(env,robot,-Xangle,Xangle);
	}//否则跑位
	else if(turnangle > Limitedangle || turnangle < -Limitedangle)	
	{//adjust angle
		/////////////////测试这一段
		if(turnangle > 60  || turnangle < -60)
			anglespeedmax = 0;
		else if(turnangle > 30  || turnangle < -30)
			anglespeedmax = 100;
		else if(turnangle > 10  || turnangle < -10)
			anglespeedmax = 150;
		else 
			anglespeedmax = 200;
		///////////////测试这一段
		PAngle(env,robot,posangle,anglespeedmax);

	}
	else
	{
		if(face)
			Velocity(env,robot,f,f);
		else
			Velocity(env,robot,-f,-f);
	}//it is time to rush

}


//PositionAndThrough 让robot以最快MAX 冲向pos，中间没有减速控制
void PositionAndThrough(Environment *env,int robot,Vector3D pos ,double MAX)
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//控制转交速度的变量
	double max=MAX;
	double Limitedangle=2;
	double Limiteddis=0;
	double  distance;
	double turnangle,posangle,vangle;
	double dx,dy;
	double a=SPEED_A;
	double b=SPEED_B;
	double v,v1;
	double f;
	double s=0;
	int n=0;
	bool face=true;			//判断小车是否是正面前进

	v= sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	
	dx = pos.x - p->robot[robot].pos.x ;
	dy = pos.y - p->robot[robot].pos.y ;
	
	distance = Distance(p->robot[robot].pos , pos);
	posangle = Atan(dy,dx);

	turnangle = posangle - p->robot[robot].rotation;		//think more!!
	RegulateAngle(turnangle);

	if(turnangle > 90)
	{
		face=false;
		turnangle-=180;
	}
	else if(turnangle < -90)
	{
		face=false;
		turnangle+=180;
	}
	else 
	{
		face=true;
	}

	vangle = p->myspeed[robot].z - posangle;
	RegulateAngle(vangle);
	if( vangle <-90 || vangle > 90 )
		v=-v;
	v1=v;

	if(distance > Limiteddis)
	{//it is too early to count the steps
		if(turnangle > Limitedangle || turnangle < -Limitedangle)
		{//adjust angle
			/////////////////测试这一段
			if(turnangle > 20  || turnangle < -20)
				anglespeedmax = 0;
			else if(turnangle > 10  || turnangle < -10)
				anglespeedmax = 125;
			else if(turnangle > 5  || turnangle < -5)
				anglespeedmax = 180;
			else 
				anglespeedmax = 200;
			///////////////测试这一段
			PAngle(env,robot,posangle,anglespeedmax);

		}
		else
		{
			f=max;
			if(face)
				Velocity(env,robot,f,f);
			else
				Velocity(env,robot,-f,-f);

		}//it is time to rush

	}
	else
	{


	}//abserlutely count
}

//门将


//PredictBall	预测经过 steps 个周期之后球的位置 经过 steps 个周期之后球的位置 考虑撞墙的过程//很重要的一个函数
void PredictBall(Environment *env,int steps)
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	Vector3D predictball;
	Vector3D ballspeed;
	double  a,b;     //a 为路程 //b为时间
    int i=0;


	predictball.x = p->curball.x;			//赋初值
	predictball.y = p->curball.y;

	ballspeed.x = p->ballspeed.x ;
	ballspeed.y = p->ballspeed.y ;
	ballspeed.z = p->ballspeed.z ;
	
	for(i=0;i<steps;i++)
	{
		predictball.x += ballspeed.x ;
		predictball.y += ballspeed.y ;
//处理撞墙
		if( predictball.x > FRIGHT &&( predictball.y<33.9320 || predictball.y>49.6801 ))//       是不 是有问题哦？？？？？？？
		{
			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;
            
			a=ballspeed.y/ballspeed.x*(FRIGHT-predictball.x);    //寻找撞墙的点，以前是用不撞墙的点
			predictball.x=FRIGHT;
			predictball.y += a;
            
			b=1-a/ballspeed.y;
		
			ballspeed.x *=-SPEED_NORMAL;	//loose      改变撞墙后的速度
			ballspeed.y *= SPEED_TANGENT;     

			predictball.x += (b*ballspeed.x) ;	//go on      用撞墙点寻找预测点
			predictball.y += (b*ballspeed.y) ;

		}
		else if( predictball.x < FLEFT &&( predictball.y<33.9320 || predictball.y>49.6801 ) )
		{
			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			a=ballspeed.y/ballspeed.x*(FLEFT-predictball.x);
			predictball.x=FLEFT;
			predictball.y += a;

			b=1-a/ballspeed.y;
			
			ballspeed.x *=-SPEED_NORMAL;	//loose 
			ballspeed.y *= SPEED_TANGENT;

			predictball.x += (b*ballspeed.x) ;	//go on      用撞墙点寻找预测点
			predictball.y += (b*ballspeed.y) ;
		}
		else if( predictball.y < FBOT )
		{
			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			a=ballspeed.x/ballspeed.y*(FBOT-predictball.y);
			predictball.y=FBOT;
			predictball.x += a;

			b=1-a/ballspeed.x;
			
			ballspeed.x *= SPEED_TANGENT;	//loose 
			ballspeed.y *=-SPEED_NORMAL;

			predictball.x += (b*ballspeed.x) ;	//go on      用撞墙点寻找预测点
			predictball.y += (b*ballspeed.y) ;

		}
		else if( predictball.y > FTOP)
		{
			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			a=ballspeed.x/ballspeed.y*(FTOP-predictball.y);
			predictball.y=FTOP;
			predictball.x += a;

			b=1-a/ballspeed.x;
			
			ballspeed.x *= SPEED_TANGENT;	//loose 
			ballspeed.y *=-SPEED_NORMAL;

			predictball.x += (b*ballspeed.x) ;	//go on      用撞墙点寻找预测点
			predictball.y += (b*ballspeed.y) ;

		}

		if( predictball.x + predictball.y > FRIGHT +FTOP -CORNER )
		{//右上
			double vx,vy;	
			vy= rua*ballspeed.y + rua*ballspeed.x;	//变换1
			vx=-rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy - rua * vx;	//变换2
			ballspeed.x = rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;

		}
		else if( predictball.x + predictball.y < FLEFTX +FBOT+CORNER)
		{//左下
			double vx,vy;	
			vy= rua*ballspeed.y + rua*ballspeed.x;	//变换1
			vx=-rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy - rua * vx;	//变换2
			ballspeed.x = rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;	
		}
		else if( predictball.x - predictball.y > FRIGHT -FBOT -CORNER)
		{//右下
			double vx,vy;	
			vy= rua*ballspeed.y - rua*ballspeed.x;	//变换1
			vx= rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy + rua * vx;	//变换2
			ballspeed.x = -rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;			
		}
		else if( predictball.y - predictball.x > FTOP - FLEFT-CORNER)
		{//左上
			double vx,vy;	
			vy= rua*ballspeed.y - rua*ballspeed.x;	//变换1
			vx= rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy + rua * vx;	//变换2
			ballspeed.x = -rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;			
		}

//处理四角		
	}

	p->preball.x = predictball.x ;
	p->preball.y = predictball.y ;
	p->preball.z = Atan( ballspeed.y ,ballspeed.x );
}


//让robot踢球到ToPos
void Kick(Environment *env, int robot, Vector3D ToPos)//ChaseAndKick
{
	Mydata * p;
	p = (Mydata *)env->userData;

	double RunAngle;

	Vector3D RobotToBall;		//人和球的相对位置
	RobotToBall.x = p->curball.x - p->robot[robot].pos.x;
	RobotToBall.y = p->curball.y - p->robot[robot].pos.y;
	RobotToBall.z = Atan(p->robot[robot].pos, p->curball);//从球员到球的向量与X轴夹角

	Vector3D BallToPos;		//球和球门的相对位置
	BallToPos.x = ToPos.x - p->curball.x;
	BallToPos.y = ToPos.y - p->curball.y;
	BallToPos.z = Atan(p->curball, ToPos);

	if (fabs(RobotToBall.z - BallToPos.z) <= 75)
	{//正向角，可直接加速
		RunAngle = RobotToBall.z - BallToPos.z;
		RegulateAngle(RunAngle);
		RunAngle = RobotToBall.z + RunAngle / 2;	// 可以调整  2 
		RegulateAngle(RunAngle);
		PAngle(env, robot, RunAngle, 125);
	}
	else if (fabs(RobotToBall.z - BallToPos.z)>75 && fabs(RobotToBall.z - BallToPos.z) <= 120)
	{//侧向角，小幅绕后
		RunAngle = RobotToBall.z - BallToPos.z;
		RegulateAngle(RunAngle);
		RunAngle = RobotToBall.z + RunAngle / 1.5;	// 可以调整  1.5 
		RegulateAngle(RunAngle);
		PAngle(env, robot, RunAngle, 125);
	}
	else
	{//反向角，侧向绕以达到侧向角标准
		RunAngle = BallToPos.z + 180;
		RegulateAngle(RunAngle);
		RunAngle = RunAngle + (RobotToBall.z - BallToPos.z) / 4;
		RegulateAngle(RunAngle);
		PAngle(env, robot, RunAngle, 125);
	}

}


//让robot踢球到robot1
void Kick(Environment *env , int  robot ,int robot1)//踢人
{
	Mydata * p;
	p=(Mydata *)env->userData;
	Vector3D RobotToBall;		//人和球的相对位置
	RobotToBall.x = p->robot[robot1].pos.x- p->robot[robot].pos.x ;
	RobotToBall.y = p->robot[robot1].pos.y- p->robot[robot].pos.y ;
	RobotToBall.z = Atan(p->robot[robot].pos , p->robot[robot1].pos);
	
	Vector3D BallToGate ;		//球和球门的相对位置
	BallToGate.x = CONSTGATE.x- p->robot[robot1].pos.x;
	BallToGate.y = CONSTGATE.y- p->robot[robot1].pos.y;
	BallToGate.z = Atan(p->robot[robot1].pos,CONSTGATE);
	
	double RunAngle ;
	RunAngle = RobotToBall.z - BallToGate.z;
	RegulateAngle(RunAngle);
	
	RunAngle = RobotToBall.z + RunAngle /2 ;	// 可以调整  2 
	RegulateAngle(RunAngle);

	PAngle(env,robot,RunAngle,125);

}


//让robot steps步以后旋转？
void Kick(Environment *env,int robot,int steps,double limits){

	Mydata *p=(Mydata*)env->userData;
	double dx,dy,angle;
    
	dx=p->curball.x-p->robot[robot].pos.x;
	dy=p->curball.y-p->robot[robot].pos.y;
	angle=Atan(dy,dx);
    PredictBall(env,steps);

	if(angle<90&&angle>-90){
		if(p->curball.y>41.8)
	      PositionBallX(env,robot,p->preball,-125,3);
		else
		  PositionBallX(env,robot,p->preball,125,3);
	}
	else 
      shoot(env,robot);
}


//小车2相对于小车1的位置 别看反了哦oooooo~
int RobotRobotCheck(Environment*env, int robot1, int robot2)
{

	Mydata * p;
	p = (Mydata *)env->userData;
	Vector3D robot1pos = p->robot[robot1].pos;
	Vector3D robot2pos = p->robot[robot2].pos;
	if ((fabs(robot1pos.x - robot2pos.x)) < CAR / 1.5 && (robot2pos.y > robot1pos.y))
	{
		return UP;
	}
	else if ((fabs(robot1pos.x - robot2pos.x))<CAR / 1.5 && (robot2pos.y<robot1pos.y))
	{
		return DOWN;
	}
	else if ((fabs(robot1pos.y - robot2pos.y))<CAR / 1.5 && (robot2pos.x>robot1pos.x))
	{
		return RIGHT;
	}
	else if ((fabs(robot1pos.y - robot2pos.y)) <CAR / 1.5 && (robot2pos.x < robot1pos.x))
	{
		return LEFT;
	}
	else if (robot1pos.x < robot2pos.x&&robot1pos.y < robot2pos.y)
	{
		return RIGHT_UP;
	}
	else if (robot1pos.x < robot2pos.x&&robot1pos.y > robot2pos.y)
	{
		return RIGHT_DOWN;
	}
	else if (robot1pos.x > robot2pos.x&&robot1pos.y > robot2pos.y)
	{
		return LEFT_DOWN;
	}
	else if (robot1pos.x > robot2pos.x&&robot1pos.y < robot2pos.y)
	{
		return LEFT_UP;
	}

}


//小球相对于车的位置 别看反了哦oooooo~
int RobotBallCheck(Environment*env, int robot)
{

	Mydata * p;
	p = (Mydata *)env->userData;
	Vector3D robotpos = p->robot[robot].pos;
	Vector3D ballpos = p->preball;
	if ((fabs(robotpos.x - ballpos.x)) <  CAR &&(ballpos.y > robotpos.y))
	{
		return UP;
	}
	else if ((fabs(robotpos.x - ballpos.x))<CAR / 2 && (ballpos.y<robotpos.y))//3.5!!!!!
	{
		return DOWN;
	}
	else if ((fabs(robotpos.y - ballpos.y))<CAR / 2 && (ballpos.x>robotpos.x))
	{
		return RIGHT;
	}
	else if ((fabs(robotpos.y - ballpos.y)) < CAR / 2 && (ballpos.x < robotpos.x))
	{
		return LEFT;
	}
	else if (robotpos.x < ballpos.x&&robotpos.y < ballpos.y)
	{
		return RIGHT_UP;
	}
	else if (robotpos.x < ballpos.x&&robotpos.y > ballpos.y)
	{
		return RIGHT_DOWN;
	}
	else if (robotpos.x > ballpos.x&&robotpos.y > ballpos.y)
	{
		return LEFT_DOWN;
	}
	else if (robotpos.x > ballpos.x&&robotpos.y < ballpos.y)
	{
		return LEFT_UP;
	}

}


//射门 不好使不建议用
void shoot(Environment *env, int robot) {

	Mydata *p = (Mydata *)env->userData;
	double w1, w2, alfa;
	double dx, dy;

	if (p->curball.y>GBOT&&p->curball.y <= (GBOT + GTOP) / 2) {//已修改 原为if(p->curball.y>GBOT&&p->curball.y<=GTOP/2){

		if (p->ballspeed.z<85 && p->ballspeed.z<95)
			PositionBallX(env, robot, p->curball, -90, 4);

		else if (p->ballspeed.z<-85 && p->ballspeed.z>-95)
			PositionBallX(env, robot, p->curball, 90, 4);
	}

	else if (p->curball.y >= (GBOT + GTOP) / 2 && p->curball.y <= GTOP) {//已修改

		if (p->ballspeed.z<85 && p->ballspeed.z<95)
			PositionBallX(env, robot, p->curball, -90, 4);

		else if (p->ballspeed.z<-85 && p->ballspeed.z>-95)
			PositionBallX(env, robot, p->curball, 90, 4);
	}

	if (p->robot[robot].pos.x <= p->curball.x) {

		// PredictBall(env,2);  
		dx = GRIGHT - p->robot[robot].pos.x;
		dy = GTOP - p->robot[robot].pos.y;
		w1 = Atan(dy, dx);

		dx = GRIGHT - p->robot[robot].pos.x;
		dy = GBOT - p->robot[robot].pos.y;
		w2 = Atan(dy, dx);

		dx = p->preball.x - p->robot[robot].pos.x;
		dy = p->preball.y - p->robot[robot].pos.y;
		alfa = Atan(dy, dx);

		if ((w1 - alfa)*(w2 - alfa) <= 0)
			PAngle(env, robot, alfa, 125);

		else if (p->curball.y<FBOT + 4 && p->robot[robot].pos.y<FBOT + 4)
			Kick(env, robot, BOTGATE);

		else if (p->curball.y>FTOP - 10 && p->robot[robot].pos.y>FBOT - 10)
			Kick(env, robot, TOPGATE);
		else
			Kick(env, robot, CONSTGATE);

	}
	else
		Kick(env, robot, CONSTGATE);
}


//判断是否能射门，上个函数里头用
bool shoot1(Environment *env, int robot) {

	Mydata *p = (Mydata *)env->userData;
	double count;
	int i = 1, k;
	double angle;
	double dx, dy;

	if (p->ballspeed.y >= 0.5 || p->ballspeed.y <= -0.5) {

		if (p->ballspeed.y>0) {

			count = (GTOP - p->curball.y) / p->ballspeed.y;
		}
		else {
			count = fabs((p->curball.y - GBOT) / p->ballspeed.y);
		}
	}

	if (count >= 600)
		count = 600;

	while (i <= count) {

		PredictBall(env, i);
		k = Count(env, robot, p->preball);

		if (abs(i - k) <= 3)
			if (p->preball.y <= GTOP + 4 && p->preball.y >= GBOT - 4) {//调整角度准备射门
				break;
			}
		i++;
	}

	if (i <= count) {
		dx = p->preball.x - p->robot[robot].pos.x;
		dy = p->preball.y - p->robot[robot].pos.y;
		angle = Atan(dy, dx);
		PAngle(env, robot, angle, 125);
		return true;
	}
	else
		return false;
}

void PlayerInit(Environment *env,int Method)  //根据5v5足球战术配置球员初始位置
{
	const double Init1_2_1_X[5] = {12.5,32.5,45.5,45.5,52.5};
	const double Init1_2_1_Y[5] = {41.8,42.0,60.5,20.2,41.7};
	const double Init1_2_1_Z[5] = {0.0,0.0,0.0,0.0,0.0};
	env->userData = (void*) new Mydata;

	Mydata* p;
	p = (Mydata*)env->userData;
	int i;
	switch (Method)
	{
	case 1: // 1-2-1阵型
	{
		for (i = 0; i < 5; i++)
		{
			p->robot[i].pos.x = Init1_2_1_X[i];
			p->robot[i].pos.y = Init1_2_1_Y[i];
			p->robot[i].pos.z - Init1_2_1_Z[i];
			p->myoldpos[i].x = InlitializeMyPosition_X[i];
			p->myoldpos[i].y = InlitializeMyPosition_Y[i];
			p->myoldpos[i].z = InlitializeMyPosition_Z[i];
		}
	}
	break;
	//  给出其他形式阵型
	//
	default:break;
	}
}
