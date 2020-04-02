#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeaconActor.generated.h"

UENUM(BlueprintType)
enum class EBeaconState : uint8
{
	None UMETA(DisplayName = "None"),
	Damaged UMETA(DisplayName = "Damaged"),
	Drilling UMETA(DisplayName = "Drilling"),
	Armed UMETA(DisplayName = "Armed"),
	Destroyed UMETA(DisplayName = "Destroyed"),
};

UCLASS()
class SHELLS_API ABeaconActor : public AActor
{
	GENERATED_BODY()

public:
	ABeaconActor();
	void Tick(float DeltaTime) override;

protected:
	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* DrillSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Root1Component;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Root2Component;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Root3Component;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* BeaconExplosionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* BeaconEnergyComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UDestructibleComponent* DestructibleMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UDestructibleComponent* DestructibleGround;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* BeaconLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float GroundImpulseForce = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float PillarImpulseForce = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float SphereSpeed = 1400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float DrillSphereRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float DrillTimerDelay;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Gameplay")
		float Progress = 5;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Gameplay")
		int ExplorersDrilling = 0;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	bool WasDestroyed;
	float DestructionProgress;
	EBeaconState State;

	/** Sound to play when the beacon is destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DestroySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* RumbleSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float SoundMultiplier = .5f;

public:
	//UPROPERTY(EditDefaultsOnly, Category = "Effects")
	//	class UParticleSystem* DestructionEffect;
	//UPROPERTY(EditDefaultsOnly, Category = "Effects")
	//	class UMaterialInstanceDynamic* GlowingMaterialInst;

	void BeginDrilling();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBeginDrilling();
	virtual void ServerBeginDrilling_Implementation();
	virtual bool ServerBeginDrilling_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopDrilling();
	virtual void ServerStopDrilling_Implementation();
	virtual bool ServerStopDrilling_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastBeginDrill();
	virtual void MulticastBeginDrill_Implementation();
	virtual bool MulticastBeginDrill_Validate();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastEndDrill();
	virtual void MulticastEndDrill_Implementation();
	virtual bool MulticastEndDrill_Validate();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
		void OnDrillingSuccess();
	virtual void OnDrillingSuccess_Implementation();
	virtual bool OnDrillingSuccess_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void OnDrillingFailure();
	virtual void OnDrillingFailure_Implementation();
	virtual bool OnDrillingFailure_Validate();

	void SetDestroyed();
	FORCEINLINE bool GetWasDestroyed() const { return WasDestroyed; };

	FTimerHandle DrillTimerHandle;
	bool bIsBeingDrilled;
};
