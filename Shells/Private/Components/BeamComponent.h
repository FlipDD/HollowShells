
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeamComponent.generated.h"

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:
	UPROPERTY() TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY() FVector_NetQuantize TraceTo;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHELLS_API UBeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBeamComponent();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void BeginPlay() override;

	void Fire(FVector SpawnLocation, FRotator SpawnRotation);
	void StartFire();
	void StopFire();

	void PlayFireEffects(FVector TraceEnd) const;
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* RockImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Cooldown;

    UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(FVector SpawnLocation, FRotator SpawnRotation);
	virtual void ServerFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation);
	virtual bool ServerFire_Validate(FVector SpawnLocation, FRotator SpawnRotation);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
		float BulletSpread;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace HitScanTrace;
	UFUNCTION() void OnRep_HitScanTrace() const;

	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime;
	float bCanShoot;
};
