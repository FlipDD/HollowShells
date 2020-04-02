
#include "NetworkedGameMode.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerController.h"
#include "NetworkedGameState.h"
#include "World/SpawnVolume.h"
#include "TimerManager.h"

#include "Beast/ShellsCharacter.h"
#include "Components/PowerComponent.h"
#include "Explorer/ExplorerCharacter.h"

#include "Engine.h"
#include "LobbyGameInstance.h"

ANetworkedGameMode::ANetworkedGameMode()
{
	// Defaults
	DecayRate = 0.02f;
	PowerDrainDelay = 1.0f;
	PowerToWinMultiplier = 1.25f;

	ConstructorHelpers::FClassFinder<AShellsCharacter> BeastBPClass(TEXT("/Game/Blueprints/BeastCharacterBP.BeastCharacterBP_C"));
	if (!ensure(BeastBPClass.Class != nullptr)) return;
	BeastClass = BeastBPClass.Class;

	ConstructorHelpers::FClassFinder<AExplorerCharacter> ExplorerBPClass(TEXT("/Game/Blueprints/BP_ExplorerCharacter.BP_ExplorerCharacter_C"));
	if (!ensure(ExplorerBPClass.Class != nullptr)) return;
	ExplorerClass = ExplorerBPClass.Class;
}

void ANetworkedGameMode::BeginPlay()
{
	Super::BeginPlay();

	const auto World = GetWorld();
	check(World);

	const auto State = Cast<ANetworkedGameState>(GameState);
	check(State);

	// Gather spawn volumes
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), Found);

	UGameplayStatics::GetAllActorsOfClass(World, AExplorerCharacter::StaticClass(), ExploresInGame);

	for (auto Actor : Found) {
		if (auto SV = Cast<ASpawnVolume>(Actor)) {
			SpawnVolumeActors.AddUnique(SV);
		}
	}

	HandleNewState(EMatchState::Playing);
	
	for (auto It = World->GetControllerIterator(); It; ++It) {
		if (const auto PlayerController = Cast<APlayerController>(*It)) {
			//RestartPlayer(PlayerController);
			if (const auto BatteryCharacter = Cast<AExplorerCharacter>(PlayerController->GetPawn())) {
				break;
			}
		}
	}
}



void ANetworkedGameMode::GameOver()
{
    
}
void ANetworkedGameMode::RespawnExplorer(AActor *Explorer)
{
    
}

void ANetworkedGameMode::RespawnBeast()
{
    
}

void ANetworkedGameMode::RespawnPlayers()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		const auto PC = It->Get();
		if (PC && PC->GetPawn() == nullptr) {
			RestartPlayer(PC);
		}
	}
}

void ANetworkedGameMode::CheckAnyPlayerAlive()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto PC = It->Get();
		if (PC && PC->GetPawn()) {
			const auto Pawn = PC->GetPawn();
			const auto Power = Cast<UPowerComponent>(Pawn->GetComponentByClass(UPowerComponent::StaticClass()));
			if (ensure(Power) && Power->GetPower() > 0.0f) {
				return;
			}
		}
	}

	// No player alive
	HandleNewState(EMatchState::VictoryBeast);
}

void ANetworkedGameMode::RespawnPlayer(APlayerController *Player)
{
    
}

void ANetworkedGameMode::Update()
{
	const auto World = GetWorld();
	check(World);

	auto State = Cast<ANetworkedGameState>(GameState);
	check(State);

	if (State->NumBeaconsActive == 0) {
		HandleNewState(EMatchState::VictoryShells);
	}

	for (auto It = World->GetControllerIterator(); It; ++It) {
		if (const auto PlayerController = Cast<APlayerController>(*It)) {
			if (auto Character = Cast<AExplorerCharacter>(PlayerController->GetPawn())) {
				if (Character->GetCurrentPower() <= 0) {
					Character->OnPlayerDeath();
					State->NumPlayersActive -= 1;
					if (State->NumPlayersActive == 0) {
						HandleNewState(EMatchState::VictoryBeast);
					}
				}
			}
		}
	}
}

void ANetworkedGameMode::HandleNewState(EMatchState NewState)
{
	const auto World = GetWorld();
	check(World);

	auto State = Cast<ANetworkedGameState>(GameState);
	check(State);

	if (NewState == State->GetCurrentState())
		return;

	State->ServerSetCurrentState(NewState);

	switch (NewState)
	{
	case EMatchState::None:
		break;
	case EMatchState::Wait:
		break;
	case EMatchState::Playing:
		for (auto S : SpawnVolumeActors) {
			S->SetSpawningActive(true);
		}
		GetWorldTimerManager().SetTimer(PowerDrainTimer, this, &ANetworkedGameMode::Update, PowerDrainDelay, true);
		break;
	case EMatchState::VictoryBeast:
	case EMatchState::VictoryShells:
		for (auto S : SpawnVolumeActors) {
			S->SetSpawningActive(false);
		}
		GetWorldTimerManager().ClearTimer(PowerDrainTimer);
		break;
	default:
		break;
	}
}

void ANetworkedGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;

	if (NumberOfPlayers >= 2) {
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ANetworkedGameMode::StartGame, 10);
	}

	if (NewPlayer->HasAuthority()) {
		DefaultPawnClass = BeastClass;
	} else {
		DefaultPawnClass = ExplorerClass;
	}
}

void ANetworkedGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NumberOfPlayers;
}

void ANetworkedGameMode::StartGame()
{
	/*auto GameInstance = Cast<ULobbyGameInstance>(GetGameInstance());
	if (!ensure(GameInstance)) return;

	GameInstance->StartSession();

	auto World = GetWorld();
	if (!ensure(World)) return;

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/FirstPersonExampleMap?listen");*/
}