// Copyright 2019 - Filipe Ribeiro & Raul Leal


#include "LeaderboardMenu.h"
#include "VerticalBox.h"
#include "NetworkedGameState.h"
#include "MainPlayerController.h"
#include "MainPlayerState.h"
#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MatchmakingRow.h"
#include "LobbyGameMode.h"
#include "UI/MenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Explorer/ExplorerCharacter.h"
#include "Beast/ShellsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "GameFramework/Pawn.h"

ULeaderboardMenu::ULeaderboardMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MatchMakingRowBPClass(TEXT("/Game/Blueprints/Widgets/WBP_LeaderboardRow.WBP_LeaderboardRow_C"));
	if (!ensure(MatchMakingRowBPClass.Class != nullptr)) return;

	MatchmakingRowClass = MatchMakingRowBPClass.Class;
}

bool ULeaderboardMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &ULeaderboardMenu::CloseLeaderboard);

	return true;
}

void ULeaderboardMenu::CloseLeaderboard()
{
	Teardown();
}

void ULeaderboardMenu::ServerGetPlayerStates_Implementation()
{
	const auto World = this->GetWorld();
	if (!ensure(World != nullptr)) return;
	auto GS = World->GetGameState();
	if (!ensure(GS != nullptr)) return;
	TArray<APlayerState*> Players = GS->PlayerArray;
	TArray<AMainPlayerState*> NewPlayerStates;
	for (int i = 0; i < Players.Num(); i++)
	{
		auto PSs = Cast<AMainPlayerState>(Players[i]);
		if (PSs) NewPlayerStates.Add(PSs);
	}
	UE_LOG(LogTemp, Warning, TEXT("Found %d ps's"), NewPlayerStates.Num());
	
	PS = NewPlayerStates;
}
bool ULeaderboardMenu::ServerGetPlayerStates_Validate()
{
	return true;
}

	//ServerGetPlayerStates();

