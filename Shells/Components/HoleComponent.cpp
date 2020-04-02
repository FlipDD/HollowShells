
#include "HoleComponent.h"

UHoleComponent::UHoleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHoleComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UHoleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

