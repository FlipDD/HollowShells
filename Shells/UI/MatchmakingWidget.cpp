// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchmakingWidget.h"
#include "Button.h"
#include "ScrollBox.h"
#include "TextBlock.h"
#include "VerticalBox.h"
#include "VerticalBoxSlot.h"

#include "Session/MatchmakingClient.h"
#include "Session/SessionButton.h"

//void UMatchmakingWidget::NativeOnInitialized()
//{
//	ConnectButton->OnClicked.AddDynamic(this, &UMatchmakingWidget::OnConnectClicked);
//
//	HostButton->SetIsEnabled(false);
//	HostButton->OnClicked.AddDynamic(this, &UMatchmakingWidget::OnHostClicked);
//
//	const auto Controller = GetWorld()->GetFirstPlayerController();
//	if (Controller) {
//		Controller->bShowMouseCursor = true;
//		Controller->bEnableClickEvents = true;
//		Controller->bEnableMouseOverEvents = true;
//        // Controller->SetInputMode(UI only)
//	}
//}
//
//void UMatchmakingWidget::StartGameHost(int Port)
//{
//	HostPort = Port;
//	bReadyToHost = true;
//}
//
//
//void UMatchmakingWidget::UpdateSessionsList(FString ServerInfo)
//{
//	TArray<FString> Out;
//	ServerInfo.ParseIntoArray(Out, TEXT("|"), true);
//	for (int i = 1; i < Out.Num() - 3; i += 4) {
//		FSessionInfo TempInfo;
//		TempInfo.ID = FCString::Atoi(*Out[i]);
//		TempInfo.Name = Out[i + 1];
//		TempInfo.ServerIp = Out[i + 2];
//		TempInfo.ServerPort = FCString::Atoi(*Out[i + 3]);
//		ServersList.Add(TempInfo);
//	}
//}
//
//void UMatchmakingWidget::OnConnectClicked()
//{
//	Client = new MatchmakingClient(this);
//}
//
//void UMatchmakingWidget::OnHostClicked()
//{
//	Client->HostNewGame("My test server", "8804");
//}
//
//void UMatchmakingWidget::OnUpdateServerList()
//{
//	if (Client) {
//		if (Client->IsConnected()) {
//			HostButton->SetIsEnabled(true);
//			if (bReadyToHost) {
//				if (auto Controller = GetWorld()->GetFirstPlayerController()) {
//					Controller->ConsoleCommand("open MyMap?listen");
//				}
//			}
//		}
//		if (ServersList.Num() > 0) {
//			if (ServerListScrollBox) {
//				auto AllChildren = ServerListScrollBox->GetAllChildren();
//
//				for (int i = 0; i < AllChildren.Num(); i++) {
//					AllChildren[i]->RemoveFromParent();
//				}
//
//				for (int i = 0; i < ServersList.Num(); i++) {
//					auto ItemWidgetsBox = NewObject<UVerticalBox>();
//					ServerListScrollBox->AddChild(ItemWidgetsBox);
//
//					auto ItemWidget = NewObject<USessionButton>(this);
//					ItemWidget->SetSessionInfo(&ServersList[i]);
//
//					auto ItemWidgetText = NewObject<UTextBlock>();
//					ItemWidgetText->SetText(FText::FromString(ServersList[i].Name));
//					ItemWidget->AddChild(ItemWidgetText);
//
//					auto Slot = ItemWidgetsBox->AddChildToVerticalBox(ItemWidget);
//					Slot->SetPadding(FMargin{ 5 });
//				}
//			}
//		}
//	}
//}