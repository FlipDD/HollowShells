// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class SHELLS_API ASPowerupActor : public AActor
{
	GENERATED_BODY()

public:
	ASPowerupActor();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_bIsActive)
		bool bIsActive;

	UFUNCTION()
		void OnRep_bIsActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupStateChanged(bool bNewIsActive);

public:
	void ActivatePowerup(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnActivated(AActor* ActiveFor);
};
