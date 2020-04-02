// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchmakingWidget.generated.h"

//USTRUCT()
//struct FSessionInfo
//{
//	GENERATED_BODY();
//
//	UPROPERTY() int ID;
//	UPROPERTY() FString Name;
//	UPROPERTY() FString ServerIp;
//	UPROPERTY() int ServerPort;
//};

UCLASS()
class SHELLS_API UMatchmakingWidget : public UUserWidget
{
	GENERATED_BODY()

//public:
//	void NativeOnInitialized() override;
//
//	void UpdateSessionsList(FString ServerInfo);
//	void StartGameHost(int Port);
//
//protected:
//	UPROPERTY(meta = (BindWidget))
//		class UButton* HostButton;
//
//	UPROPERTY(meta = (BindWidget))
//		class UButton* ConnectButton;
//
//	UPROPERTY(meta = (BindWidget))
//		class UScrollBox *ServerListScrollBox;
//
//private:
//	UFUNCTION()
//		void OnUpdateServerList();
//	UFUNCTION()
//		void OnConnectClicked();
//	UFUNCTION()
//		void OnHostClicked();
//
//	FTimerHandle ServerListTimerHandle;
//
//	TArray<FSessionInfo> ServersList;
//	class MatchmakingClient *Client;
//
//	bool bReadyToHost;
//	int HostPort;
};
