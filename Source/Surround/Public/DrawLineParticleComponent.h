// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DrawLineParticleComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURROUND_API UDrawLineParticleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDrawLineParticleComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector m_ParticlePosition;

		TArray<FVector> m_MovePositions;

	int32 MoveIdx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bActive;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void Init(const FVector& ptPos);// , const TArray<FVector> ptmoveposes);

	UFUNCTION(BlueprintCallable)
		FVector GetUpdateParticlePosition(const TArray<FVector> moveposes);
		
	float GetLineSpeed(const TArray<FVector> moveposes);
};
