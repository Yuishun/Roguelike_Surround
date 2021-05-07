// Fill out your copyright notice in the Description page of Project Settings.


#include "DgGenerator.h"

// Sets default values
ADgGenerator::ADgGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	width = 150;
	height = 100;
	MinRoomWidth = 7;
	MinRoomHeight = 7;
	MaxRoomWidth = 50;
	MaxRoomHeight = 50;
}

// Called when the game starts or when spawned
void ADgGenerator::BeginPlay()
{
	Super::BeginPlay();

	GenerateMap();
}

TSubclassOf<AActor> ADgGenerator::GetBluePrintClass(const FString& path)
{
	return TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous(); // パスに該当するクラスを取得
}

AActor* ADgGenerator::SpawnActor_BluePrintClass(const FString& path, FVector Location, FRotator Rotator)
{
	// AActor::GetWorldから、UWorldを得る
	UWorld* const World = GetWorld();
	// BlueprintのWallアクターを取得する
	TSubclassOf<class AActor> sc = GetBluePrintClass(path);
	if (!World || sc == nullptr)	// Nullチェック
	{
		return nullptr;
	}

	return World->SpawnActor<AActor>(sc, Location, Rotator);
}

//=================================================================================
// Map Array2D
//=================================================================================
void ADgGenerator::SetMap(int32 x, int32 y, int32 val)
{
	map[y * width + x] = val;
}

int32 ADgGenerator::GetMap(int32 x, int32 y)
{
	return map[y * width + x];
}

void ADgGenerator::FillArrayRect(int32 x, int32 y, int32 w, int32 h, int32 val)
{
	for (int32 j = 0; j < h; j++)
	{
		for (int32 i = 0; i < w; i++)
		{
			SetMap(x + i, y + j, val);
		}
	}
}
void ADgGenerator::FillArrayRectLTRB(int32 l, int32 t, int32 r, int32 b, int32 val)
{
	FillArrayRect(l, t, r - l, b - t, val);
}

void ADgGenerator::SpawnMap()
{
	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			int32 chip = map[x + y * width];	// チップの状態を取得
			if (chip == (int32)EMapState::ENone)
				continue;

			// 壁を置くべきか
			FDungeonRect wallDir;
			if (PlaceWallInMap(x, y, wallDir))
			{
				Spawn_Wall(x, y, wallDir);
			}

			// 床だった場合
			if (chip == (int32)EMapState::EFloor)
			{
				Spawn_Floor(x, y, wallDir);
			}
			// 道だった場合
			else if (chip == (int32)EMapState::ERoad)
			{
				Spawn_Road(x, y, wallDir);
			}

		}
	}
}

void ADgGenerator::Spawn_Floor(int32 x, int32 y, FDungeonRect walldir)
{
	// 相対位置とワールドスケールに変換
	FVector location = Super::GetActorLocation();
	float px = x * 100 + location.X;
	float py = y * 100 + location.Y;

	// 生成位置
	FVector Location(px, py, location.Z);
	// 生成角度 (左右に配置する場合はZ軸を90度回転させる)
	FRotator Rotator(0, 0, 0);
	// 生成
	auto* actor = SpawnActor_BluePrintClass(
		FString("/Game/Surround/Blueprint/Road.Road_C"), Location, Rotator);
	if (actor == nullptr)
	{
		return;
	}

	SetSprite_Floor(actor, walldir);
}

void ADgGenerator::Spawn_Road(int32 x, int32 y, FDungeonRect walldir)
{
	// 相対位置とワールドスケールに変換
	FVector location = Super::GetActorLocation();
	float px = x * 100 + location.X;
	float py = y * 100 + location.Y;

	// 生成位置
	FVector Location(px, py, location.Z);
	// 生成角度 (左右に配置する場合はZ軸を90度回転させる)
	FRotator Rotator(0, 0, 0);
	// 生成
	auto* actor = SpawnActor_BluePrintClass(
		FString("/Game/Surround/Blueprint/Road.Road_C"), Location, Rotator);
	if (actor == nullptr)
	{
		return;
	}

	SetSprite_Road(actor, walldir);
}

