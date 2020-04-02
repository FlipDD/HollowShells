#include "ExplorerAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

void UExplorerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningPawn = TryGetPawnOwner();
}

void UExplorerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!OwningPawn)
		return;

    const auto Velocity = OwningPawn->GetVelocity();
    const auto Forward = OwningPawn->GetActorForwardVector();
    const auto Right = OwningPawn->GetActorRightVector();
	RightwardSpeed = FVector::DotProduct(Right, Velocity);
	ForwardSpeed = FVector::DotProduct(Velocity, Forward);

	IsMoving = OwningPawn->GetVelocity().SizeSquared() > 25;

	if (Velocity.Y < 30)
		bIsInAir = OwningPawn->GetMovementComponent()->IsFalling();
}