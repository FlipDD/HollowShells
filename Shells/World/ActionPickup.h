#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ActionPickup.generated.h"

UCLASS()
class SHELLS_API AActionPickup : public APickup
{
	GENERATED_BODY()

public:
	AActionPickup();

	UFUNCTION(BlueprintPure, Category = "Power")
		float GetPower() const { return Power; };

protected:
	void PickedUpBy(APawn *Pawn) override;

	float Power;
};
