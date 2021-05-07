// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

// Sets default values for this component's properties
UEnemyManager::UEnemyManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEnemyManager::CreateEnemies(int32 num)
{
	for (int32 i = 0; i < num; i++)
	{
		//enemies.Add()
	}
}

// Called every frame
void UEnemyManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UEnemyManager::WannihilationEnemy()
{
	if (enemies.Num() <= 0)
		return false;

	bool refbool = true;
	for (auto enemy : enemies)
	{
		refbool &= enemy->bisDead;
		if (!refbool)
		{
			return false;
		}
	}
	return true;
}
