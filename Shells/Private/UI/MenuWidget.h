
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"

#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHELLS_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Setup();
	void Teardown();

	void SetMenuInterface(IMenuInterface* Interface);

protected:
	IMenuInterface* MenuInterface;
};