// 壁の生成
void ADgGenerator::Spawn_Wall(int32 x, int32 y, FDungeonRect walldir)
{
	FVector location = Super::GetActorLocation();
	float px = x * 100 + location.X;
	float py = y * 100 + location.Y;
	for (int32 i = 0; i < 4; i++)
	{
		int32 wall = 0;	// 壁を作るかどうか
		float posx = px;
		float posy = py;
		switch (i)
		{
		case 0:
			wall = walldir.left;
			posx -= 50;
			break;
		case 1:
			wall = walldir.right;
			posx += 50;
			break;
		case 2:
			wall = walldir.top;
			posy -= 50;
			break;
		case 3:
			wall = walldir.bottom;
			posy += 50;
			break;
		}
		if (wall == 0)	// 作らないなら飛ばす
		{
			continue;
		}

		// 生成位置
		FVector Location(posx, posy, location.Z);
		// 生成角度 (左右に配置する場合はZ軸を90度回転させる)
		FRotator Rotator(0, i <= 1 ? 90 : 0, 0);
		// 生成
		auto* actor = SpawnActor_BluePrintClass(
			FString("/Game/Surround/Blueprint/Wall.Wall_C"), Location, Rotator);
		if (actor == nullptr)
		{
			return;
		}

		if (i < 3)	// bottom以外は、スプライトを設定する
		{
			SetSprite_Wall(x, y, actor);
		}
	}
}

// 指定のマップチップの四方に何もない場所(壁となる場所)があるかどうか
// true = ある, false = ない
// refwalldir どの方向に壁となる場所があるか。 各要素が 1 = その方向に壁を作るべき
bool ADgGenerator::PlaceWallInMap(int32 x, int32 y, FDungeonRect& refwalldir)
{
	bool ref = false;
	if (GetMap(x - 1, y) == (int32)EMapState::ENone)
	{
		refwalldir.left = 1;
		ref = true;
	}
	if (GetMap(x + 1, y) == (int32)EMapState::ENone)
	{
		refwalldir.right = 1;
		ref = true;
	}
	if (GetMap(x, y - 1) == (int32)EMapState::ENone)
	{
		refwalldir.top = 1;
		ref = true;
	}
	if (GetMap(x, y + 1) == (int32)EMapState::ENone)
	{
		refwalldir.bottom = 1;
		ref = true;
	}
	return ref;
}

//=================================================================================
// GenerateMap
//=================================================================================
void ADgGenerator::GenerateMap()
{
	// 初期化
	map.Empty();
	divs.Empty();
	roads.Empty();

	// 配列作成
	map.Init((int32)EMapState::ENone, width * height);

	// 最初の区画作成
	CreateDivision(0, 0, width - 1, height - 1);

	// 区画の分割
	SplitDivision(FMath::RandBool(), 0);

	// 部屋を作成
	CreateRoom();

	// 部屋を繋げる
	ConnectRooms();

	// 部屋の中身を作成
	CreateRoomContents();

	// プレイヤーを配置
	SetPlayerLocation(*(divs.FindByPredicate([](const UDgRect* div)
		{
			return div->RoomState == ERoomState::START;
		})));

	// マップを生成
	SpawnMap();
}

void ADgGenerator::SetPlayerLocation(UDgRect* startRect)
{
	if (player == nullptr)
	{
		return;
	}

	FVector location = GetActorLocation();
	location = location + FVector((startRect->Room.left + startRect->Room.Width() * 0.5f) * 100,
		(startRect->Room.top + startRect->Room.Height() * 0.5f) * 100,
		100);

	player->SetActorLocation(
		location
	);
}

//=================================================================================
// Division
//=================================================================================
void ADgGenerator::CreateDivision(int32 left, int32 top, int32 right, int32 bottom)
{
	UDgRect* rect = NewObject<UDgRect>();
	rect->Outer.Set(left, top, right, bottom);
	divs.Add(rect);
}

