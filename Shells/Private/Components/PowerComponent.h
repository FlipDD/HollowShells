
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PowerComponent.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnPowerChangedSignature,
class UPowerComponent*, OwningHealthComp,
	float, Power,
	float, Delta,
	const class UDamageType*, DamageType,
	class AController*, Instigator,
	AActor*, DamageCauser);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHELLS_API UPowerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPowerComponent();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnPowerChangedSignature OnPowerChanged;

	UFUNCTION(BlueprintCallable, Category = "PowerComponent")
		void Charge(float HealAmount);

	UFUNCTION()
		float GetPower() const { return CurrentPower; }

protected:
	void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerComponent")
		float MaxPower;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, BlueprintReadOnly, Category = "PowerComponent")
		float CurrentPower;
	UFUNCTION() void OnRep_CurrentPower(float OldPower);

	UPROPERTY(ReplicatedUsing = OnRep_bIsDead, BlueprintReadOnly, Category = "PowerComponent")
		bool bIsDead;
    UFUNCTION() void OnRep_bIsDead();

	UFUNCTION()
		void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);

};
