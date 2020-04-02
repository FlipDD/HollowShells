

#include "InventoryItemUserWidget.h"
#include "Components/TextBlock.h"

void UInventoryItemWidget::SetItemName(FText Text)
{
	if (TextBlock_Name)
	{
		TextBlock_Name->SetText(Text);
	}
}

void UInventoryItemWidget::SetItemAmount(int Amount)
{
	if (TextBlock_Count)
	{
		FText text = FText::FromString(FString::FromInt(Amount));

		TextBlock_Count->SetText(text);
	}
}
