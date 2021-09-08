// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomController.h"
#include "Surround_GameInstance.h"

// Sets default values
ARoomController::ARoomController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_EnemyManager = CreateDefaultSubobject<UEnemyManager>(TEXT("EnemyManager"));
	//m_EnemyManager->SetupAttachment(RootComponent);
	RootComponent = m_EnemyManager;
}

// Called when the game starts or when spawned
void ARoomController::BeginPlay()
{
	Super::BeginPlay();

}

void ARoomController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ARoomController::Init(UDgRect* dgRect)
{
	m_player = USurround_GameInstance::GetInstance()->Player;
	m_DgRect = dgRect;

	// 敵・ボスが出てくるなら、閉じる判断をすべきである
	bShouldRoomDoor = m_DgRect->RoomState == ERoomState::ENEMY ||
		m_DgRect->RoomState == ERoomState::BOSS;
	if (!bShouldRoomDoor)	// 部屋を閉じないならこの先の処理を行わない
	{
		return;
	}

	// ドアの状態（空いている）
	m_RoomDoorState = ERoomDoorState::OPEN;
	// =====================================================================
		// 部屋を閉じる用のアクターの生成
	for (int32 i = 0; i < 4; i++)
	{
		FVector dirvec = FVector::ZeroVector;
		switch (i)
		{
		case 0:	// left
			dirvec = FVector(-1, 0, 0);
			break;
		case 1:	// right
			dirvec = FVector(1, 0, 0);
			break;
		case 2:	// top
			dirvec = FVector(0, -1, 0);
			break;
		case 3:	// bottom
			dirvec = FVector(0, 1, 0);
			break;
		}

		// 道が存在するかどうか
		int32 roadpos = m_DgRect->GetRoadPos(dirvec);
		if (roadpos >= 0)	// 0以上なら道がある
		{
			FVector location = FVector::ZeroVector;
			switch (i)
			{
			case 0:	// left
				location = FVector(m_DgRect->Room.left - 0.5f, roadpos, 0);
				break;
			case 1:	// right
				location = FVector(m_DgRect->Room.right - 0.5f, roadpos, 0);
				break;
			case 2:	// top
				location = FVector(roadpos, m_DgRect->Room.top - 0.5f, 0);
				break;
			case 3:	// bottom
				location = FVector(roadpos, m_DgRect->Room.bottom - 0.5f, 0);
				break;
			}

			// AActor::GetWorldから、UWorldを得る
			UWorld* const World = GetWorld();
			// BlueprintのWallアクターを取得する
			TSubclassOf<class AActor> sc =
				TSoftClassPtr<AActor>(FSoftObjectPath(
					*FString("/Game/Surround/Blueprint/Wall.Wall_C"))
					).LoadSynchronous();
			if (!World || sc == nullptr)	// Nullチェック
			{
				continue;
			}

			// 閉じ込める用のアクターを追加 左右の場合は90度回転
			m_CloseWallActors.Add(
				World->SpawnActor<AActor>(sc, location * 100,
					i <= 1? FRotator(0, 90, 0) : FRotator(0, 0, 0))
			);
		}
	}
	InitSetSpriteEvent();
	// =====================================================================
		// 敵をスポーンさせる準備
	if (bShouldRoomDoor)
	{
		if (m_DgRect->RoomState == ERoomState::ENEMY)
		{
			m_EnemyManager->m_player = m_player;
			m_EnemyManager->CreateEnemies(m_DgRect->enemyNum);
		}
		else if (m_DgRect->RoomState == ERoomState::BOSS)
		{

		}
	}
}

// Called every frame
void ARoomController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 部屋を閉じるかどうかの判断をすべき場合
	if (bShouldRoomDoor)
	{
		// 部屋を閉じるべきであり、プレイヤーが部屋に入った時
		if (m_RoomDoorState == ERoomDoorState::OPEN && CheckPlayerInRoom()
			&& !m_EnemyManager->WannihilationEnemy())
		{
			CloseRoom();
			DeployEnemies();
		}
		// 部屋を開けるべきであり、敵が全滅している場合
		else if (m_RoomDoorState == ERoomDoorState::CLOSE
			&& m_EnemyManager->WannihilationEnemy())
		{
			OpenRoom();
		}
	}
}

