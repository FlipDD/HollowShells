#pragma once

#include "CoreMinimal.h"
#include "UI/MenuWidget.h"
#include "CharacterSelectionMenu.generated.h"

UCLASS()
class SHELLS_API UCharacterSelectionMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:
	bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* ChooseBeastButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* ChooseExplorerButton;

	UFUNCTION()
		void TryChooseBeast();
	UFUNCTION()
		void TryChooseExplorer();
	
};
