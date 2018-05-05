#include "strategy.h"
#include "vsv.h"
#include"iostream"
#include"tchar.h"

//������� dllmain dll����
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

//ֻ��������ڲ���֮�������������Ҫ����ֵ������Create �н��У����Բ����
extern "C" STRATEGY_API void Create(Environment *env)
{
	//��ʼ������
	env->userData = (void*) new Mydata;
	
	Mydata* p;
	p = (Mydata*)env->userData;


	//��ʼ����������Ϣ
	for (int i = 0; i < 5; i++)
	{
	//�ҷ�--------------------------------------------------------------------------------------------------

		//�ҷ����ڵ�λ�ó�ʼ��
		p->robot[i].pos.x = InlitializeMyPosition_X[i];
		p->robot[i].pos.y = InlitializeMyPosition_Y[i];
		p->robot[i].pos.z = InlitializeMyPosition_Z[i];			//Z�������û��
		//�ҷ���һ֡��λ�ó�ʼ��
		p->myoldpos[i].x = InlitializeMyPosition_X[i];
		p->myoldpos[i].y = InlitializeMyPosition_Y[i];
		p->myoldpos[i].z = InlitializeMyPosition_Z[i];			//Z��������һ֡С��������ķ���
		//�ҷ�ͨ����һ֡�����ڵ�λ�ü�������ٶ�
		p->myspeed[i].x = 0;
		p->myspeed[i].y = 0;
		p->myspeed[i].z = 0;
		//�ҷ���һ֡���ύ�ĳ���������
		p->myoldvelocity[i].x = 0;
		p->myoldvelocity[i].y = 0;
		p->myoldvelocity[i].z = 0;


	//�Է�--------------------------------------------------------------------------------------------------

		//�Է����ڵ�λ�ó�ʼ��
		p->opp[i].pos.x = InlitializeOppPosition_X[i];
		p->opp[i].pos.y = InlitializeOppPosition_Y[i];
		p->opp[i].pos.z = InlitializeOppPosition_Z[i];			//Z�������û��

		//�Է���һ֡��λ�ó�ʼ��
		p->opoldpos[i].x = InlitializeOppPosition_X[i];
		p->opoldpos[i].y = InlitializeOppPosition_Y[i];
		p->opoldpos[i].z = InlitializeOppPosition_Z[i];			//Z��������һ֡С��������ķ���
	
		//�Է�ͨ����һ֡�����ڵ�λ�ü�������ٶ�
		p->opspeed[0].x = 0;
		p->opspeed[0].y = 0;
		p->opspeed[0].z = 0;

	}
	PlayerInit(env,1 );  //�������ͳ�ʼλ�ã���������ѡ�񡣺���ԭ�ͣ�void PlayerInit(Environment *env,int Method)
	p->locked = false;				// ��������ʼ��
	p->mygrand = true;				// ���ظ���ֵ �����ǻƶ� ֮�����

	//С��--------------------------------------------------------------------------------------------------

	//С����һ֡��λ�ó�ʼ��
	p->oldball.x = (FLEFT + FRIGHT) / 2.0;
	p->oldball.y = (FTOP + FBOT) / 2.0;
	p->oldball.z = 0;                                           
	
    //С�����ڵ�λ�ó�ʼ��
	p->curball.x = (FLEFT + FRIGHT) / 2.0;
	p->curball.y = (FTOP + FBOT) / 2.0;
	p->curball.z = 0;											
	
    //С��ͨ������Ԥ���Ժ���ֵ�λ��
	p->preball.x = (FLEFT + FRIGHT) / 2.0;
	p->preball.y = (FTOP + FBOT) / 2.0;
	p->preball.z = 0;										//z�Ḵ��ΪС��������ٶȷ�����Ϣ�洢	
	
	//С��ͨ����һ֡�����ڼ����������
	p->ballspeed.x = 0;
	p->ballspeed.y = 0;
	p->ballspeed.z = 0;											

	p->penaltycount=0;
	p->initpenaltycount=0;
	p->penaltyangle = false;

	//�Ͻ��޸�
	HWND hwnd = GetActiveWindow();		            //��ȡ��ǰ������
	SetWindowTextA(hwnd, "bjut2018fira5v5����У��");//���Ĵ��ڱ���Ϊ����

}

/*Stategy  ���Ե���ڣ��ú�������ʽ�����޸ģ���������
���� Environment *env ������ȡ��ǰ�ı�����Ϣ��
����ͨ���޸� Environment.home.velocityLeft, Environment.home.velocityRight��ֵ�����ƻ����˵��˶�
���幤����
	See(env);			// Ԥ����
	Action(env);		//����
	End ( env );		//���ڴ���
	�ֵ���
*/
extern "C" STRATEGY_API void Strategy ( Environment *env )
{
	Mydata * p;
	p=(Mydata *)env->userData;

	if(!p->locked)		//�жϳ��ص���
	{//ȷ������
		if( env->home[0].pos.x < 50.0 )
			p->mygrand=true; // �ǻƶ�
		else
			p->mygrand=false;//������
		p->locked=true;//��ס
	}
			// Ԥ����	
	See(env);

			 //����
	Action(env);
	
			 //���ڴ���
	End ( env );		
}

//��������ʱ�������� ����� �������
extern "C" STRATEGY_API void Destroy(Environment *env)
{
	Mydata * p;
	p = (Mydata *)env->userData;

	if (env->userData != NULL)	delete (Mydata *)env->userData;
}


