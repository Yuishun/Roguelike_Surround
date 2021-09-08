// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawLine.h"

// Sets default values
ADrawLine::ADrawLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("generated_mesh"));

	RootComponent = mesh;

	spline = CreateDefaultSubobject<USplineComponent>(TEXT("debug_line"));
	spline->SetupAttachment(RootComponent);
	spline->bDrawDebug = true;
	spline->bInputSplinePointsToConstructionScript = true;
	
}

// Called when the game starts or when spawned
void ADrawLine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADrawLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADrawLine::Init()
{
	used_idxes.Empty();
	vertices.Empty();
	indices.Empty();
	vertline_idxes.Empty();
}

void ADrawLine::CreateDrawLineMesh(const TArray<FVector>& lineposes)
{
	// メンバー変数の初期化
	Init();
	// 頂点群
	//TArray<FVector> vertices;
	// インデックス群
	//TArray<int32> indices;
	// 法線群（）
	TArray<FVector> normals;
	normals.Emplace(0, 0, 1);
	// テクスチャー座標群
	TArray<FVector2D> texcoords0;
	// 頂点カラー群（今回は空っぽのまま）
	TArray<FLinearColor> vertex_colors;
	// 接線群（今回は空っぽのまま）
	TArray<FProcMeshTangent> tangents;

	for (int32 i = 0; i < lineposes.Num(); i++)
	{
		// 頂点
		vertices.Emplace(lineposes[i] - lineposes[0]);
		vertices[i].Z = 0;

		// 頂点が繋がっている順番
		vertline_idxes.Add(i);

		// テクスチャ座標
		texcoords0.Emplace(0, 0);
	}

	// 追加される頂点はverticeのこのインデックスからである
	addvert_border = vertices.Num();

	TArray<FVector> Addvertices;
	// Key = 追加するvertline_idxesのIndex, Value = 追加するvertline_idxesの中身の配列
	TMap<int32, TArray<int32>> Addvertline;
	FVector AddvertPoint;
	for (int32 i = 0; i < vertices.Num(); i++)
	{
		int32 i_next = (i + 1) % vertices.Num();
		for (int32 j = i + 2; j < vertices.Num(); j++)
		{
			int32 j_next = (j + 1) % vertices.Num();
			// 終点と始点が重なっているか
			if (j == i || j == i_next || j_next == i || j_next == i_next)
			{
				continue;
			}
			if (ColSegments(vertices[i], vertices[i_next],
				vertices[j], vertices[j_next],
				AddvertPoint))
			{
				// 頂点を追加
				Addvertices.Add(AddvertPoint);
				// 追加されるIndexを保存
				if (!Addvertline.Contains(i + 1))
					Addvertline.Add(i + 1);
				// 追加されるIndexが最後より大きい場合、逆順に追加する
				if (i + 1 >= vertices.Num())
					Addvertline[i + 1].Insert(vertices.Num() + Addvertices.Num() - 1, 0);
				else
					Addvertline[i + 1].Add(vertices.Num() + Addvertices.Num() - 1);
				if (!Addvertline.Contains(j + 1))
					Addvertline.Add(j + 1);
				if (j + 1 >= vertices.Num())
					Addvertline[j + 1].Insert(vertices.Num() + Addvertices.Num() - 1, 0);
				else
					Addvertline[j + 1].Add(vertices.Num() + Addvertices.Num() - 1);
			}
		}
	}
	// 追加の頂点を追加
	vertices.Insert(Addvertices, addvert_border);
	{
		int32 idx_offset = 0;
		// 追加の頂点に伴うラインの更新
		for (auto& idx : Addvertline)
		{
			vertline_idxes.Insert(idx.Value, idx.Key + idx_offset);
			// 追加した場所が末尾でなければ、オフセットをずらす
			if(idx.Key + idx_offset != vertline_idxes.Num() - idx.Value.Num())
				idx_offset += idx.Value.Num();
		}
	}

	//for (int32 i = 0; i < lineposes.Num(); i += 2)
	//{
	//	// インデックス
	//	indices.Emplace(i);
	//	indices.Emplace((i + 1) % lineposes.Num());
	//	indices.Emplace((i + 2) % lineposes.Num());
	//}

	// 追加されていない頂点の残りが1つ以下になるまで探索
	//if (Addvertices.Num() == 0)
	//{

		while ((vertices.Num() - used_idxes.Num()) > 3)
		{
			error_num = 0;
			DetecteMeshIdx(true);
			if (error_num > 100)
				break;
		}



	//}
	//else
	//{
	//	for (int32 i = 0; i < vertline_idxes.Num(); i++)
	//	{
	//		if (vertline_idxes[i] >= addvert_border)
	//			continue;

	//		cur_idx = i;
	//		next_idx = FindNextIdx(cur_idx);
	//		prev_idx = FindPrevIdx(cur_idx);

	//		SetCounterclockwiseIdx(LineIdxToVert(next_idx) - LineIdxToVert(cur_idx),
	//			LineIdxToVert(prev_idx) - LineIdxToVert(cur_idx));
	//	}
	//}

	spline->ClearSplinePoints();
	
	for (int32 i = 0; i < vertices.Num(); i++)
	{
		spline->AddSplineLocalPoint(vertices[i]);
		spline->SetSplinePointType(i, ESplinePointType::Type::Linear);
	}

	// UProceduralMeshComponent::CreateMeshSection_LinearColor でメッシュを生成。
// 第1引数: セクション番号; 0, 1, 2, ... を与える事で1つの UProceduralMeshComponent に複数のメッシュを内部的に同時に生成できます。
// 第2引数: 頂点群
// 第3引数: インデックス群
// 第4引数: 法線群
// 第5引数: テクスチャー座標群
// 第6引数: 頂点カラー群
// 第7引数: 接線群
// 第8引数: コリジョン生成フラグ
	mesh->CreateMeshSection_LinearColor(0, vertices, indices, normals, texcoords0, vertex_colors, tangents, true);
}

