#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/PowerComponent.h"
#include "Components/LauncherComponent.h"
#include "Components/DrillComponent.h"
#include "Components/ClimbComponent.h"

#include "Engine/TriggerBox.h"


#include "ExplorerCharacter.generated.h"

UENUM()
enum class EExplorerState : uint8 {
    None,
    Drilling,
    Dead
};

UCLASS(config = Game)
class AExplorerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AExplorerCharacter();
	void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Power")
		void PowerChangeEffect();

	UFUNCTION(NetMulticast, Reliable)
		void OnPlayerDeath();
	virtual void OnPlayerDeath_Implementation();

	UFUNCTION(BlueprintPure, Category = "Power")
		float GetCurrentPower() const { return PowerComponent ? PowerComponent->GetPower() : 0; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRVariables", meta = (AllowPrivateAccess = "true"))
		bool bWantsToGrip;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }

	void PlayHurtMontage() const;
	void PlayDazzedEffect() const;

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetInsanityMode();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool IsInsane = false;

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1, FName StartSectionName = NAME_None);
	UFUNCTION(NetMulticast, Reliable)
		void MulticastStopAnimMontage(class UAnimMontage* AnimMontage);
	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayInsanityMontage();
	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayDestroyedShellMontage();
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		UAnimMontage* FallingMontage;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		UAnimMontage* DrillingMontage;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		UAnimMontage* EndDrillingMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "InsanityVariables")
		float InsanityTimerMax = 4;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "InsanityVariables")
		float DestroyShellTimer = 10;

protected:
	void BeginPlay() override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void CollectPickups();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerCollectPickups();
	virtual void ServerCollectPickups_Implementation();
	virtual bool ServerCollectPickups_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetWalkSpeed(float Speed);

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector LauncherOffset;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* MuzzleLocation;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	// Drilling sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DrillBeginSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DrillEndSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float DrillRotationInterp = 0.5f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	//	class USoundBase* DrillLoopSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;


	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
		bool bDied;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
		bool IsDrilling;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
		bool IsDrillOverlapping;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Player")
		void StopFire();
	UFUNCTION(BlueprintCallable, Category = "Player")
		void StartRun();
	UFUNCTION(BlueprintCallable, Category = "Player")
		void StopRun();

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;
	/** AnimMontage to play when the explorer is insane */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* InsaneMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
		UAnimMontage* DyingMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
		UAnimMontage* HurtMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
		UAnimMontage* JumpMontage;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Jump() override;
	void StopJumping() override;
	void BeginCrouch();
	void EndCrouch();
	void ToggleCrouch();
	void Fire() const;

	void WaitForHeadset();

	UFUNCTION(BlueprintCallable)
	void RumbleController();

	FTimerHandle StartDrillSoundHandle;

	void StartPlayingDrillSound();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UHapticFeedbackEffect_Base* HapticEffect;

	FTimerHandle HapticTimerHandle;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
		float ZoomInterpSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		float DefaultFOV;

	void BeginZoom();
	void EndZoom();

	UPROPERTY(EditAnywhere)
		class UCurveFloat* RadiusVsVelocity;
	UPROPERTY()
		class UMaterialInstanceDynamic* BlinkerMaterialInstance;
	UPROPERTY(EditAnywhere, Category = "Post Process")
		class UMaterialInstance* BlinkerMaterialBase;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* CollectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		UPowerComponent* PowerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		ULauncherComponent* LauncherComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		UDrillComponent* DrillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		UClimbComponent* ClimbComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* DrillMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* HandMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Post Process")
		class UPostProcessComponent* VelocityPostProcessComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* DrillAudioLoopComponent;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		float CollectionSphereRadius;

	UFUNCTION()
		void OnPowerChanged(UPowerComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InsanityComponents", meta = (AllowPrivateAccess = "true"))
		AActor* AttachedActor;
	
public:
	UPROPERTY(Replicated)
		int PID;
	UPROPERTY(Replicated)
		FString Name;
	UPROPERTY(Replicated)
		int TotalBeastPoints;
	UPROPERTY(Replicated)
		int TotalBeastDeaths;
	UPROPERTY(Replicated)
		int TotalMechaPoints;
	UPROPERTY(Replicated)
		int TotalMechaDeaths;

	void AddDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* DazzedComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* DestroyShellRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* VRShootPS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* VRDrillSparksPS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* VRDrillSmokePS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRVariables")
		EControllerHand HandL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRVariables")
		EControllerHand HandR;

	UFUNCTION()
		void OnDrillOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnDrillOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return FollowCamera; }

};