//See����ÿһ֡��ִ�� ����ÿһ֡Ԥ������� ��������任 ���һЩ�����о����õ���������ÿ����Ա�����ʺͽ��ٶȵ�
void See ( Environment *env )
{
	Mydata * p;
	p=(Mydata *)env->userData;

	p->ballArea = CheckBall(env);//��鳡��
	
//����λ��----------------------------------------------------------------------------------------------------
	int i = 0;
	if(p->mygrand)//������ҷ��ǻƶӵĻ�
	{

		p->gameState = env->gameState ;				//��ϵͳ������Ϸ״̬
		p->curball.x = env->currentBall.pos.x;		//��ϵͳ����С������
		p->curball.y = env->currentBall.pos.y;

		//��ϵͳ����С������������Ƕ�
		for(i=0;i<5;i++)
		{
			//�ҷ�
			p->robot[i].pos.x = env->home[i].pos.x ;	
			p->robot[i].pos.y = env->home[i].pos.y ;
			p->robot[i].rotation= env->home[i].rotation;
			RegulateAngle(p->opp[i].rotation);//�淶�Ƕ� 
			//�Է�
			p->opp[i].pos.x =env->opponent[i].pos.x;	
			p->opp[i].pos.y =env->opponent[i].pos.y;
			p->opp[i].rotation =env->opponent[i].rotation;
			RegulateAngle(p->opp[i].rotation);//�淶�Ƕ� 

		}
	}//ԭ���겻�䣬���Ƿ�����ߣ��ƶӣ�����ġ�

	else//���������
	{
		p->gameState = env->gameState;				//��ϵͳ������Ϸ״̬

		
		//��ϵͳ����С������ ��Ϊ�������ҳ� ��Ҫ�����ת�� ת���Ժ� ����ֻ��Ҫ���ջƶ�д ����ͨ��
		
		p->curball.x =FLEFT+FRIGHT + CORRECTX - env->currentBall.pos.x;		//������仯
		p->curball.y =FBOT+FTOP + CORRECTY - env->currentBall.pos.y;
		//������Z��

		
		//С��ͬ��
		for(i=0;i<5;i++)
		{
			//�ҷ�
			p->robot[i].pos.x =FLEFT+FRIGHT + CORRECTX - env->home[i].pos.x ;	//�ҷ���Ա����任
			p->robot[i].pos.y =FBOT+FTOP + CORRECTY - env->home[i].pos.y ;
			p->robot[i].rotation= 180.0 + env->home[i].rotation;
			RegulateAngle(p->robot[i].rotation);
			//�Է�
			p->opp[i].pos.x = FLEFT+FRIGHT + CORRECTX- env->opponent[i].pos.x;	//�Է�����任
			p->opp[i].pos.y = FBOT+FTOP + CORRECTY - env->opponent[i].pos.y;
			p->opp[i].rotation =  180 + env->opponent[i].rotation;
			RegulateAngle(p->robot[i].rotation);

			//���ܵĻ����Լ���С���Ľ��ٶȣ�������


		}
	}

//�����ٶ�----------------------------------------------------------------------------------------------------
	
	//С���ٶ�
	for(i=0;i<5;i++)
	{
		//�ҷ�
		p->myspeed[i].x = ( p->robot[i].pos.x - p->myoldpos[i].x);	
		p->myspeed[i].y = ( p->robot[i].pos.y - p->myoldpos[i].y);
		p->myspeed[i].z = Atan(p->myspeed[i].y,p->myspeed[i].x);	//С���˶�����
		//�Է�
		p->opspeed[i].x = ( p->opp[i].pos.x - p->opoldpos[i].x);
		p->opspeed[i].y = ( p->opp[i].pos.y - p->opoldpos[i].y);
		p->opspeed[i].z = Atan(p->opspeed[i].y,p->opspeed[i].x);	//С���˶�����

		
	}


	//С���ٶ�
	p->ballspeed.x = p->curball.x - p->oldball.x;
	p->ballspeed.y = p->curball.y - p->oldball.y;
	p->ballspeed.z = Atan( p->ballspeed.y , p->ballspeed.x );   //С���˶�����

	//=================================================================================================
	do {
		//���Ԥ��һ֡��

		//��Ҳ��֪��ΪʲôҪ���Ԥ�⣬����ɾ���Ժ�С�����Ҷ� ˭Ҫ���ܰ����ĺþ͸ġ�����by ��һ͢
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
			///����ײǽ
			//���������

			////����ײǽ		
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


		PredictBall(env);		//���������λ��
		p->curball = p->preball;

		p->oldball.x = x;
		p->oldball.y = y;

		PredictBall(env);		//Ԥ����һ�����λ��





		p->ballspeed.x = p->curball.x - p->oldball.x;
		p->ballspeed.y = p->curball.y - p->oldball.y;
		p->ballspeed.z = Atan(p->ballspeed.y, p->ballspeed.x);

	

	} while (0);
}


