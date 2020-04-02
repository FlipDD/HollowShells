
#include "NetworkedGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "MainPlayerController.h"

ANetworkedGameState::ANetworkedGameState()
{
	CurrentState = EMatchState::None;
}

void ANetworkedGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetworkedGameState, CurrentState);
	DOREPLIFETIME(ANetworkedGameState, NumBeaconsActive);
	DOREPLIFETIME(ANetworkedGameState, NumPlayersActive);
}

int ANetworkedGameState::CountBeaconsInLevel() const
{
	TArray<AActor *> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABeaconActor::StaticClass(), Found);
	return Found.Num();
}

void ANetworkedGameState::MulticastOnLevelComplete_Implementation(APawn * Character, bool bSucceeded)
{
	if (bSucceeded)
	{
		for (auto It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			auto Pawn = It->Get();
			if (Pawn && Pawn->IsLocallyControlled()) {
				Pawn->DisableInput(nullptr);
			}
		}
		for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			auto Controller = Cast<AMainPlayerController>(It->Get());
			if (Controller && Controller->IsLocalController()) {
				Controller->OnLevelCompleted(Character, bSucceeded);
			}
		}
	} else {
		if (Character) {
			Character->DisableInput(nullptr);
			auto Controller = Cast<AMainPlayerController>(Character->GetController());
			if (Controller && Controller->IsLocalController()) {
				Controller->OnLevelCompleted(Character, bSucceeded);
			}
		}
	}
}

void ANetworkedGameState::OnRep_NumBeaconsActive()
{
}

void ANetworkedGameState::OnRep_NumPlayersActive()
{
}

void ANetworkedGameState::ServerSetCurrentState(EMatchState NewState)
{
	if (Role != ROLE_Authority)
		return;
	
	CurrentState = NewState;
}

void ANetworkedGameState::OnRep_CurrentState()
{

}
