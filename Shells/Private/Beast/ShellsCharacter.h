#pragma once

#include "Components/TimelineComponent.h"
#include "Components/PostProcessComponent.h"
#include "Containers/EnumAsByte.h"
#include "Classes/CableComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ShellsCharacter.generated.h"

class UInputComponent;

UENUM(BlueprintType)
enum class ActionType : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Swipe	UMETA(DisplayName = "Swipe"),
	Jump	UMETA(DisplayName = "Jump"),
	Insanity	UMETA(DisplayName = "Insanity"),
	DestroyShell	UMETA(DisplayName = "DestroyShell")
};

UCLASS(config = Game)
class AShellsCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* MotionController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* HandMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* VRMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* VRInvisibleMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ArcDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		class USplineComponent* ArcSpline;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* GrabShpere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ArcEndPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TeleportCylinder;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Ring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* DestroyProgressWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* MotionControllerCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* VRSwipeTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRComponents", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* VRSwipeTargetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		class UCableComponent* InsanityCable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SwipeComponents", meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* SwipeCollider;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class AHandController> HandControllerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Insanity", meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* InsanityBuzzLoopAudio;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Insanity", meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* DestroyLoopAudio;

	UFUNCTION()
		void OnClawOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnMotionControllerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	AShellsCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Enum)
		ActionType State;

	UPROPERTY()
		class UMenuWidget* DeathScreen;
	UPROPERTY()
		class UBeastInGameUI* BeastInGameUI;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;



	//virtual void OnConstruction(const FTransform & Transform) override;

	//UFUNCTION(BlueprintCallable, Category = "VRTemplate|Grabbing")
	//	AActor* GetActorNearHand() const;

	//UFUNCTION(BlueprintCallable, Category = "VRTemplate|Grabbing")
	//	void GrabActor();

	//UFUNCTION(BlueprintCallable, Category = "VRTemplate|Grabbing")
	//	void ReleaseActor();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void ActivateTeleporter();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void DisableTeleporter();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		bool TraceTeleportDestination(TArray<FVector>& TracePoints, FVector& NavMeshLocation, FVector& TraceLocation);

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void ClearArc();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void UpdateArcSpline(bool FoundValidLocation, TArray<FVector> SplinePoints);

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void UpdateArcEndPoint(bool ValidLocationFound, FVector NewLocation);

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void GetTeleportDestination(FVector& MyLocation, FRotator& MyRotation);

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void TeleportPressed();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void TeleportReleased();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void ExecuteTeleportation();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void TeleportActor();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void SetJumpEndPoint();

	UFUNCTION(BlueprintCallable, Category = "JumpComponents")
		void PrepareJump();

	UFUNCTION()
		void UpdateJumpPosition(float DeltaTime);
	UFUNCTION()
		void FinishJump();
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_bStartedJumping, BlueprintReadOnly, Category = "JumpComponents")
		bool bStartedJumping;
	UFUNCTION()
		void OnRep_bStartedJumping();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "JumpComponents")
		bool bChannelling;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "JumpComponents")
		bool PlayingDazzedEmitter;

	FTimerHandle FadeTimerHandle;
	FTimerHandle DeathTimerHandle;
	
	FTimerHandle VRSwipeHandle;

	UFUNCTION()
		void HandleSuccessfulSwipe();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents")
		float FadeOutDuration;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents")
		bool bIsTeleporting;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents")
		bool SuccessfulSwipe;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents")
		bool IsPreparingSwipeVR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents")
		bool IsSwippingVR;

	//UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_DealDamage, BlueprintReadWrite, Category = Movement)
	//	int Health;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = Movement)
		int Health;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	/** AnimMontage to play each time we swipe */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* SwipeMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* InsanityMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* JumpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* LandMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* FallingMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* RecoveryMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DyingMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* GettingUpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DestroyShellMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* FinishDestroyShellMontage;

	UPROPERTY(EditAnywhere, Category = "Materials")
		UMaterialInterface* JumpDecalMaterial;

	////UFUNCTION(BlueprintCallable, Category = "VRTemplate|RoomScale")
	////	void SetupRoomScaleOutline();

	////UFUNCTION(BlueprintCallable, Category = "VRTemplate|RoomScale")
	////	void UpdateRoomScaleOutline();

	////UFUNCTION(BlueprintCallable, Category = "VRTemplate")
	////	void RumbleController(float Intensity);

	////UFUNCTION()
	////	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	////UFUNCTION(BlueprintCallable, Category = "VRTemplate")
	////	void UpdateHandAnimation();

	void SetTeleportRotation(FRotator& NewTeleportRotation);

