
#include "MenuWidget.h"

void UMenuWidget::Setup()
{
	this->AddToViewport();

    const auto World = GetWorld();
	if (!ensure(World != nullptr)) return;

    auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::Teardown()
{

	auto World = GetWorld();
	if (!ensureAlways(World != nullptr)) return;

	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	RemoveFromViewport();

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = false;
}

void UMenuWidget::SetMenuInterface(IMenuInterface* Interface)
{
	MenuInterface = Interface;
}

