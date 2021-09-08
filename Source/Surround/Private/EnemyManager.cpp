// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

// Sets default values for this component's properties
UEnemyManager::UEnemyManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bReadyEnemyMove = false;
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
		auto enemy = GetCreateEnemyType();
		if (!enemy)
		{
			continue;
		}
		enemies.Add(
			enemy
		);
	}

	bReadyEnemyMove = true;
}

// Called every frame
void UEnemyManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!bReadyEnemyMove)
		return;

	for (auto enemy : enemies)
	{
		if (enemy->bisDead)
		{
			continue;
		}
		enemy->Move(m_player);
	}

}

bool UEnemyManager::WannihilationEnemy()
{
	if (enemies.Num() <= 0)
		return true;

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

AEnemy* UEnemyManager::GetCreateEnemyType()
{
	FString path = "/Game/Surround/Blueprint/";

	path += "Enemy_Normal.Enemy_Normal_C";

	// AActor::GetWorldから、UWorldを得る
	UWorld* const World = GetWorld();
	// BlueprintのWallアクターを取得する
	TSubclassOf<class AActor> sc = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous(); // パスに該当するクラスを取得;
	if (!World || sc == nullptr)	// Nullチェック
	{
		return nullptr;
	}

	auto actor = World->SpawnActor<AActor>(sc, FVector::ZeroVector, FRotator(0, 0, 0));

	//actor->SetActorHiddenInGame(true);

	return Cast<AEnemy>(actor);
}

void UEnemyManager::SetEnemyLocation(const int32& i, FVector location)
{
	if (i >= enemies.Num())
	{
		return;
	}

	//enemies[i]->SetActorHiddenInGame(false);

	// カプセルコンポーネントの高さをZの値にする
	auto capcel = enemies[i]->GetComponentByClass(UCapsuleComponent::StaticClass());
	if(capcel)
		location.Z = Cast<UCapsuleComponent>(capcel)->GetUnscaledCapsuleHalfHeight();
	enemies[i]->SetActorLocation(location);
}