private:
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRTemplate|Variables", meta = (AllowPrivateAccess = "true"))
		EGrip_Code CurrentGripState;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InsanityComponents", meta = (AllowPrivateAccess = "true"))
		AActor* AttachedActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		TArray<class USplineMeshComponent*> SplineMeshes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		bool bLastFrameValidDestination;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRVariables", meta = (AllowPrivateAccess = "true"))
		bool bIsRoomScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		FVector TeleportDestination;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		bool bIsValidTeleportDestination;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRVariables", meta = (AllowPrivateAccess = "true"))
		bool bWantsToGrip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRVariables", meta = (AllowPrivateAccess = "true"))
		FRotator TeleportRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		float TeleportLaunchVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRVariables", meta = (AllowPrivateAccess = "true"))
		FRotator InitialControllerRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRVariables", meta = (AllowPrivateAccess = "true"))
		class UHapticFeedbackEffect_Base* VRHapticEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* BeamMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		class UMaterial* BeamMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		FLinearColor TeleportFadeColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		bool bIsTeleporterActive;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		bool bCanClearArc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "JumpComponents", meta = (AllowPrivateAccess = "true"))
		bool bCanUpdateSpline;

	UPROPERTY()
		class AExplorerCharacter* InsaneExplorer;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float ImpulseForce = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float JumpInterpSpeed = 3;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		uint32 bUsingMotionControllers : 1;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> SwipeAttackCameraShake;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UCameraShake> FinishJumpCameraShake;

	void EnableCollisions();
	void DisableCollisions();
	void EnablePawnControl();
	void DisablePawnControl();
	void EnableClawCollision();
	void DisableClawCollision();
	void SetStateIdle();
	void CheckIfSwipeHit();

	void CheckVisibilityForInsanity(float DeltaTime);
	void CheckExplorersInRange(float Radius, float HalfHeight, TArray<AActor*> &ExplorersFound);
	bool IsExplorerInSight(AActor *ExplorerToCheck);

	void OnOverlapPortal(FVector portalLocation, bool onPortal);

	void WaitForHeadset();

	UFUNCTION(BlueprintCallable)
	void EnableBodyPhysics();
	UFUNCTION(BlueprintCallable)
	void DisableBodyPhysics();

	bool OnPortal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
		FVector PortalLocation;

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void StartJump();

	UFUNCTION(Server, Reliable, WithValidation, Category = "HealthSystem")
		void ServerDealDamage(int Damage);
	UFUNCTION(BlueprintCallable, Category = "HealthSystem")
		void DealDamage(int Damage);

	UFUNCTION()
		void StopAnim();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastStopAnim();
	UFUNCTION()
		void DissolveMaterial();
	// TODO Change this to unreliable
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastDissolveChanges(bool IsDefault = false);
	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetCableMaterial(UMaterialInstance* Mat);

	UFUNCTION()
		void FadeOutScreen();
	UPROPERTY(Replicated)
		float InitialDissolveValue = 1.5f;
	UPROPERTY()
		FTimerHandle DissolveHandle;
	UPROPERTY()
		FTimerHandle FadeHandle;


	UFUNCTION()
		void Respawn();