//Action����ÿһ֡��ִ�� �Բ�ͬ��GameState ���벻ͬ��Ϸ״̬
void Action(Environment *env)			
{
	Mydata * p;
	p = (Mydata *)env->userData;

	static bool ourpenalty = false;			//����
	static bool Freekick = false;			//������
	static bool Freeball = false;			//����
	static bool Placeball = false;			//�г�����
	static bool Goalkick = false;			//����



	static clock_t count = 0;	//�����ų���һ֡����硱������ ��һ����ʱ��//�κ���
	static clock_t start = count;

	static bool choosepenalty = false;
	static bool panel1 = false, panel2 = false, panel3 = false, panel4 = false;

	static int goalcount = 0;
	static int freecount = 0;
	static int placecount = 0;
	static int penaltycount = 0;
	switch (p->gameState)				//��״̬�ж�Ŀǰ�����Լ������� �����������ʱ���û���ж��� ֱ����normalgame
	{
	case FREE_KICK:
		//�򵥲���ԭ�� ʱ��Ϊ���� ����л�gamestate�Ļ���Ȼ��Ҫ����ʱ��ȥ��λ�� ÿһ֡ѭ����ʱ�򶼶�start����ʱ
		//�����֡��start����֡��start ���������count=start-5,��ʵ������count=start������5000���룬Ҳ����5�루5�ɸģ�����ô���ǿ���˵����һ�����µĿ�ʼ��
		//�������ǵ��� �������������� ���������� ��Ϊ������ǲ�����ť����״̬ ����������߷ɺ���Ȼ�ǵ���״̬��������Ҫȥִ�С�Normalgame��
		//��ע��1sִ��60�� Ҳ����1s��60֡  sht
		start = clock();
		if (start - count >= 5000 || 0 == count)
		{
			See(env);   //��ܵ�һ֡�����쳣 
			count = start;
			freecount = 0;//ѡ������ֵĿ��ؿ���
		}
		count = start - 5;  //5�����д��

		if (
			p->curball.x < 78 + 3 && p->curball.x>78 - 3 && p->curball.y > 41 - 3 && p->curball.y < 41 + 3
			&& p->robot[1].pos.x < p->curball.x &&p->robot[2].pos.x < p->curball.x &&p->robot[3].pos.x < p->curball.x &&p->robot[4].pos.x < p->curball.x
			&&freecount == 0
			)// ���ڵ���� 1-4�ų�С���������
		{
			Freekick = true;
		}
		if (freecount>30)//�ж��˳�����			����80֡		
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




	case FREE_BALL:	// Free ball tm��������
		start = clock();
		if (start - count >= 5000 || 0 == count)
		{
			See(env);   //��ܵ�һ֡�����쳣 
			count = start;
		}
		count = start - 5;  //5�����д��
		if (p->curball.x > 71 - 3 && p->curball.x < 71 + 3)//��������x							//1 2 ����߰볡��4��������������°볡   
		{//�ҷ�
			if (p->curball.y > 65 - 3 && p->curball.y < 65 + 3)//��������y
			{//���Ϸ������
				if ((p->robot[3].pos.x > 60 - 3 && p->robot[3].pos.x < 60 + 3)
					&& (p->robot[3].pos.y > 65 - 3 && p->robot[3].pos.y < 65 + 3)
					&& p->robot[1].pos.x < 50.1158	&&	p->robot[2].pos.x < 50.1158  &&  p->robot[4].pos.y < 41.7038
					)
					//���robot����
					Freeball = true;
			}
		else if (p->curball.y > 18 - 3 && p->curball.y < 18 + 3)//��������y							  //�����ʱ��4������	
			{//���·������																				  //1��2����߰볡��3��������������ϰ볡	�ϰ볡��ʱ����΢�ж���Щ��
				if ((p->robot[2].pos.x > 60 - 3 && p->robot[2].pos.x < 60 + 3)
					&& (p->robot[2].pos.y > 18 - 3 && p->robot[2].pos.y < 18 + 3)
					&& p->robot[1].pos.x < 50.1158	&&	p->robot[2].pos.x<50.1158  &&  p->robot[3].pos.y>41.7038
					)
					Freeball = true;
			}
		}
		else if (p->curball.x > 29 - 3 && p->curball.x < 29 + 3)
		{//��																							  //�����ʱ��1������
			if (p->curball.y > 65 - 3 && p->curball.y < 65 + 3)//��������y								  //2 4 ��������볡�·� ��3�Ų����ж�Ҫ��
			{//���Ϸ������
				if ((p->robot[1].pos.x > 18 - 3 && p->robot[1].pos.x < 18 + 3)
					&& (p->robot[1].pos.y > 65 - 3 && p->robot[1].pos.y < 65 + 3)
					&& p->robot[2].pos.y < 41.7038	&&p->robot[4].pos.y < 41.7038
					)
					Freeball = true;
			}
			else if (p->curball.y > 18 - 3 && p->curball.y < 18 + 3)//��������y							//�����ʱ��2������
			{//���·������
				if ((p->robot[2].pos.x > 18 - 3 && p->robot[2].pos.x < 18 + 3)							//1 3 �ڰ볡�Ϸ� ��4�Ų���Ҫ��
					&& (p->robot[2].pos.y > 19 - 3 && p->robot[2].pos.y < 19 + 3) && (p->robot[1].pos.x < p->robot[3].pos.x)
					&& p->robot[1].pos.y > 41.7038	&&p->robot[3].pos.y > 41.7038
					)
					Freeball = true;
			}
		}
		if (sqrt(pow(p->ballspeed.x, 2) + pow(p->ballspeed.y, 2)) >0.3)  //�˳��ж� Ӧ�ò�׼ȷ 0.3���Ը�
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
			See(env);   //��ܵ�һ֡�����쳣 
			count = start;
			placecount = 0;
		}
		count = start - 5;  //5�����д��

		if (p->curball.x < 50.1158 + 3 && p->curball.x>50.1158 - 3 && p->curball.y > 41.7038 - 3 && p->curball.y < 41.7038 + 3 && placecount == 0)
			// �������ĵ� 1 2 3 ���ڼ����볡 4 ���ڶ���볡���� ���ص�ʱ��ȫ�ڼ����볡 ���幥�ط����Լ�������PlaceBallGame�����з���
		{
			Placeball = true;
		}
		if (placecount>35)//�ж��˳�����			����80֡		
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
			See(env);   //��ܵ�һ֡�����쳣 
			count = start;
			choosepenalty = true;  //ѡ������ֵĿ��ؿ���
			penaltycount = 0;
		}
		count = start - 5;  //5�����д��

		if (p->curball.x < 78 + 3 && p->curball.x>78 - 3 && p->curball.y > 41 - 3 && p->curball.y < 41 + 3)
		{
			int robotnum = 0;
			for (int i = 0; i < 5; i++)
				if (p->robot[i].pos.x <= 51)  //Ӧ��Ϊ�г������� �ſ��
					robotnum++;
			if (robotnum >= 4 && choosepenalty == true)
			{
				ourpenalty = true;
				if (p->robot[1].pos.x >= 50.3)//���أ�1��վ�ڵ���λ
					panel1 = true;
				else if (p->robot[2].pos.x >= 50.3)//���أ�2��վ�ڵ���λ
					panel2 = true;
				else if (p->robot[3].pos.x >= 50.3)//���أ�3��վ�ڵ���λ
					panel3 = true;
				else if (p->robot[4].pos.x >= 50.3)//���أ�4��վ�ڵ���λ
					panel4 = true;
				choosepenalty = false;
			}
		}
		else if (p->curball.x > 93.4259 || (p->curball.x >= 93.4259 - 5 && p->ballspeed.x < 0.1) || p->ballspeed.x<-0.1)//goal or our ball was blocked by the enemy
		{

			penaltycount = 0;
			ourpenalty = false;//�������ҷ�����׶�	

		}
		if (ourpenalty)
		{
			//�ж��������Ե�ʱ�����������
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
			See(env);   //��ܵ�һ֡�����쳣 
			count = start;
			goalcount = 0;
		}
		count = start - 5;  //5�����д��

							//�����ж� 1���ڽ��� ������Ա�ڶ���볡 �Լ�С���ٶȶ��ܵ� ����ֻ���Լ�һ������Ա
		if (p->curball.x > 6.8118&&p->curball.x<13.5753&& p->curball.y>31.6176&&p->curball.y < 51.51125
			&&goalcount == 0
			&& OnlyGoalieInMenqu(env) == true)//�ж���������
		{
			Goalkick = true;
		}
		//29.26 x   18.18 y
		if (goalcount>30)//�ж��˳�����			����80֡		
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


