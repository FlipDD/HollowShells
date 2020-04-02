#include "Item.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "MainPlayerController.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
}

void AItem::PickedUpBy(APawn * Pawn)
{
	auto Player = Cast<AMainPlayerController>(Pawn->Controller);
	if (!Player)
		return;

	Player->AddItem();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComp)
		return;

	auto Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
		return;

	auto Player = Cast<AMainPlayerController>(Pawn->Controller);
	if (!Player)
		return;

	PickedUpBy(Pawn);
}