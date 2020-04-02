
#include "Pickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundWave.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APickup::APickup() {
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetupAttachment(GetStaticMeshComponent());
	
	if (Role != ROLE_Authority)
		return;
	
	bIsActive = true;
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickup, bIsActive)
}

bool APickup::IsActive() const
{
	return bIsActive;
}

void APickup::SetActive(bool NewPickupState) {
	if (Role != ROLE_Authority)
		return;

	bIsActive = NewPickupState;
}

void APickup::NotifyActorBeginOverlap(AActor *OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void APickup::WasCollected_Implementation()
{
	PlayEffect();
	SetLifeSpan(1.0f);
}

void APickup::PickedUpBy(APawn *Pawn)
{
	if (Role != ROLE_Authority)
		return;

	PickupInstigator = Pawn;
	ClientOnPickedUpBy(Pawn);
}

void APickup::OnRep_IsActive() {
	UE_LOG(LogClass, Log, TEXT("APickup::OnRep_IsActive %s"), *GetName())
}

void APickup::ClientOnPickedUpBy_Implementation(APawn *Pawn)
{
	PickupInstigator = Pawn;
	WasCollected();
}

void APickup::PlayEffect()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, CollectionEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), 1, 1, 1, nullptr);
}
