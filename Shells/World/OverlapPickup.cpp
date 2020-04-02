#include "OverlapPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Explorer/ExplorerCharacter.h"

AOverlapPickup::AOverlapPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOverlapPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0, RotationSpeed * DeltaTime, 0));
}

void AOverlapPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Pawn = dynamic_cast<AExplorerCharacter*>(OtherActor);
	if (!Pawn) return;

	PickedUpBy(Pawn);
	SetActive(false);
}

void AOverlapPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);

	if (Role != ROLE_Authority)
		return;

	PlayEffect();
	SetLifeSpan(1.0f);
}
