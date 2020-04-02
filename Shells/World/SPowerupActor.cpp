
#include "SPowerupActor.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"


ASPowerupActor::ASPowerupActor()
{
	bIsActive = false;
	SetReplicates(true);
}


void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASPowerupActor, bIsActive);
}


void ASPowerupActor::OnRep_bIsActive()
{
	OnPowerupStateChanged(bIsActive);
}


void ASPowerupActor::ActivatePowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);

	bIsActive = true;
	OnRep_bIsActive();
}

