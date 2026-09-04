#include "Game.h"
#include "Renderer.h"
#include "Polygon.h"
#include "Input.h"
#include "controller.h"
#include "Text.h"
#include "sound.h"
#include "Collision.h"
#include "Framework/texture.h"
#include <algorithm>

Polygon polygon;
Polygon groundPolygon;
unsigned int groundTexture = 0;

bool g_jumpState = false;
const float g_gravity = 4500.0f;				//重力加速度
const float g_jumpinitialSpeed = 1700.0f;		//ジャンプの初速
float g_verticalVelocity = 0.0f;				//縦方向の現在の速度


Float3 leftControllerRotate;
Float3 leftControllerAccel;
Float3 rightControllerRotate;
Float3 rightControllerAccel;

static GameObject* s_AnimObj = nullptr;
static GameObject* s_background = nullptr;
static GameObject* s_box = nullptr;
static GameObject* s_aim = nullptr;

Rectangle g_cursorRect;
Rectangle g_boxRect;
bool g_boxInsideCursor = false;


//static GameObject* s_view = nullptr;

/*
//3つの頂点データを用意
VERTEX_3D vertex[6];


float r = 1.0f;
float g = 0.2f;
float b = 0.7f;



void Game::InitializeSample()
{
	
}

void Game::UpdateSample(float dt)
{
	if (r <= 0.0f)
	{
		r += 0.1f;
	}
	else if (r >= 1.0f)
	{
		r = 0.0f;
	}

	if (g <= 0.0f)
	{
		r += 0.1f;
	}
	else if (g >= 1.0f)
	{
		r = 0.0f;
	}
	if (b <= 0.0f)
	{
		r += 0.1f;
	}
	else if (b >= 1.0f)
	{
		r = 0.0f;
	}
	r += 0.05f;
	g += 0.3f;
	b += 0.15f;
}



void Game::DrawSample()
{
	//テクスチャを使わない
	Renderer::SetTexture(0);

	
	
	//各頂点の座標を設定する（画面座標：左上が(0,0))
	vertex[0].Position = MakeFloat3(900.0f, 100.0f, 0.0f);
	vertex[1].Position = MakeFloat3(900.0f, 600.0f, 0.0f);
	vertex[2].Position = MakeFloat3(200.0f, 600.0f, 0.0f);
	vertex[3].Position = MakeFloat3(200.0f, 100.0f, 0.0f);
	vertex[4].Position = MakeFloat3(900.0f, 100.0f, 0.0f);
	vertex[5].Position = MakeFloat3(200.0f, 600.0f, 0.0f);

	//色（白）とテクスチャ座標
	for (int i = 0; i < 6; i++)
	{
		vertex[i].Color = MakeFloat4(r, g, b, 1.0f);
		vertex[i].TexCoord = MakeFloat2(1.0f, 1.0f);
	}

	

	//描画
	Renderer::DrawVertices(vertex,6);
}*/

//3つの頂点データを用意
VERTEX_3D vertex[4];

float x = 470.0f ,y = 200.0f;
float w = 50.0f, h = 50.0f;

float r = 1.0f;
float g = 0.2f;
float b = 0.7f;

double x_right = false;		
double x_left = false;		
double y_up = false;		
double y_down = false;		

void DrawDebugRect(const Rectangle& rect, const Float4& color)
{
	VERTEX_3D v[4];

	float left = rect.x;
	float right = rect.x + rect.width;
	float top = rect.y;
	float bottom = rect.y + rect.height;

	v[0].Position = MakeFloat3(left, top, 0.0f);
	v[1].Position = MakeFloat3(right, top, 0.0f);
	v[2].Position = MakeFloat3(left, bottom, 0.0f);
	v[3].Position = MakeFloat3(right, bottom, 0.0f);

	for (int i = 0; i < 4; i++)
	{
		v[i].Color = color;
		v[i].TexCoord = MakeFloat2(0.0f, 0.0f);
	}

	Renderer::SetTexture(0);
	Renderer::DrawVertices(v, 4, DrawMode::TriangleStrip);
}


