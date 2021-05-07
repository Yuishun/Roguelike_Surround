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
	return TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous(); // �p�X�ɊY������N���X���擾
}

AActor* ADgGenerator::SpawnActor_BluePrintClass(const FString& path, FVector Location, FRotator Rotator)
{
	// AActor::GetWorld����AUWorld�𓾂�
	UWorld* const World = GetWorld();
	// Blueprint��Wall�A�N�^�[���擾����
	TSubclassOf<class AActor> sc = GetBluePrintClass(path);
	if (!World || sc == nullptr)	// Null�`�F�b�N
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
			int32 chip = map[x + y * width];	// �`�b�v�̏�Ԃ��擾
			if (chip == (int32)EMapState::ENone)
				continue;

			// �ǂ�u���ׂ���
			FDungeonRect wallDir;
			if (PlaceWallInMap(x, y, wallDir))
			{
				Spawn_Wall(x, y, wallDir);
			}

			// ���������ꍇ
			if (chip == (int32)EMapState::EFloor)
			{
				Spawn_Floor(x, y, wallDir);
			}
			// ���������ꍇ
			else if (chip == (int32)EMapState::ERoad)
			{
				Spawn_Road(x, y, wallDir);
			}

		}
	}
}

void ADgGenerator::Spawn_Floor(int32 x, int32 y, FDungeonRect walldir)
{
	// ���Έʒu�ƃ��[���h�X�P�[���ɕϊ�
	FVector location = Super::GetActorLocation();
	float px = x * 100 + location.X;
	float py = y * 100 + location.Y;

	// �����ʒu
	FVector Location(px, py, location.Z);
	// �����p�x (���E�ɔz�u����ꍇ��Z����90�x��]������)
	FRotator Rotator(0, 0, 0);
	// ����
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
	// ���Έʒu�ƃ��[���h�X�P�[���ɕϊ�
	FVector location = Super::GetActorLocation();
	float px = x * 100 + location.X;
	float py = y * 100 + location.Y;

	// �����ʒu
	FVector Location(px, py, location.Z);
	// �����p�x (���E�ɔz�u����ꍇ��Z����90�x��]������)
	FRotator Rotator(0, 0, 0);
	// ����
	auto* actor = SpawnActor_BluePrintClass(
		FString("/Game/Surround/Blueprint/Road.Road_C"), Location, Rotator);
	if (actor == nullptr)
	{
		return;
	}

	SetSprite_Road(actor, walldir);
}

// �ǂ̐���
void ADgGenerator::Spawn_Wall(int32 x, int32 y, FDungeonRect walldir)
{
	FVector location = Super::GetActorLocation();
	float px = x * 100 + location.X;
	float py = y * 100 + location.Y;
	for (int32 i = 0; i < 4; i++)
	{
		int32 wall = 0;	// �ǂ���邩�ǂ���
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
		if (wall == 0)	// ���Ȃ��Ȃ��΂�
		{
			continue;
		}

		// �����ʒu
		FVector Location(posx, posy, location.Z);
		// �����p�x (���E�ɔz�u����ꍇ��Z����90�x��]������)
		FRotator Rotator(0, i <= 1 ? 90 : 0, 0);
		// ����
		auto* actor = SpawnActor_BluePrintClass(
			FString("/Game/Surround/Blueprint/Wall.Wall_C"), Location, Rotator);
		if (actor == nullptr)
		{
			return;
		}

		if (i < 3)	// bottom�ȊO�́A�X�v���C�g��ݒ肷��
		{
			SetSprite_Wall(x, y, actor);
		}
	}
}

