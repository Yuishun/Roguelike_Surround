// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterStatus.h"
#include "PlayerPawn.h"
#include "Enemy.generated.h"

UCLASS()
class SURROUND_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	bool bisDead;

	UPROPERTY()
		UCharacterStatus* Status;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Move(APlayerPawn* player) {}

	UFUNCTION(BlueprintCallable)
		void SetActiveAllComponents(bool active);

};
