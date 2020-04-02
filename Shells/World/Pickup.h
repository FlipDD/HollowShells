
#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class SHELLS_API APickup : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	APickup();
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	
	// Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UParticleSystem* CollectionEffect;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USoundWave* Sound;

	// Properties
	UFUNCTION(BlueprintPure, Category = "Pickup")
		bool IsActive() const;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void SetActive(bool NewPickupState);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
		bool bIsActive;

	UFUNCTION()
		virtual void OnRep_IsActive();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
		APawn *PickupInstigator;

	// Events
public:
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UFUNCTION(BlueprintNativeEvent, Category="Pickup")
	void WasCollected();
	virtual void WasCollected_Implementation();

	UFUNCTION(BlueprintAuthorityOnly, Category = "Pickup")
	virtual void PickedUpBy(APawn *Pawn);

	void PlayEffect();

private:
	UFUNCTION(NetMulticast, Unreliable)
	void ClientOnPickedUpBy(APawn *Pawn);
	void ClientOnPickedUpBy_Implementation(APawn *Pawn);
};
