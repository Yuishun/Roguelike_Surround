// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// このポーンが最小値のプレイヤーで制御されるように設定。0番目のプレイヤー
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// ルートコンポーネント作成
	m_Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	RootComponent = m_Collision;
	// カメラオブジェクト作成
	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));

	// 可視オブジェクトを作成
	m_VisibleComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("OurVisibleComponent"));
	
	// カメラをRootComponent以下へぶら下げる
	m_Camera->SetupAttachment(RootComponent);
	// カメラ位置を調整
	m_Camera->SetRelativeLocation(FVector(0.0f, 400.0f, 650.0f));
	m_Camera->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	// 可視オブジェクトをRootComponentにぶら下げ
	m_VisibleComponent->SetupAttachment(RootComponent);

	// MovementComponentを生成
	m_MovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>
		(TEXT("CustomMovementComponent"));
	m_MovementComponent->UpdatedComponent = RootComponent;
	
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// カメラのローカル座標を取得
	m_DefaultCameraLocalOffset = m_Camera->GetComponentLocation() - GetActorLocation();
	
	m_PlayerController = GetWorld()->GetFirstPlayerController();
	m_PrevPlayerState = EPlayerState::Idle;
	m_PrevPlayerDirState = EPlayerDirState::Front;
	bDrawLine = false;
	
	// DrawLineを３つスポーンして管理
	m_NowDrawLineIdx = 0;
	for (int32 i = 0; i < 3; i++)
	{
		auto* actor = GetWorld()->SpawnActor<AActor>(
			TSoftClassPtr<AActor>(FSoftObjectPath("/Game/Surround/Blueprint/DrawLine.DrawLine_C")).LoadSynchronous(),
			FVector(0, 0, 0),
			FRotator(0, 0, 0)
			);

		m_DrawLines.Add(
			dynamic_cast<ADrawLine*>(actor)
		);
	}

	// パーティクルのスポーンと連携
	m_DrawLineParticle= GetWorld()->SpawnActor<AActor>(
		TSoftClassPtr<AActor>(FSoftObjectPath("/Game/Surround/Blueprint/DrawLineParticle.DrawLineParticle_C")).LoadSynchronous(),
		FVector(0, 0, 0),
		FRotator(0, 0, 0)
		);
	auto* actorcomponent = m_DrawLineParticle->GetComponentByClass(UDrawLineParticleComponent::StaticClass());
	m_DrawLinePTComponent = Cast<UDrawLineParticleComponent>(actorcomponent);
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EPlayerState NowPlayerState;
	if (bDrawLine)
	{
		NowPlayerState = EPlayerState::DrawLine;
		// カメラを固定
		m_Camera->SetWorldLocation(m_DrawLineCameraWorldPos);

		// 現在のマウスポジションを取得
		m_DrawLinePositions[m_MousePositionIdx] =
			GetMouseWorldPos();

		// 初期地点と一定距離以下なら、初期地点に向かうようにする
		if (FVector::Dist(m_DrawLinePositions[0], m_DrawLinePositions[m_MousePositionIdx]) <= 60)
		{
			m_DrawLinePositions[m_MousePositionIdx] = m_DrawLinePositions[0];
		}

		// パーティクルポジションのアップデート
		m_DrawLinePTComponent->GetUpdateParticlePosition(m_DrawLinePositions);
	}
	else
	{
		if (m_InputVector.IsNearlyZero())
		{
			NowPlayerState = EPlayerState::Idle;
		}
		else
		{
			NowPlayerState = EPlayerState::Walk;
		}
	}
	
	EPlayerDirState NowPlayerDirState =
		VectorToPlayerDirState(m_InputVector);
	if (m_PrevPlayerDirState != NowPlayerDirState || 
		m_PrevPlayerState != NowPlayerState)
	{
		SetFlipBook(NowPlayerDirState, NowPlayerState);
		m_PrevPlayerDirState = NowPlayerDirState;
		m_PrevPlayerState = NowPlayerState;
	}

	// 入力方向をリセット
	m_InputVector = FVector::ZeroVector;
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveX", this, &APlayerPawn::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &APlayerPawn::Move_YAxis);
	InputComponent->BindAction("DrawLine", EInputEvent::IE_Pressed, this, &APlayerPawn::DrawLine_Begin);
	InputComponent->BindAction("DrawLine_Click", EInputEvent::IE_Pressed, this, &APlayerPawn::DrawLine_Move);
}

void APlayerPawn::Move_XAxis(float AxisValue)
{
	if (bDrawLine)
	{
		return;
	}

	m_InputVector.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	m_MovementComponent->AddInputVector(FVector::ForwardVector * AxisValue);
}

void APlayerPawn::Move_YAxis(float AxisValue)
{
	if (bDrawLine)
	{
		return;
	}

	m_InputVector.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	m_MovementComponent->AddInputVector(FVector::RightVector * AxisValue);
}