// �w��̃}�b�v�`�b�v�̎l���ɉ����Ȃ��ꏊ(�ǂƂȂ�ꏊ)�����邩�ǂ���
// true = ����, false = �Ȃ�
// refwalldir �ǂ̕����ɕǂƂȂ�ꏊ�����邩�B �e�v�f�� 1 = ���̕����ɕǂ����ׂ�
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
	// ������
	map.Empty();
	divs.Empty();
	roads.Empty();

	// �z��쐬
	map.Init((int32)EMapState::ENone, width * height);

	// �ŏ��̋��쐬
	CreateDivision(0, 0, width - 1, height - 1);

	// ���̕���
	SplitDivision(FMath::RandBool(), 0);

	// �������쐬
	CreateRoom();

	// �������q����
	ConnectRooms();

	// �����̒��g���쐬
	CreateRoomContents();

	// �v���C���[��z�u
	SetPlayerLocation(*(divs.FindByPredicate([](const UDgRect* div)
		{
			return div->RoomState == ERoomState::START;
		})));

	// �}�b�v�𐶐�
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

	// ��������傫��������Ă��邩(�ŏ��̕��Ɨ]���ƒʘH�����邩)
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
	if (size < (csize + OUTER_MERGIN) * 2 + 1)	// 1 = �ʘH�̕�
	{
		return;
	}


	// ������������ɂ���āAa��b�̒l��ύX
	// true = �c , false = ��
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

	// 2�_���̋������Z�o���A�傫���Ȃ肷���Ȃ��悤�␳����
	int32 ab = FMath::Abs<int32>(b - a);
	int32 max = isVert ? MaxRoomHeight : MaxRoomWidth;
	ab = FMath::Min<int32>(ab, max);

	// �����_���Ȓn�_���o��
	int32 p = a + FMath::RandRange(0, ab);

	// ��������
	// true = �c , false = ��
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

	// �ő�̖ʐς̃G���A�ő�R���烉���_���Ɏ��̕�������������߂�
	TArray<UDgRect*> maxRect;	// �ʐς��傫�����̂��ő�R�i�[����z��
	for (auto& div : divs)
	{
		if (maxRect.Num() < 3)
		{
			maxRect.Add(div);
		}
		else
		{
			// maxRect�̒��ōŏ��̖ʐ�
			int32 minArea = maxRect[0]->Outer.Area();
			int32 idx = 0;	// minArea������maxRect�̃C���f�b�N�X
			for (int32 i = 1; i < maxRect.Num(); i++)
			{
				if (minArea > maxRect[i]->Outer.Area())
				{
					minArea = maxRect[i]->Outer.Area();
					idx = i;
				}
			}
			// minArea���ʐς��傫���ꍇ
			if (div->Outer.Area() > minArea)
			{
				// maxRect�̃T�C�Y��3�ɗ}����
				maxRect.RemoveAt(idx);
				maxRect.Add(div);
			}
		}
	}
	// maxRect�Ɠ����v�f�̃C���f�b�N�X�����߂�
	int32 nextid = divs.Find(maxRect[FMath::RandRange(0, maxRect.Num() - 1)]);
	if (nextid == INDEX_NONE)
	{
		nextid = divs.Num() - 1;
	}


	// �������������ς��Ă�����x
	SplitDivision(!isVert, nextid);
}

//=================================================================================
// Room
//=================================================================================
void ADgGenerator::CreateRoom()
{
	for (auto& div : divs)
	{
		// ������n�邱�Ƃ��ł��邩
		if (div->Outer.Width() < MinRoomWidth + OUTER_MERGIN ||
			div->Outer.Height() < MinRoomHeight + OUTER_MERGIN)
		{
			continue;
		}

		// ��̃T�C�Y
		int32 dw = div->Outer.Width() - OUTER_MERGIN;
		int32 dh = div->Outer.Height() - OUTER_MERGIN;

		// �����_���ȃT�C�Y�����߂�
		int32 sw = FMath::RandRange(MinRoomWidth, dw);
		int32 sh = FMath::RandRange(MinRoomHeight, dh);
		sw = FMath::Min(sw, MaxRoomWidth);
		sh = FMath::Min(sh, MaxRoomHeight);

		// �󂫃X�y�[�X�����߂�
		int32 rw = dw - sw;
		int32 rh = dh - sh;

		// �����̍�������߂�
		int32 rx = FMath::RandRange(2, rw);
		int32 ry = FMath::RandRange(2, rh);

		// ������}�b�v�̍��W�ɕϊ�
		int32 left = div->Outer.left + rx;
		int32 top = div->Outer.top + ry;

		// �v�f��ݒ�
		div->Room.Set(
			left, top,
			left + sw, top + sh
		);

		// �}�b�v�ɏ�������
		FillArrayRect(div->Room.left, div->Room.top,
			div->Room.Width(), div->Room.Height(),
			(int32)EMapState::EFloor);
	}
}

