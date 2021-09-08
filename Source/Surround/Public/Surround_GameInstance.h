// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerPawn.h"
#include "Surround_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURROUND_API USurround_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	static USurround_GameInstance* GetInstance();

	APlayerPawn* Player;

	int32 Level;

};
