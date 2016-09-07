#include <DxLib.h>

/*�萔��`*/
#define windowX  rect.right
#define windowY  rect.bottom
#define SHOT_MAX  2
#define ENEMY_MAX 30


/*�\���̒�`*/

//���@�̏��
typedef struct{
	int flag;  //�����t���O
	int x,y;   //���W
	int img;   //�摜
	int range; //�����蔻��
	int score; //�X�R�A
	int combo; //�R���{
	int bomb;  //�{����
}ch_rec;

//�G�@�̏��
typedef struct{
	int flag;        //�t���O
	int cnt;         //�J�E���^
	int x,y;         //���W
	int moveX,moveY; //�ړ�����
	int img;	     //�摜
	int range;       //�����蔻��
	int pattern;     //�s���p�^�[��
	int shot_time;   //�V���b�g��ł��n�߂鎞��
	int shot_flag;   //�V���b�g��ł��Ă�����
}ene_rec;

//�V���b�g�̏��
typedef struct{
	int flag;  //�t���O
	int x,y;   //���W
	int img;   //�摜
	int range; //�����蔻��
}shot_rec;	


/*�O���[�o���ϐ��錾*/
int Key[256];			   //�L�[���͏��
int GameCount;			   //�Q�[���J�E���g
ch_rec   Player;		   //�v���C���[�̍\����
ene_rec  Enemy[ENEMY_MAX]; //�G�̍\����
shot_rec Shot[SHOT_MAX];   //�V���b�g�\��
RECT     rect;			   //�E�B���h�E�T�C�Y�̍\����


/*�v���g�^�C�v�錾*/

//WinMain�֐�
int  ProcessLoop();   //���C�����[�v�̌Œ菈��
void KeyStateCheck(); //�L�[���͏���
void Init();          //����������

//GameMain�֐�	
void GameMain();	     //�Q�[���Ɋւ���֐�
void LoadData();		 //�摜���̃f�[�^�ǂݍ���
void Graph();			 //�摜�`��
void CharMove();		 //���@�ړ�����
void CharShot();	     //���@�V���b�g����
void CharBomb();		 //���@�{������
void EnemyMain();	     //�G���o�^
void EnemyCont(int cnt); //�G����
void EnemyEnter(int x,int y,int pattern); //�G���o�^
void HitCheck();         //�����蔻�艉�Z
void GameOver();		 //�Q�[���I�[�o�[
int  LinearFunc(int y,int cnt);  //�ꎞ�֐��v�Z




/*���C���֐�*/
int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow ){
	//�ϐ��錾
	int func_state = 0;
	
	//����������
	ChangeWindowMode(FALSE);
	DxLib_Init();
	SetDrawScreen( DX_SCREEN_BACK );
	//�N���C�A���g�T�C�Y�擾
	GetClientRect(GetMainWindowHandle(),&rect);
	Init();

	//���C�����[�v
	while( ProcessLoop() == 0 ){
		//�Q�[�����C��
		if(Player.flag >= 1){
			if(Player.flag == 1){
				Init();
				PlaySoundFile("./sound_file/��܂��Ƃ̂��傭.wav",DX_PLAYTYPE_LOOP);
				Player.flag++;
			}
			GameMain();
		}else	 GameOver();

		if( Key[KEY_INPUT_ESCAPE] == 1 ) break;
		if( Key[KEY_INPUT_RETURN] == 1 ) Player.flag = 1;

		ScreenFlip();
	}
	
	DxLib_End();
	return 0;
}

void GameOver(){
	DrawFormatString(windowX/2-150,windowY/2,GetColor(255,255,255),"�Q�[���I�[�o�[ �X�R�A=%d",Player.score);
	DrawString(windowX/2-150,windowY/2+20,"Enter�Ń��g���C",GetColor(255,255,255));
	StopSoundFile();
}

/*���C�����[�v�̌Œ菈��*/
int ProcessLoop(){
	if( ProcessMessage()  != 0 ) return -1;
	if( ClearDrawScreen() != 0 ) return -1;
	KeyStateCheck();
	return 0;
}

