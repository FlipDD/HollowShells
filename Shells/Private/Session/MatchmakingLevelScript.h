// Fill out your copyright notice in the Descr

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Session/TCPClient.h"
#include "ScrollBox.h"

#include "MatchmakingLevelScript.generated.h"

USTRUCT()
struct FSessionInfo
{
	GENERATED_BODY();

	UPROPERTY() 
		int id;
	UPROPERTY() 
		FString name;
	UPROPERTY() 
		FString serverIp;
	UPROPERTY() 
		int serverPort;
	UPROPERTY()
		int nplayers;
};

UCLASS()
class SHELLS_API AMatchmakingLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AMatchmakingLevelScript();
	void UpdateSessionsList(FString ServerInfo);
	void StartGameHost(int Port, int Index);
	UFUNCTION()
		void AddPlayer(int ListIndex);
	UFUNCTION()
		void SendJoinMessage(int ListIndex);

	TArray<FSessionInfo*> *ServersList;

protected:
	void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TCPClient *TcpClient;

	bool bReadyToHost;
	int HostPort;

	//UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MatchmakingWidgetClass;

	UPROPERTY()
    UUserWidget *MatchmakingWidget;
	UPROPERTY()
	UScrollBox *ServerListScrollBoxWidget;
	UPROPERTY()
	FTimerHandle ServerListTimerHandle;

    UFUNCTION()
		void OnUpdateServerList();
	UFUNCTION()
		void OnConnectClicked();
	UFUNCTION()
		void OnHostClicked();
};