//End����ÿһ֡��ִ�� �ύ�Զ�Ա���ٵ��޸� ���汾���ڵ�״̬���´μ���ʹ��

void End(Environment *env)
{
	///��һЩ��ɨ�Ĺ���
	///��һЩ��¼������

	Mydata * p;
	p = (Mydata *)env->userData;
	int i = 0;

	//�ύ����
	for (i = 0; i < 5; i++)
	{
		env->home[i].velocityLeft = p->robot[i].velocityLeft;
		env->home[i].velocityRight = p->robot[i].velocityRight;
	}

	//��¼�ҷ�����
	for (i = 0; i < 5; i++)
	{
		p->myoldvelocity[i].x = p->robot[i].velocityLeft;
		p->myoldvelocity[i].y = p->robot[i].velocityRight;
	}

	//������һ����Ҳ��֪��ΪʲôԤ��һ���������������ϢҪ��env�����»�ȡ������dyt
	if (p->mygrand)//���ǻƷ�
	{
		p->oldball.x = env->currentBall.pos.x;		//������仯
		p->oldball.y = env->currentBall.pos.y;


		for (i = 0; i<5; i++)
		{
			p->myoldpos[i].x = env->home[i].pos.x;	//�ҷ���Ա����任
			p->myoldpos[i].y = env->home[i].pos.y;
			p->myoldpos[i].z = env->home[i].rotation;
			RegulateAngle(p->opoldpos[i].z);

			p->opoldpos[i].x = env->opponent[i].pos.x;	//�Է�����任
			p->opoldpos[i].y = env->opponent[i].pos.y;
			p->opoldpos[i].z = env->opponent[i].rotation;
			RegulateAngle(p->opoldpos[i].z);

		}
	}

	else//��������
	{
		p->oldball.x = FLEFT + FRIGHT + CORRECTX - env->currentBall.pos.x;		//������仯
		p->oldball.y = FBOT + FTOP + CORRECTY - env->currentBall.pos.y;

		for (i = 0; i<5; i++)
		{
			p->myoldpos[i].x = FLEFT + FRIGHT + CORRECTX - env->home[i].pos.x;	//�ҷ���Ա����任
			p->myoldpos[i].y = FBOT + FTOP + CORRECTY - env->home[i].pos.y;
			p->myoldpos[i].z = 180.0 + env->home[i].rotation;
			RegulateAngle(p->myoldpos[i].z);

			p->opoldpos[i].x = FLEFT + FRIGHT + CORRECTX - env->opponent[i].pos.x;	//�Է�����任
			p->opoldpos[i].y = FBOT + FTOP + CORRECTY - env->opponent[i].pos.y;
			p->opoldpos[i].z = 180 + env->opponent[i].rotation;
			RegulateAngle(p->opoldpos[i].z);
		}
	}

}


// CheckBall	���������ڵķ�����
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

//���ߺ���=======================================================================


//RegulateAngle �淶angle�Ĵ�С�ڣ�-180��+180��֮��
void RegulateAngle(double &angle)
{
	while (angle >= 180.0)angle -= 360.0;
	while (angle < -180.0)angle += 360.0;
}


//toint ת�������� ���������룩
int toint(double num)
{
	return num * 10 < (int)num * 10 + 5 ? (int)num : (int)num + 1;
}
 
 
//Atan ��y/x����ֵ��Ӧ�ĽǶȣ�-180��+180��֮��
double Atan(double y, double x)
{
	if (x != 0.0 || y != 0.0)
		return 180 * atan2(y, x) / PI;
	else return 0.0;
}


//Atan (end-begin)ʸ���ĽǶȣ���-180��+180��֮��
double Atan(Vector3D begin, Vector3D end)
{
	double y, x;
	y = end.y - begin.y;
	x = end.x - begin.x;
	return Atan(y, x);
}


//Distance  ����֮�����
double Distance(Vector3D pos1, Vector3D pos2)
{
	return sqrt((pos1.x - pos2.x)*(pos1.x - pos2.x) + (pos1.y - pos2.y)*(pos1.y - pos2.y));
}

//AngleOne �����ڵ�ǰ���ٶ�omiga�Ļ���������������vl,vr���ƣ���һ�����ڴﵽ�Ľ��ٶ�
double AngleOne(double omiga, double vl, double vr)
{
	//		omiga = p->robot[i].rotation - p->myoldpos[i].z ;
	//		RegulateAngle(omiga);
	if (vl>125)vl = 125;
	if (vl<-125)vl = -125;
	if (vr>125)vr = 125;
	if (vr<-125)vr = -125;
	double angle = (vr - vl) / 2;

	RegulateAngle(omiga);		//Ӧ��û�д���180 �Ľ��ٶȰ�

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

	RegulateAngle(omiga);		//Ӧ��û�д���180 �Ľ��ٶȰ�

	return omiga;

}


//VelocityOne �����ڵ�ǰ�ٶ�speed�Ļ���������������vl,vr���ƣ���һ�����ڴﵽ���ٶ�
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


//Count ����robot�ܵ�posλ�õ�������
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
	do {//��λ�Ĳ���
		//��û�п��Ƿ��� ����ô����  
		v = VelocityOne(v, 110, 110);
		s += v;
		n++;
	} while (s < distance);
	n--;
	return n;
}


//�����ٵ�����ÿ֡����Ϊ����ÿ��
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


//Meetball_p	���robot׷��������λ��
Vector3D Meetball_p(Environment *env, int robot)
{//���robot׷�����λ��
	Mydata * p;
	p = (Mydata *)env->userData;

	Vector3D meetpoint = { 0,0,-1 };
	double dis = Distance(p->curball, p->robot[robot].pos);

	double t = 0;
	double vb = 0;
	double v = 1.9;		//��������ٶȼ���
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
			t = dis / v;	//�õ�����
			meetpoint.x = p->ballspeed.x *t + p->curball.x;
			meetpoint.y = p->ballspeed.y *t + p->curball.y;
			meetpoint.z = t;
		}
	}

	return meetpoint;

}


