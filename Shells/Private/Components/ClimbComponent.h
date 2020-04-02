// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHELLS_API UClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClimbComponent();

protected:
	void BeginPlay() override;

public:
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void ForwardTracer();

	UFUNCTION()
		void HeightTracer();

	UFUNCTION()
		void CanGrab(bool isGrabbing);

	UFUNCTION()
		void GrabLedge();

	UFUNCTION()
		void CanClimb(bool bIsClimbing);

	UFUNCTION()
		void ClimbLedge();

	UFUNCTION()
		void ExitLedge();

	UFUNCTION()
		void Drop();

	UFUNCTION()
		FVector MovementLocation() const;

	UFUNCTION()
		bool LedgeInRange() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb", Meta = (BluePrintProtected = "true"))
		TEnumAsByte<ECollisionChannel> LedgeChannel;

	class UCharacterMovementComponent* GetCharacterMovement();


	FVector WallLocation;
	FVector WallNormal;
	FVector HeightLocation;

	bool bIsClimbingLedge;
	bool bIsHanging;


};
