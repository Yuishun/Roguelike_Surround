// Fill out your copyright notice in the Description page of Project Settings.


#include "CollidingPawnMovementComponent.h"

void UCollidingPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//すべてがまだ有効であり、移動が許可されていることを確認します。
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Pawn :: Tickで設定した移動ベクトルを取得（そしてクリア）します
   CollidingMove(ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * speed);

}

void UCollidingPawnMovementComponent::CollidingMove(FVector DesiredMovementThisFrame)
{
    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        FHitResult Hit;
        // SafeMoveUpdatedComponentは、Unreal Engineの物理学を使用して、堅固な障壁を尊重しながらポーン移動コンポーネントを移動します。
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

        //何かにぶつかった場合は、それに沿ってスライド
        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
        }
    }
}