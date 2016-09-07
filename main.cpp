#include <DxLib.h>

/*定数定義*/
#define windowX  rect.right
#define windowY  rect.bottom
#define SHOT_MAX  2
#define ENEMY_MAX 30


/*構造体定義*/

//自機の情報
typedef struct{
	int flag;  //生存フラグ
	int x,y;   //座標
	int img;   //画像
	int range; //当たり判定
	int score; //スコア
	int combo; //コンボ
	int bomb;  //ボム個数
}ch_rec;

//敵機の情報
typedef struct{
	int flag;        //フラグ
	int cnt;         //カウンタ
	int x,y;         //座標
	int moveX,moveY; //移動距離
	int img;	     //画像
	int range;       //当たり判定
	int pattern;     //行動パターン
	int shot_time;   //ショットを打ち始める時間
	int shot_flag;   //ショットを打っている状態
}ene_rec;

//ショットの情報
typedef struct{
	int flag;  //フラグ
	int x,y;   //座標
	int img;   //画像
	int range; //当たり判定
}shot_rec;	


/*グローバル変数宣言*/
int Key[256];			   //キー入力状態
int GameCount;			   //ゲームカウント
ch_rec   Player;		   //プレイヤーの構造体
ene_rec  Enemy[ENEMY_MAX]; //敵の構造体
shot_rec Shot[SHOT_MAX];   //ショット構造
RECT     rect;			   //ウィンドウサイズの構造体


/*プロトタイプ宣言*/

//WinMain関数
int  ProcessLoop();   //メインループの固定処理
void KeyStateCheck(); //キー入力処理
void Init();          //初期化処理

//GameMain関数	
void GameMain();	     //ゲームに関する関数
void LoadData();		 //画像等のデータ読み込み
void Graph();			 //画像描画
void CharMove();		 //自機移動処理
void CharShot();	     //自機ショット制御
void CharBomb();		 //自機ボム制御
void EnemyMain();	     //敵情報登録
void EnemyCont(int cnt); //敵制御
void EnemyEnter(int x,int y,int pattern); //敵情報登録
void HitCheck();         //当たり判定演算
void GameOver();		 //ゲームオーバー
int  LinearFunc(int y,int cnt);  //一時関数計算