bool ARoomController::CheckPlayerInRoom()
{
	FVector location = m_player->GetActorLocation();
	FDungeonRect room = m_DgRect->Room;
	room.left *= 100;
	room.right -= 1;
	room.right *= 100;
	room.top *= 100;
	room.bottom -= 1;
	room.bottom *= 100;

	return room.left < location.X && room.right > location.X
		&& room.top < location.Y && room.bottom > location.Y;
}

void ARoomController::CloseRoom()
{
	m_RoomDoorState = ERoomDoorState::ANIMATION;
	m_time = 0;
	for (auto wall : m_CloseWallActors)
	{
		UBoxComponent* box = Cast<UBoxComponent>(wall->GetComponentByClass(UBoxComponent::StaticClass()));
		box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	GetWorldTimerManager().SetTimer(
		m_CloseHandle, this, &ARoomController::CloseRoomEvent, 0.02f, true
	);
}

void ARoomController::CloseRoomEvent()
{
	UE_LOG(LogTemp, Display, TEXT("CloseEvent"));
	m_time += GetWorld()->DeltaTimeSeconds * 3;
	for (auto wall : m_CloseWallActors)
	{
		UPaperSpriteComponent* papersprite = Cast<UPaperSpriteComponent>(wall->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
		// Componentの位置の設定
		FVector location = papersprite->GetComponentLocation();
		location.Z = FMath::Lerp(300.f, 0.f, m_time);
		papersprite->SetWorldLocation(location);
		// Color Alpha　の設定
		FLinearColor color = papersprite->GetSpriteColor();
		color.A = FMath::Clamp<float>(m_time, 0, 1);
		papersprite->SetSpriteColor(color);
	}
	// 終了処理
	if (m_time >= 1)
	{
		GetWorldTimerManager().PauseTimer(m_CloseHandle);
		m_RoomDoorState = ERoomDoorState::CLOSE;
		m_time = 0;
	}
}

void ARoomController::OpenRoom()
{
	m_RoomDoorState = ERoomDoorState::ANIMATION;
	m_time = 0;
	for (auto wall : m_CloseWallActors)
	{
		UBoxComponent* box = Cast<UBoxComponent>(wall->GetComponentByClass(UBoxComponent::StaticClass()));
		box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	GetWorldTimerManager().SetTimer(
		m_OpenHandle, this, &ARoomController::OpenRoomEvent, 0.02f, true
	);
}

void ARoomController::OpenRoomEvent()
{
	m_time += GetWorld()->DeltaTimeSeconds * 3;
	for (auto wall : m_CloseWallActors)
	{
		UPaperSpriteComponent* papersprite = Cast<UPaperSpriteComponent>(wall->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
		// Componentの位置の設定
		FVector location = papersprite->GetComponentLocation();
		location.Z = FMath::Lerp(0.f, 300.f, m_time);
		papersprite->SetWorldLocation(location);
		// Color Alpha　の設定
		FLinearColor color = papersprite->GetSpriteColor();
		color.A = FMath::Clamp<float>(1 - m_time, 0, 1);
		papersprite->SetSpriteColor(color);
	}
	// 終了処理
	if (m_time >= 1)
	{
		GetWorldTimerManager().PauseTimer(m_OpenHandle);
		m_time = 0;
		m_RoomDoorState = ERoomDoorState::OPEN;
		bShouldRoomDoor = false;
	}
}

void ARoomController::DeployEnemies()
{
	auto& room = m_DgRect->Room;
	for (int32 i = 0; i < m_DgRect->enemyNum; i++)
	{
		int32 x = FMath::RandRange(room.left + 1, room.right - 2);
		int32 y = FMath::RandRange(room.top + 1, room.bottom - 2);

		m_EnemyManager->SetEnemyLocation(i,
			FVector((x + 0.5f) * 100,
				(y + 0.5f) * 100,
				0)
		);
	}
}
