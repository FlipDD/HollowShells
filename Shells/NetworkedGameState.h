
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "World/BeaconActor.h"
#include "Explorer/ExplorerCharacter.h"
#include "NetworkedGameState.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8 {
	None,
	Wait,
	Playing,
	VictoryBeast,
	VictoryShells
};

UCLASS()
class SHELLS_API ANetworkedGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANetworkedGameState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	// State
	UFUNCTION(BlueprintPure, Category = "Gameplay")
		EMatchState GetCurrentState() const { return CurrentState; }

	UFUNCTION()
		void ServerSetCurrentState(EMatchState NewState);

	// OnLevel Complete
	UFUNCTION(NetMulticast, Reliable)
		void MulticastOnLevelComplete(APawn *Character, bool bSucceded);
	virtual void MulticastOnLevelComplete_Implementation(APawn *Character, bool bSucceded);

	// Beacons
	UFUNCTION(BlueprintCallable)
		int CountBeaconsInLevel() const;

	UPROPERTY(ReplicatedUsing = OnRep_NumBeaconsActive, BlueprintReadOnly, Category = "Gameplay")
		float NumBeaconsActive;
	UFUNCTION() void OnRep_NumBeaconsActive();

	// Players
	UPROPERTY(ReplicatedUsing = OnRep_NumPlayersActive, BlueprintReadOnly, Category = "Gameplay")
		float NumPlayersActive;
	UFUNCTION() void OnRep_NumPlayersActive();

	// State
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
		EMatchState CurrentState;
	UFUNCTION() void OnRep_CurrentState();
};
