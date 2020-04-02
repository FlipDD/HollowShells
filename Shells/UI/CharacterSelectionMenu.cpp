#include "CharacterSelectionMenu.h"

#include "Components/Button.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyGameMode.h"

#include "LobbyGameInstance.h"
#include "LobbyGameMode.h"
#include "MainPlayerController.h"

bool UCharacterSelectionMenu::Initialize()
{
    const bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(ChooseBeastButton != nullptr)) return false;
	ChooseBeastButton->OnClicked.AddDynamic(this, &UCharacterSelectionMenu::TryChooseBeast);

	if (!ensure(ChooseExplorerButton != nullptr)) return false;
	ChooseExplorerButton->OnClicked.AddDynamic(this, &UCharacterSelectionMenu::TryChooseExplorer);

	return true;
}

void UCharacterSelectionMenu::TryChooseBeast()
{
    auto NewLocation = FVector(0.f, 0.f, 300.f);

    auto PC = Cast<AMainPlayerController>(GetOwningPlayer());
	if (!ensure(PC)) return;

	auto I = Cast<ULobbyGameInstance>(GetGameInstance());
	if (!ensure(I)) return;
	
	//PC->ServerSetPawn(I->BeastClass);

	Teardown();
}

void UCharacterSelectionMenu::TryChooseExplorer()
{
    auto NewLocation = FVector(0.f, 0.f, 300.f);

    auto PC = Cast<AMainPlayerController>(GetOwningPlayer());
	if (!ensure(PC)) return;

	auto I = Cast<ULobbyGameInstance>(GetGameInstance());
	if (!ensure(I)) return;

//	PC->ServerSetPawn(I->ExplorerClass);

	Teardown();
}

