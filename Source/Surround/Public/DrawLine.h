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

	// 頂点座標のリスト
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> vertices;	
	// インデックスのリスト
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> indices;
	// 「頂点座標のインデックスを表す」リスト（インデックスの前後は線として繋がっている）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> vertline_idxes;
	// これ以降のインデックスの頂点(vertices)は、追加されたものである
	UPROPERTY(EditAnywhere)
	int32 addvert_border;

	TArray<int32> used_idxes;		// 使われているverticesのインデックスを指すリスト

	int32 prev_idx;			// cur_idxの前のインデックス
	int32 cur_idx;			// 生成対象のvertline_idxesのインデックス
	int32 next_idx;			// cur_idxの次のインデックス
	int32 error_num = 0;	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DetecteMeshIdx(bool bgetfarpos);	// 三角形のインデックスをリストに追加
	void SetCounterclockwiseIdx(const FVector& ToNext, const FVector& ToPrev);	// 3つのインデックスを反時計回りになるように追加

	void FindFarPoint();	// 原点から一番遠い点を探す
	bool IsUsedIdx(const int32& i);	// 指定のインデックスが使用済みリストに入っているか
	int32 FindNextIdx(const int32& i);	// 指定のインデックスの隣を探す
	int32 FindPrevIdx(const int32& i);	// 指定のインデックスの隣を探す

	bool IsIncludePoint();	// 三角形内に点が入ってしまっているかどうか
	bool CheckInPoint(const FVector& target);	// 指定の点が三角形内にあるかどうか

	bool ColSegments(const FVector& pointA, const FVector& pointB,	// pointA:始点１, pointB:終点１
		const FVector& pointC, const FVector& pointD,				// pointA:始点２, pointB:終点２
		/*float& outT1, float& outT2,*/ FVector& outPos);				// outT1:内分比１,outT2:内分比２,outPos:交点

	FVector LineIdxToVert(const int32& idx);	// vertline_idxesのインデックスをFVectorに変換
	int32 VertToLineIdx(const FVector& target);	// FVectorをvertline_idxesのインデックスに変換

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
