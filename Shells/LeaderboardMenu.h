// Copyright 2019 - Filipe Ribeiro & Raul Leal

#pragma once

#include "CoreMinimal.h"
#include "UI/MenuWidget.h"
#include "LeaderboardMenu.generated.h"

/**
 * 
 */
UCLASS()
class SHELLS_API ULeaderboardMenu : public UMenuWidget
{
	GENERATED_BODY()

	ULeaderboardMenu(const FObjectInitializer& ObjectInitializer);

protected:
	bool Initialize() override;
	
public:
	UPROPERTY(meta = (BindWidget))
		class UPanelWidget* PlayersList;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;
	
	UFUNCTION(BlueprintCallable)
	void SetPlayersList();

	UFUNCTION(BlueprintCallable)
	void CloseLeaderboard();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerGetPlayerStates();
	void ServerGetPlayerStates_Implementation();
	bool ServerGetPlayerStates_Validate();

	TArray<class AMainPlayerState*> PS;


	TSubclassOf<class UUserWidget> MatchmakingRowClass;
};
