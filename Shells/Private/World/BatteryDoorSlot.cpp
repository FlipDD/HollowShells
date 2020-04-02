#include "BatteryDoorSlot.h"

#include "Components/BoxComponent.h"

#include "MovingRock.h"

ABatteryDoorSlot::ABatteryDoorSlot()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("TriggerVolume");
	check(TriggerVolume);
	TriggerVolume->SetupAttachment(RootComponent);

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABatteryDoorSlot::OnOverlapBegin);
}

void ABatteryDoorSlot::BeginPlay()
{
	Super::BeginPlay();

}

void ABatteryDoorSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABatteryDoorSlot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (AMovingRock* Platform : RocksToTrigger)
	{
		Platform->Unlock();
	}
	UE_LOG(LogTemp, Warning, TEXT("Unlocked"));
}

