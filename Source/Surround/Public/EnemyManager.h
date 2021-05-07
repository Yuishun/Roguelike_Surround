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

	// “G‚ğƒXƒ|[ƒ“‚³‚¹‚é‚©
	bool bShouldSpawn;

	// ŠÇ—‚·‚é“G
	UPROPERTY(EditAnywhere)
	TArray<AEnemy*> enemies;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// “G‚ª‘S–Å‚µ‚½‚©
	bool WannihilationEnemy();

	// “G‚ğw’è‚Ì”Aì‚é
	void CreateEnemies(int32 num);

	// ì‚é“G‚Ìí—Ş‚ğŒˆ’è

};
