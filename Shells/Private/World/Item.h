// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Energy UMETA(DisplayName = "Energy"),
	Artifact UMETA(DisplayName = "Artifact"),
};

USTRUCT(BlueprintType)
struct FItemData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Amount;
};

UCLASS()
class SHELLS_API AItem : public APickup
{
	GENERATED_BODY()
	
public:	
	AItem();
	void Tick(float DeltaTime) override;

protected:
	void BeginPlay() override;
	void PickedUpBy(APawn *Pawn) override;

public:	
	UFUNCTION()
		virtual void OnOverlapBegin(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, 
			bool bFromSweep, 
			const FHitResult& SweepResult);

};
