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

	// �G���������Ԃ�
	bool bReadyEnemyMove;

	// �v���C���[�̎Q��
	APlayerPawn* m_player;

	// �Ǘ�����G
	UPROPERTY(EditAnywhere)
	TArray<AEnemy*> enemies;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// �G���S�ł�����
	bool WannihilationEnemy();

	// �G���w��̐��A���
	void CreateEnemies(int32 num);

	// ���G�̎�ނ����肵��������
	AEnemy* GetCreateEnemyType();

	// �G���A�N�e�B�u�ɂ��āA�z�u����
	void SetEnemyLocation(const int32& i, FVector location);
};