/*�L�[���͏���*/
void KeyStateCheck(){
	char tmp_key[256];
	int  count;

	//�S�ẴL�[�̏�Ԃ��擾
	GetHitKeyStateAll( tmp_key );

	//�L�[��Ԃ�z��Ɋi�[
	for( count=0 ; count<256 ; count++ ){
		if( tmp_key[count] == 1 ) Key[count]++;
		else					  Key[count]=0;
	}

}

/*����������*/
void Init(){
	//�N���C�A���g�T�C�Y�擾
	//GetClientRect(GetMainWindowHandle(),&rect);

	memset(&Player,0,sizeof(Player));
	memset(Enemy,0,sizeof(Enemy));
	memset(Shot,0,sizeof(Shot));
	GameCount = 0;

	//�v���C���[�\���̏�����
	Player.x     = windowX/2;
	Player.y     = windowY/2;
	Player.range = 20;
	Player.flag  = 1;
	Player.bomb  = 3;

	LoadData();
}

/*�摜���f�[�^�ǂݍ���*/
void LoadData(){
	//�v���C���[�摜�ǂݍ���
	Player.img = LoadGraph( "./pic_file/Player1.jpg" );
	//�G�摜�ǂݍ���
	for( int i=0 ; i < ENEMY_MAX ; i++ ){
		Enemy[i].img  = LoadGraph( "./pic_file/enemy.png" );
	}

	//�V���b�g�摜�ǂݍ���
	for( int i=0 ; i < SHOT_MAX ; i++ ){
		Shot[i].img   = LoadGraph( "./pic_file/shot.png" );
	}
}




/*�Q�[�����C���֐�*/
void GameMain(){
	GameCount++;
	CharMove();  //���@�ړ�����
	CharShot();  //���@�V���b�g����
	CharBomb();  //���@�V���b�g����
	EnemyMain(); //�G�@����
	HitCheck();  //�����蔻�艉�Z
	Graph();     //�`�揈��
}

/*�摜�`��*/
void Graph(){
	//�v���C���[�`��
	DrawGraph(Player.x,Player.y,Player.img,TRUE);
	
	//�X�R�A�\��
	DrawFormatString(40,windowY-20,GetColor(255,255,255),"�X�R�A=%d",Player.score);
	DrawFormatString(40,windowY-40,GetColor(255,255,255),"�R���{=%d",Player.combo);
	DrawFormatString(40,windowY-60,GetColor(255,255,255),"�{��=%d",Player.bomb);
	
	//�G�`��
	for( int cnt=0 ; cnt < ENEMY_MAX ; cnt++){
		if( Enemy[cnt].flag == 1 ) DrawGraph(Enemy[cnt].x,Enemy[cnt].y,Enemy[cnt].img,TRUE);
	}
	//�V���b�g�`��
	for( int count=0 ; count < SHOT_MAX ; count++ ){
		if( Shot[count].flag == 1 ) DrawGraph(Shot[count].x,Shot[count].y,Shot[count].img,TRUE);
	}
}

/*���@�ړ�����*/
void CharMove(){
	int tmpX,tmpY;

	//���݂̎��@���W���i�[
	tmpX = Player.x;
	tmpY = Player.y;

	//�ړ�����
	if( Key[KEY_INPUT_RIGHT] > 0 ) tmpX += 4;
	if( Key[KEY_INPUT_LEFT]  > 0 ) tmpX -= 4;
	if( Key[KEY_INPUT_UP]    > 0 ) tmpY -= 4;
	if( Key[KEY_INPUT_DOWN]  > 0 ) tmpY += 4;

	//�ړ��悪��ʓ��Ȃ���ۂɈړ�������
	if( tmpX >= 0 && tmpX < windowX-40 ) Player.x = tmpX;
	if( tmpY >= 0 && tmpY < windowY-40 ) Player.y = tmpY;
}

