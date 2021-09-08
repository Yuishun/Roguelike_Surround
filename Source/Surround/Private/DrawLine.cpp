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
	// �����o�[�ϐ��̏�����
	Init();
	// ���_�Q
	//TArray<FVector> vertices;
	// �C���f�b�N�X�Q
	//TArray<int32> indices;
	// �@���Q�i�j
	TArray<FVector> normals;
	normals.Emplace(0, 0, 1);
	// �e�N�X�`���[���W�Q
	TArray<FVector2D> texcoords0;
	// ���_�J���[�Q�i����͋���ۂ̂܂܁j
	TArray<FLinearColor> vertex_colors;
	// �ڐ��Q�i����͋���ۂ̂܂܁j
	TArray<FProcMeshTangent> tangents;

	for (int32 i = 0; i < lineposes.Num(); i++)
	{
		// ���_
		vertices.Emplace(lineposes[i] - lineposes[0]);
		vertices[i].Z = 0;

		// ���_���q�����Ă��鏇��
		vertline_idxes.Add(i);

		// �e�N�X�`�����W
		texcoords0.Emplace(0, 0);
	}

	// �ǉ�����钸�_��vertice�̂��̃C���f�b�N�X����ł���
	addvert_border = vertices.Num();

	TArray<FVector> Addvertices;
	// Key = �ǉ�����vertline_idxes��Index, Value = �ǉ�����vertline_idxes�̒��g�̔z��
	TMap<int32, TArray<int32>> Addvertline;
	FVector AddvertPoint;
	for (int32 i = 0; i < vertices.Num(); i++)
	{
		int32 i_next = (i + 1) % vertices.Num();
		for (int32 j = i + 2; j < vertices.Num(); j++)
		{
			int32 j_next = (j + 1) % vertices.Num();
			// �I�_�Ǝn�_���d�Ȃ��Ă��邩
			if (j == i || j == i_next || j_next == i || j_next == i_next)
			{
				continue;
			}
			if (ColSegments(vertices[i], vertices[i_next],
				vertices[j], vertices[j_next],
				AddvertPoint))
			{
				// ���_��ǉ�
				Addvertices.Add(AddvertPoint);
				// �ǉ������Index��ۑ�
				if (!Addvertline.Contains(i + 1))
					Addvertline.Add(i + 1);
				// �ǉ������Index���Ō���傫���ꍇ�A�t���ɒǉ�����
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
	// �ǉ��̒��_��ǉ�
	vertices.Insert(Addvertices, addvert_border);
	{
		int32 idx_offset = 0;
		// �ǉ��̒��_�ɔ������C���̍X�V
		for (auto& idx : Addvertline)
		{
			vertline_idxes.Insert(idx.Value, idx.Key + idx_offset);
			// �ǉ������ꏊ�������łȂ���΁A�I�t�Z�b�g�����炷
			if(idx.Key + idx_offset != vertline_idxes.Num() - idx.Value.Num())
				idx_offset += idx.Value.Num();
		}
	}

	//for (int32 i = 0; i < lineposes.Num(); i += 2)
	//{
	//	// �C���f�b�N�X
	//	indices.Emplace(i);
	//	indices.Emplace((i + 1) % lineposes.Num());
	//	indices.Emplace((i + 2) % lineposes.Num());
	//}

	// �ǉ�����Ă��Ȃ����_�̎c�肪1�ȉ��ɂȂ�܂ŒT��
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

	// UProceduralMeshComponent::CreateMeshSection_LinearColor �Ń��b�V���𐶐��B
// ��1����: �Z�N�V�����ԍ�; 0, 1, 2, ... ��^���鎖��1�� UProceduralMeshComponent �ɕ����̃��b�V��������I�ɓ����ɐ����ł��܂��B
// ��2����: ���_�Q
// ��3����: �C���f�b�N�X�Q
// ��4����: �@���Q
// ��5����: �e�N�X�`���[���W�Q
// ��6����: ���_�J���[�Q
// ��7����: �ڐ��Q
// ��8����: �R���W���������t���O
	mesh->CreateMeshSection_LinearColor(0, vertices, indices, normals, texcoords0, vertex_colors, tangents, true);
}

void ADrawLine::SetCounterclockwiseIdx(const FVector& ToNext, const FVector& ToPrev)
{
	if (next_idx == prev_idx)
	{
		UE_LOG(LogTemp, Error, TEXT("next_idx == prev_idx"));
		return;
	}

	// �O�ς����߂�
	float cross = (ToNext.X * ToPrev.Y) - (ToNext.Y * ToPrev.X);

	indices.Add(vertline_idxes[cur_idx]);
	if (cross < 0)	// edge1�̍�����edge2������i�����v���j
	{
		indices.Add(vertline_idxes[next_idx]);
		indices.Add(vertline_idxes[prev_idx]);
	}
	else				// edge1�̉E����edge2������i���v����j
	{
		// �����v���ɏC��
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
		// �O�p�`���Ƀ|�C���g������
		UE_LOG(LogTemp, Display, TEXT("point in triangle"));

		// �O�p�`�̌�����ۑ�
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
			// prev_cross��next_cross�̕����������ɂȂ�܂Ŏ���Index�ɐi�߂�
		} while (FMath::Sign<float>(prev_cross.Z) !=
			FMath::Sign<float>(next_cross.Z));

		if (++error_num > 100)
		{
			UE_LOG(LogTemp, Error, TEXT("error_num over 100"));
			return;
		}

		DetecteMeshIdx(false);	// ���̃C���f�b�N�X��T�����ɃC���f�b�N�X��ǉ�
		return;
	}
	
	// �����v���ɂȂ�悤�ɃC���f�b�N�X��ǉ�
	SetCounterclockwiseIdx(edge1, edge2);

	used_idxes.Add(vertline_idxes[cur_idx]);
}

bool ADrawLine::IsIncludePoint()
{
	for (int32 i = 0; i < vertline_idxes.Num(); i++)
	{
		// �C���f�b�N�X���g�p�ς݂��A�O�p�`�ɂȂ��Ă���ꍇ�͔�΂�
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

		// ���������łȂ��Ȃ�A�O�p�`�̊O���ł���
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
		if (IsUsedIdx(i) ||			// �g�p�ς݂̃C���f�b�N�X�Ȃ珈�����΂�
			i >= addvert_border)	// �ǉ����ꂽ���_�Ȃ珈�����΂�(�O���̎O�p�`���w�����ꂪ���邽��)
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

	faridx = cur_idx;	// �f�o�b�O�p(���̏����ŉ�肷���Ȃ�����)
	// �_��������ɂ���ꍇ�⓯���_���w���Ă���ꍇ
	while (FMath::IsNearlyZero(
		FVector::CrossProduct(
		LineIdxToVert(next_idx) - LineIdxToVert(cur_idx),
		LineIdxToVert(prev_idx) - LineIdxToVert(cur_idx)).Z
		)
	)
	{
		if (vertline_idxes[next_idx] == vertline_idxes[prev_idx])
		{
			// �����_���w���Ă���̂�next����i�߂�
			next_idx = FindNextIdx(next_idx);
		}
		else
		{
			// ������ɂȂ��Ă���̂ň���炷
			cur_idx = FindNextIdx(cur_idx);
			next_idx = FindNextIdx(cur_idx);
			prev_idx = FindPrevIdx(cur_idx);
		}

		if (faridx == cur_idx)	// 1�����Ă�������Ȃ��ꍇ
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
	FVector seg1 = pointB - pointA;	// �����P
	FVector seg2 = pointD - pointC;	// �����Q
	FVector v = pointC - pointA;	// �n�_���m�����ԃx�N�g��
	float v1_v2 = (seg1.X * seg2.Y) - (seg1.Y * seg2.X);
	if (v1_v2 == 0.0f)
	{
		// ���s���
		return false;
	}

	float v_v1 = (v.X * seg1.Y) - (v.Y * seg1.X);
	float v_v2 = (v.X * seg2.Y) - (v.Y * seg2.X);

	float t1 = v_v2 / v1_v2;	// �����P�̓�����
	float t2 = v_v1 / v1_v2;	// �����Q�̓�����

	//if (outT1)
	//	outT1 = t1;
	//if (outT2)
	//	outT2 = t2;

	const float eps = 0.00001f;
	if (t1 + eps < 0 || t1 - eps>1 || t2 + eps < 0 || t2 - eps>1)
	{
		// �������Ă��Ȃ�
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
	// �R���W�������N�G���I�����[��
	mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FTimerHandle _Handle;
	// time�b��ɁA�R���W������߂�
	GetWorld()->GetTimerManager().SetTimer(_Handle, this, 
		&ADrawLine::DeActiveMeshCollision, time, false);
}

void ADrawLine::DeActiveMeshCollision()
{
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}