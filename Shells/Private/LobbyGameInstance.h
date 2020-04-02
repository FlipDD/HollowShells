#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"

#include "UI/MenuInterface.h"
#include "LobbyGameInstance.generated.h"

UCLASS()
class SHELLS_API ULobbyGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	ULobbyGameInstance(const FObjectInitializer & ObjectInitializer);

	void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();

	UFUNCTION(BlueprintCallable)
	void InGameLoadMenu();

	UFUNCTION(BlueprintCallable)
	void LoadSelectionMenu();

	UFUNCTION(BlueprintCallable)
		void LoadLeaderboardMenu();

	UFUNCTION(Exec)
	void Host(FString ServerName) override;
	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	UFUNCTION(Exec)
		void JoinIP(const FString& Address);
	UFUNCTION(Exec)
		void HostIP();

	void StartSession();
	void LoadMainMenu() override;
	void RefreshServerList() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<class APawn> ExplorerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<class APawn> BeastClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TSubclassOf<class UUserWidget> DeathScreenClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TSubclassOf<class UUserWidget> BeastInGameClass;

private:
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;
	TSubclassOf<class UUserWidget> LeaderboardMenuClass;
	TSubclassOf<class UUserWidget> CharacterSelectionClass;

	class UMainMenu* Menu;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	FString DesiredServerName;
	void CreateSession();
};