/*�V���b�g����*/
void CharShot(){
	int count;

	for( count=0 ; count < SHOT_MAX ; count++ ){
		//�t���O�̂������V���b�g������Βe���v�Z
		if( Shot[count].flag == 1){

			Shot[count].y -= 10; //�e��

			//�V���b�g����ʊO�ɂł���t���O��܂�
			if( Shot[count].y < 0 ) Shot[count].flag =0;
		}
	}

	//�V���b�g�L�[��������Ă�����
	if( Key[KEY_INPUT_Z] == 1 ){
		//�t���O�̗����ĂȂ��V���b�g��T��
		for( count=0 ; count < SHOT_MAX ; count++ ){
			if( Shot[count].flag == 0 ) break;
		}

		//�t���O�̗����ĂȂ��V���b�g������Γo�^
		if( count < SHOT_MAX ){
			Shot[count].flag++;
			Shot[count].x     = Player.x;
			Shot[count].y     = Player.y+40;
			Shot[count].range = 20;
		}
	}

}

/*���@�{������*/
void CharBomb(){
	if( Key[KEY_INPUT_X] == 1 ){
			Player.bomb --;
		for( int cnt=0 ; cnt < ENEMY_MAX ; cnt++){
			if( Enemy[cnt].flag == 1 && Player.bomb >= 0){
				Enemy[cnt].flag = 0;
				Player.score += 5;
				Player.combo ++;
			}
		}
	}
	if( Player.combo % 10 == 0 && Player.combo != 0 ){
		Player.bomb++;
		Player.combo++;
	}
}

/*�G�@���o�^*/
void EnemyMain(){
	int cnt;

	//�G�̃t���O�������Ă�Ȃ�
	for( cnt=0 ; cnt < ENEMY_MAX ; cnt++){ 
		if( Enemy[cnt].flag == 1 ){
			Enemy[cnt].cnt++;
			EnemyCont(cnt);
			if( Enemy[cnt].cnt > 300 ){
				Enemy[cnt].flag = 0;
				Player.combo    = 0;
			}
		}
	}

	//���J�E���g�œG�@�o�^
	if( GameCount % 250 == 0 ){
		EnemyEnter(windowX/2,-40,1);
		EnemyEnter(windowX/2+40,-60,1);
		EnemyEnter(windowX/2-40,-60,1);
	}
		
	if( GameCount % 300 == 0 ){
		EnemyEnter(-40,-40,2);
		EnemyEnter(-60,0,2);
		EnemyEnter(windowX+40,-40,3);
		EnemyEnter(windowX+60,0,3);
	}

	if( GameCount % 350 == 0 ){
		EnemyEnter(windowX/2,windowY-40,6);
		EnemyEnter(windowX/2+40,windowY-40,6);
		EnemyEnter(windowX/2-40,windowY-40,6);
	}
	
	if( GameCount % 450 == 0 ){
		EnemyEnter(-40,windowY/2,4);
		EnemyEnter(-40,windowY/2+40,4);
		EnemyEnter(windowX,windowY/2+80,5);
	}

	if( GameCount % 500 == 0 ){
		EnemyEnter(0,0,4);
		EnemyEnter(windowX,40,5);
	}

	if( GameCount % 1200 == 0){
		EnemyEnter(0,0,7);
		EnemyEnter(40,20,7);
		EnemyEnter(-40,20,7);
	}

	if( GameCount % 600 == 0 ){
		EnemyEnter(windowX-40,windowY-40,6);
		EnemyEnter(-40,windowY-40,4);
	}
}

/*�G���o�^*/
void EnemyEnter(int x,int y,int pattern){
	int cnt;
	
	for( cnt=0 ; cnt < ENEMY_MAX ; cnt++ ){
		if( Enemy[cnt].flag == 0 ) break;
	}

	if( cnt != ENEMY_MAX ){
		Enemy[cnt].flag    = 1;
		Enemy[cnt].x       = x;
		Enemy[cnt].y       = y;
		Enemy[cnt].range   = 20;
		Enemy[cnt].cnt     = 0;
		Enemy[cnt].pattern = pattern;
	}

}

