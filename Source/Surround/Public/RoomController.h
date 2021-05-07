// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "DgRect.h"
#include "PlayerPawn.h"
#include "EnemyManager.h"
#include "RoomController.generated.h"

UENUM(BlueprintType)
enum class ERoomDoorState : uint8
{
	CLOSE,
	OPEN,
	ANIMATION,
};

UCLASS()
class SURROUND_API ARoomController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoomController();

	// プレイヤーの参照を保持
	APlayerPawn* m_player;

	// 管理する区画の情報
	UDgRect* m_DgRect;

	// 管理するEnemyManager
	UEnemyManager* m_EnemyManager;

	// 部屋を閉じるActor
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TArray<AActor*> m_CloseWallActors;

	// ドアを管理するべき状態かを示す
	bool bShouldRoomDoor;

	// ドアの状態
	UPROPERTY(BlueprintReadWrite)
		ERoomDoorState m_RoomDoorState;

	// ドアの開け閉め用のタイマーハンドル
	FTimerHandle m_OpenHandle;
	FTimerHandle m_CloseHandle;
	// タイマーで使う時間
	float m_time;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 部屋を閉じるEvent
	void CloseRoomEvent();
	// 部屋を開けるEvent
	void OpenRoomEvent();

	// プレイヤーが部屋に入った時、部屋を閉じる
	void CloseRoom();
	// プレイヤーが部屋の敵を殲滅した時、部屋を開ける
	void OpenRoom();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 初期化
	void Init(APlayerPawn* player, UDgRect* dgRect);
	// 初期化でスプライトをセットするEvent
	UFUNCTION(BlueprintImplementableEvent, Category = "RoomWall")
		void InitSetSpriteEvent();


	// プレイヤーが部屋に入っているかを返す
	bool CheckPlayerInRoom();
};