void Game::InitializeSample()
{
	//GameObjectを生成
	s_AnimObj = AddObject(new GameObject);
	s_background = AddObject(new GameObject);
	//s_view = AddObject(new GameObject);
	s_box = AddObject(new GameObject);
	s_aim = AddObject(new GameObject);

	//スプライトの設定
	SpriteSheet& sprite = s_AnimObj->GetSprite();
	sprite.SetPolygonSize(MakeFloat2(300.0f, 100.0f));
	SpriteSheet& bgsprite = s_background->GetSprite();
	bgsprite.SetPolygonSize(MakeFloat2(1200.0f, 800.0f));
	SpriteSheet& boxSprite = s_box->GetSprite();
	boxSprite.SetPolygonSize(MakeFloat2(80.0f, 60.0f));
	SpriteSheet aimSprite;
	/*SpriteSheet& sprite2 = s_view->GetSprite();
	sprite.SetPolygonSize(MakeFloat2(1200.0f, 800.0f));*/

	//テクスチャ読み込み＆分割
	sprite.LoadTexture("rom:/texture/robo.tga");
	sprite.DivideAnimationCells(9, 1);
	bgsprite.LoadTexture("rom:/texture/background.tga");
	bgsprite.DivideAnimationCells(1, 1);
	boxSprite.LoadTexture("rom:/texture/box.tga");
	boxSprite.DivideAnimationCells(1, 1);
	aimSprite.LoadTexture("rom:/texture/aim.tga");
	int aimTextureID = aimSprite.GetTextureID();
	/*sprite2.LoadTexture("rom:/texture/view.tga");
	sprite2.DivideAnimationCells(1, 1);*/

	//アニメーション登録
	sprite.CreateAnimation("Count", 0, 8);
	sprite.SetAnimationFrameTime("Count", 45);
	sprite.SetAnimation("Count");
	bgsprite.CreateAnimation("Idle", 0, 0);
	bgsprite.SetAnimation("Idle");
	boxSprite.CreateAnimation("Idle", 0, 0);
	boxSprite.SetAnimation("Idle");
	//sprite2.CreateAnimation("View", 0, 0);
	//sprite2.SetAnimation("View");

	s_AnimObj->SetPosition(MakeFloat3(0.0f, 40.0f, 0.0f));
	s_AnimObj->Activation();
	s_AnimObj->Show();

	s_background->SetPosition(MakeFloat3(0.0f, 0.0f, 100.0f));
	s_background->Activation();
	s_background->Show();
	//s_view->SetPosition(MakeFloat3(0.0f, 0.0f, 0.0f));
	//s_view->Activation();
	//s_view->Show();

	s_box->SetPosition(MakeFloat3(400.0f, 0.0f, 0.0f));
	s_box->Activation();
	s_box->Show();

	

	polygon.position = MakeFloat3(100.0f,100.0f, 0.0f);
	polygon.width = w;
	polygon.height = h;
	polygon.color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	polygon.textureID = aimTextureID;
	
	polygon.Draw();

	//コマンドを追加
	//スペースキーとBボタンの両方でジャンプするようにする
	Input::Bind(Command::Jump, Key::Space);
	Input::Bind(Command::Jump, Button::B);
	Input::Bind(Command::NewCommand, Key::Enter);
	Input::Bind(Command::NewCommand, Button::A);

	//地面のテクスチャ読み込み
	groundTexture = LoadTexture("rom/texture/ground/ground1.tga");

	//地面のポリゴン
	groundPolygon.position = MakeFloat3(640.0f, 600.0f, 0.0f); // 画面の下側
	groundPolygon.width = 2000.0f;
	groundPolygon.height = 420.0f;
	groundPolygon.color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	groundPolygon.textureID = groundTexture;
}