void ADrawLine::SetCounterclockwiseIdx(const FVector& ToNext, const FVector& ToPrev)
{
	if (next_idx == prev_idx)
	{
		UE_LOG(LogTemp, Error, TEXT("next_idx == prev_idx"));
		return;
	}

	// 外積を求める
	float cross = (ToNext.X * ToPrev.Y) - (ToNext.Y * ToPrev.X);

	indices.Add(vertline_idxes[cur_idx]);
	if (cross < 0)	// edge1の左側にedge2がある（反時計回り）
	{
		indices.Add(vertline_idxes[next_idx]);
		indices.Add(vertline_idxes[prev_idx]);
	}
	else				// edge1の右側にedge2がある（時計周り）
	{
		// 反時計回りに修正
		indices.Add(vertline_idxes[prev_idx]);
		indices.Add(vertline_idxes[next_idx]);
	}
}

void ADrawLine::DetecteMeshIdx(bool bgetfarpos)
{
	if (bgetfarpos)
	{
		FindFarPoint();
	}

	FVector a = LineIdxToVert(cur_idx);
	FVector b = LineIdxToVert(next_idx);
	FVector c = LineIdxToVert(prev_idx);

	FVector edge1 = b - a;
	FVector edge2 = c - a;

	float angle = FMath::Acos(edge1.CosineAngle2D(edge2));
	if (FMath::Abs(angle) >= 180)
	{
		UE_LOG(LogTemp, Error, TEXT("angle:error %f"), &angle);
		return;
	}

	if (IsIncludePoint())
	{
		// 三角形内にポイントがある
		UE_LOG(LogTemp, Display, TEXT("point in triangle"));

		// 三角形の向きを保存
		FVector prev_cross = FVector::CrossProduct(edge1, edge2);
		FVector next_cross;
		do
		{
			cur_idx = FindNextIdx(cur_idx);
			next_idx = FindNextIdx(cur_idx);
			prev_idx = FindPrevIdx(cur_idx);
			next_cross = FVector::CrossProduct(
				LineIdxToVert(next_idx) - LineIdxToVert(cur_idx),
				LineIdxToVert(prev_idx) - LineIdxToVert(cur_idx)
			);
			// prev_crossとnext_crossの符号が同じになるまで次のIndexに進める
		} while (FMath::Sign<float>(prev_cross.Z) !=
			FMath::Sign<float>(next_cross.Z));

		if (++error_num > 100)
		{
			UE_LOG(LogTemp, Error, TEXT("error_num over 100"));
			return;
		}

		DetecteMeshIdx(false);	// 次のインデックスを探さずにインデックスを追加
		return;
	}
	
	// 反時計回りになるようにインデックスを追加
	SetCounterclockwiseIdx(edge1, edge2);

	used_idxes.Add(vertline_idxes[cur_idx]);
}

bool ADrawLine::IsIncludePoint()
{
	for (int32 i = 0; i < vertline_idxes.Num(); i++)
	{
		// インデックスが使用済みか、三角形になっている場合は飛ばす
		if (IsUsedIdx(vertline_idxes[i]) ||
			cur_idx == i || next_idx == i || prev_idx == i)
		{
			continue;
		}

		if (CheckInPoint(LineIdxToVert(i)))
		{
			return true;
		}
	}
	return false;
}
bool ADrawLine::CheckInPoint(const FVector& target)
{
	TArray<FVector> tp;
	tp.Add(LineIdxToVert(cur_idx));
	tp.Add(LineIdxToVert(next_idx));
	tp.Add(LineIdxToVert(prev_idx));

	if (tp.Find(target) != INDEX_NONE)
	{
		return false;
	}
	
	FVector prevnormal = FVector::ZeroVector;

	for (int32 i = 0; i < tp.Num(); i++)
	{
		FVector edge1 = (target - tp[i]);
		FVector edge2 = (target - tp[(i + 1) % tp.Num()]);

		FVector normal = FVector::CrossProduct(edge1, edge2).GetSafeNormal();

		if (prevnormal == FVector::ZeroVector)
		{
			prevnormal = normal;
			continue;
		}

		// 同じ方向でないなら、三角形の外側である
		if (FVector::DotProduct(prevnormal, normal) <= 0.99f)
		{
			return false;
		}
	}
	return true;
}

