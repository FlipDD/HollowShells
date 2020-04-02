#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BatteryDoorSlot.generated.h"

UCLASS()
class SHELLS_API ABatteryDoorSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	ABatteryDoorSlot();

protected:
	void BeginPlay() override;

public:	
	void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere)
		TArray<class AMovingRock*> RocksToTrigger;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
