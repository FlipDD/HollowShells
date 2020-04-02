#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.h"
#include "OverlapPickup.generated.h"

UCLASS()
class SHELLS_API AOverlapPickup : public APickup
{
	GENERATED_BODY()

public:
	AOverlapPickup();

	void Tick(float DeltaTime) override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	void PickedUpBy(APawn *Pawn) override;

public:
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float RotationSpeed;
};
