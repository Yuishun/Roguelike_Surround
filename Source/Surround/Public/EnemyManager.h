// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Enemy.h"
#include "Enemy_Normal.h"
#include "PlayerPawn.h"
#include "EnemyManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURROUND_API UEnemyManager : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyManager();

	// 敵が動ける状態か
	bool bReadyEnemyMove;

	// プレイヤーの参照
	APlayerPawn* m_player;

	// 管理する敵
	UPROPERTY(EditAnywhere)
	TArray<AEnemy*> enemies;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 敵が全滅したか
	bool WannihilationEnemy();

	// 敵を指定の数、作る
	void CreateEnemies(int32 num);

	// 作る敵の種類を決定し生成する
	AEnemy* GetCreateEnemyType();

	// 敵をアクティブにして、配置する
	void SetEnemyLocation(const int32& i, FVector location);
};
