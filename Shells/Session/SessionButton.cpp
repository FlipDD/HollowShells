// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionButton.h"
#include "MatchmakingLevelScript.h"
#include "Engine/World.h"
#include "Session/MatchmakingLevelScript.h"


USessionButton::USessionButton()
{
    OnClicked.AddDynamic(this, &USessionButton::OnClick);
}

void USessionButton::OnClick()
{
	FTimerHandle WaitForResponseHandle;
	if (GetOuter()->GetWorld())
	{
		auto LevelScript = Cast<AMatchmakingLevelScript>(GetOuter()->GetWorld()->GetLevelScriptActor());
		//LevelScript->SendJoinMessage(CurrentSessionIndex);
		UE_LOG(LogTemp, Warning, TEXT("Button message might have been sent??!!!"));

		//GetWorld()->GetTimerManager().SetTimer(WaitForResponseHandle, this, &USessionButton::DelayedClickButton, 2);
	}
	APlayerController* pController = GetOuter()->GetWorld()->GetFirstPlayerController();
	if (pController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Button clicked!!"));
		if (this->SessionInfo->nplayers < 4)
		{
			FString cmd = "open " + this->SessionInfo->serverIp + ":" + FString::FromInt(this->SessionInfo->serverPort);
			pController->ConsoleCommand(cmd);
		}
	}
}

void USessionButton::DelayedClickButton()
{
}

void USessionButton::SetSessionInfo(FSessionInfo *Info, int SessionIndex)
{
	SessionInfo = Info;
	CurrentSessionIndex = SessionIndex;
}