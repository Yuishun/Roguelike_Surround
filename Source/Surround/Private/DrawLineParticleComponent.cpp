// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawLineParticleComponent.h"

// Sets default values for this component's properties
UDrawLineParticleComponent::UDrawLineParticleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	Init(FVector::ZeroVector);
}


// Called when the game starts
void UDrawLineParticleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDrawLineParticleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDrawLineParticleComponent::Init(const FVector& ptPos)//, const TArray<FVector> ptmoveposes)
{
	m_ParticlePosition = ptPos;
	//m_MovePositions = ptmoveposes;
	MoveIdx = 0;
	bActive = true;
}

FVector UDrawLineParticleComponent::GetUpdateParticlePosition(const TArray<FVector> moveposes)
{
	int32 nextIdx = (MoveIdx + 1) % moveposes.Num();

	// 頂点が3つ以上の時かつ、同じ位置の場合、さらに、もう一つ進める
	while (moveposes.Num() >= 3 &&
		moveposes[nextIdx] - moveposes[MoveIdx] == FVector::ZeroVector)
	{
		MoveIdx = nextIdx;
		nextIdx = (MoveIdx + 1) % moveposes.Num();
	}

	// 距離ベクトル
	FVector distance =
		(moveposes[nextIdx] - moveposes[MoveIdx]);
	FVector dirVec = distance.GetSafeNormal();	// 方向

	// 位置を更新
	m_ParticlePosition += dirVec * GetLineSpeed(moveposes) * GetWorld()->GetDeltaSeconds();

	// 行きすぎたら修正して、目的地を変更する
	if (FVector::DotProduct(-distance, m_ParticlePosition - moveposes[nextIdx]) < 0)
	{
		m_ParticlePosition = moveposes[nextIdx];
		MoveIdx = nextIdx;
	}

	return m_ParticlePosition;
}

float UDrawLineParticleComponent::GetLineSpeed(const TArray<FVector> moveposes)
{
	float alldistance = 0;
	for (int32 i = 0; i < moveposes.Num() - 1; i++)
	{
		alldistance +=
			(moveposes[i + 1] - moveposes[i]).Size();
	}
	return alldistance / 2;
}