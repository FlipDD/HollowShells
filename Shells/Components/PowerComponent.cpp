
#include "PowerComponent.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "NetworkedGameMode.h"

UPowerComponent::UPowerComponent()
{
	MaxPower = 3;
	bIsDead = false;
	SetIsReplicated(true);
}

void UPowerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPowerComponent, CurrentPower);
}

void UPowerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto MyOwner = GetOwner();
		if (MyOwner) {
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UPowerComponent::HandleTakeDamage);
		}
	}

	CurrentPower = MaxPower;
}

void UPowerComponent::OnRep_CurrentPower(float OldHealth)
{
	const auto Damage = CurrentPower - OldHealth;
	OnPowerChanged.Broadcast(this, CurrentPower, Damage, nullptr, nullptr, nullptr);
}

void UPowerComponent::OnRep_bIsDead()
{
}

void UPowerComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy,
	AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead) {
		return;
	}

	if (DamageCauser != DamagedActor) {
		return;
	}

    CurrentPower = FMath::Clamp(CurrentPower - Damage, 0.0f, MaxPower);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(CurrentPower));

	OnPowerChanged.Broadcast(this, CurrentPower, Damage, DamageType, InstigatedBy, DamageCauser);

	bIsDead = CurrentPower <= 0.0f;
	if (bIsDead) {
		const auto Mode = Cast<ANetworkedGameMode>(GetWorld()->GetAuthGameMode());
		if (Mode) {
			Mode->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

void UPowerComponent::Charge(float HealAmount)
{
	if (HealAmount <= 0.0f || CurrentPower <= 0.0f) {
		return;
	}

	CurrentPower = FMath::Clamp(CurrentPower + HealAmount, 0.0f, MaxPower);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(CurrentPower), *FString::SanitizeFloat(HealAmount));

	OnPowerChanged.Broadcast(this, CurrentPower, -HealAmount, nullptr, nullptr, nullptr);
}