
#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

#include "Pickup.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	if (Role == ROLE_Authority) {
		SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
		RootComponent = SpawnBox;

		SpawnDelayMin = 1;
		SpawnDelayMax = 5;
	}
}

void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	if (!SpawnBox)
		return FVector();

	const FVector SpawnOrigin = SpawnBox->Bounds.Origin;
	const FVector SpawnExtent = SpawnBox->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawnVolume::SetSpawningActive(bool ShouldSpawn)
{
	if (Role != ROLE_Authority)
		return;

	if (ShouldSpawn) {
		SpawnDelay = FMath::FRandRange(SpawnDelayMin, SpawnDelayMax);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);
	} else {
		GetWorldTimerManager().ClearTimer(SpawnTimer);
	}

}

void ASpawnVolume::SpawnPickup()
{
	if (Role != ROLE_Authority || !WhatToSpawn)
		return;

	if (const auto World = GetWorld()) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		const auto SpawnLocation = GetRandomPointInVolume();

		FRotator SpawnRotation;
		SpawnRotation.Yaw = FMath::FRand() * 360.0f;
		SpawnRotation.Pitch = FMath::FRand() * 360.0f;
		SpawnRotation.Roll = FMath::FRand() * 360.0f;

		const auto SpawnedPickup = World->SpawnActor<APickup>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

		// Delay before spawning next pickup
		SetSpawningActive(true);
	}
}

