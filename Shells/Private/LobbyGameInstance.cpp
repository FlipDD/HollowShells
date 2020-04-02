// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "UI/MainMenu.h"
#include "UI/MenuWidget.h"

#include "Explorer/ExplorerCharacter.h"
#include "Beast/ShellsCharacter.h"
#include "MainPlayerController.h"
#include "LeaderboardMenu.h"

const static FName SESSION_NAME = TEXT("Shells");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

ULobbyGameInstance::ULobbyGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu.WBP_MainMenu_C"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu.WBP_InGameMenu_C"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;
	InGameMenuClass = InGameMenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> LeaderboardMenuBPClass(TEXT("/Game/Blueprints/Widgets/WB_Leaderboard.WB_Leaderboard_C"));
	if (!ensure(LeaderboardMenuBPClass.Class != nullptr)) return;
	LeaderboardMenuClass = LeaderboardMenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> CharacterSelectionBPClass(TEXT("/Game/MenuSystem/WB_CharacterSelection.WB_CharacterSelection_C"));
	if (!ensure(CharacterSelectionBPClass.Class != nullptr)) return;
	CharacterSelectionClass = CharacterSelectionBPClass.Class;

	ConstructorHelpers::FClassFinder<AShellsCharacter> BeastBPClass(TEXT("/Game/Blueprints/BeastCharacterBP.BeastCharacterBP_C"));
	if (!ensure(BeastBPClass.Class != nullptr)) return;
	BeastClass = BeastBPClass.Class;

	ConstructorHelpers::FClassFinder<AExplorerCharacter> ExplorerBPClass(TEXT("/Game/Blueprints/BP_ExplorerCharacter.BP_ExplorerCharacter_C"));
	if (!ensure(ExplorerBPClass.Class != nullptr)) return;
	ExplorerClass = ExplorerBPClass.Class;
}

void ULobbyGameInstance::Init()
{
	const auto Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ULobbyGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ULobbyGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ULobbyGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ULobbyGameInstance::OnJoinSessionComplete);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("SessionInterface Invalid"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
	}
}

void ULobbyGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;

	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void ULobbyGameInstance::InGameLoadMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	auto Menu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void ULobbyGameInstance::LoadLeaderboardMenu()
{
	if (!ensure(LeaderboardMenuClass != nullptr)) return;

	auto Menu = CreateWidget<UMenuWidget>(this, LeaderboardMenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	Menu->SetMenuInterface(this);

	auto LeaderboardMenu = Cast<ULeaderboardMenu>(Menu);
	if (LeaderboardMenu)
		LeaderboardMenu->SetPlayersList();
}

void ULobbyGameInstance::LoadSelectionMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	auto Menu = CreateWidget<UMenuWidget>(this, CharacterSelectionClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	Menu->SetMenuInterface(this);
}


void ULobbyGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid())
	{
		const auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession) {
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else {
			CreateSession();
		}
	}
}

void ULobbyGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) {
		CreateSession();
	}
}

void ULobbyGameInstance::CreateSession()
{
	if (SessionInterface.IsValid()) {
		FOnlineSessionSettings SessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") {
			SessionSettings.bIsLANMatch = true;
		}
		else {
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.NumPublicConnections = 8;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bAllowInvites = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bUsesStats = false;
		SessionSettings.bAntiCheatProtected = false;
		SessionSettings.BuildUniqueId = 0;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
		UE_LOG(LogTemp, Warning, TEXT("Session created"));
	}
}

void ULobbyGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	auto Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	if (!Success) {
		Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("NO Hosting"));

		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}

	if (Menu != nullptr) {
		Menu->Teardown();
		}


	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	auto World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/Maps/Lobby?listen", false, true);

}

void ULobbyGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = INT32_MAX;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("COULDNT REFRESH"));
	}
}

void ULobbyGameInstance::OnFindSessionsComplete(bool bSuccess)
{
	if (bSuccess && SessionSearch.IsValid() && Menu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));

		TArray<FServerData> ServerNames;

		for (const auto& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *SearchResult.GetSessionIdStr());
			FServerData Data;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUsername = SearchResult.Session.OwningUserName;
			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName)) {
				Data.Name = ServerName;
			}
			else {
				Data.Name = "Could not find name.";
			}
			ServerNames.Add(Data);
		}

		Menu->SetServerList(ServerNames);
	}
}

void ULobbyGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;
	if (Menu) {
		Menu->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void ULobbyGameInstance::JoinIP(const FString& Address)
{
	const auto Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	auto PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute, true);
}

void ULobbyGameInstance::HostIP()
{
	const auto Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	auto World = GetWorld();
	if (!ensure(World != nullptr)) return;

	const auto PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("about to host")));

	World->ServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?listen", false, true);
}

void ULobbyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address)) {
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
		return;
	}

	auto Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	auto PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute, true);
}

void ULobbyGameInstance::StartSession()
{
	if (SessionInterface.IsValid()) {
		SessionInterface->StartSession(SESSION_NAME);
	}
}

void ULobbyGameInstance::LoadMainMenu()
{
	auto PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute, true);
}