/*メイン関数*/
int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow ){
	//変数宣言
	int func_state = 0;
	
	//初期化処理
	ChangeWindowMode(FALSE);
	DxLib_Init();
	SetDrawScreen( DX_SCREEN_BACK );
	//クライアントサイズ取得
	GetClientRect(GetMainWindowHandle(),&rect);
	Init();

	//メインループ
	while( ProcessLoop() == 0 ){
		//ゲームメイン
		if(Player.flag >= 1){
			if(Player.flag == 1){
				Init();
				PlaySoundFile("./sound_file/やまもとのきょく.wav",DX_PLAYTYPE_LOOP);
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
	DrawFormatString(windowX/2-150,windowY/2,GetColor(255,255,255),"ゲームオーバー スコア=%d",Player.score);
	DrawString(windowX/2-150,windowY/2+20,"Enterでリトライ",GetColor(255,255,255));
	StopSoundFile();
}

/*メインループの固定処理*/
int ProcessLoop(){
	if( ProcessMessage()  != 0 ) return -1;
	if( ClearDrawScreen() != 0 ) return -1;
	KeyStateCheck();
	return 0;
}

/*キー入力処理*/
void KeyStateCheck(){
	char tmp_key[256];
	int  count;

	//全てのキーの状態を取得
	GetHitKeyStateAll( tmp_key );

	//キー状態を配列に格納
	for( count=0 ; count<256 ; count++ ){
		if( tmp_key[count] == 1 ) Key[count]++;
		else					  Key[count]=0;
	}

}

/*初期化処理*/
void Init(){
	//クライアントサイズ取得
	//GetClientRect(GetMainWindowHandle(),&rect);

	memset(&Player,0,sizeof(Player));
	memset(Enemy,0,sizeof(Enemy));
	memset(Shot,0,sizeof(Shot));
	GameCount = 0;

	//プレイヤー構造体初期化
	Player.x     = windowX/2;
	Player.y     = windowY/2;
	Player.range = 20;
	Player.flag  = 1;
	Player.bomb  = 3;

	LoadData();
}

/*画像等データ読み込み*/
void LoadData(){
	//プレイヤー画像読み込み
	Player.img = LoadGraph( "./pic_file/Player1.jpg" );
	//敵画像読み込み
	for( int i=0 ; i < ENEMY_MAX ; i++ ){
		Enemy[i].img  = LoadGraph( "./pic_file/enemy.png" );
	}

	//ショット画像読み込み
	for( int i=0 ; i < SHOT_MAX ; i++ ){
		Shot[i].img   = LoadGraph( "./pic_file/shot.png" );
	}
}




/*ゲームメイン関数*/
void GameMain(){
	GameCount++;
	CharMove();  //自機移動処理
	CharShot();  //自機ショット制御
	CharBomb();  //自機ショット制御
	EnemyMain(); //敵機制御
	HitCheck();  //当たり判定演算
	Graph();     //描画処理
}

/*画像描画*/
void Graph(){
	//プレイヤー描画
	DrawGraph(Player.x,Player.y,Player.img,TRUE);
	
	//スコア表示
	DrawFormatString(40,windowY-20,GetColor(255,255,255),"スコア=%d",Player.score);
	DrawFormatString(40,windowY-40,GetColor(255,255,255),"コンボ=%d",Player.combo);
	DrawFormatString(40,windowY-60,GetColor(255,255,255),"ボム=%d",Player.bomb);
	
	//敵描画
	for( int cnt=0 ; cnt < ENEMY_MAX ; cnt++){
		if( Enemy[cnt].flag == 1 ) DrawGraph(Enemy[cnt].x,Enemy[cnt].y,Enemy[cnt].img,TRUE);
	}
	//ショット描画
	for( int count=0 ; count < SHOT_MAX ; count++ ){
		if( Shot[count].flag == 1 ) DrawGraph(Shot[count].x,Shot[count].y,Shot[count].img,TRUE);
	}
}

/*自機移動制御*/
void CharMove(){
	int tmpX,tmpY;

	//現在の自機座標を格納
	tmpX = Player.x;
	tmpY = Player.y;

	//移動処理
	if( Key[KEY_INPUT_RIGHT] > 0 ) tmpX += 4;
	if( Key[KEY_INPUT_LEFT]  > 0 ) tmpX -= 4;
	if( Key[KEY_INPUT_UP]    > 0 ) tmpY -= 4;
	if( Key[KEY_INPUT_DOWN]  > 0 ) tmpY += 4;

	//移動先が画面内なら実際に移動させる
	if( tmpX >= 0 && tmpX < windowX-40 ) Player.x = tmpX;
	if( tmpY >= 0 && tmpY < windowY-40 ) Player.y = tmpY;
}

/*ショット処理*/
void CharShot(){
	int count;

	for( count=0 ; count < SHOT_MAX ; count++ ){
		//フラグのたったショットがあれば弾道計算
		if( Shot[count].flag == 1){

			Shot[count].y -= 10; //弾速

			//ショットが画面外にでたらフラグを折る
			if( Shot[count].y < 0 ) Shot[count].flag =0;
		}
	}

	//ショットキーが押されていたら
	if( Key[KEY_INPUT_Z] == 1 ){
		//フラグの立ってないショットを探す
		for( count=0 ; count < SHOT_MAX ; count++ ){
			if( Shot[count].flag == 0 ) break;
		}

		//フラグの立ってないショットがあれば登録
		if( count < SHOT_MAX ){
			Shot[count].flag++;
			Shot[count].x     = Player.x;
			Shot[count].y     = Player.y+40;
			Shot[count].range = 20;
		}
	}

}

/*自機ボム制御*/
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

/*敵機情報登録*/
void EnemyMain(){
	int cnt;

	//敵のフラグが立ってるなら
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

	//一定カウントで敵機登録
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

/*敵情報登録*/
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

/*敵の行動制御*/
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

/*一時関数制御*/
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

/*当たり判定演算*/
void HitCheck(){
	int a,b,c; 
	int Scnt,Ecnt;
	
	for( Ecnt=0 ; Ecnt < ENEMY_MAX ; Ecnt++ ){
		
		
		if(Enemy[Ecnt].flag == 1){
			//敵とプレイヤーの当たり判定
			a = Player.x - Enemy[Ecnt].x;
			b = Player.y - Enemy[Ecnt].y;
			c = Player.range + Enemy[Ecnt].range;
			
			if( c*c > a*a + b*b ) Player.flag = 0;
				
			//敵とショットの当たり判定↓↓
			for( Scnt=0 ; Scnt < SHOT_MAX ; Scnt++ ){
				//もしも使用中のショットがあれば	
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
				//使用中ショット確認↑↑
			}
			//敵ショットの当たり判定↑↑
		}
		//エネミーフラグチェック↑↑
	}
	//全てのエネミーチェック↑↑
}