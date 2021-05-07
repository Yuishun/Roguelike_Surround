// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/SplineComponent.h"
#include "DrawLine.generated.h"

UCLASS()
class SURROUND_API ADrawLine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrawLine();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* spline;

	// ���_���W�̃��X�g
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> vertices;	
	// �C���f�b�N�X�̃��X�g
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> indices;
	// �u���_���W�̃C���f�b�N�X��\���v���X�g�i�C���f�b�N�X�̑O��͐��Ƃ��Čq�����Ă���j
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> vertline_idxes;
	// ����ȍ~�̃C���f�b�N�X�̒��_(vertices)�́A�ǉ����ꂽ���̂ł���
	UPROPERTY(EditAnywhere)
	int32 addvert_border;

	TArray<int32> used_idxes;		// �g���Ă���vertices�̃C���f�b�N�X���w�����X�g

	int32 prev_idx;			// cur_idx�̑O�̃C���f�b�N�X
	int32 cur_idx;			// �����Ώۂ�vertline_idxes�̃C���f�b�N�X
	int32 next_idx;			// cur_idx�̎��̃C���f�b�N�X
	int32 error_num = 0;	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DetecteMeshIdx(bool bgetfarpos);	// �O�p�`�̃C���f�b�N�X�����X�g�ɒǉ�
	void SetCounterclockwiseIdx(const FVector& ToNext, const FVector& ToPrev);	// 3�̃C���f�b�N�X�𔽎��v���ɂȂ�悤�ɒǉ�

	void FindFarPoint();	// ���_�����ԉ����_��T��
	bool IsUsedIdx(const int32& i);	// �w��̃C���f�b�N�X���g�p�ς݃��X�g�ɓ����Ă��邩
	int32 FindNextIdx(const int32& i);	// �w��̃C���f�b�N�X�ׂ̗�T��
	int32 FindPrevIdx(const int32& i);	// �w��̃C���f�b�N�X�ׂ̗�T��

	bool IsIncludePoint();	// �O�p�`���ɓ_�������Ă��܂��Ă��邩�ǂ���
	bool CheckInPoint(const FVector& target);	// �w��̓_���O�p�`���ɂ��邩�ǂ���

	bool ColSegments(const FVector& pointA, const FVector& pointB,	// pointA:�n�_�P, pointB:�I�_�P
		const FVector& pointC, const FVector& pointD,				// pointA:�n�_�Q, pointB:�I�_�Q
		/*float& outT1, float& outT2,*/ FVector& outPos);				// outT1:������P,outT2:������Q,outPos:��_

	FVector LineIdxToVert(const int32& idx);	// vertline_idxes�̃C���f�b�N�X��FVector�ɕϊ�
	int32 VertToLineIdx(const FVector& target);	// FVector��vertline_idxes�̃C���f�b�N�X�ɕϊ�

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void CreateDrawLineMesh(const TArray<FVector>& lineposes);

	UFUNCTION(BlueprintCallable)
		void Init();

	void ActiveMeshCollision(float time);
	void DeActiveMeshCollision();
};
