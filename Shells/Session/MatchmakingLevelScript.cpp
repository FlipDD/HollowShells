
#include "MatchmakingLevelScript.h"

#include "Button.h"
#include "VerticalBox.h"
#include "TextBlock.h"
#include "TimerManager.h"
#include "SessionButton.h"
#include "VerticalBoxSlot.h"
#include "ScrollBox.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DateTime.h"


#include "Session/SessionButton.h" 

#include "Kismet/GameplayStatics.h"


AMatchmakingLevelScript::AMatchmakingLevelScript()
{
	ConstructorHelpers::FClassFinder<UUserWidget> MatchMakingBPClass(TEXT("/Game/Blueprints/Widgets/WBP_Matchmaking.WBP_Matchmaking_C"));
	if (!ensure(MatchMakingBPClass.Class != nullptr)) return;
	MatchmakingWidgetClass = MatchMakingBPClass.Class;
}

void AMatchmakingLevelScript::BeginPlay()
{
	Super::BeginPlay();

	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != "MainMenu")
		return;

	bReadyToHost = false;
	ServersList = new TArray<FSessionInfo*>();

	GetWorld()->GetTimerManager().SetTimer(ServerListTimerHandle, this, &AMatchmakingLevelScript::OnUpdateServerList, 2, true);

    if (MatchmakingWidgetClass) {
		MatchmakingWidget = CreateWidget<UUserWidget>(GetWorld(), MatchmakingWidgetClass);
		MatchmakingWidget->AddToViewport();

        auto ConnectButton = Cast<UButton>(MatchmakingWidget->GetWidgetFromName(TEXT("ConnectButton")));
		if (ConnectButton) {
		    ConnectButton->OnClicked.AddDynamic(this, &AMatchmakingLevelScript::OnConnectClicked);
		}

        auto HostButton = Cast<UButton>(MatchmakingWidget->GetWidgetFromName(TEXT("HostButton")));
		if (HostButton) {
			HostButton->SetIsEnabled(false);
			HostButton->OnClicked.AddDynamic(this, &AMatchmakingLevelScript::OnHostClicked);
		}

		ServerListScrollBoxWidget = Cast<UScrollBox>(MatchmakingWidget->GetWidgetFromName(TEXT("MyScrollBox")));
		auto Controller = GetWorld()->GetFirstPlayerController();
		if (Controller) {
			Controller->bShowMouseCursor = true;
			Controller->bEnableClickEvents = true;
			Controller->bEnableMouseOverEvents = true;
		}
	}
}

void AMatchmakingLevelScript::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}

void AMatchmakingLevelScript::StartGameHost(int Port, int Index)
{
	HostPort = Port;
	bReadyToHost = true;
//	(*ServersList)[Index]->nplayers = 1;
}

void AMatchmakingLevelScript::UpdateSessionsList(FString ServerInfo)
{
	TArray<FString> Out;
	ServerInfo.ParseIntoArray(Out, TEXT("|"), true);
	//for (int i = 1; i < Out.Num() - 3; i += 4) 
	for (int i = 1; i < Out.Num() - 3; i += 4)
	{
		FSessionInfo *TempInfo = new FSessionInfo();
		TempInfo->id = FCString::Atoi(*Out[i]);
		TempInfo->name = Out[i + 1];
		TempInfo->serverIp = Out[i + 2];
		TempInfo->serverPort = FCString::Atoi(*Out[i + 3]);
		
		//TempInfo->serverPort = FCString::Atoi(*Out[i + 4]);
		//// New info
		//TempInfo->nplayers = FCString::Atoi(*Out[i + 3]);
		bool sExist = false;

		if (!ensureAlways(!ServersList->IsValidIndex(0)))
			return;
		if (!ensureAlways(ServersList != nullptr))
			return;

		for (int j = 0; j < ServersList->Num(); j++)
		{
			if ((*ServersList)[j]->id == TempInfo->id)
			{
				sExist = true;
			}
		}
		if (!sExist)
		{
			ServersList->Add(TempInfo);
		}
	}
}

