#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingRock.generated.h"

UCLASS()
class SHELLS_API AMovingRock : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Sound")
		class UAudioComponent* RumbleSound;
	
public:	
	AMovingRock();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		float Speed;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
		FVector TargetLocation;

	void AddActiveTrigger();
	void RemoveActiveTrigger();
	void Unlock();

	bool CanPlay;

	UPROPERTY(ReplicatedUsing = OnRep_IsMoving)
	bool bIsMoving;

	UFUNCTION()
	void OnRep_IsMoving();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:
	FVector GlobalTargetLocation;
	FVector GlobalStartLocation;

	UPROPERTY(EditAnywhere)
		int ActiveTriggers = 0;
	UPROPERTY(EditAnywhere)
		int TargetTriggers = 1;
};
