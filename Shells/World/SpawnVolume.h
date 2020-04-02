
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class SHELLS_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();
	void Tick(float DeltaTime) override;

	FORCEINLINE
		class UBoxComponent *GetWhereToSpawn() const { return SpawnBox; }

	UFUNCTION(BlueprintPure, Category = "Spawn")
		FVector GetRandomPointInVolume() const;

	UFUNCTION(BlueprintCallable, Category = "Spawn")
		void SetSpawningActive(bool ShouldSpawn);

protected:
	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<class APickup> WhatToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float SpawnDelayMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float SpawnDelayMax;

	FTimerHandle SpawnTimer;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn", Meta = (AllowPrivateAccess = "true"))
		class UBoxComponent *SpawnBox;

    UFUNCTION()
	void SpawnPickup();

	float SpawnDelay;
};
