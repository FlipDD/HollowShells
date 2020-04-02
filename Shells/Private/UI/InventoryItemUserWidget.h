
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHELLS_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock *TextBlock_Name;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock *TextBlock_Count;

	void SetItemName(FText Text);
	void SetItemAmount(int Amount);
};