void Game::UpdateSample(float dt)
{
	Float2 moveVector;

	moveVector.x = Input::GetAxis(Command::MoveX);

	//傾き量でキャラを移動させる(switch)
	leftControllerRotate = GetControllerLeftAngle();
	leftControllerAccel = GetControllerLeftAcceleration();
	rightControllerRotate = GetControllerRightAngle();
	rightControllerAccel = GetControllerRightAcceleration();



	float Input = rightControllerRotate.z * 0.5;

	if (std::abs(Input) < 0.1f)
	{
		Input = 0.0f;
	}

	//player移動WASD
	float moveDirX = Input::GetAxis(Command::Axis::MoveX);
	float moveDirY = Input::GetAxis(Command::Axis::MoveY);
	float  moveSpeed = 400.0f;

	//左右移動
	float speed = 50.0f;
	Float3 moveValue = { 0.0f,0.0f,0.0f };
	moveValue.x = moveDirX * moveSpeed * dt;
	//moveValue.x=moveVector*speed*dt;
	//smoveValue.y = moveDirY * moveSpeed * dt;

	//// カーソルポリゴン移動（マウス）
	//Float2 mousePos = Input::GetMousePosition();
	//polygon.position.x = mousePos.x;
	//polygon.position.y = mousePos.y;
	
	/*polygon.position.x += moveSpeed * moveDirX * dt;
	polygon.position.y += moveSpeed * moveDirY * dt;*/



	//
		//プレイヤーの位置
		Float3 playerPos = s_AnimObj->GetPosition();
		//ジャンプ開始時のY座標
		float g_jumpStartY = playerPos.y;

		//箱の位置
		Float3 boxPos = s_box->GetPosition();
		//プレイヤーが向かうターゲット位置
		Float3 cursorPos = polygon.position;

		//マウス座標を取得
		Float2 mousePos = Input::GetMousePosition();

		//カーソルポリゴンの位置をマウスと連動
		polygon.position.x = mousePos.x;
		polygon.position.y = mousePos.y;

		//座標のずれの補正
		float offsetX = -620.0f; 
		float offsetY = -350.0f;

		// 目標となる座標を決定
		float targetX = cursorPos.x + offsetX;
		float targetY = cursorPos.y + offsetY;

		// プレイヤーから目標座標への距離を求める
		float distanceX = targetX - playerPos.x;
		float distanceY = targetY - playerPos.y;

		//箱からプレイヤーへの距離を求める
		float boxDistanceX = playerPos.x- boxPos.x;
		float boxDistanceY = playerPos.y- boxPos.y;


		//引っ張る力の係数（値がでかいほど早く動く）
		float pullForce = 8.0f;

		//boxの当たり判定用の矩形を作成
		Rectangle boxRect;
		float boxW = s_box->GetSprite().GetPolygonSize().x;
		float boxH = s_box->GetSprite().GetPolygonSize().y;

		boxRect.x = boxPos.x+600.0f;
		boxRect.y = boxPos.y+330.0f;
		boxRect.width = boxW;
		boxRect.height = boxH;

		//polygonの当たり判定用の矩形を作成
		Rectangle cursorRect;
		cursorRect.x = polygon.position.x - polygon.width / 2.0f;
		cursorRect.y = polygon.position.y - polygon.height / 2.0f;
		cursorRect.width = polygon.width;
		cursorRect.height = polygon.height;

		// プレイヤーの現在位置
		Float3 movedPos = s_AnimObj->GetPosition();

		// プレイヤーの当たり判定サイズ
		float playerW = s_AnimObj->GetSprite().GetPolygonSize().x;
		float playerH = s_AnimObj->GetSprite().GetPolygonSize().y;

		// プレイヤー矩形
		Rectangle playerRect;
		playerRect.x = movedPos.x - playerW / 2.0f;
		playerRect.y = movedPos.y - playerH / 2.0f-20;
		playerRect.width = playerW;
		playerRect.height = playerH;

		// 地面の当たり判定
		Rectangle groundRect;
		groundRect.x = groundPolygon.position.x - groundPolygon.width / 2.0f;
		groundRect.y = groundPolygon.position.y - groundPolygon.height / 2.0f;
		groundRect.width = groundPolygon.width;
		groundRect.height = groundPolygon.height;

		//ジャンプ処理
		if (Input::IsTrigger(Command::Jump))
		{
			if (!g_jumpState)
			{
				g_verticalVelocity = g_jumpinitialSpeed;//初速
				g_jumpState = true;

				PlaySE(SAMPLE_02);
			}
		}

		if (g_jumpState == true)
		{
			g_verticalVelocity -= g_gravity * dt;
			moveValue.y -= g_verticalVelocity * dt;

			//PlayBGM(SAMPLE01);
			PlayBGM(Cyber_BGM);
			PlaySE(Landing);
		}

		//箱とカーソルの当たり判定をチェック
		bool boxInsideCursor = CheckRectangleCollision(boxRect, cursorRect);

		//選んだ場所にプレイヤーを移動させる処理
		if (Input::IsTrigger(Command::NewCommand))
		{
		
			g_boxRect = boxRect;
			g_cursorRect = cursorRect;
			g_boxInsideCursor = boxInsideCursor;
			Float3 playerMove;

			//boxの引き寄せ
			if (g_boxInsideCursor == true)
			{
				//boxをplayerの位置に引き寄せる
				Float3 boxMove;
				boxMove.x = (playerPos.x - boxPos.x);
				boxMove.y = (playerPos.y - boxPos.y);
				boxMove.z = 0.0f;

				s_box->Move(boxMove);
			}

			if (g_boxInsideCursor == false)
			{
				
				//box が当たり判定外 → player を引き寄せる
				
				playerMove.x = (targetX - playerPos.x);
				playerMove.y = (targetY - playerPos.y);

				s_AnimObj->Move(playerMove);
			}

		/*//距離が近いときはカーソルにピタッと合わせる
		if (std::abs(distanceX) < 5.0f && std::abs(distanceY) < 5.0f)
		{
			playerPos.x = targetX;
			playerPos.y = targetY;
			s_AnimObj->SetPosition(playerPos);
		}
		else
		{		//プレイヤーが引っ張られる処理

			//毎フレーム移動量に加算する //挙動が引っ張られるような感じになる
			moveValue.x += distanceX * pullForce * dt;
			moveValue.y += distanceY * pullForce * dt;
		}

		if (std::abs(boxDistanceX) < 3.0f && std::abs(distanceY) < 5.0f)
		{
			boxPos.x = playerPos.x;
			boxPos.y = playerPos.y;
		}
		else
		{	//箱がプレイヤーに引っ張られる処理
			Float3 boxMove;
			boxMove.x = boxDistanceX * pullForce * dt;
			boxMove.y = boxDistanceY * pullForce * dt;
			boxMove.z = 0.0f;

			s_box->Move(boxMove);
		}*/
		}

	s_AnimObj->Move(moveValue);

	Float3 pos = s_AnimObj->GetPosition();
	// 地面の上面座標
	float groundTop = groundRect.y-300;
	// プレイヤーの足元座標
	float playerBottom = playerRect.y + playerRect.height;

	// 衝突判定
	if (CheckRectangleCollision(playerRect, groundRect))
	{
		// 下にめり込んでいるなら上に補正
		if (playerBottom > groundTop)
		{
			pos.y = groundTop - playerH / 2.0f;
			s_AnimObj->SetPosition(pos);
			g_jumpState = false; // 着地
			g_verticalVelocity = 0.0f;
		}
	}
	
	

	//画面外に出ていかないようにする
	Float3 clampedPos = s_AnimObj->GetPosition();
	const float c_SCREENWIDTH = 1280.0f;
	const float c_SCREENHEIGHT = 720.0f;
	const float c_objWIDTH = 720.0f;

	float halfpolygonWidth = polygon.width/2.0f;
	float halfpolygonHeight = polygon.height / 2.0f;

	//端の制限(player)
	if (clampedPos.x < -450.0f)
	{
		clampedPos.x = -450.0f;
	}
	else if (clampedPos.x > (c_SCREENWIDTH - c_objWIDTH))
	{
		clampedPos.x = (c_SCREENWIDTH - c_objWIDTH);
	}
	else if (clampedPos.y < -300.0f)
	{
		clampedPos.y = -300.0f;
	}
	else if (clampedPos.y > (c_SCREENWIDTH - c_SCREENHEIGHT))
	{
		clampedPos.y = (c_SCREENWIDTH - c_SCREENHEIGHT);
	}

	//端の制限(polygon)
	if (polygon.position.x-halfpolygonWidth<0.0f)
	{
		polygon.position.x = halfpolygonWidth;
	}
	else if (polygon.position.x+halfpolygonWidth > c_SCREENWIDTH)
	{
		polygon.position.x = (c_SCREENWIDTH - halfpolygonWidth);
	}
	else if (polygon.position.y - halfpolygonHeight < 0.0f)
	{
		polygon.position.y = halfpolygonHeight;
	}
	else if (polygon.position.y + halfpolygonHeight > c_SCREENHEIGHT)
	{
		polygon.position.y = (c_SCREENHEIGHT - halfpolygonHeight);
	}

	//再配置
	s_AnimObj->SetPosition(clampedPos);

	if (Input::IsPress(Command::ShoulderL))
	{
		SetControllerLeftAngleBase();
		SetControllerRightAngleBase();

		s_AnimObj->SetPosition(MakeFloat3(0.0f, 0.0f, 0.0f));
	}


	/*if (r <= 0.0f)
	{
		r += 0.1f;
	}
	else if (r >= 1.0f)
	{
		r = 0.0f;
	}

	if (g <= 0.0f)
	{
		g += 0.1f;
	}
	else if (g >= 1.0f)
	{
		g = 0.0f;
	}
	if (b <= 0.0f)
	{
		b += 0.1f;
	}
	else if (b >= 1.0f)
	{
		b = 0.0f;
	}
	r += 0.05f;
	g += 0.3f;
	b += 0.15f;
	*/
}


