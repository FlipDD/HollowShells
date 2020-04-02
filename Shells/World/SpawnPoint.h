
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class SHELLS_API ASpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ASpawnPoint();
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		TSubclassOf<class ASPowerupActor> PowerUpClass;

	class ASPowerupActor* PowerUpInstance;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		float CooldownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

	void Respawn();
};
