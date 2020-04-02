#include "ActionPickup.h"
#include "Engine/StaticMeshActor.h"

AActionPickup::AActionPickup() {
	bReplicateMovement = true;
	SetReplicateMovement(true);
}

void AActionPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);

	if (Role != ROLE_Authority)
		return;

	PlayEffect();
	SetLifeSpan(1.0f);
}