void ADgGenerator::SplitDivision(bool isVert, int32 id)
{
	int32 a, b;
	UDgRect& parent = *(divs[id]);

	// 分割する大きさが足りているか(最小の幅と余白と通路が入るか)
	int32 size, csize;
	if (isVert)
	{
		size = parent.Outer.Height();
		csize = MinRoomHeight;
	}
	else
	{
		size = parent.Outer.Width();
		csize = MinRoomWidth;
	}
	if (size < (csize + OUTER_MERGIN) * 2 + 1)	// 1 = 通路の幅
	{
		return;
	}


	// 分割する方向によって、aとbの値を変更
	// true = 縦 , false = 横
	if (isVert)
	{
		a = parent.Outer.top + (MinRoomHeight + OUTER_MERGIN);
		b = parent.Outer.bottom - (MinRoomHeight + OUTER_MERGIN);
	}
	else
	{
		a = parent.Outer.left + (MinRoomWidth + OUTER_MERGIN);
		b = parent.Outer.right - (MinRoomWidth + OUTER_MERGIN);
	}

	// 2点感の距離を算出し、大きくなりすぎないよう補正する
	int32 ab = FMath::Abs<int32>(b - a);
	int32 max = isVert ? MaxRoomHeight : MaxRoomWidth;
	ab = FMath::Min<int32>(ab, max);

	// ランダムな地点を出す
	int32 p = a + FMath::RandRange(0, ab);

	// 分割する
	// true = 縦 , false = 横
	if (isVert)
	{
		CreateDivision(parent.Outer.left, p,
			parent.Outer.right, parent.Outer.bottom);
		parent.Outer.bottom = p;
	}
	else
	{
		CreateDivision(p, parent.Outer.top,
			parent.Outer.right, parent.Outer.bottom);
		parent.Outer.right = p;
	}

	// 最大の面積のエリア最大３つからランダムに次の分割する区画を決める
	TArray<UDgRect*> maxRect;	// 面積が大きいものを最大３つ格納する配列
	for (auto& div : divs)
	{
		if (maxRect.Num() < 3)
		{
			maxRect.Add(div);
		}
		else
		{
			// maxRectの中で最小の面積
			int32 minArea = maxRect[0]->Outer.Area();
			int32 idx = 0;	// minAreaを持つmaxRectのインデックス
			for (int32 i = 1; i < maxRect.Num(); i++)
			{
				if (minArea > maxRect[i]->Outer.Area())
				{
					minArea = maxRect[i]->Outer.Area();
					idx = i;
				}
			}
			// minAreaより面積が大きい場合
			if (div->Outer.Area() > minArea)
			{
				// maxRectのサイズを3つに抑える
				maxRect.RemoveAt(idx);
				maxRect.Add(div);
			}
		}
	}
	// maxRectと同じ要素のインデックスを求める
	int32 nextid = divs.Find(maxRect[FMath::RandRange(0, maxRect.Num() - 1)]);
	if (nextid == INDEX_NONE)
	{
		nextid = divs.Num() - 1;
	}


	// 分割する方向を変えてもう一度
	SplitDivision(!isVert, nextid);
}

//=================================================================================
// Room
//=================================================================================
void ADgGenerator::CreateRoom()
{
	for (auto& div : divs)
	{
		// 部屋を創ることができるか
		if (div->Outer.Width() < MinRoomWidth + OUTER_MERGIN ||
			div->Outer.Height() < MinRoomHeight + OUTER_MERGIN)
		{
			continue;
		}

		// 基準のサイズ
		int32 dw = div->Outer.Width() - OUTER_MERGIN;
		int32 dh = div->Outer.Height() - OUTER_MERGIN;

		// ランダムなサイズを求める
		int32 sw = FMath::RandRange(MinRoomWidth, dw);
		int32 sh = FMath::RandRange(MinRoomHeight, dh);
		sw = FMath::Min(sw, MaxRoomWidth);
		sh = FMath::Min(sh, MaxRoomHeight);

		// 空きスペースを求める
		int32 rw = dw - sw;
		int32 rh = dh - sh;

		// 部屋の左上を決める
		int32 rx = FMath::RandRange(2, rw);
		int32 ry = FMath::RandRange(2, rh);

		// 左上をマップの座標に変換
		int32 left = div->Outer.left + rx;
		int32 top = div->Outer.top + ry;

		// 要素を設定
		div->Room.Set(
			left, top,
			left + sw, top + sh
		);

		// マップに書き込む
		FillArrayRect(div->Room.left, div->Room.top,
			div->Room.Width(), div->Room.Height(),
			(int32)EMapState::EFloor);
	}
}

// 部屋の中身を作成する
void ADgGenerator::CreateRoomContents()
{
	// 一番狭い部屋をスタート地点とする
	int32 lowArea = width * height;
	UDgRect* startRect = nullptr;
	// 一番広い部屋をゴール（ボス）地点とする
	int32 highArea = 0;
	UDgRect* endRect = nullptr;
	for (auto& div : divs)
	{
		if (!div->HasRoom())
		{
			continue;
		}

		// スタート地点を決める
		if (lowArea > div->Room.Area())
		{
			lowArea = div->Room.Area();
			startRect = div;
		}
		// ゴール地点を決める
		if (highArea < div->Room.Area())
		{
			highArea = div->Room.Area();
			endRect = div;
		}

		// 通常の部屋として作成する
		div->RoomState = ERoomState::ENEMY;

		// 通常の部屋で敵が出てくる数を決定
		div->enemyNum = FMath::RandRange(2, div->Room.Area() / 4 + 3);
	}

	// スタート地点の中身を作成
	startRect->RoomState = ERoomState::START;

	// ゴール地点の中身を作成
	endRect->RoomState = ERoomState::BOSS;

	for (auto& div : divs)
	{
		// RoomControllerへ情報を渡してスポーン
		auto* actor = SpawnActor_BluePrintClass(
			FString("/Game/Surround/Blueprint/RoomController.RoomController_C"),
			FVector::ZeroVector, FRotator(0, 0, 0));
		// Castして、初期化する
		Cast<ARoomController>(actor)->Init(player, div);
	}
}


