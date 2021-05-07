// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterStatus.generated.h"

/**
 * キャラクターのステータスの基本
 */
UCLASS()
class SURROUND_API UCharacterStatus : public UObject
{
	GENERATED_BODY()
	
public:

	// 体力
	UPROPERTY(BlueprintReadWrite)
		float HP;

	// 攻撃力
	UPROPERTY(BlueprintReadWrite)
		float Attack;


public:

	void Init(float hp, float attack)
	{
		HP = hp;
		Attack = attack;
	}

};
