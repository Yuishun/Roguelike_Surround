// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Enemy.h"
#include "EnemyManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURROUND_API UEnemyManager : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyManager();

	// 敵をスポーンさせるか
	bool bShouldSpawn;

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

	// 作る敵の種類を決定

};