void AMatchmakingLevelScript::OnConnectClicked()
{
    TcpClient = new TCPClient(this);
}

void AMatchmakingLevelScript::OnHostClicked()
{
	FDateTime time;
	int32 CurrTimeHour = time.GetHour();
	int32 CurrTimeMin = time.GetMinute();
	int32 CurrTimeSec = time.GetSecond();
	FString timeString = FString::FromInt(CurrTimeHour) + ":" + FString::FromInt(CurrTimeMin) + ":" + FString::FromInt(CurrTimeSec);

    TcpClient->HostNewGame("Hollow session " + timeString, "7777");
}

void AMatchmakingLevelScript::AddPlayer(int ListIndex)
{
	 if ((*ServersList)[ListIndex])
		(*ServersList)[ListIndex]->nplayers += 1;
}

void AMatchmakingLevelScript::SendJoinMessage(int ListIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Joining at n: %d"), ListIndex);
	TcpClient->AddPlayerToSession(ListIndex);
}

void AMatchmakingLevelScript::OnUpdateServerList()
{
	if (TcpClient) {
		if (TcpClient->IsConnected())
		{
			UButton* connectButton = Cast<UButton>(MatchmakingWidget->GetWidgetFromName(TEXT("ConnectButton")));
			if (connectButton)
			{
				connectButton->SetIsEnabled(false);
			}
			UButton* hostButton = Cast<UButton>(MatchmakingWidget->GetWidgetFromName(TEXT("HostButton")));
			if (connectButton)
			{
				hostButton->SetIsEnabled(true);
			}
			TcpClient->RequestServerList();
			if (bReadyToHost)
			{
				APlayerController* pController = GetWorld()->GetFirstPlayerController();
				if (pController)
				{
					pController->ConsoleCommand("open FirstPersonExampleMap?listen");
				}
			}
		}
		if (ServersList->Num() >= 0) {
			//UE_LOG(LogTemp, Warning, TEXT("Message 660!!"));

			if (MatchmakingWidget && ServerListScrollBoxWidget) {
				//	UE_LOG(LogTemp, Warning, TEXT("Message 661!!"));

				//if (ServerListScrollBoxWidget)
				//{
				//	if (ServerListScrollBoxWidget->HasAnyChildren())
				//	{
				TArray<UWidget*> allchildren = ServerListScrollBoxWidget->GetAllChildren();

				for (int i = 0; i < allchildren.Num(); i++)
				{
					UE_LOG(LogTemp, Warning, TEXT("Message 663!!"));
					allchildren[i]->RemoveFromParent();
				}
				//	}
				//}
				for (int i = 0; i < ServersList->Num(); i++)
				{
					UE_LOG(LogTemp, Warning, TEXT("Message 664!!"));

					UVerticalBox* itemWidgetBox = NewObject<UVerticalBox>();
					ServerListScrollBoxWidget->AddChild(itemWidgetBox);
					auto itemWidget = NewObject <USessionButton>(this);

					itemWidget->SetSessionInfo((*ServersList)[i], i);
					UTextBlock* itemWidgetText = NewObject <UTextBlock>();

					int playerInSession = (*ServersList)[i]->nplayers + 1;
					FString playersInSessionString = FString::FromInt(playerInSession);

					FDateTime time;
					float CurrTimeSec = GetWorld()->GetDeltaSeconds();
					UE_LOG(LogTemp, Warning, TEXT("current secs %d"), CurrTimeSec);

					itemWidgetText->SetText(FText::FromString((*ServersList)[i]->name + " - " + playersInSessionString + "/4" + FString::SanitizeFloat(CurrTimeSec)));

					itemWidget->AddChild(itemWidgetText);
					UVerticalBoxSlot* slot = itemWidgetBox->AddChildToVerticalBox(itemWidget);
					static FMargin Padding(5);
					slot->SetPadding(Padding);
				}
			}
		}
	}
}