//=================================================================================
// Road
//=================================================================================
void ADgGenerator::ConnectRooms()
{
	// 1.隣り合っている区画をroadsに保存
	for (auto& div1 : divs)
	{
		if (!div1->HasRoom())
		{
			continue;
		}

		for (auto& div2 : divs)
		{
			if (!div2->HasRoom() ||			// 部屋をもっていないか、
				ExistenceRoad(div1, div2) || // すでに組み合わせが存在するか、
				div1 == div2)				// 同じ区画だった場合	
			{
				continue;
			}
			FVector refVec = AdjacentRoom(div1, div2);	// 区画が隣り合っているか
			if (refVec == FVector::ZeroVector)	// 隣り合っていない場合
			{
				continue;
			}
			// 道を生成
			CreateRoad(div1, div2, refVec);
		}
	}

	// 2.複数道が繋がっているものからランダムに道を削除する
	TArray<FDgRoad> delRoads;	// 削除される道のリスト（後にまとめて消す用）
	for (auto& road : roads)
	{
		if (road.divA->TotalRoads() >= 2 &&	// 区画に2つ以上道が繋がれる予定かつ
			road.divB->TotalRoads() >= 2 &&
			//!road.IfDeleteIsolationRoom(roads) &&	// 消しても孤立しないかつ
			FMath::RandBool())			// ランダムに
		{
			// 道を削除する
			road.Delete();
			// 道を削除リストに追加
			if (IsIsolationRoom(road))
			{
				// 消した場合、すべての区画が孤立しない場合
				delRoads.Add(road);
			}
		}
	}
	// 道を削除
	for (auto& d_road : delRoads)
	{
		roads.Remove(d_road);
	}

	// 3.mapにroadの通りに道を書き込む
	for (auto& road : roads)
	{
		// 道の始点(道の方向と垂直軸の位置)
		int32 beginA = -1, beginB = -1;
		// すでに道が作られている場合、その道を使う
		if (road.ExistenceRoad(true))	// divA
		{
			beginA = road.divA->GetRoadPos(road.AtoB);
		}
		if (road.ExistenceRoad(false))	// divB
		{
			beginB = road.divB->GetRoadPos(-road.AtoB);
		}

		if (road.AtoB.Y == 0)	// 横方向に伸ばす場合
		{
			if (beginA == -1)
			{
				beginA = FMath::RandRange(road.divA->Room.top + 2, road.divA->Room.bottom - 2);
				road.divA->SetRoadPos(road.AtoB, beginA);
				road.divA->SetRoads(road.AtoB, -1);
			}
			if (beginB == -1)
			{
				beginB = FMath::RandRange(road.divB->Room.top + 2, road.divB->Room.bottom - 2);
				road.divB->SetRoadPos(-road.AtoB, beginB);
				road.divB->SetRoads(-road.AtoB, -1);
			}
		}
		else					// 縦方向に伸ばす場合
		{
			if (beginA == -1)
			{
				beginA = FMath::RandRange(road.divA->Room.left + 2, road.divA->Room.right - 2);
				road.divA->SetRoadPos(road.AtoB, beginA);
				road.divA->SetRoads(road.AtoB, -1);
			}
			if (beginB == -1)
			{
				beginB = FMath::RandRange(road.divB->Room.left + 2, road.divB->Room.right - 2);
				road.divB->SetRoadPos(-road.AtoB, beginB);
				road.divB->SetRoads(-road.AtoB, -1);
			}
		}

		// 部屋から区画までの道の終点 (endA == endB)
		int32 endA, endB;
		endA = road.divA->GetOuter(road.AtoB);
		endB = road.divB->GetOuter(-road.AtoB);

		// 部屋から区画までの道とその道同士を繋げてmapに描画
		if (road.AtoB.X != 0)
		{
			if (road.AtoB.X < 0)		// Aから左方向に伸ばす
			{
				FillArrayRectLTRB(endA, beginA, road.divA->Room.left, beginA + 1, (int32)EMapState::ERoad);
				FillArrayRectLTRB(road.divB->Room.right, beginB, endB + 1, beginB + 1, (int32)EMapState::ERoad);
			}
			else if (road.AtoB.X > 0)	// Aから右方向に伸ばす
			{
				FillArrayRectLTRB(road.divA->Room.right, beginA, endA + 1, beginA + 1, (int32)EMapState::ERoad);
				FillArrayRectLTRB(endB, beginB, road.divB->Room.left, beginB + 1, (int32)EMapState::ERoad);
			}

			// 上で出来た道同士を繋げる	end to end
			FillArrayRectLTRB(
				endA,	// endA == endB
				beginA < beginB ? beginA : beginB,
				endA + 1,
				beginA < beginB ? beginB : beginA,
				(int32)EMapState::ERoad);
		}
		else {
			if (road.AtoB.Y < 0)	// Aから上方向に伸ばす
			{
				FillArrayRectLTRB(beginA, endA, beginA + 1, road.divA->Room.top, (int32)EMapState::ERoad);
				FillArrayRectLTRB(beginB, road.divB->Room.bottom, beginB + 1, endB + 1, (int32)EMapState::ERoad);
			}
			else if (road.AtoB.Y > 0)	// Aから下方向に伸ばす
			{
				FillArrayRectLTRB(beginA, road.divA->Room.bottom, beginA + 1, endA + 1, (int32)EMapState::ERoad);
				FillArrayRectLTRB(beginB, endB, beginB + 1, road.divB->Room.top, (int32)EMapState::ERoad);
			}

			// 上で出来た道同士を繋げる end to end
			FillArrayRectLTRB(
				beginA < beginB ? beginA : beginB,
				endA,	// endA == endB
				beginA < beginB ? beginB : beginA,
				endA + 1,
				(int32)EMapState::ERoad);
		}
	}

}