void ULeaderboardMenu::SetPlayersList()
{
	const auto World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	PlayersList->ClearChildren();

	UMatchmakingRow* Row = CreateWidget<UMatchmakingRow>(World, MatchmakingRowClass);
	if (!ensure(Row != nullptr)) return;

	auto E = Cast<AMainPlayerState>(GetOwningPlayerPawn()->GetPlayerState());

	
		FString pid = FString::FromInt(E->PID);
		FString tbd = FString::FromInt(E->TotalBeastDeaths);
		FString tbp = FString::FromInt(E->TotalBeastPoints);
		FString tmd = FString::FromInt(E->TotalMechaDeaths);
		FString tmp = FString::FromInt(E->TotalMechaPoints);

		Row->PlayerName->SetText(FText::FromString(E->Name));
		Row->BDeaths->SetText(FText::FromString(tbd));
		Row->BPoints->SetText(FText::FromString(tbp));
		Row->MDeaths->SetText(FText::FromString(tmd));
		Row->MPoints->SetText(FText::FromString(tmp));

		PlayersList->AddChild(Row);

	Row->Setup(this, 0);

	/*TArray<AActor*> Exp;
	UGameplayStatics::GetAllActorsOfClass(World, AExplorerCharacter::StaticClass(), Exp);
	TArray<AActor*> Beast;
	UGameplayStatics::GetAllActorsOfClass(World, AShellsCharacter::StaticClass(), Beast);*/
	/*for (int i = 0; i < Exp.Num(); i++)
	{
		auto E = Cast<AExplorerCharacter>(Exp[i]);
		if (!E) return;

		FString pid = FString::FromInt(E->PID);
		FString tbd = FString::FromInt(E->TotalBeastDeaths);
		FString tbp = FString::FromInt(E->TotalBeastPoints);
		FString tmd = FString::FromInt(E->TotalMechaDeaths);
		FString tmp = FString::FromInt(E->TotalMechaPoints);

		Row->PlayerName->SetText(FText::FromString(E->Name));
		Row->BDeaths->SetText(FText::FromString(tbd));
		Row->BPoints->SetText(FText::FromString(tbp));
		Row->MDeaths->SetText(FText::FromString(tmd));
		Row->MPoints->SetText(FText::FromString(tmp));

		PlayersList->AddChild(Row);
	}

	for (int i = 0; i < Beast.Num(); i++)
	{
		auto E = Cast<AShellsCharacter>(Exp[i]);
		if (!E) return;

		FString pid = FString::FromInt(E->PID);
		FString tbd = FString::FromInt(E->TotalBeastDeaths);
		FString tbp = FString::FromInt(E->TotalBeastPoints);
		FString tmd = FString::FromInt(E->TotalMechaDeaths);
		FString tmp = FString::FromInt(E->TotalMechaPoints);

		Row->PlayerName->SetText(FText::FromString(E->Name));
		Row->BDeaths->SetText(FText::FromString(tbd));
		Row->BPoints->SetText(FText::FromString(tbp));
		Row->MDeaths->SetText(FText::FromString(tmd));
		Row->MPoints->SetText(FText::FromString(tmp));

		PlayersList->AddChild(Row);
	}*/


	//for (int i = 0; i < Exp.Num(); i++)
	//{
	////	auto Exps = Cast<AExplorerCharacter>(Exp[i]);
	//	//UE_LOG(LogTemp, Warning, TEXT("Looped set players list"))

	//	//	auto E = Cast<AExplorerCharacter>(PSDate);
	//	//if (!E) return;
	//	//UE_LOG(LogTemp, Warning, TEXT("Explorer exist"))

	//	//	auto APS = Cast<APlayerState>(E->GetPlayerState());
	//	//if (!APS) return;
	//	//UE_LOG(LogTemp, Warning, TEXT("a player state exist"));

	//	//auto PS = Cast<AMainPlayerState>(E->GetPlayerState());
	//	//if (!PS) return;
	//	//UE_LOG(LogTemp, Warning, TEXT("a player state exist"));


	//	UMatchmakingRow* Row = CreateWidget<UMatchmakingRow>(World, MatchmakingRowClass);
	//	if (!ensure(Row != nullptr)) return;

	//	UE_LOG(LogTemp, Warning, TEXT("Explorer and PlayerState exist	"))

	//	auto E = Cast<AExplorerCharacter>(Exp[i]);

	//	Row->Setup(this, 0);
	//	FString pid = FString::FromInt(E->PID);
	//	FString tbd = FString::FromInt(E->TotalBeastDeaths);
	//	FString tbp = FString::FromInt(E->TotalBeastPoints);
	//	FString tmd = FString::FromInt(E->TotalMechaDeaths);
	//	FString tmp = FString::FromInt(E->TotalMechaPoints);


	//	Row->PlayerName->SetText(FText::FromString(pid));
	//	Row->BDeaths->SetText(FText::FromString(tbd));
	//	Row->BPoints->SetText(FText::FromString(tbp));
	//	Row->MDeaths->SetText(FText::FromString(tmd));
	//	Row->MPoints->SetText(FText::FromString(tmp));

	//	PlayersList->AddChild(Row);
	//}

	//	auto PS = Cast<AMainPlayerState>(GetOwningPlayerState());
	//if (!PS) return;
	//UE_LOG(LogTemp, Warning, TEXT("Explorer and PlayerState exist	"))
	//	UMatchmakingRow* Row = CreateWidget<UMatchmakingRow>(World, MatchmakingRowClass);
	//if (!ensure(Row != nullptr)) return;


	//Row->Setup(this, 0);
	//FString pid = FString::FromInt(PS->TotalBeastDeaths);
	//FString tbd = FString::FromInt(PS->TotalBeastDeaths);
	//FString tbp = FString::FromInt(PS->TotalBeastPoints);
	//FString tmd = FString::FromInt(PS->TotalMechaDeaths);
	//FString tmp = FString::FromInt(PS->TotalMechaPoints);

	//Row->PlayerName->SetText(FText::FromString(pid));
	//Row->BDeaths->SetText(FText::FromString(tbd));
	//Row->BPoints->SetText(FText::FromString(tbp));
	//Row->MDeaths->SetText(FText::FromString(tmd));
	//Row->MPoints->SetText(FText::FromString(tmp));

	//PlayersList->AddChild(Row);
}