void APlayerPawn::DrawLine_Begin()
{
	bDrawLine = !bDrawLine;

	if (bDrawLine)	// DrawLine状態になる
	{
		// カメラを固定する位置を保存
		m_DrawLineCameraWorldPos = m_Camera->GetComponentLocation();

		// DrawLinePositionsを初期化
		m_DrawLinePositions.Empty();
		// 現在の地点を保存
		FVector location = GetActorLocation();
		location.Z = 0;
		m_DrawLinePositions.Add(location);
		// 次の地点を準備
		m_DrawLinePositions.Add(FVector::ZeroVector);
		m_MousePositionIdx = 1;	// ポジションのインデックスを保存

		// 初期位置と配列を渡す
		m_DrawLinePTComponent->Init(location);

		// 指定のDrawLineを現在の位置に移動
		location.Z = 10;
		m_DrawLines[m_NowDrawLineIdx]->SetActorLocation(location);
		m_DrawLines[m_NowDrawLineIdx]->mesh->ClearAllMeshSections();	// meshの形をリセット
		m_DrawLineParticle->SetActorLocation(location);
	}
	else			// DrawLine状態を解除
	{
		// マウスの位置を除外
		m_DrawLinePositions.RemoveAt(m_MousePositionIdx);
		DrawLine_End();
	}
}

void APlayerPawn::DrawLine_Move()
{
	if (!bDrawLine)
	{
		return;
	}

	FVector MouseWorldLocation = m_DrawLinePositions[m_MousePositionIdx];

	for (int32 i = 0; i < m_MousePositionIdx; i++)
	{
		// クリックした地点が完全にどこかの点と同じ場合
		if (MouseWorldLocation == m_DrawLinePositions[i])
		{
			m_DrawLinePositions.RemoveAt(m_MousePositionIdx);
			if (i == 0)		// マウスの位置が始点と同じ場合、終了処理へ
			{
				DrawLine_End();
			}
			return;
		}
	}

	FVector ActorLocation = GetActorLocation();
	ActorLocation.Z = 0;
	//MouseWorldLocation.Z = 100;
	m_InputVector = MouseWorldLocation - ActorLocation;	// 目的地へのベクトル
	m_MovementComponent->CollidingMove(m_InputVector);	// MovementComponentで動かす
	m_DrawLinePositions[m_MousePositionIdx] = GetActorLocation();	// 動いた後の位置を入れる
	m_DrawLinePositions[m_MousePositionIdx].Z = 0;
	// 次の点を準備
	m_MousePositionIdx = m_DrawLinePositions.Add(FVector::ZeroVector);
}

void APlayerPawn::DrawLine_End()
{
	// カメラをローカル座標に戻す
	m_Camera->SetWorldLocation(GetActorLocation() + m_DefaultCameraLocalOffset);

	if (m_DrawLinePositions.Num() > 2) {
		m_DrawLines[m_NowDrawLineIdx]->CreateDrawLineMesh(m_DrawLinePositions);
		m_NowDrawLineIdx = (m_NowDrawLineIdx + 1) % m_DrawLines.Num();
	}

	for (auto* drawline : m_DrawLines)
	{
		if (drawline->vertices.Num() > 0)
		{
			drawline->ActiveMeshCollision(1);
		}
	}

	bDrawLine = false;
	m_DrawLinePTComponent->bActive = false;
}

UPawnMovementComponent* APlayerPawn::GetMovementComponent() const
{
	return m_MovementComponent;
}

EPlayerDirState APlayerPawn::VectorToPlayerDirState(FVector Dir)
{
	EPlayerDirState refDir = m_PrevPlayerDirState;
	if (Dir.Y > 0)
	{
		if (Dir.X < 0)
		{
			refDir = EPlayerDirState::Front_LEFT;
		}
		else if (Dir.X == 0)
		{
			refDir = EPlayerDirState::Front;
		}
		else
		{
			refDir = EPlayerDirState::Front_RIGHT;
		}
	}
	else if (Dir.Y < 0)
	{
		if (Dir.X < 0)
		{
			refDir = EPlayerDirState::Back_LEFT;
		}
		else if (Dir.X == 0)
		{
			refDir = EPlayerDirState::Back;
		}
		else
		{
			refDir = EPlayerDirState::Back_RIGHT;
		}
	}
	else if (Dir.X > 0)
	{
		refDir = EPlayerDirState::Right;
	}
	else if(Dir.X < 0)
	{
		refDir = EPlayerDirState::Left;
	}

	return refDir;
}

float APlayerPawn::N_SidedArea()
{
	float refArea = 0;
	int32 DL_NUM = m_DrawLinePositions.Num();
	for (int32 i = 0; i < DL_NUM; i++)
	{
		refArea +=
			FMath::Abs(
				(m_DrawLinePositions[i].X * i * m_DrawLinePositions[(i + 1) % DL_NUM].Y) -
				(m_DrawLinePositions[(i + 1) % DL_NUM].X * m_DrawLinePositions[i].Y * i));
	}
	return 0.5f * refArea;
}

FVector APlayerPawn::GetMouseWorldPos()
{
	FVector WorldLocation;
	FVector WorldDirection;
	m_PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	// マウスのクリックした地点
	FVector MouseWorldLocation = FMath::LinePlaneIntersection(
		WorldLocation,
		WorldLocation + WorldDirection,
		FVector(0, 0, 0),
		FVector::UpVector
	);

	return MouseWorldLocation;
}