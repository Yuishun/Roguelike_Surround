// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DgRect.h"
#include "PlayerPawn.h"
#include "RoomController.h"
#include "DgGenerator.generated.h"

/**
* マップの状態
*/
UENUM(BlueprintType)
enum class EMapState : uint8
{
	ENone = 0,
	EFloor,
	ERoad,
	EWall,
};

/**
* ダンジョン生成クラス
*/
UCLASS()
class SURROUND_API ADgGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADgGenerator();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<int32> map;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDgRoad> roads;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0"))
		int32 width;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0"))
		int32 height;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0"))
		int32 MinRoomWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0"))
		int32 MinRoomHeight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0"))
		int32 MaxRoomWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0"))
		int32 MaxRoomHeight;

	const int32 OUTER_MERGIN = 3;	// 区画と通路の余白

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<UDgRect*> divs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		APlayerPawn* player = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Map")
		void FillArrayRect(int32 x, int32 y, int32 w, int32 h, int32 val);
	UFUNCTION(BlueprintCallable, Category = "Map")
		void FillArrayRectLTRB(int32 l, int32 t, int32 r, int32 b, int32 val);

	UFUNCTION( BlueprintCallable, Category = "Map")
		void Spawn_Floor(int32 x, int32 y, FDungeonRect walldir);	// 床を生成する
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Map")
		void SetSprite_Floor(AActor* wallActor, FDungeonRect walldir);	// 床のスプライト設定
	UFUNCTION(BlueprintCallable, Category = "Map")
		void Spawn_Road(int32 x, int32 y, FDungeonRect walldir);	// 道を生成する
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Map")
		void SetSprite_Road(AActor* wallActor, FDungeonRect walldir);	// 床のスプライト設定
	UFUNCTION(BlueprintCallable, Category = "Map")
		void Spawn_Wall(int32 x, int32 y, FDungeonRect walldir);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Map")
		void SetSprite_Wall(int32 x, int32 y, AActor* wallActor);	// 壁のスプライトを設定する

	UFUNCTION(BlueprintCallable, Category = "Map")
		void SetPlayerLocation(UDgRect* startRect);

	UFUNCTION()
	TSubclassOf<AActor> GetBluePrintClass(const FString& path);
	UFUNCTION()
	AActor* SpawnActor_BluePrintClass(const FString& path, FVector Location, FRotator Rotator);

	UFUNCTION(BlueprintCallable, Category = "Map")
		void SetMap(int32 x, int32 y, int32 val);
	UFUNCTION(BlueprintCallable, Category = "Map")
		int32 GetMap(int32 x, int32 y);
	UFUNCTION(BlueprintCallable, Category = "Map")
		bool PlaceWallInMap(int32 x, int32 y, FDungeonRect& refwalldir);

	UFUNCTION(BlueprintCallable, Category = "Road")
		void CreateRoad(UDgRect* divA, UDgRect* divB, FVector AtoB);

	UFUNCTION(BlueprintCallable, Category = "Road")
		bool ExistenceRoad(UDgRect* divA, UDgRect* divB);

	UFUNCTION(BlueprintCallable, Category = "Room")
		FVector AdjacentRoom(UDgRect* divA, UDgRect* divB);

	UFUNCTION(BlueprintCallable, Category = "Room")
		void CreateRoomContents();

	UFUNCTION(BlueprintCallable, Category = "Room")
		bool IsIsolationRoom(FDgRoad& deleteroad);


public:	

	UFUNCTION(BlueprintCallable, Category = "Generate")
		void GenerateMap();

	UFUNCTION(BlueprintCallable, Category = "Map")
		void SpawnMap();

	UFUNCTION(BlueprintCallable, Category = "Division")
		void CreateDivision(int32 left, int32 top, int32 right, int32 bottom);

	UFUNCTION(BlueprintCallable, Category = "Division")
		void SplitDivision(bool isVert, int32 id = 0);

	UFUNCTION(BlueprintCallable, Category = "Room")
		void CreateRoom();

	UFUNCTION(BlueprintCallable, Category = "Room")
		void ConnectRooms();

};