// �����̒��g���쐬����
void ADgGenerator::CreateRoomContents()
{
	// ��ԋ����������X�^�[�g�n�_�Ƃ���
	int32 lowArea = width * height;
	UDgRect* startRect = nullptr;
	// ��ԍL���������S�[���i�{�X�j�n�_�Ƃ���
	int32 highArea = 0;
	UDgRect* endRect = nullptr;
	for (auto& div : divs)
	{
		if (!div->HasRoom())
		{
			continue;
		}

		// �X�^�[�g�n�_�����߂�
		if (lowArea > div->Room.Area())
		{
			lowArea = div->Room.Area();
			startRect = div;
		}
		// �S�[���n�_�����߂�
		if (highArea < div->Room.Area())
		{
			highArea = div->Room.Area();
			endRect = div;
		}

		// �ʏ�̕����Ƃ��č쐬����
		div->RoomState = ERoomState::ENEMY;

		// �ʏ�̕����œG���o�Ă��鐔������
		div->enemyNum = FMath::RandRange(2, div->Room.Area() / 4 + 3);
	}

	// �X�^�[�g�n�_�̒��g���쐬
	startRect->RoomState = ERoomState::START;

	// �S�[���n�_�̒��g���쐬
	endRect->RoomState = ERoomState::BOSS;

	for (auto& div : divs)
	{
		// RoomController�֏���n���ăX�|�[��
		auto* actor = SpawnActor_BluePrintClass(
			FString("/Game/Surround/Blueprint/RoomController.RoomController_C"),
			FVector::ZeroVector, FRotator(0, 0, 0));
		// Cast���āA����������
		Cast<ARoomController>(actor)->Init(player, div);
	}
}


//=================================================================================
// Road
//=================================================================================
void ADgGenerator::ConnectRooms()
{
	// 1.�ׂ荇���Ă������roads�ɕۑ�
	for (auto& div1 : divs)
	{
		if (!div1->HasRoom())
		{
			continue;
		}

		for (auto& div2 : divs)
		{
			if (!div2->HasRoom() ||			// �����������Ă��Ȃ����A
				ExistenceRoad(div1, div2) || // ���łɑg�ݍ��킹�����݂��邩�A
				div1 == div2)				// ������悾�����ꍇ	
			{
				continue;
			}
			FVector refVec = AdjacentRoom(div1, div2);	// ��悪�ׂ荇���Ă��邩
			if (refVec == FVector::ZeroVector)	// �ׂ荇���Ă��Ȃ��ꍇ
			{
				continue;
			}
			// ���𐶐�
			CreateRoad(div1, div2, refVec);
		}
	}

	// 2.���������q�����Ă�����̂��烉���_���ɓ����폜����
	TArray<FDgRoad> delRoads;	// �폜����铹�̃��X�g�i��ɂ܂Ƃ߂ď����p�j
	for (auto& road : roads)
	{
		if (road.divA->TotalRoads() >= 2 &&	// ����2�ȏ㓹���q�����\�肩��
			road.divB->TotalRoads() >= 2 &&
			//!road.IfDeleteIsolationRoom(roads) &&	// �����Ă��Ǘ����Ȃ�����
			FMath::RandBool())			// �����_����
		{
			// �����폜����
			road.Delete();
			// �����폜���X�g�ɒǉ�
			if (IsIsolationRoom(road))
			{
				// �������ꍇ�A���ׂĂ̋�悪�Ǘ����Ȃ��ꍇ
				delRoads.Add(road);
			}
		}
	}
	// �����폜
	for (auto& d_road : delRoads)
	{
		roads.Remove(d_road);
	}

	// 3.map��road�̒ʂ�ɓ�����������
	for (auto& road : roads)
	{
		// ���̎n�_(���̕����Ɛ������̈ʒu)
		int32 beginA = -1, beginB = -1;
		// ���łɓ�������Ă���ꍇ�A���̓����g��
		if (road.ExistenceRoad(true))	// divA
		{
			beginA = road.divA->GetRoadPos(road.AtoB);
		}
		if (road.ExistenceRoad(false))	// divB
		{
			beginB = road.divB->GetRoadPos(-road.AtoB);
		}

		if (road.AtoB.Y == 0)	// �������ɐL�΂��ꍇ
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
		else					// �c�����ɐL�΂��ꍇ
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

		// ����������܂ł̓��̏I�_ (endA == endB)
		int32 endA, endB;
		endA = road.divA->GetOuter(road.AtoB);
		endB = road.divB->GetOuter(-road.AtoB);

		// ����������܂ł̓��Ƃ��̓����m���q����map�ɕ`��
		if (road.AtoB.X != 0)
		{
			if (road.AtoB.X < 0)		// A���獶�����ɐL�΂�
			{
				FillArrayRectLTRB(endA, beginA, road.divA->Room.left, beginA + 1, (int32)EMapState::ERoad);
				FillArrayRectLTRB(road.divB->Room.right, beginB, endB + 1, beginB + 1, (int32)EMapState::ERoad);
			}
			else if (road.AtoB.X > 0)	// A����E�����ɐL�΂�
			{
				FillArrayRectLTRB(road.divA->Room.right, beginA, endA + 1, beginA + 1, (int32)EMapState::ERoad);
				FillArrayRectLTRB(endB, beginB, road.divB->Room.left, beginB + 1, (int32)EMapState::ERoad);
			}

			// ��ŏo���������m���q����	end to end
			FillArrayRectLTRB(
				endA,	// endA == endB
				beginA < beginB ? beginA : beginB,
				endA + 1,
				beginA < beginB ? beginB : beginA,
				(int32)EMapState::ERoad);
		}
		else {
			if (road.AtoB.Y < 0)	// A���������ɐL�΂�
			{
				FillArrayRectLTRB(beginA, endA, beginA + 1, road.divA->Room.top, (int32)EMapState::ERoad);
				FillArrayRectLTRB(beginB, road.divB->Room.bottom, beginB + 1, endB + 1, (int32)EMapState::ERoad);
			}
			else if (road.AtoB.Y > 0)	// A���牺�����ɐL�΂�
			{
				FillArrayRectLTRB(beginA, road.divA->Room.bottom, beginA + 1, endA + 1, (int32)EMapState::ERoad);
				FillArrayRectLTRB(beginB, endB, beginB + 1, road.divB->Room.top, (int32)EMapState::ERoad);
			}

			// ��ŏo���������m���q���� end to end
			FillArrayRectLTRB(
				beginA < beginB ? beginA : beginB,
				endA,	// endA == endB
				beginA < beginB ? beginB : beginA,
				endA + 1,
				(int32)EMapState::ERoad);
		}
	}

}

