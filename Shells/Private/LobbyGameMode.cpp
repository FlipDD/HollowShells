#include "LobbyGameMode.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "LobbyGameInstance.h"
#include "NetworkedSession.h"

#include "Explorer/ExplorerCharacter.h"
#include "Beast/ShellsCharacter.h"
#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "LobbyGameInstance.h"

#include "Engine.h"


ALobbyGameMode::ALobbyGameMode()
{
	GameSessionClass = ANetworkedSession::StaticClass();

	NumberOfPlayersToSpawn = 0;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;
	 
	if (NumberOfPlayers >= 2)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, 10);
	}

	UE_LOG(LogTemp, Warning, TEXT("Post login"));

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExplorerCharacter::StaticClass(), ExplorersInGame);
	int n = ExplorersInGame.Num();
	UE_LOG(LogTemp, Warning, TEXT("Number of explorers found: %d"), n);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NumberOfPlayers;
}

void ALobbyGameMode::StartGame()
{
	//auto GameInstance = Cast<ULobbyGameInstance>(GetGameInstance());
	//if (!ensure(GameInstance)) return;

	//GameInstance->StartSession();

 //   auto World = GetWorld();
	//if (!ensure(World)) return;

	//bUseSeamlessTravel = true;
	//World->ServerTravel("/Game/Maps/FirstPersonExampleMap?listen");
}

//void ALobbyGameMode::RestartPlayer(AController* NewPlayer)
//{
//	Super::RestartPlayer(NewPlayer);
//
//}
//
//UClass* ALobbyGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
//{
//	auto IC = Cast<AMainPlayerController>(InController);
//	if (IC)
//	{
//		if (GetWorld()->GetName() == "FirstPersonExampleMap")
//		{
//			NumberOfPlayersToSpawn++;
//			auto I = Cast<ULobbyGameInstance>(GetGameInstance());
//			if (!ensureAlways(I)) return DefaultPawnClass;
//			if (NumberOfPlayersToSpawn == 3)
//			{
//				return I->BeastClass;
//			}
//			else
//			{
//				return I->ExplorerClass;
//			}
//			auto Explorer = Cast<AExplorerCharacter>(IC->GetPlayerPawnClass());
//			if (!ensureAlways(Explorer)) return DefaultPawnClass;
//
//			return IC->GetPlayerPawnClass();
//		}
//
//		return IC->GetPlayerPawnClass();
//	}
//	UE_LOG(LogTemp, Warning, TEXT("DEFAULT???"));
//	return DefaultPawnClass;
//}
