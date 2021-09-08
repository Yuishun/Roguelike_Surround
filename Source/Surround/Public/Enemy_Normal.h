// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Enemy_Normal.generated.h"

/**
 * •’Ê‚ÌG‹›“G
 */
UCLASS()
class SURROUND_API AEnemy_Normal : public AEnemy
{
	GENERATED_BODY()
	
public:

	AEnemy_Normal();

	virtual void Move(APlayerPawn* player) override;


};