void Game::DrawSample()
{
	/*//テクスチャを使わない
	Renderer::SetTexture(0);

	UpdateSample(1.0f);

	//各頂点の座標を設定する（画面座標：左上が(0,0))
	vertex[0].Position = MakeFloat3(x,y, 0.0f);
	vertex[1].Position = MakeFloat3(x+w, y, 0.0f);
	vertex[2].Position = MakeFloat3(x, y+h, 0.0f);
	vertex[3].Position = MakeFloat3(x+w,y+h, 0.0f);

	//色（白）とテクスチャ座標
	for (int i = 0; i < 4; i++)
	{
		vertex[i].Color = MakeFloat4(r, g, b, 1.0f);
		vertex[i].TexCoord = MakeFloat2(1.0f, 1.0f);
	}

	//描画
	Renderer::DrawVertices(vertex, 4,DrawMode::TriangleStrip);*/

	/*Text::SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	Text::Draw("Left Angle", 100, 10, 32);
	Text::Draw(std::to_string(leftControllerRotate.x).c_str(), 290, 10, 32);
	Text::Draw(std::to_string(leftControllerRotate.y).c_str(), 290, 40, 32);
	Text::Draw(std::to_string(leftControllerRotate.z).c_str(), 290, 70, 32);

	Text::Draw("Left Axl", 100, 110, 32);
	Text::Draw(std::to_string(leftControllerAccel.x).c_str(), 290, 110, 32);
	Text::Draw(std::to_string(leftControllerAccel.y).c_str(), 290, 140, 32);
	Text::Draw(std::to_string(leftControllerAccel.z).c_str(), 290, 170, 32);

	Text::Draw("Right Angle", 100, 210, 32);
	Text::Draw(std::to_string(rightControllerRotate.x).c_str(), 290, 210, 32);
	Text::Draw(std::to_string(rightControllerRotate.y).c_str(), 290, 240, 32);
	Text::Draw(std::to_string(rightControllerRotate.z).c_str(), 290, 270, 32);

	Text::Draw("Right Axl", 100, 310, 32);
	Text::Draw(std::to_string(rightControllerAccel.x).c_str(), 290, 310, 32);
	Text::Draw(std::to_string(rightControllerAccel.y).c_str(), 290, 340, 32);
	Text::Draw(std::to_string(rightControllerAccel.z).c_str(), 290, 370, 32);*/

	// ★ デバッグ矩形描画
	DrawDebugRect(g_cursorRect, MakeFloat4(1, 1, 1, 0.3f));  // 白
	DrawDebugRect(g_boxRect, MakeFloat4(1, 0, 0, 0.3f));  // 赤

	// ★ 判定ログ
	Text::Draw(g_boxInsideCursor ? "Inside" : "Outside", 100, 500, 32);


	polygon.Draw();
	groundPolygon.Draw();
}