// 全ての部屋が繋がっているかどうか
bool ADgGenerator::IsIsolationRoom(FDgRoad& deleteroad)
{
	int32 roomnum = 0;
	TArray<UDgRect*> PassedRoom;
	for (auto div : divs)
	{
		if (div->HasRoom())
		{
			roomnum++;
			if (PassedRoom.Num() == 0)
			{
				PassedRoom.Add(div);
			}
		}
	}
	TArray<FDgRoad*> ignoreroads;
	ignoreroads.Add(&deleteroad);
	//while (PassedRoom.Num() < roomnum && deleteroads.Num() < roads.Num())
	for (int32 i = 0; i < PassedRoom.Num(); i++)
	{
		for (auto& road : roads)
		{
			if (ignoreroads.Find(&road) != INDEX_NONE)
			{
				continue;
			}

			if (road.divA == PassedRoom[i])
			{
				ignoreroads.Add(&road);
				PassedRoom.AddUnique(road.divB);
			}
			else if (road.divB == PassedRoom[i])
			{
				ignoreroads.Add(&road);
				PassedRoom.AddUnique(road.divA);
			}
		}
	}

	return PassedRoom.Num() == roomnum;
}

// 返されるVectorのXとYがそれぞれ、0以外なら隣り合っている
// divAから見て X = 左右 -1/1, Y = 上下 -1/1
FVector ADgGenerator::AdjacentRoom(UDgRect* divA, UDgRect* divB)
{
	// 返される方向
	FVector refVec = FVector::ZeroVector;
	int32 dir = 0;

	// 左右位置関係を表す
	UDgRect* left, * right;
	if (divA->Outer.left <= divB->Outer.left)
	{
		left = divA;
		right = divB;
		dir = 1;
	}
	else
	{
		left = divB;
		right = divA;
		dir = -1;
	}

	// leftの区画のrightがrightの区画のleftと一致している場合、隣り合っている
	if (left->Outer.right == right->Outer.left)
	{
		refVec.X = dir;
		return refVec;
	}

	// 上下の位置関係を表す
	UDgRect* top, * bottom;
	if (divA->Outer.top <= divB->Outer.top)
	{
		top = divA;
		bottom = divB;
		dir = 1;
	}
	else
	{
		top = divB;
		bottom = divA;
		dir = -1;
	}

	// topの区画のbottomがbottomの区画のtopと一致している場合、隣り合っている
	if (top->Outer.bottom == bottom->Outer.top)
	{
		refVec.Y = dir;
	}

	return refVec;
}

// roadsにすでに存在している組み合わせかどうか
bool ADgGenerator::ExistenceRoad(UDgRect* divA, UDgRect* divB)
{
	for (auto& road : roads)
	{
		if (road.Equal(divA, divB))
			return true;
	}
	return false;
}

// 繋がる道の組み合わせを作成する(AとBが隣り合っているかは確認しない)
void ADgGenerator::CreateRoad(UDgRect* divA, UDgRect* divB, FVector AtoB)
{
	FDgRoad road;
	road.Set(divA, divB, AtoB);
	roads.Add(road);
}