void ADrawLine::FindFarPoint()
{
	int32 faridx = -1;
	float fardist = -1;

	for (int32 i = 0; i < vertices.Num(); i++)
	{
		if (IsUsedIdx(i) ||			// 使用済みのインデックスなら処理を飛ばす
			i >= addvert_border)	// 追加された頂点なら処理を飛ばす(外側の三角形を指す恐れがあるため)
		{
			continue;
		}

		float dist = FVector::DistSquared(FVector::ZeroVector, vertices[i]);
		if (dist > fardist)
		{
			fardist = dist;
			faridx = i;
		}
	}

	cur_idx = VertToLineIdx(vertices[faridx]);
	next_idx = FindNextIdx(cur_idx);
	prev_idx = FindPrevIdx(cur_idx);

	faridx = cur_idx;	// デバッグ用(下の処理で回りすぎないため)
	// 点が直線上にある場合や同じ点を指している場合
	while (FMath::IsNearlyZero(
		FVector::CrossProduct(
		LineIdxToVert(next_idx) - LineIdxToVert(cur_idx),
		LineIdxToVert(prev_idx) - LineIdxToVert(cur_idx)).Z
		)
	)
	{
		if (vertline_idxes[next_idx] == vertline_idxes[prev_idx])
		{
			// 同じ点を指しているのでnextを一つ進める
			next_idx = FindNextIdx(next_idx);
		}
		else
		{
			// 直線状になっているので一つずらす
			cur_idx = FindNextIdx(cur_idx);
			next_idx = FindNextIdx(cur_idx);
			prev_idx = FindPrevIdx(cur_idx);
		}

		if (faridx == cur_idx)	// 1周しても見つからない場合
		{
			UE_LOG(LogTemp, Error, TEXT("error FindFarPoint over 100"));
			break;
		}
	}
}

bool ADrawLine::IsUsedIdx(const int32& i)
{
	for (auto& useIdx : used_idxes)
	{
		if (useIdx == i)
		{
			return true;
		}
	}
	return false;
}

int32 ADrawLine::FindNextIdx(const int32& i)
{
	int32 idx = i;
	while (true)
	{
		idx = (idx + 1) % vertline_idxes.Num();
		if (!IsUsedIdx(vertline_idxes[idx]))
		{
			return idx;
		}
	}
}
int32 ADrawLine::FindPrevIdx(const int32& i)
{
	int32 idx = i;
	while (true)
	{
		idx = (idx-1) >= 0 ? idx - 1 : vertline_idxes.Num() - 1;
		if (!IsUsedIdx(vertline_idxes[idx]))
		{
			return idx;
		}
	}
}

bool ADrawLine::ColSegments(const FVector& pointA, const FVector& pointB,
	const FVector& pointC, const FVector& pointD,
	/*float& outT1, float& outT2,*/ FVector& outPos)
{
	FVector seg1 = pointB - pointA;	// 線分１
	FVector seg2 = pointD - pointC;	// 線分２
	FVector v = pointC - pointA;	// 始点同士を結ぶベクトル
	float v1_v2 = (seg1.X * seg2.Y) - (seg1.Y * seg2.X);
	if (v1_v2 == 0.0f)
	{
		// 平行状態
		return false;
	}

	float v_v1 = (v.X * seg1.Y) - (v.Y * seg1.X);
	float v_v2 = (v.X * seg2.Y) - (v.Y * seg2.X);

	float t1 = v_v2 / v1_v2;	// 線分１の内分比
	float t2 = v_v1 / v1_v2;	// 線分２の内分比

	//if (outT1)
	//	outT1 = t1;
	//if (outT2)
	//	outT2 = t2;

	const float eps = 0.00001f;
	if (t1 + eps < 0 || t1 - eps>1 || t2 + eps < 0 || t2 - eps>1)
	{
		// 交差していない
		return false;
	}
	
	outPos = pointA + seg1 * t1;

	return true;
}

FVector ADrawLine::LineIdxToVert(const int32& idx)
{
	return vertices[vertline_idxes[idx]];
}

int32 ADrawLine::VertToLineIdx(const FVector& target)
{
	for (int32 i = 0; i < vertline_idxes.Num(); i++)
	{
		if (LineIdxToVert(i) == target)
		{
			return i;
		}
	}
	return -1;
}

void ADrawLine::ActiveMeshCollision(float time)
{
	// コリジョンをクエリオンリーに
	mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FTimerHandle _Handle;
	// time秒後に、コリジョンを戻す
	GetWorld()->GetTimerManager().SetTimer(_Handle, this, 
		&ADrawLine::DeActiveMeshCollision, time, false);
}

void ADrawLine::DeActiveMeshCollision()
{
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}