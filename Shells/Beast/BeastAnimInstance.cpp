#include "Beast/BeastAnimInstance.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/PawnMovementComponent.h"

void UBeastAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = TryGetPawnOwner();
}

void UBeastAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!OwningPawn)
		return;

	bIsMoving = OwningPawn->GetVelocity().SizeSquared() > 25;
	bIsInAir = OwningPawn->GetMovementComponent()->IsFalling();

	FVector Velocity = OwningPawn->GetVelocity();
	FVector Forward = OwningPawn->GetActorForwardVector();
	FVector Right = OwningPawn->GetActorRightVector();
	RightwardSpeed = FVector::DotProduct(Right, Velocity);
	ForwardSpeed = FVector::DotProduct(Velocity, Forward);
}
