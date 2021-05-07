// Fill out ym_ copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipBookComponent.h"
#include "Camera/CameraComponent.h"
#include "CollidingPawnMovementComponent.h"
#include "DrawLine.h"
#include "DrawLineParticleComponent.h"
#include "PlayerPawn.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle,
	Walk,
	DrawLine,
	Dead,
};

UENUM(BlueprintType)
enum class EPlayerDirState : uint8
{
	Front_LEFT,
	Front,
	Front_RIGHT,
	Right,
	Back_RIGHT,
	Back,
	Back_LEFT,
	Left,
};

UCLASS()
class SURROUND_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* m_Collision;

	UPROPERTY(EditAnywhere)
	UCameraComponent* m_Camera;
	UPROPERTY()
		FVector m_DefaultCameraLocalOffset;
	UPROPERTY()
		FVector m_DrawLineCameraWorldPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* m_VisibleComponent;

	UPROPERTY(EditAnywhere)
	UCollidingPawnMovementComponent* m_MovementComponent;

	UPROPERTY(EditAnywhere)
	AActor* m_DrawLineParticle;

	UPROPERTY(EditAnywhere)
	UDrawLineParticleComponent* m_DrawLinePTComponent;

	UPROPERTY(BlueprintReadWrite)
		FVector m_InputVector;
	UPROPERTY(BlueprintReadWrite)
	EPlayerState m_PrevPlayerState;
	UPROPERTY(BlueprintReadWrite)
	EPlayerDirState m_PrevPlayerDirState;
	UPROPERTY()
	APlayerController* m_PlayerController;
	UPROPERTY(BlueprintReadWrite)
		int32 m_MousePositionIdx;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FVector> m_DrawLinePositions;
	UPROPERTY(BlueprintReadWrite)
		TArray<ADrawLine*> m_DrawLines;
	UPROPERTY(BlueprintReadWrite)
		int32 m_NowDrawLineIdx;
	UPROPERTY(BlueprintReadWrite)
	bool bDrawLine;
	UPROPERTY(BlueprintReadWrite)
	bool bDrawLine_Moving;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	// X方向への移動
	void Move_XAxis(float AxisValue);
	// Y方向への移動
	void Move_YAxis(float AxisValue);
	// フリップブックの選択
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FlipBook")
	void SetFlipBook(EPlayerDirState PDir, EPlayerState PState);
	EPlayerDirState VectorToPlayerDirState(FVector Dir);

	// DrawLine状態へ移行する
	void DrawLine_Begin();
	// DrawLine状態でのクリック時の移動
	void DrawLine_Move();
	// DrawLine状態の終了
	void DrawLine_End();
	// DrawLineの面積を求める
	UFUNCTION()
		float N_SidedArea();

	// マウスのワールド座標を取得
	FVector GetMouseWorldPos();
};
