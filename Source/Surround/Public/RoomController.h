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

	// �v���C���[�̎Q�Ƃ�ێ�
	APlayerPawn* m_player;

	// �Ǘ�������̏��
	UDgRect* m_DgRect;

	// �Ǘ�����EnemyManager
	UEnemyManager* m_EnemyManager;

	// ���������Actor
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TArray<AActor*> m_CloseWallActors;

	// �h�A���Ǘ�����ׂ���Ԃ�������
	bool bShouldRoomDoor;

	// �h�A�̏��
	UPROPERTY(BlueprintReadWrite)
		ERoomDoorState m_RoomDoorState;

	// �h�A�̊J���ߗp�̃^�C�}�[�n���h��
	FTimerHandle m_OpenHandle;
	FTimerHandle m_CloseHandle;
	// �^�C�}�[�Ŏg������
	float m_time;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ���������Event
	void CloseRoomEvent();
	// �������J����Event
	void OpenRoomEvent();

	// �v���C���[�������ɓ��������A���������
	void CloseRoom();
	// �v���C���[�������̓G��r�ł������A�������J����
	void OpenRoom();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ������
	void Init(APlayerPawn* player, UDgRect* dgRect);
	// �������ŃX�v���C�g���Z�b�g����Event
	UFUNCTION(BlueprintImplementableEvent, Category = "RoomWall")
		void InitSetSpriteEvent();


	// �v���C���[�������ɓ����Ă��邩��Ԃ�
	bool CheckPlayerInRoom();
};
