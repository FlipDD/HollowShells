#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "World/Item.h"
#include "MainPlayerController.generated.h"

UCLASS()
class SHELLS_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI")
	TSubclassOf<class UInventoryBaseWidget> UIInventoryAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI")
	class UInventoryBaseWidget* UIInventory;
	
	UFUNCTION()
	FORCEINLINE UClass* GetPlayerPawnClass() const { return PlayerPawnClass; }

	void AddItem();
	void OnLevelCompleted(APawn *Pawn, bool bSucceded);

	// RPC
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerSetPawn(TSubclassOf<APawn> InPawnClass);
	virtual bool ServerSetPawn_Validate(TSubclassOf<APawn> InPawnClass);
	virtual void ServerSetPawn_Implementation(TSubclassOf<APawn> InPawnClass);
	// EndRPC

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// RPC
	UFUNCTION(Reliable, Client)
		void DeterminePawnClass();
	virtual void DeterminePawnClass_Implementation();
	// EndRPC

	// Replicated
	UPROPERTY(Replicated)
		TSubclassOf<APawn> PlayerPawnClass;
	// EndReplicated

private:
	FItemData Item;
};
