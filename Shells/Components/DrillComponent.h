// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DrillComponent.generated.h"

UENUM()
enum class EDrillState : uint8 {
    Idle,
    Drilling
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHELLS_API UDrillComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UDrillComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void StartDrilling();
    void ServerStartDrilling();
    void StopDrilling();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		class UParticleSystem* DrillingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		class UParticleSystem* RockDrillingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		class UParticleSystemComponent* ParticleComponent;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
		float DrillSphereRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
		class USphereComponent* DrillSphere;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentState, BlueprintReadOnly, Category = "Effects")
		EDrillState CurrentState;

	UFUNCTION() void OnRep_CurrentState();

protected:
	void BeginPlay() override;
};
