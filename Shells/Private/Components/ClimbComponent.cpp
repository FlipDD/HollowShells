
#include "ClimbComponent.h"

#include "WorldCollision.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Explorer/ExplorerCharacter.h"

UClimbComponent::UClimbComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UClimbComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ForwardTracer();
	HeightTracer();
}


void UClimbComponent::ForwardTracer()
{
	FHitResult OutHit;
	const FCollisionShape CollisionShape;
	const auto Forward = GetOwner()->GetActorForwardVector();
	const auto End = GetOwner()->GetActorLocation() + FVector(Forward.X * 150, Forward.Y * 150, Forward.Z);

	if (GetWorld()->SweepSingleByChannel(OutHit, GetOwner()->GetActorLocation(), End, FQuat::Identity, LedgeChannel, CollisionShape)) {
		WallLocation = OutHit.Location;
		WallNormal = OutHit.Normal;
	}
}

void UClimbComponent::HeightTracer()
{
	FHitResult OutHit;
	const FCollisionShape CollisionShape;
	const auto Range = GetOwner()->GetActorForwardVector() * 70;
	const auto Start = GetOwner()->GetActorLocation() + FVector(0, 0, 500) + Range;
	const auto End = Start - FVector(0, 0, 500);

	if (GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, LedgeChannel, CollisionShape)) {
		HeightLocation = OutHit.Location;
		if (LedgeInRange()) {
			if (!bIsClimbingLedge) {
				GrabLedge();
				UE_LOG(LogTemp, Warning, TEXT("Grabbed ledge"));
			}
		}
	}
}

void UClimbComponent::GrabLedge()
{
	CanGrab(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	bIsHanging = true;

	auto NewRotation = WallNormal.Rotation();
	NewRotation.Yaw -= 180;

	// TODO: Don't be lazy, do this properly
	auto Info = FLatentActionInfo();
	Info.CallbackTarget = this;
	Info.ExecutionFunction = "StopMovementImmediately";
	const auto Character = Cast<AExplorerCharacter>(GetOwner());
	//UKismetSystemLibrary::MoveComponentTo(Character->CapsuleComponent, MovementLocation(), NewRotation,
	//	false, false, 0.13f, false, EMoveComponentAction::Move, Info);
}

void UClimbComponent::CanClimb(bool bIsClimbing)
{
	bIsClimbingLedge = bIsClimbing;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void UClimbComponent::ClimbLedge()
{
	if (!bIsClimbingLedge)
	{
		CanClimb(false);
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		bIsClimbingLedge = true;
		bIsHanging = false;
		UE_LOG(LogTemp, Warning, TEXT("Climbing ledge"))

	}
}

void UClimbComponent::ExitLedge()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	CanGrab(false);
	bIsHanging = false;
}

FVector UClimbComponent::MovementLocation() const
{
	const auto Offset = WallNormal * FVector(22, 22, 0);
	return FVector(WallLocation.X + Offset.X, WallLocation.Y + Offset.Y, HeightLocation.Z - 120);
}

void UClimbComponent::CanGrab(bool isGrabbing)
{
	bIsHanging = isGrabbing;
}

bool UClimbComponent::LedgeInRange() const
{
	const auto Character = Cast<ACharacter>(GetOwner());
	const auto SocketLocation = Character->GetMesh()->GetSocketLocation(FName("PelvisSocket"));
	const auto Distance = SocketLocation.Z - HeightLocation.Z;
	return Distance > -50 && Distance < 0;
}

UCharacterMovementComponent* UClimbComponent::GetCharacterMovement()
{
	const auto Owner = GetOwner();
	const auto Character = Cast<ACharacter>(Owner);
	return Character ? Character->GetCharacterMovement() : nullptr;
}

void UClimbComponent::Drop()
{
	if (bIsHanging)
		ExitLedge();
}
