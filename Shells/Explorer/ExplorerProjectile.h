
#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ExplorerProjectile.generated.h"

UCLASS(config = Game)
class SHELLS_API AExplorerProjectile : public AActor
{
	GENERATED_BODY()

public:
	AExplorerProjectile();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE class USphereComponent* GetCollisionComp() const { return SphereComponent; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		class USphereComponent* SphereComponent;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
		UParticleSystem* RockImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* ImpactSound;

private:
	void SetInstigator(class AExplorerCharacter *Player);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetInstigator(class AExplorerCharacter *Player);
	virtual void ServerSetInstigator_Implementation(class AExplorerCharacter *Player);
	virtual bool ServerSetInstigator_Validate(class AExplorerCharacter *Player);

	UFUNCTION()
		void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint) const;
};
