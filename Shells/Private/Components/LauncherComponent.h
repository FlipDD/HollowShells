
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Explorer/ExplorerProjectile.h"
#include "LauncherComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHELLS_API ULauncherComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	ULauncherComponent();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Fire(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* ComponentToAttach);
	void StartFire();
	void StopFire();

protected:
	void BeginPlay() override;

	void PlayFireEffects(USceneComponent* ComponentToAttach) const;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<AExplorerProjectile> ProjectileClass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<class UCameraShake> CamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	    float Cooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
		float Spread;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component = NULL);
	virtual void ServerFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component = NULL);
	virtual bool ServerFire_Validate(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component = NULL);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Animation")
		void MulticastFire(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component = NULL);

    UPROPERTY(Replicated)
	bool bCanShoot;

	FTimerHandle TimerHandle_Cooldown;
    float LastFireTime;
};
