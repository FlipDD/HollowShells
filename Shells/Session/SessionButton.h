
#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "SessionButton.generated.h"

/**
 *
 */
UCLASS()
class SHELLS_API USessionButton : public UButton
{
	GENERATED_BODY()

public:
	USessionButton();
    void SetSessionInfo(struct FSessionInfo *Info, int SessionIndex);

private:
	UFUNCTION()
		void OnClick();

	void DelayedClickButton();

	FSessionInfo* SessionInfo;
	int CurrentSessionIndex;
};