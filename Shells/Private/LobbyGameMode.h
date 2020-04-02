#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class SHELLS_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ALobbyGameMode();

	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

	TArray<AActor*> ExplorersInGame;

	//void RestartPlayer(AController* NewPlayer) override;

	//UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

private:

	void StartGame();

    uint32 NumberOfPlayers = 0;
	uint32 NumberOfPlayersToSpawn = 0;

	FTimerHandle GameStartTimer;
};