//Meetball_y	����ö�Ա���������� y ���� maxn��������׷��������ʱ�䣨��������
Vector3D Meetball_y(Environment *env, double X, double maxn)
{//����ö�Ա���������� y ����

	Mydata * p;
	p = (Mydata *)env->userData;
	double dx = X - p->curball.x;

	Vector3D meetpoint = { 0,0,-1 };


	if (dx > 0)
	{//����  ��,Ŀ�����ұ�
		if (p->ballspeed.x > 0.1)
			meetpoint.z = dx / p->ballspeed.x;
	}
	else
	{//���� �ұ� ,Ŀ������	//dx < 0 
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


//Meetball_x	����ö�Ա���������� x ���� maxn��������׷��������ʱ�䣨��������
Vector3D Meetball_x(Environment *env, double Y, double maxn)
{//����ö�Ա���������� x ����

	Mydata * p;
	p = (Mydata *)env->userData;
	double dy = Y - p->curball.y;

	Vector3D meetpoint = { 0,0,-1 };


	if (dy > 0)
	{//����  �·�,Ŀ������
		if (p->ballspeed.y > 0.01)
			meetpoint.z = dy / p->ballspeed.y;
	}
	else
	{//���� �Ϸ� ,Ŀ������	//dx < 0 
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


//Within  �ж�robot��Ա����ľ����Ƿ���LENGTH�涨�ķ�Χ��
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


//ֻ���Ž���������
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


//������Ա�ڶԷ��������м�����
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


//����normalgame���������õ��Ķ�������
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


//��������====================================================================

//Velocity �޸�robot����������vl,vr
void Velocity(Environment *env, int robot, double vl, double vr)
{
	Mydata * p;
	p = (Mydata *)env->userData;

	//vl,vr����ȡֵ��Χ��!!!
	if (vl>125)vl = 125;
	if (vl<-125)vl = -125;
	if (vr>125)vr = 125;
	if (vr<-125)vr = -125;

	if (true)
	{//�ٶȵ��ر����
		if (vl == 0 && vr != 0)
			vl = 0.00001;
		if (vr == 0 && vl != 0)
			vr = 0.00001;
	}
	p->robot[robot].velocityLeft = vl;
	p->robot[robot].velocityRight = vr;
}


//��������     ����Ϊ˳ʱ����ת 
void Spin(Environment *env, int Number, double Rotate)
{
	env->home[Number].velocityLeft = Rotate;
	env->home[Number].velocityRight = -Rotate;
};


//Angle ��robotת��angle�ķ���
void Angle( Environment *env, int robot,double angle)
{

	Mydata * p;
	p=(Mydata *)env->userData;

	double speed = 0;		//��pangle�ӹ�
	double accuracy=1;
	double turnangle=0,nextangle=0;
	double FF=125;		//�����ٶ�

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
	double f=0;	//�൱�ڼ���ʱ,�����ٶȣ�
//	int n=0;
	bool turnleft=true;			//�ж�С���Ƿ��Ǹ�����ת
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
			//����!!
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 > 0  );
		
		nextangle-=v1;

		if(nextangle < turnangle)
		{//������������� ���� f ȡ�෴��
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
	
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 < 0 )
			{
				do{//�ý��͹�����
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
		{//������������� ���� f ȡ�෴��
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 > 0 )
			{
				do{//�ý��͹�����
					f--;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 > turnangle && f >-FF);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}

}


//Angle ��robotת������pos�ķ���
void Angle( Environment *env, int robot,Vector3D pos)
{

	Mydata * p;
	p=(Mydata *)env->userData;

	double speed = 0;		//��pangle�ӹ�
	double accuracy=1;
	double turnangle=0,nextangle=0;
	double FF=125;		//�����ٶ�
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
	double f=0;	//�൱�ڼ���ʱ,�����ٶȣ�
//	int n=0;
	bool turnleft=true;			//�ж�С���Ƿ��Ǹ�����ת
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
			//����!!
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 > 0  );
		
		nextangle-=v1;

		if(nextangle < turnangle)
		{//������������� ���� f ȡ�෴��
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
	
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 < 0 )
			{
				do{//�ý��͹�����
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
		{//������������� ���� f ȡ�෴��
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 > 0 )
			{
				do{//�ý��͹�����
					f--;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 > turnangle && f >-FF);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}

}


//PAngle ��robot��angle�ķ����ܣ�����speed���������������
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
	double FF=125;		//�����ٶ�
	double f=0;	//�൱�ڼ���ʱ,�����ٶȣ�
//	int n=0;
	bool turnleft=true;			//�ж�С���Ƿ��Ǹ�����ת
	double a=ANGLE_A;
	double b=ANGLE_B;
	
	bool face ;
	if(  turnangle < 90 && turnangle > -90 )	//����Ƿ�������λ
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
			//����!!
			v1 =AngleOne(v1,speed-f,speed+f);//+= a *( b *f-v1);		// v1   
			nextangle+=v1;
		}while( v1 > 0  );
		
		nextangle-=v1;

		if(nextangle < turnangle)
		{//������������� ���� f ȡ�෴��
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
	
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 < 0 )
			{
				do{//�ý��͹�����
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
		{//������������� ���� f ȡ�෴��
			Velocity(env,robot,speed+f,speed-f);
		}
		else 
		{//reduce
			v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
			if( v1 > 0 )
			{
				do{//�ý��͹�����
					f--;
					v1 = AngleOne(v,speed-f,speed+f);  //v + a *( b *f-v);
				}while( v1 > turnangle && f >-125);
			}
			Velocity(env,robot,speed-f,speed+f);
		}		
	}
}


//PositionAndStop ��robot �ܵ�pos����������limit�ľ����ڣ���������bestangleͣ����
void PositionAndStop(Environment *env,int  robot,Vector3D pos ,double bestangle,double limit) 
{	//���ǵ����ܵ�	��ͣ�� �����ټ���
	//�ر������Ż�
	//���о��� ����ת���ת�ǹ��� ���ܵ���ʱ��!!!
	//ת������Σ�յĹ���

	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//����ת���ٶȵı���
	double vmax=125;			//Ĭ�ϵ���λ���ٶ�
	double Limitedangle=2;		//Ĭ�ϼ��ٷ�Χ

	if( limit < 0.5 )
		limit =0.5;
	double Limiteddis=limit;	//���ٷ�Χ��һ�����ޣ���֤���������ܶ�
	
	double  distance;			//robot��Ŀ���ľ���
	double turnangle,posangle,vangle;	//ת���Ƕ� ��Ŀ������robot�ĽǶȣ��ٶȵľ��ԽǶ�
	double dx,dy;				//pos  ��robot�������
	double a=SPEED_A;			//����
	double b=SPEED_B;
	double v,v1;				//��ʱ�����ٶȵĴ�С!!!
	double f=vmax;				//���ٶȱ���
	double s=0;					//Ԥ��ļ���λ��(·��)
	int n=0;					//��λ�Ĳ���
	bool face=true;			//�ж�С���Ƿ�������ǰ��

	v= sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	//��ʱ�����ٶȵĴ�С!!!
	dx = pos.x - p->robot[robot].pos.x ;		//pos  ��robot�������
	dy = pos.y - p->robot[robot].pos.y ;
	
	distance = Distance(p->robot[robot].pos , pos);
	posangle = Atan(dy,dx);

	turnangle = p->robot[robot].rotation - posangle;		//ת���Ƕ� 
	RegulateAngle(turnangle);

	if(turnangle > 90)
	{//�ж�С���Ƿ�������ǰ��
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

	vangle = p->myspeed[robot].z - p->robot[robot].rotation;		//�ٶȵķ����robot����ļн�
	RegulateAngle(vangle);					//��Ҫ���������Ƽ��ٶȵĴ�С
	if( vangle <-90 || vangle > 90 )		//ͬʱ�ж�v������
		v=-v;

	if(face)
	{//forward	��λ��������˵Ļ�  ��v=0
		//��vl,vr=0 ����vl,vr=125 ��һ��������һ���ٽ������Ǿ��� 
		//v = SPEED_ZERO
		if(v < -SPEED_ZERO)
		{
			Velocity(env,robot,0,0);
			return ;
		}
	}
	else if(v > SPEED_ZERO)
	{//back		��λ��������˵Ļ�  ��v=0
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
			/////////////////������һ��
			//����goalie��һ��Ӧ���ر�ע��
			//��������	1.knock the robot,especially the opponent
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
			///////////////������һ��
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
		{		//�����Ƕ�	return!!!!!!
			//radious of robot is about 1.5 ,so the distance is very short
			if(turnangle > Limitedangle || turnangle < -Limitedangle)
			{	
				Angle(env,robot,posangle);
				return ;
			}
		}

		if(distance < 0.4)
		{	//ֹͣ��ת��		return!!!!!!
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
				f=-vmax;		//���ٶ�  Ϊ  0000000
				v1=VelocityOne(v1,-f,-f);		//����һ��
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
				{//�����������������
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v > SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//����һ��
						if( v1 < 0 )
						{
							do{//�ý��͹�����
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
				f=vmax;		//���ٶ�!!!!!
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
				{//�����������������
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v < -SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//����һ��
						if( v1 > 0 )
						{
							do{//�ý��͹�����
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


//PositionAndStop ��robot �ܵ�pos����������limit�ľ����ڣ���������faceͣ����
void PositionAndStop(Environment *env, int  robot, Vector3D pos, Vector3D face, double limit)
{
	PositionAndStop(env, robot, pos, Atan(pos, face), limit);
}


//GoaliePosition ��robot  �ܵ�pos����������limit�ľ����ڣ���������bestangleͣ���� �������Աд��
void GoaliePosition(Environment *env,int  robot,Vector3D pos ,double bestangle,double limit) 
{	//���ǵ����ܵ�	��ͣ�� �����ټ���
	//�ر������Ż�
	//���о��� ����ת���ת�ǹ��� ���ܵ���ʱ��!!!
	//ת������Σ�յĹ���

	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//����ת���ٶȵı���
	double vmax=125;			//Ĭ�ϵ���λ���ٶ�
	double Limitedangle=2;		//Ĭ�ϼ��ٷ�Χ

	if( limit < 0.5 )
		limit =0.5;
	double Limiteddis=limit;	//���ٷ�Χ��һ�����ޣ���֤���������ܶ�
	
	double  distance;			//robot��Ŀ���ľ���
	double turnangle,posangle,vangle;	//ת���Ƕ� ��Ŀ������robot�ĽǶȣ��ٶȵľ��ԽǶ�
	double dx,dy;				//pos  ��robot�������
	double a=SPEED_A;			//����
	double b=SPEED_B;
	double v,v1;				//��ʱ�����ٶȵĴ�С!!!
	double f=vmax;				//���ٶȱ���
	double s=0;					//Ԥ��ļ���λ��(·��)
	int n=0;					//��λ�Ĳ���
	bool face=true;			//�ж�С���Ƿ�������ǰ��

	v= sqrt(p->myspeed[robot].x * p->myspeed[robot].x + p->myspeed[robot].y*p->myspeed[robot].y);
	//��ʱ�����ٶȵĴ�С!!!
	dx = pos.x - p->robot[robot].pos.x ;		//pos  ��robot�������
	dy = pos.y - p->robot[robot].pos.y ;
	
	distance = Distance(p->robot[robot].pos , pos);
	posangle = Atan(dy,dx);

	turnangle = p->robot[robot].rotation - posangle;		//ת���Ƕ� 
	RegulateAngle(turnangle);

	if(turnangle > 90)
	{//�ж�С���Ƿ�������ǰ��
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

	vangle = p->myspeed[robot].z - p->robot[robot].rotation;		//�ٶȵķ����robot����ļн�
	RegulateAngle(vangle);					//��Ҫ���������Ƽ��ٶȵĴ�С
	if( vangle <-90 || vangle > 90 )		//ͬʱ�ж�v������
		v=-v;

	if(face)
	{//forward	��λ��������˵Ļ�  ��v=0
		//��vl,vr=0 ����vl,vr=125 ��һ��������һ���ٽ������Ǿ��� 
		//v = SPEED_ZERO
		if(v < -SPEED_ZERO)
		{
			Velocity(env,robot,0,0);
			return ;
		}
	}
	else if(v > SPEED_ZERO)
	{//back		��λ��������˵Ļ�  ��v=0
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
			/////////////////������һ��
			//����goalie��һ��Ӧ���ر�ע��
			//��������	1.knock the robot,especially the opponent
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
			///////////////������һ��

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
		{		//�����Ƕ�	return!!!!!!
			//radious of robot is about 1.5 ,so the distance is very short
			if(turnangle > Limitedangle || turnangle < -Limitedangle)
			{	
				Angle(env,robot,posangle);
				return ;
			}
		}

		if(distance < 0.4)
		{	//ֹͣ��ת��		return!!!!!!
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
				f=-vmax;		//���ٶ�  Ϊ  0000000
				v1=VelocityOne(v1,-f,-f);		//����һ��
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
				{//�����������������
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v > SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//����һ��
						if( v1 < 0 )
						{
							do{//�ý��͹�����
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
				f=vmax;		//���ٶ�!!!!!
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
				{//�����������������
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v < -SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//����һ��
						if( v1 > 0 )
						{
							do{//�ý��͹�����
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


//PositionAndStopX  ��robot  �ܵ�pos����������limit�ľ����ڣ�������Xangle�Ľ��ٶ���ת
void PositionAndStopX(Environment *env,int  robot,Vector3D pos ,double Xangle,double limit) 
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//����ת���ٶȵı���
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
	bool face=true;			//�ж�С���Ƿ�������ǰ��

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
			/////////////////������һ��
			if(turnangle > 20  || turnangle < -20)
				anglespeedmax = 0;
			else if(turnangle > 10  || turnangle < -10)
				anglespeedmax = 125;
			else if(turnangle > 5  || turnangle < -5)
				anglespeedmax = 180;
			else 
				anglespeedmax = 200;
			///////////////������һ��
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
		{		//�����Ƕ�	return!!!!!!
			if(turnangle > Limitedangle || turnangle < -Limitedangle)
			{	
				Angle(env,robot,posangle);
				return ;
			}

		}

		if(distance < 1)
		{	//ֹͣ��ת��		return!!!!!!
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
				f=-vmax;		//���ٶ�  Ϊ  0000000
				v1=VelocityOne(v1,-f,-f);		//����һ��
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
				{//�����������������
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v > SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//����һ��
						if( v1 < 0 )
						{
							do{//�ý��͹�����
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
				f=vmax;		//���ٶ�!!!!!
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
				{//�����������������
					Velocity(env,robot,-f,-f);
				}
				else
				{
					if(v < -SPEED_ZERO)
						Velocity(env,robot,0,0);
					else
					{
						v1=VelocityOne(v,f,f);		//����һ��
						if( v1 > 0 )
						{
							do{//�ý��͹�����
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


//PositionBallX  ��robot  �ܵ�pos���� �������С��limit�ľ�������Xangle�Ľ��ٶ���ת
void PositionBallX(Environment *env,int  robot,Vector3D pos ,double Xangle,double limit) 
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//����ת���ٶȵı���
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
	bool face=true;			//�ж�С���Ƿ�������ǰ��
	bool turnornot=false ;	//�Ƿ���ת,��ʱ����

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

	if(distance > Limiteddis  )	//������ת��Χ��  ��ת
		turnornot = false ;
	
	if(turnornot)
	{//�������� ת!!!
		Velocity(env,robot,-Xangle,Xangle);
	}//������λ
	else if(turnangle > Limitedangle || turnangle < -Limitedangle)	
	{//adjust angle
		/////////////////������һ��
		if(turnangle > 60  || turnangle < -60)
			anglespeedmax = 0;
		else if(turnangle > 30  || turnangle < -30)
			anglespeedmax = 100;
		else if(turnangle > 10  || turnangle < -10)
			anglespeedmax = 150;
		else 
			anglespeedmax = 200;
		///////////////������һ��
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


//PositionAndThrough ��robot�����MAX ����pos���м�û�м��ٿ���
void PositionAndThrough(Environment *env,int robot,Vector3D pos ,double MAX)
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	double anglespeedmax=0;		//����ת���ٶȵı���
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
	bool face=true;			//�ж�С���Ƿ�������ǰ��

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
			/////////////////������һ��
			if(turnangle > 20  || turnangle < -20)
				anglespeedmax = 0;
			else if(turnangle > 10  || turnangle < -10)
				anglespeedmax = 125;
			else if(turnangle > 5  || turnangle < -5)
				anglespeedmax = 180;
			else 
				anglespeedmax = 200;
			///////////////������һ��
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

//�Ž�


//PredictBall	Ԥ�⾭�� steps ������֮�����λ�� ���� steps ������֮�����λ�� ����ײǽ�Ĺ���//����Ҫ��һ������
void PredictBall(Environment *env,int steps)
{
	Mydata * p;
	p=(Mydata *)env->userData;
	
	Vector3D predictball;
	Vector3D ballspeed;
	double  a,b;     //a Ϊ·�� //bΪʱ��
    int i=0;


	predictball.x = p->curball.x;			//����ֵ
	predictball.y = p->curball.y;

	ballspeed.x = p->ballspeed.x ;
	ballspeed.y = p->ballspeed.y ;
	ballspeed.z = p->ballspeed.z ;
	
	for(i=0;i<steps;i++)
	{
		predictball.x += ballspeed.x ;
		predictball.y += ballspeed.y ;
//����ײǽ
		if( predictball.x > FRIGHT &&( predictball.y<33.9320 || predictball.y>49.6801 ))//       �ǲ� ��������Ŷ��������������
		{
			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;
            
			a=ballspeed.y/ballspeed.x*(FRIGHT-predictball.x);    //Ѱ��ײǽ�ĵ㣬��ǰ���ò�ײǽ�ĵ�
			predictball.x=FRIGHT;
			predictball.y += a;
            
			b=1-a/ballspeed.y;
		
			ballspeed.x *=-SPEED_NORMAL;	//loose      �ı�ײǽ����ٶ�
			ballspeed.y *= SPEED_TANGENT;     

			predictball.x += (b*ballspeed.x) ;	//go on      ��ײǽ��Ѱ��Ԥ���
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

			predictball.x += (b*ballspeed.x) ;	//go on      ��ײǽ��Ѱ��Ԥ���
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

			predictball.x += (b*ballspeed.x) ;	//go on      ��ײǽ��Ѱ��Ԥ���
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

			predictball.x += (b*ballspeed.x) ;	//go on      ��ײǽ��Ѱ��Ԥ���
			predictball.y += (b*ballspeed.y) ;

		}

		if( predictball.x + predictball.y > FRIGHT +FTOP -CORNER )
		{//����
			double vx,vy;	
			vy= rua*ballspeed.y + rua*ballspeed.x;	//�任1
			vx=-rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy - rua * vx;	//�任2
			ballspeed.x = rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;

		}
		else if( predictball.x + predictball.y < FLEFTX +FBOT+CORNER)
		{//����
			double vx,vy;	
			vy= rua*ballspeed.y + rua*ballspeed.x;	//�任1
			vx=-rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy - rua * vx;	//�任2
			ballspeed.x = rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;	
		}
		else if( predictball.x - predictball.y > FRIGHT -FBOT -CORNER)
		{//����
			double vx,vy;	
			vy= rua*ballspeed.y - rua*ballspeed.x;	//�任1
			vx= rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy + rua * vx;	//�任2
			ballspeed.x = -rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;			
		}
		else if( predictball.y - predictball.x > FTOP - FLEFT-CORNER)
		{//����
			double vx,vy;	
			vy= rua*ballspeed.y - rua*ballspeed.x;	//�任1
			vx= rua*ballspeed.y + rua*ballspeed.x;

			predictball.x -= ballspeed.x ;	//retern
			predictball.y -= ballspeed.y ;

			vx *= SPEED_TANGENT;	//loose 
			vy *=-SPEED_NORMAL;

			ballspeed.y = rua * vy + rua * vx;	//�任2
			ballspeed.x = -rua * vy + rua * vx;

			predictball.x += ballspeed.x ;	//go on
			predictball.y += ballspeed.y ;			
		}

//�����Ľ�		
	}

	p->preball.x = predictball.x ;
	p->preball.y = predictball.y ;
	p->preball.z = Atan( ballspeed.y ,ballspeed.x );
}


//��robot����ToPos
void Kick(Environment *env, int robot, Vector3D ToPos)//ChaseAndKick
{
	Mydata * p;
	p = (Mydata *)env->userData;

	double RunAngle;

	Vector3D RobotToBall;		//�˺�������λ��
	RobotToBall.x = p->curball.x - p->robot[robot].pos.x;
	RobotToBall.y = p->curball.y - p->robot[robot].pos.y;
	RobotToBall.z = Atan(p->robot[robot].pos, p->curball);//����Ա�����������X��н�

	Vector3D BallToPos;		//������ŵ����λ��
	BallToPos.x = ToPos.x - p->curball.x;
	BallToPos.y = ToPos.y - p->curball.y;
	BallToPos.z = Atan(p->curball, ToPos);

	if (fabs(RobotToBall.z - BallToPos.z) <= 75)
	{//����ǣ���ֱ�Ӽ���
		RunAngle = RobotToBall.z - BallToPos.z;
		RegulateAngle(RunAngle);
		RunAngle = RobotToBall.z + RunAngle / 2;	// ���Ե���  2 
		RegulateAngle(RunAngle);
		PAngle(env, robot, RunAngle, 125);
	}
	else if (fabs(RobotToBall.z - BallToPos.z)>75 && fabs(RobotToBall.z - BallToPos.z) <= 120)
	{//����ǣ�С���ƺ�
		RunAngle = RobotToBall.z - BallToPos.z;
		RegulateAngle(RunAngle);
		RunAngle = RobotToBall.z + RunAngle / 1.5;	// ���Ե���  1.5 
		RegulateAngle(RunAngle);
		PAngle(env, robot, RunAngle, 125);
	}
	else
	{//����ǣ��������Դﵽ����Ǳ�׼
		RunAngle = BallToPos.z + 180;
		RegulateAngle(RunAngle);
		RunAngle = RunAngle + (RobotToBall.z - BallToPos.z) / 4;
		RegulateAngle(RunAngle);
		PAngle(env, robot, RunAngle, 125);
	}

}


//��robot����robot1
void Kick(Environment *env , int  robot ,int robot1)//����
{
	Mydata * p;
	p=(Mydata *)env->userData;
	Vector3D RobotToBall;		//�˺�������λ��
	RobotToBall.x = p->robot[robot1].pos.x- p->robot[robot].pos.x ;
	RobotToBall.y = p->robot[robot1].pos.y- p->robot[robot].pos.y ;
	RobotToBall.z = Atan(p->robot[robot].pos , p->robot[robot1].pos);
	
	Vector3D BallToGate ;		//������ŵ����λ��
	BallToGate.x = CONSTGATE.x- p->robot[robot1].pos.x;
	BallToGate.y = CONSTGATE.y- p->robot[robot1].pos.y;
	BallToGate.z = Atan(p->robot[robot1].pos,CONSTGATE);
	
	double RunAngle ;
	RunAngle = RobotToBall.z - BallToGate.z;
	RegulateAngle(RunAngle);
	
	RunAngle = RobotToBall.z + RunAngle /2 ;	// ���Ե���  2 
	RegulateAngle(RunAngle);

	PAngle(env,robot,RunAngle,125);

}


//��robot steps���Ժ���ת��
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


//С��2�����С��1��λ�� �𿴷���Ŷoooooo~
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


//С������ڳ���λ�� �𿴷���Ŷoooooo~
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


//���� ����ʹ��������
void shoot(Environment *env, int robot) {

	Mydata *p = (Mydata *)env->userData;
	double w1, w2, alfa;
	double dx, dy;

	if (p->curball.y>GBOT&&p->curball.y <= (GBOT + GTOP) / 2) {//���޸� ԭΪif(p->curball.y>GBOT&&p->curball.y<=GTOP/2){

		if (p->ballspeed.z<85 && p->ballspeed.z<95)
			PositionBallX(env, robot, p->curball, -90, 4);

		else if (p->ballspeed.z<-85 && p->ballspeed.z>-95)
			PositionBallX(env, robot, p->curball, 90, 4);
	}

	else if (p->curball.y >= (GBOT + GTOP) / 2 && p->curball.y <= GTOP) {//���޸�

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


//�ж��Ƿ������ţ��ϸ�������ͷ��
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
			if (p->preball.y <= GTOP + 4 && p->preball.y >= GBOT - 4) {//�����Ƕ�׼������
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

void PlayerInit(Environment *env,int Method)  //����5v5����ս��������Ա��ʼλ��
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
	case 1: // 1-2-1����
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
	//  ����������ʽ����
	//
	default:break;
	}
}
