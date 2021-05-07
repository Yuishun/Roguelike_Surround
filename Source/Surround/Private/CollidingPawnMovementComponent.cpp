// Fill out your copyright notice in the Description page of Project Settings.


#include "CollidingPawnMovementComponent.h"

void UCollidingPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//���ׂĂ��܂��L���ł���A�ړ���������Ă��邱�Ƃ��m�F���܂��B
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Pawn :: Tick�Őݒ肵���ړ��x�N�g�����擾�i�����ăN���A�j���܂�
   CollidingMove(ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * speed);

}

void UCollidingPawnMovementComponent::CollidingMove(FVector DesiredMovementThisFrame)
{
    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        FHitResult Hit;
        // SafeMoveUpdatedComponent�́AUnreal Engine�̕����w���g�p���āA���łȏ�ǂ𑸏d���Ȃ���|�[���ړ��R���|�[�l���g���ړ����܂��B
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

        //�����ɂԂ������ꍇ�́A����ɉ����ăX���C�h
        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
        }
    }
}