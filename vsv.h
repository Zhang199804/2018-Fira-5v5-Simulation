const double SPEED_ODD=0.662;	//0.338;左右轮速为0时的减速参数
const double SPEED_ZERO = 0.1896;	// 0 减速度 和 125减速度的临界值
const double CORNER=5;
const double rua = 0.9;  //不是小僵尸的标识 是有用的

void Goalie ( Environment *env, int robot );
void PenaltyGoalie ( Environment *env, Robot *robot );
void See ( Environment *env );
void End ( Environment *env );
void Action(Environment *env );
void PredictBall(Environment *env,int steps=1);
double Distance (Vector3D pos1,Vector3D pos2); //求两点之间的距离


int Count( Environment *env, int robot,Vector3D pos);
bool Within(Environment* env,int robot,double LENGTH);


void Order( Environment *env );
int CheckBall(Environment *env);


void PositionAndStop(Environment *env,int  robot,Vector3D pos ,double bestangle=90,double limit=1);
void PositionAndStop(Environment *env, int  robot, Vector3D pos, Vector3D face, double limit=1);
void PositionAndStopX(Environment *env,int  robot,Vector3D pos ,double Xangle=90,double limit=2); 
void PositionBallX(Environment *env,int  robot,Vector3D pos ,double Xangle=90,double limit=3.5); 
void PositionAndThrough(Environment *env,int robot,Vector3D pos ,double MAX=125);		
void GoaliePosition(Environment *env,int  robot,Vector3D pos ,double bestangle=90,double limit=1.5);
//Limit = 1.5 is ok!
void Kick(Environment *env , int  robot , Vector3D ToPos );
void Kick(Environment *env , int  robot ,int robot1);
void Kick(Environment *env, int robot, int steps, double limits);

void spinkick(Environment *env,int robot,Vector3D pos,int speed);  //2013.10.27


void Angle( Environment *env, int robot,double angle);
void Angle( Environment *env, int robot,Vector3D pos);
void PAngle( Environment *env, int robot,double angle,double speed=0);
void Velocity(Environment *env, int robot,double vl,double vr);
double AngleOne(double omiga,double vl,double vr);
Vector3D Meetball_p( Environment *env, int robot);
Vector3D Meetball_y( Environment *env,double X,double maxn=50);
Vector3D Meetball_x( Environment *env,double Y,double maxn=50);
inline double Atan(Vector3D begin,Vector3D end);

int withnum(Environment *env, int dis);

/// !!!!!!!speed 以和robot同向为正
double VelocityOne(double speed,double vl,double vr);		/// speed 以和robot同向为正

void RegulateAngle(double &angle);
int toint(double num);
double Atan(double y,double x);
double Atan(Vector3D begin,Vector3D end);
double Distance (Vector3D pos1,Vector3D pos2);

bool shoot1(Environment *env,int robot);
void shoot(Environment *env,int robot);

void Spin(Environment *env,int Number,double Rotate);
double BallSpeed(Environment *env,int steps);
void attackopp(Environment *env,int robot,int opp);

int RobotBallCheck(Environment*env, int robot);
int RobotRobotCheck(Environment*env, int robot1, int robot2);


void spinkick(Environment *env, int robot, Vector3D pos, int speed);
bool OnlyGoalieInMenqu(Environment *env);
int opareanum(Environment* env);
void PlayerInit(Environment *env, int Method);