#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"	
#include "ZombieMechaCharacter.generated.h"

UCLASS()
class SHELLS_API AZombieMechaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AZombieMechaCharacter();
	void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UPawnSensingComponent* SensingComponent;

	UPROPERTY(Replicated)
		bool IsChasing;

	UFUNCTION()
		void SeePlayer(APawn* pawn);

	UFUNCTION()
		void AIMovementCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	TArray<AActor*> Waypoints;
	class AAIController* AIController;
	TScriptDelegate<FWeakObjectPtr> MovementCompleteDelegate;
	class ATargetPoint* GetRandomWaypoint();
	AActor* Target;
};