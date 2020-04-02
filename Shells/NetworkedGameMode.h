
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetworkedGameState.h"
#include "World/SpawnVolume.h"
#include "Beast/ShellsCharacter.h"
#include "NetworkedGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled,
AActor*, VictimActor,
AActor*, KillerActor,
AController*, KillerController);

UCLASS()
class SHELLS_API ANetworkedGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANetworkedGameMode();

	void BeginPlay() override;

	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

	void GameOver();
	void RespawnExplorer(AActor *Explorer);
	void RespawnBeast();

	void RespawnPlayers();
    void CheckAnyPlayerAlive();
    void RespawnPlayer(APlayerController *Player);

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
		FOnActorKilled OnActorKilled;

	UFUNCTION(BlueprintPure, Category = "Power")
		float GetDecayRate() const { return DecayRate; }

	UFUNCTION(BlueprintPure, Category = "Power")
		float GetPowerToWinMultiplier() const { return PowerToWinMultiplier; };

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<AExplorerCharacter> ExplorerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<AShellsCharacter> BeastClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
		float PowerDrainDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
		float DecayRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
		float PowerToWinMultiplier;
	
	FTimerHandle PowerDrainTimer;

private:
	void Update();

	TArray<ASpawnVolume*> SpawnVolumeActors;

	void HandleNewState(EMatchState NewState);

public:
	TArray<AActor*> ExploresInGame;

private:
	void StartGame();

	uint32 NumberOfPlayers = 0;
	FTimerHandle GameStartTimer;
};