// �S�Ă̕������q�����Ă��邩�ǂ���
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

// �Ԃ����Vector��X��Y�����ꂼ��A0�ȊO�Ȃ�ׂ荇���Ă���
// divA���猩�� X = ���E -1/1, Y = �㉺ -1/1
FVector ADgGenerator::AdjacentRoom(UDgRect* divA, UDgRect* divB)
{
	// �Ԃ�������
	FVector refVec = FVector::ZeroVector;
	int32 dir = 0;

	// ���E�ʒu�֌W��\��
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

	// left�̋���right��right�̋���left�ƈ�v���Ă���ꍇ�A�ׂ荇���Ă���
	if (left->Outer.right == right->Outer.left)
	{
		refVec.X = dir;
		return refVec;
	}

	// �㉺�̈ʒu�֌W��\��
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

	// top�̋���bottom��bottom�̋���top�ƈ�v���Ă���ꍇ�A�ׂ荇���Ă���
	if (top->Outer.bottom == bottom->Outer.top)
	{
		refVec.Y = dir;
	}

	return refVec;
}

// roads�ɂ��łɑ��݂��Ă���g�ݍ��킹���ǂ���
bool ADgGenerator::ExistenceRoad(UDgRect* divA, UDgRect* divB)
{
	for (auto& road : roads)
	{
		if (road.Equal(divA, divB))
			return true;
	}
	return false;
}

// �q���铹�̑g�ݍ��킹���쐬����(A��B���ׂ荇���Ă��邩�͊m�F���Ȃ�)
void ADgGenerator::CreateRoad(UDgRect* divA, UDgRect* divB, FVector AtoB)
{
	FDgRoad road;
	road.Set(divA, divB, AtoB);
	roads.Add(road);
}

