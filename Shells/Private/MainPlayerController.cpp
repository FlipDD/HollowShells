
#include "MainPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "NetworkedGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InventoryBaseUserWidget.h"
#include "LobbyGameInstance.h"

AMainPlayerController::AMainPlayerController()
{
	bReplicates = true;

	UE_LOG(LogTemp, Warning, TEXT("Constructed PC"));
}

void AMainPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AMainPlayerController, PlayerPawnClass);
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//DeterminePawnClass();

	Item.Name = FText::FromString("Test item 01");
	Item.Amount = 0;

	if (UIInventoryAsset)
	{
		UIInventory = CreateWidget<UInventoryBaseWidget>(this, UIInventoryAsset);
		UIInventory->AddToViewport();
		UIInventory->SetVisibility(ESlateVisibility::Visible);
		UIInventory->SetItemData(Item);
	}
	//ServerSetPawn(PlayerPawnClass);
		//if (!ensureAlways(PlayerPawnClass)) return;
		//FString name = PlayerPawnClass->GetName();
		//UE_LOG(LogTemp, Warning, TEXT("Begin player controller : %s"), *name);

}

// Pawn Class
void AMainPlayerController::DeterminePawnClass_Implementation()
{
	if (IsLocalController())
	{
		ServerSetPawn(PlayerPawnClass);
	}
}

bool AMainPlayerController::ServerSetPawn_Validate(TSubclassOf<APawn> InPawnClass)
{
	return true;
}

void AMainPlayerController::ServerSetPawn_Implementation(TSubclassOf<APawn> InPawnClass)
{
	PlayerPawnClass = InPawnClass;

	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

void AMainPlayerController::AddItem()
{
	Item.Amount++;
	UIInventory->SetItemData(Item);
}

void AMainPlayerController::OnLevelCompleted(APawn *PlayerPawn, bool bSucceded)
{

}