/*�G�̍s������*/
void EnemyCont(int cnt){
	switch( Enemy[cnt].pattern ){
		case 1:
			if( Enemy[cnt].cnt < 50  ){
				Enemy[cnt].y += 2;
			}else if( Enemy[cnt].cnt > 250 ){
				Enemy[cnt].y -= 2;
			}
		break;

		case 2:
			if( Enemy[cnt].cnt < 50 ){
				Enemy[cnt].x += 2;
				Enemy[cnt].y += 2;
			}else if( Enemy[cnt].cnt < 100 ){
				Enemy[cnt].x += 2;
			}else if( Enemy[cnt].cnt > 200 ){
				Enemy[cnt].y -= 2;
			}
		break;

		case 3:
			if( Enemy[cnt].cnt < 50 ){
				Enemy[cnt].x -= 2;
				Enemy[cnt].y += 2;
			}else if( Enemy[cnt].cnt < 100 ){
				Enemy[cnt].x -= 2;
			}else if( Enemy[cnt].cnt > 200 ){
				Enemy[cnt].y -= 2;
			}
		break;

		case 4:
			if( Enemy[cnt].cnt < 500 ){
				Enemy[cnt].x += 4;
			}
		break;

		case 5:
			if( Enemy[cnt].cnt < 500 ){
				Enemy[cnt].x -= 4;
			}
		break;

		case 6:
			if( Enemy[cnt].cnt < 50 ){
				Enemy[cnt].y -= 4;
			}else if(Enemy[cnt].cnt < 150 ){
				Enemy[cnt].x -= 2;
				Enemy[cnt].y += 2;
			}else if(Enemy[cnt].cnt < 500 ){
				Enemy[cnt].x += 5;
				Enemy[cnt].y -= 3;
			}
		break;

		case 7:
			if( Enemy[cnt].cnt < 150){
				Enemy[cnt].x += 5;
				Enemy[cnt].y += 5;
			}else if( Enemy[cnt].cnt < 300){
				Enemy[cnt].x -= 5;
				Enemy[cnt].y -= 5;
			}
		break;
	}
}

/*�ꎞ�֐�����*/
int LinearFunc(int y,int cnt){
	int a,b,d,x;
	double c;

	a = Player.x - Enemy[cnt].x;
	b = Player.y - Enemy[cnt].y;
	if( a != 0) c = b / a;
	d = Player.y - Player.x * c;

	x = ( y - d ) / c;


	return x;
}

/*�����蔻�艉�Z*/
void HitCheck(){
	int a,b,c; 
	int Scnt,Ecnt;
	
	for( Ecnt=0 ; Ecnt < ENEMY_MAX ; Ecnt++ ){
		
		
		if(Enemy[Ecnt].flag == 1){
			//�G�ƃv���C���[�̓����蔻��
			a = Player.x - Enemy[Ecnt].x;
			b = Player.y - Enemy[Ecnt].y;
			c = Player.range + Enemy[Ecnt].range;
			
			if( c*c > a*a + b*b ) Player.flag = 0;
				
			//�G�ƃV���b�g�̓����蔻�聫��
			for( Scnt=0 ; Scnt < SHOT_MAX ; Scnt++ ){
				//�������g�p���̃V���b�g�������	
				if( Shot[Scnt].flag == 1 ){
					
					a = Enemy[Ecnt].x - Shot[Scnt].x;
					b = Enemy[Ecnt].y - Shot[Scnt].y;
					c = Enemy[Ecnt].range + Shot[Scnt].range;
						
					if( c*c > a*a + b*b ){
						Player.combo ++;
						Player.score += 90 + (Player.combo * 10);
						Enemy[Ecnt].flag = 0;
					}

				}
				//�g�p���V���b�g�m�F����
			}
			//�G�V���b�g�̓����蔻�聪��
		}
		//�G�l�~�[�t���O�`�F�b�N����
	}
	//�S�ẴG�l�~�[�`�F�b�N����
}