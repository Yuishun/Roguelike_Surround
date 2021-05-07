// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DgRect.generated.h"

/**
* 矩形座標管理構造体
*/
USTRUCT(BlueprintType)
struct FDungeonRect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int32 left;

	UPROPERTY(BlueprintReadWrite)
		int32 top;

	UPROPERTY(BlueprintReadWrite)
		int32 right;

	UPROPERTY(BlueprintReadWrite)
		int32 bottom;

public:

	FDungeonRect()
	{
		Set(0, 0, 0, 0);
	}

	void Set(int32 l, int32 t, int32 r, int32 b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	int32 Width() { return right - left; }
	int32 Height() { return bottom - top; }

	int32 Area() { return Width() * Height(); }

	void Copy(FDungeonRect rect)
	{
		left = rect.left;
		top = rect.top;
		right = rect.right;
		bottom = rect.bottom;
	}

	bool operator==(const FDungeonRect& rect)
	{
		return left == rect.left &&
			top == rect.top &&
			right == rect.right &&
			bottom == rect.bottom;
	}
};

UENUM(BlueprintType)
enum class ERoomState : uint8
{
	START,
	ENEMY,
	SHOP,
	BOSS,
};


/**
 * 区画を管理するクラス
 */
UCLASS()
class SURROUND_API UDgRect : public UObject
{
	GENERATED_BODY()
	

public:

	// 区画の情報
	UPROPERTY()
		FDungeonRect Outer;

	// 部屋の情報
	UPROPERTY()
		FDungeonRect Room;

	// 繋がっている道の数
	UPROPERTY()
		FDungeonRect Roads;

	// 繋がっている道の位置
	UPROPERTY()
		FDungeonRect RoadsPos;

	UPROPERTY(BlueprintReadWrite)
		ERoomState RoomState;

	UPROPERTY(BlueprintReadWrite)
		int32 enemyNum;

public:
	UDgRect();

	UFUNCTION(BlueprintCallable)
	bool HasRoom();

	UFUNCTION(BlueprintCallable)
	int32 TotalRoads();

	UFUNCTION(BlueprintCallable)
	bool IsIsolation(FVector dir);

	UFUNCTION(BlueprintCallable)
	int32 GetOuter(FVector dir);

	UFUNCTION(BlueprintCallable)
	int32 GetRoom(FVector dir);
	UFUNCTION(BlueprintCallable)
	void SetRoom(FVector dir, int32 val);

	UFUNCTION(BlueprintCallable)
	void SetRoads(FVector dir, int32 val);

	UFUNCTION(BlueprintCallable)
	int32 GetRoadPos(FVector dir);
	UFUNCTION(BlueprintCallable)
	void SetRoadPos(FVector dir, int32 val);

	bool operator==(const UDgRect& rect)
	{
		return Outer == rect.Outer;
	}
};

/*
* 道を管理する構造体
* それぞれの方向に繋がっている区画を保存する
*/
USTRUCT(BlueprintType)
struct FDgRoad
{
	GENERATED_BODY()

public:

	UPROPERTY()
		UDgRect* divA;

	UPROPERTY()
		UDgRect* divB;

	UPROPERTY()
		FVector AtoB;


public:

	void Set(UDgRect* diva, UDgRect* divb, FVector atob)
	{
		divA = diva;
		divB = divb;
		AtoB = atob;
		if (AtoB.X < 0)
		{
			divA->Roads.left++;
			divB->Roads.right++;
		}
		else if (AtoB.X > 0)
		{
			divA->Roads.right++;
			divB->Roads.left++;		
		}
		else if (AtoB.Y < 0)
		{
			divA->Roads.top++;
			divB->Roads.bottom++;		
		}
		else if (AtoB.Y > 0)
		{
			divA->Roads.bottom++;
			divB->Roads.top++;
		}
	}

	void Delete()
	{
		if (AtoB.X < 0)
		{
			divA->Roads.left--;
			divB->Roads.right--;
		}
		else if (AtoB.X > 0)
		{
			divA->Roads.right--;
			divB->Roads.left--;
		}
		else if (AtoB.Y < 0)
		{
			divA->Roads.top--;
			divB->Roads.bottom--;
		}
		else if (AtoB.Y > 0)
		{
			divA->Roads.bottom--;
			divB->Roads.top--;
		}
	}

	bool Equal(UDgRect* diva, UDgRect* divb)
	{
		return (divA == diva && divB == divb) ||
			(divA == divb && divB == diva);
	}

	bool operator== (const FDgRoad& road)
	{
		return divA == road.divA &&
			divB == road.divB;
	}

	// その方向に道を生成したか	true = 生成されている
	// mode = A / B
	bool ExistenceRoad(bool mode)
	{
		int32 road = 0;
		if (AtoB.X < 0)
		{
			if(mode)
				road = divA->Roads.left;
			else
				road = divB->Roads.right;
		}
		else if (AtoB.X > 0)
		{
			if(mode)
				road = divA->Roads.right;
			else
				road = divB->Roads.left;
		}
		else if (AtoB.Y < 0)
		{
			if(mode)
				road = divA->Roads.top;
			else
				road = divB->Roads.bottom;
		}
		else if (AtoB.Y > 0)
		{
			if(mode)
				road = divA->Roads.bottom;
			else
				road = divB->Roads.top;
		}
		return road < 0;
	}

//	// もし、この道を消したなら、孤立するかどうか	true = 孤立する
//	bool IfDeleteIsolationRoom(const TArray<FDgRoad> roads)
//	{
//		// この道を消し、どちらかの繋がっている区画の道が一つだけなら、孤立する
//		return IfDeleteIsolationDiv(roads, divA) == 1 ||
//			IfDeleteIsolationDiv(roads, divB) == 1;
//	}
//
//private:
//
//	int32 IfDeleteIsolationDiv(const TArray<FDgRoad> roads, UDgRect* div)
//	{
//		// 消したときに一つしか道が繋がっていない場合
//		if (div->TotalRoads() - 1 == 1)
//		{
//			for (auto& road : roads)
//			{
//				// これと同じではない
//				if (!Equal(road.divA, road.divB))
//				{
//					// どちらかに指定の区画がある場合
//					// 残る道が繋がっている先にも、道が1つしかないなら孤立している
//					if (road.divA == div)
//					{
//						return road.divB->TotalRoads();
//					}
//					else if (road.divB == div)
//					{
//						return road.divA->TotalRoads();
//					}
//				}
//			}
//		}
//
//	}
};

