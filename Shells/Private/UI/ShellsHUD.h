#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShellsHUD.generated.h"

UCLASS()
class AShellsHUD : public AHUD
{
	GENERATED_BODY()

public:
	AShellsHUD();

	void DrawHUD() override;

private:
	class UTexture2D* CrosshairTex;
};

