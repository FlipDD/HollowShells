#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "PortalBox.generated.h"

UCLASS()
class SHELLS_API APortalBox : public ATriggerBox
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

public:
	APortalBox();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destination")
	//ATriggerBox* DestinationPortal;

	//UFUNCTION()
	//void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
	//
	//UFUNCTION()
	//void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
};