protected:

	void PlaySwipeCameraShake();

	void OnResetVR();

	void MoveForward(float Val);
	void MoveRight(float Val);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Turn(float Rate);
	void Look(float Rate);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void InsanityPressed();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void InsanityReleased();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void PlayInsanitySound();
	UPROPERTY()
		FTimerHandle PlayInsanitySoundHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float FirstDelayTime = 2.5f;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void PlayDestroySound();
	UPROPERTY()
		FTimerHandle PlayDestroySoundHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DestroyShellBeginSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* SuccessInsanitySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float InsanitySoundPitch = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* SuccessDestroySound;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float DestroySoundPitch = 1;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void InteractPressed();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void InteractHoldPressed(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void InteractReleased();

	UFUNCTION(BlueprintCallable)
		void KillExplorer(AExplorerCharacter* Explorer);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void ServerKillExplorer(AExplorerCharacter* Explorer);

	void SwipeAttack();
	void SwipeCancelVR();
	void SwipeReleased();

	void ResetSwipeWindow();

	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
	void ServerSwipeAttack();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
	void ServerBeginInsanityAttack(bool VisibilityValue, bool AttachmentValue, class UAnimMontage* Montage);
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
	void ServerEndInsanityAttack(bool VisibilityValue, bool AttachmentValue);
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
	void ServerJumpAttack();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
	void ServerPrepareJumpAttack();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
	void ServerLandAttack();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
	void ServerUpdateJumpPosition(FVector Position, FRotator Rotation);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
		void ServerTeleport();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
		void ServerFallingIdle();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
		void ServerPlayExplorerInsane(AExplorerCharacter* E);
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
		void ServerPlayExplorerDead(AExplorerCharacter* E);
	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
		void ServerSetCableProperties();

	UFUNCTION(Server, Reliable, WithValidation, Category = "Animation")
		void ServerToggleMobility(bool Value);
		UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Animation")
			void MulticastToggleMobility(bool Value);


	void UpdateHeadRotation(bool DefaultRotation = false);

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_HeadRotation, BlueprintReadWrite, Category = Movement)
		FRotator HeadRotation;

	UFUNCTION()
		void OnRep_HeadRotation();

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Animation")
		void MulticastPlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1, FName StartSectionName = NAME_None);
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Animation")
		void MulticastStopAnimMontage(class UAnimMontage* AnimMontage, ACharacter* Target);
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Animation")
		void MulticastSetCableVisibility(bool VisibilityValue, bool AttachmentValue);
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Animation")
		void MulticastPlayLandEmitter();
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Animation")
		void MulticastInsanityDazzedEmitter(class AExplorerCharacter* Explorer, bool Destroy = false);

	
	void JumpReleased();

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Destructible")
		class UDestructibleMesh* DestructibleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
		TArray<AActor*> Explorers;

	UPROPERTY()
		AActor* TargetExplorer;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		class UParticleSystem* InsanityBurst;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		class UParticleSystem* LandEffect;
	UPROPERTY(VisibleAnywhere, Category = "Effects")
		class UParticleSystemComponent* DazzedComponent;
	UPROPERTY(VisibleAnywhere, Category = "Effects")
		class UParticleSystemComponent* VRSwipeParticle;
	UPROPERTY(VisibleAnywhere, Category = "Effects")
		class UParticleSystemComponent* VRSwipeTargetParticle;

	UPROPERTY(BlueprintReadOnly)
		UMaterialParameterCollection* DissolveParameterCollection;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialParameterCollection* InsanityParameterCollection;	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialParameterCollection* DestroyShellProgressParam;
	UPROPERTY(EditDefaultsOnly, Category = "Post Process")
		class UPostProcessComponent* PostProcessComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Post Process")
		class UPostProcessComponent* VelocityPostProcessComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Post Process")
		class UPostProcessComponent* DeathPostProcessComponent;
	UPROPERTY()
		class UMaterialInstanceDynamic* BlinkerMaterialInstance;
	UPROPERTY(EditAnywhere, Category = "Post Process")
		class UMaterialInstance* BlinkerMaterialBase;
	UPROPERTY(EditAnywhere, Category = "Post Process")
		class UMaterialInstance* InsanityCableMaterial;
	UPROPERTY(EditAnywhere, Category = "Post Process")
		class UMaterialInstance* DestroyShellCableMaterial;
	UPROPERTY(EditAnywhere) 
		class UCurveFloat* RadiusVsVelocity;
	
	//Insanity timeline -- TODO
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Gameplay")
		UTimelineComponent* InsanityTimeline;
	UPROPERTY(VisibleAnywhere)
		UCurveFloat* FloatCurve;
	UFUNCTION(BlueprintCallable)
		void InsanityTimelineCallback(float val);
	UFUNCTION(NetMulticast, Unreliable, Category = "Animation")
		void MulticastUpdateInsanityCollection(float InterpolatedValue);

	UPROPERTY()
		TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InsanityVariables")
		float InsanityTimelineAlpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InsanityVariables")
		float ExplorerInsanitySpeed = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRVariables")
		float JumpSpeed;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRVariables")
		float JumpEndPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRVariables")
		float CurrentJumpDistance;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRVariables")
		EControllerHand Hand;

	FORCEINLINE class UMotionControllerComponent* GetMotionController() const { return MotionController; }
	FORCEINLINE FRotator GetInitialControllerRotation() const { return InitialControllerRotation; }
	FORCEINLINE bool GetIsValidTeleportDestination() const { return bIsValidTeleportDestination; }
	FORCEINLINE bool GetIsTeleporterActive() const { return bIsTeleporterActive; }

	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

