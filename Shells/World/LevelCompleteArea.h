#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelCompleteArea.generated.h"

UCLASS()
class SHELLS_API ALevelCompleteArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelCompleteArea();
	void Tick(float DeltaTime) override;
	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent *BoxComponent;

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);
};
