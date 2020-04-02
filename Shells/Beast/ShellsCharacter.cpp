#include "ShellsCharacter.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Containers/UnrealString.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HeadMountedDisplay.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/Color.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "TimerManager.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "DestructibleMesh.h"
#include "Misc/OutputDeviceNull.h"
#include "CollisionQueryParams.h"
#include "IXRTrackingSystem.h"
#include "HandController.h"
#include "World/PortalBox.h"
#include "Blueprint/UserWidget.h"
#include "UI/BeastInGameUI.h"
#include "UI/RespawnScreen.h"
#include "MainPlayerState.h"

#include "Explorer/ExplorerCharacter.h"
#include "Beast/BeastAnimInstance.h"
#include "LobbyGameInstance.h"

// GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, __FUNCTION__);
DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AShellsCharacter::AShellsCharacter()
{
	SetReplicates(true);
	SetReplicateMovement(true);
	GetCharacterMovement()->SetIsReplicated(true);

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	
	// ChangeVR
	FirstPersonCameraComponent->SetupAttachment(GetMesh(), FName("Head"));
	//FirstPersonCameraComponent->SetupAttachment(GetRootComponent());
	
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 20));
	FirstPersonCameraComponent->SetRelativeRotation(FRotator(90, 0, -90));

	// ChangeVR
	//FirstPersonCameraComponent->bUsePawnControlRotation = false;
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	SwipeCollider = CreateDefaultSubobject<UCapsuleComponent>("SwipeHandCollider");
	SwipeCollider->SetupAttachment(GetMesh(), FName("RightHand"));
	SwipeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwipeCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwipeCollider->OnComponentBeginOverlap.AddDynamic(this, &AShellsCharacter::OnClawOverlapBegin);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>("PostProcessComponent");
	VelocityPostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>("VelocityPostProcessComponent");
	DeathPostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>("DeathPostProcessComponent");


	if (HasAuthority())
	{
		InsanityCable = CreateDefaultSubobject<UCableComponent>("InsanityCable");
		InsanityCable->SetupAttachment(RootComponent);
		InsanityCable->SetIsReplicated(true);
	}

	//static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Curves/InsanityCurveFloat"));
	//check(Curve.Succeeded());
	//FloatCurve = Curve.Object;

	// ChangeVR
	bUsingMotionControllers = true;

	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->SetTrackingSource(EControllerHand::Right);
	//R_MotionController->SetShowDeviceModel(true);
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetTrackingSource(EControllerHand::Left);
	//L_MotionController->SetShowDeviceModel(true);
	L_MotionController->SetupAttachment(RootComponent);

	// VR/Jump components
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionContoller"));
	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	ArcDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ArcDirection"));
	ArcSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ArcSpline"));
	//GrabShpere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabShpere"));
	ArcEndPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcEndPoint"));
	TeleportCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCylinder"));
	Ring = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring"));

	DestroyProgressWidget = CreateDefaultSubobject<UWidgetComponent>("DestroyProgressWidget");
	DestroyProgressWidget->SetupAttachment(FirstPersonCameraComponent);
	DestroyProgressWidget->SetVisibility(false);

	//Arrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowThing"));
	MotionControllerCollider = CreateDefaultSubobject<USphereComponent>("MotionControllerCollider");
	MotionControllerCollider->SetupAttachment(HandMesh);
	MotionControllerCollider->OnComponentBeginOverlap.AddDynamic(this, &AShellsCharacter::OnMotionControllerOverlapBegin);

	// ChangeVR 
	//MotionController->SetupAttachment(GetRootComponent());
	MotionController->SetupAttachment(FirstPersonCameraComponent);

	MotionController->SetCollisionProfileName(TEXT("BlockAll"));

	VRMesh = CreateDefaultSubobject<USkeletalMeshComponent>("VRMesh");
	VRMesh->SetupAttachment(GetRootComponent());

	VRInvisibleMesh = CreateDefaultSubobject<USkeletalMeshComponent>("VRInvisibleMesh");
	VRInvisibleMesh->SetupAttachment(GetRootComponent());

	VRSwipeTarget = CreateDefaultSubobject<USphereComponent>("VRSwipeTarget");
	VRSwipeTarget->SetupAttachment(VRMesh);
	VRSwipeTarget->SetRelativeLocation(FVector(50, 50, 30));
	VRSwipeTarget->SetRelativeScale3D(FVector(0.5f));

	VRSwipeTargetMesh = CreateDefaultSubobject<UStaticMeshComponent>("VRSwipeTargetMesh");
	VRSwipeTargetMesh->SetupAttachment(VRSwipeTarget);
	VRSwipeTargetMesh->SetRelativeScale3D(FVector(.6f));
	VRSwipeTargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VRSwipeTargetMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	VRSwipeTargetMesh->SetVisibility(false);

	// ChangeVR
	//HandMesh->SetupAttachment(R_MotionController);
	HandMesh->SetupAttachment(MotionController);

	HandMesh->SetRelativeLocation(FVector(0, 50, 20));
	HandMesh->SetCollisionProfileName(TEXT("NoCollision"));
	HandMesh->CastShadow = false;

	ArcDirection->SetupAttachment(HandMesh);
	ArcDirection->ArrowSize = 0.2f;
	ArcSpline->SetupAttachment(HandMesh);
	//GrabShpere->SetupAttachment(HandMesh);
	//GrabShpere->SetSphereRadius(10.0f);
	//GrabShpere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	//GrabShpere->SetGenerateOverlapEvents(true);
	//GrabShpere->OnComponentBeginOverlap.AddDynamic(this, &AShellsCharacter::OnComponentBeginOverlap);

	ArcEndPoint->SetupAttachment(RootComponent);
	ArcEndPoint->bAbsoluteLocation = true;
	ArcEndPoint->bAbsoluteRotation = true;
	ArcEndPoint->bAbsoluteScale = true;
	ArcEndPoint->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));
	ArcEndPoint->SetGenerateOverlapEvents(false);
	ArcEndPoint->SetCollisionProfileName(TEXT("NoCollision"));
	ArcEndPoint->bVisible = false;

	TeleportCylinder->SetupAttachment(RootComponent);
	TeleportCylinder->bAbsoluteLocation = true;
	TeleportCylinder->bAbsoluteRotation = true;
	TeleportCylinder->bAbsoluteScale = true;
	TeleportCylinder->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.0f));
	TeleportCylinder->SetGenerateOverlapEvents(false);
	TeleportCylinder->SetCollisionProfileName(TEXT("NoCollision"));

	Ring->SetupAttachment(TeleportCylinder);
	Ring->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.15f));
	Ring->SetGenerateOverlapEvents(false);
	Ring->SetCollisionProfileName(TEXT("NoCollision"));

	DestructibleMesh = NULL;
	DestructibleMesh = CreateDefaultSubobject<UDestructibleMesh>("DestructibleMesh");

	//RoomScaleMesh->SetupAttachment(Arrow);
	//RoomScaleMesh->bAbsoluteRotation = true;
	//RoomScaleMesh->bAbsoluteScale = true;
	//RoomScaleMesh->SetGenerateOverlapEvents(false);
	//RoomScaleMesh->SetCollisionProfileName(TEXT("NoCollision"));
	//if (HapticEffect.Succeeded())
	//{
	   // VRHapticEffect = HapticEffect.Object;
	//}
	//if (SM_Beam.Succeeded())
	//{
	   // BeamMesh = SM_Beam.Object;
	   // if (M_Beam.Succeeded())
	   // {
	   //	 BeamMaterial = M_Beam.Object;
	   // }
	//}

	DazzedComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DazzedParticleSystem"));
	DazzedComponent->SetupAttachment(GetRootComponent());

	VRSwipeParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VRSwipeParticleComponent"));
	VRSwipeParticle->SetupAttachment(HandMesh);
	VRSwipeParticle->bAutoActivate = false;

	VRSwipeTargetParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VRSwipeTargetParticle"));
	VRSwipeTargetParticle->SetupAttachment(VRSwipeTarget);
	VRSwipeTargetParticle->bAutoActivate = false;

	InsanityBuzzLoopAudio = CreateDefaultSubobject<UAudioComponent>("InsanityBuzzLoopAudio");
	InsanityBuzzLoopAudio->SetupAttachment(RootComponent);
	InsanityBuzzLoopAudio->bAutoActivate = false;

	DestroyLoopAudio = CreateDefaultSubobject<UAudioComponent>("DestroyLoopAudio");
	DestroyLoopAudio->SetupAttachment(RootComponent);
	DestroyLoopAudio->bAutoActivate = false;

	// Initialization
	bWantsToGrip = false;
	InitialControllerRotation = FRotator(0.0f, 0.0f, 0.0f);

	SplineMeshes.Empty();

	bLastFrameValidDestination = false;
	bIsRoomScale = false;
	bIsTeleporterActive = false;
	TeleportDestination = FVector(0.0f, 0.0f, 0.0f);
	bIsValidTeleportDestination = false;
	TeleportRotation = FRotator(0.0f, 0.0f, 0.0f);
	TeleportLaunchVelocity = 2000.0f;
	bIsTeleporting = false;
	bCanClearArc = true;
	bCanUpdateSpline = true;
	InsanityTimelineAlpha = 0.0f;
	JumpEndPoint = 0.0f;
	JumpSpeed = 3000;
	bStartedJumping = false;
	CurrentJumpDistance = 0;
	FadeOutDuration = 0.65f;
	PostProcessComponent->BlendWeight = 0.0f;
	SuccessfulSwipe = false;
	Health = 100;
	PlayingDazzedEmitter = false;
	State = ActionType::Idle;
}

// Input
void AShellsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Movement
	PlayerInputComponent->BindAxis("MoveForward", this, &AShellsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShellsCharacter::MoveRight);
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn rate" is for devices that we choose to treat as a rate of change, such as an analog joystic k
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShellsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShellsCharacter::LookUpAtRate);

	// Insanity Attack
	PlayerInputComponent->BindAction("Insanity", IE_Pressed, this, &AShellsCharacter::InsanityPressed);
	PlayerInputComponent->BindAction("Insanity", IE_Released, this, &AShellsCharacter::InsanityReleased);
	// Jump Attack
	PlayerInputComponent->BindAction("BeastJump", IE_Pressed, this, &AShellsCharacter::TeleportPressed);
	PlayerInputComponent->BindAction("BeastJump", IE_Released, this, &AShellsCharacter::JumpReleased);
	// Swipe Attack
	PlayerInputComponent->BindAction("Swipe", IE_Pressed, this, &AShellsCharacter::SwipeAttack);
	PlayerInputComponent->BindAction("Swipe", IE_Released, this, &AShellsCharacter::SwipeReleased);

	//Others
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AShellsCharacter::InteractPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AShellsCharacter::InteractReleased);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AShellsCharacter::OnResetVR);
}

void AShellsCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExplorerCharacter::StaticClass(), Explorers);

	//SetupRoomScaleOutline();

	TeleportCylinder->SetVisibility(false, true);
	InsanityCable->SetVisibility(false);
	InsanityCable->SetMaterial(0, InsanityCableMaterial);

	FOnTimelineFloat onTimelineCallback;
	InsanityTimeline = NewObject<UTimelineComponent>(this, FName("TimelineInsanityAnimation"));
	InsanityTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript; // Indicate it comes from a blueprint so it gets cleared when we rerun construction scripts
	this->BlueprintCreatedComponents.Add(InsanityTimeline); // Add to array so it gets saved
	InsanityTimeline->SetNetAddressable();	// This component has a stable name that can be referenced for replication
	InsanityTimeline->SetPropertySetObject(this); // Set which object the timeline should drive properties on
	InsanityTimeline->SetDirectionPropertyName(FName("TimelineDirection"));
	InsanityTimeline->SetLooping(false);
	InsanityTimeline->SetTimelineLength(5.0f);
	InsanityTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	InsanityTimeline->SetPlaybackPosition(0.0f, false);
	onTimelineCallback.BindUFunction(this, FName{ TEXT("InsanityTimelineCallback") });
	InsanityTimeline->AddInterpFloat(FloatCurve, onTimelineCallback);
	InsanityTimeline->RegisterComponent();
	InsanityTimeline->SetIsReplicated(true);

	InsanityTimeline->Reverse();

	if (BlinkerMaterialBase != nullptr) 
	{
		BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
		VelocityPostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);

		BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), .2f);
	}
	
	auto GI = Cast<ULobbyGameInstance>(GetGameInstance());
	if (GI != nullptr && IsLocallyControlled())
	{
		BeastInGameUI = CreateWidget<UBeastInGameUI>(GI, GI->BeastInGameClass);
	}

	FTimerHandle WaitHeadsetHandle;
	GetWorldTimerManager().SetTimer(WaitHeadsetHandle, this, &AShellsCharacter::WaitForHeadset, 0.2f);

}

void AShellsCharacter::WaitForHeadset()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		JumpSpeed = 2500;
		HandMesh->SetRelativeLocation(FVector(-10, 0, 0));
		VRMesh->SetVisibility(true);

		// Need to call this on server
		GetMesh()->SetVisibility(false);

		// ChangeVR
		FirstPersonCameraComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FirstPersonCameraComponent->bUsePawnControlRotation = false;
		MotionController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		HandMesh->AttachToComponent(R_MotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		FRotator NewRotation = FRotator(0, 0, 90);
		HandMesh->AddRelativeRotation(FQuat(NewRotation));

	}
	else
	{
		VRMesh->SetVisibility(false);
		VRInvisibleMesh->SetVisibility(false);
		HandMesh->SetVisibility(false);
		VelocityPostProcessComponent->DestroyComponent();
	}
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "Error";
	}
}

void AShellsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() && State != ActionType::Swipe)
	{
		FRotator CameraRotation = FirstPersonCameraComponent->GetComponentRotation();
		FRotator NewRotation = FRotator(0, CameraRotation.Yaw - 90, 0);
		VRMesh->SetRelativeRotation(NewRotation);
		VRInvisibleMesh->SetRelativeRotation(NewRotation);
		//GetMesh()->SetRelativeRotation(NewRotation);
	}
	else
	{
		UpdateHeadRotation();
	}

	switch (State)
	{
	case ActionType::Swipe:
		break;
	case ActionType::Jump:
		break;
	case ActionType::Insanity:
	
		CheckVisibilityForInsanity(DeltaTime);
	
		break;
	case ActionType::DestroyShell:
		
		InteractHoldPressed(DeltaTime);
		break;
	default:
		break;
	}

	if (InsanityTimeline != NULL)
	{
		InsanityTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}

	if (bChannelling)
	{
		// Add insanity timer?
		InsanityCable->CableLength = FVector::Dist(GetActorLocation(), TargetExplorer->GetActorLocation());
		FVector ThisLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
		FVector TargetLocation = FVector(TargetExplorer->GetActorLocation().X, TargetExplorer->GetActorLocation().Y, 0);
		FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(ThisLocation, TargetLocation);
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime, 10);
		FRotator DesiredCameraRotation = UKismetMathLibrary::FindLookAtRotation(FirstPersonCameraComponent->GetComponentLocation(), TargetExplorer->GetActorLocation());
		FRotator NewCameraRotation = FMath::RInterpTo(FirstPersonCameraComponent->GetComponentRotation(), DesiredRotation, DeltaTime, 10);
		if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			SetActorRotation(NewRotation);
			FirstPersonCameraComponent->SetWorldRotation(NewCameraRotation);
		}
	}

	if (bStartedJumping)
	{
		UpdateJumpPosition(DeltaTime);
	}

	if (bCanClearArc)
	{
		ClearArc();
	}

	if (bIsTeleporterActive)
	{
		ClearArc();
		TArray<FVector> TracePoints;
		FVector NavMeshLocation;
		FVector TraceLocation;
		bIsValidTeleportDestination = TraceTeleportDestination(TracePoints, NavMeshLocation, TraceLocation);

		TeleportCylinder->SetVisibility(bIsValidTeleportDestination, true);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TArray<AActor*> IgnoreActors;
		FHitResult HitResult;

		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		IgnoreActors.Add(this);

		FVector TraceStart = NavMeshLocation;
		FVector TraceEnd = NavMeshLocation + FVector(0.0f, 0.0f, -500.0f);
		// Trace down to find a valid location for player to stand at (original NavMesh location is offset upwards, so we must find the actual floor)
		bool m_Result = UKismetSystemLibrary::LineTraceSingleForObjects(
			GetWorld(),
			TraceStart,
			TraceEnd,
			ObjectTypes,
			false,
			IgnoreActors,
			EDrawDebugTrace::None,
			HitResult,
			true,
			FLinearColor::Red
		);

		FVector NewLocation = UKismetMathLibrary::SelectVector(HitResult.ImpactPoint, NavMeshLocation, HitResult.bBlockingHit);

		TeleportCylinder->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

		bLastFrameValidDestination = bIsValidTeleportDestination;
		
		if (bCanUpdateSpline)
		{
			UpdateArcEndPoint(bIsValidTeleportDestination, TraceLocation);
			UpdateArcSpline(bIsValidTeleportDestination, TracePoints);
		} 
	}

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (RadiusVsVelocity == nullptr) return;
			float Radius = RadiusVsVelocity->GetFloatValue(GetCharacterMovement()->Velocity.Size());
		if (BlinkerMaterialInstance != nullptr)
			BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), Radius / 2.5f);
	}
}

void AShellsCharacter::PlayDestroySound()
{
	DestroyLoopAudio->Play();
}

// Interact //
void AShellsCharacter::InteractPressed()
{
	if (State != ActionType::Idle) return;

	if (OnPortal)
	{
		FTimerHandle Handle;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0, 1, 0.5f, FLinearColor::Black);
		GetWorldTimerManager().SetTimer(Handle, this, &AShellsCharacter::ServerTeleport, 0.5f);
	}
	else
	{
		TArray<AActor*> HitActors;
		CheckExplorersInRange(220, 220, HitActors);
		if (HitActors.IsValidIndex(0))
		{
			TargetExplorer = NULL;
			for (auto a : HitActors)
			{
				auto Explorer = Cast<AExplorerCharacter>(a);
				if (Explorer != nullptr && Explorer->IsInsane)
				{
					State = ActionType::DestroyShell;
					MulticastSetCableMaterial(DestroyShellCableMaterial);
					TargetExplorer = Explorer;
					ServerToggleMobility(false);
					ServerBeginInsanityAttack(true, true, DestroyShellMontage);
					bChannelling = true;

					UGameplayStatics::PlaySoundAtLocation(this, DestroyShellBeginSound, GetActorLocation());
					// 0.8 is the time of DestroyShellBeginSound
					GetWorldTimerManager().SetTimer(PlayDestroySoundHandle, this, &AShellsCharacter::PlayDestroySound, 0.8f);


					if (IsLocallyControlled())
					{
						if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) DestroyProgressWidget->SetVisibility(true);
						else if (BeastInGameUI != NULL) BeastInGameUI->AddToViewport();

						UGameplayStatics::GetPlayerCameraManager(this, 0)->StartCameraFade(0.0f, .3f, .5f, FLinearColor::Red, false, true);
					}

					break;
				}
			}
		}
	}
}

void AShellsCharacter::InteractHoldPressed(float DeltaTime)
{
	if (TargetExplorer == NULL) return;
	// Add insanity timer?
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) 
	{
		FVector ThisLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
		FVector TargetLocation = FVector(TargetExplorer->GetActorLocation().X, TargetExplorer->GetActorLocation().Y, 0);
		FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(ThisLocation, TargetLocation);
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime, 10);
		FRotator OffsetRotation = FRotator(FMath::RandRange(-.5f, .5f), FMath::RandRange(-.5f, .5f), 0);
		FRotator DesiredCameraRotation = UKismetMathLibrary::FindLookAtRotation(FirstPersonCameraComponent->GetComponentLocation(), TargetExplorer->GetActorLocation());
		FRotator NewCameraRotation = FMath::RInterpTo(FirstPersonCameraComponent->GetComponentRotation(), DesiredRotation, DeltaTime, 10);
		SetActorRotation(NewRotation + OffsetRotation);
		FirstPersonCameraComponent->SetWorldRotation(NewCameraRotation + OffsetRotation);
	}

	auto E = Cast<AExplorerCharacter>(TargetExplorer);
	if (!E) return;
	E->DestroyShellTimer -= DeltaTime;
	float Value = E->DestroyShellTimer / 10; // Is 10 really the initial value?
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DestroyShellProgressParam, FName("Alpha"), Value);

	if (E->DestroyShellTimer < 0)
	{ 
		KillExplorer(E);
		//ServerKillExplorer(E);
		//auto ExplorerPlayerState = Cast<AMainPlayerState>(E->GetPlayerState());
		auto ThisPlayerState = Cast<AMainPlayerState>(GetPlayerState());

		// TODO - Add death here
		ThisPlayerState->AddPlayerBeastPoint();
		E->AddDeath();
		auto State = Cast<AMainPlayerState>(E->GetPlayerState());
		if (State) State->AddPlayerMechaDeath();
		//ExplorerPlayerState->AddPlayerMechaDeath();
		/*E->TotalMechaDeaths++;
		TotalBeastPoints++;*/

		UGameplayStatics::PlaySoundAtLocation(this, SuccessInsanitySound, GetActorLocation(), 1, DestroySoundPitch);
	}
	UE_LOG(LogTemp, Warning, TEXT("Current destroying timer: %f"), E->	DestroyShellTimer);	
}

void AShellsCharacter::ServerKillExplorer_Implementation(AExplorerCharacter* Explorer)
{
	auto ExplorerPlayerState = Cast<AMainPlayerState>(Explorer->GetPlayerState());
	auto ThisPlayerState = Cast<AMainPlayerState>(GetPlayerState());
	ThisPlayerState->AddPlayerBeastPoint();
	ExplorerPlayerState->AddPlayerMechaDeath();
	Explorer->TotalMechaDeaths++;
}
bool AShellsCharacter::ServerKillExplorer_Validate(AExplorerCharacter* Explorer)
{
	return true;
}

void AShellsCharacter::KillExplorer(AExplorerCharacter* Explorer)
{
	if (BeastInGameUI) BeastInGameUI->PlayAnimation(BeastInGameUI->FadeOut);
	InteractReleased();
	ServerPlayExplorerDead(Explorer);
}

void AShellsCharacter::InteractReleased()
{
	if (State == ActionType::DestroyShell)
	{
		State = ActionType::Idle;
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) DestroyProgressWidget->SetVisibility(false);
		else if (BeastInGameUI != NULL) BeastInGameUI->RemoveFromViewport();

		MulticastPlayAnimMontage(FinishDestroyShellMontage);
		ServerEndInsanityAttack(false, false);

		ServerToggleMobility(true);

		bChannelling = false;

		GetWorldTimerManager().ClearTimer(PlayDestroySoundHandle);
		DestroyLoopAudio->Stop();

		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.3f, 0, .5f, FLinearColor::Red);
	}
}

void AShellsCharacter::ServerPlayExplorerDead_Implementation(AExplorerCharacter* E)
{
	E->MulticastPlayDestroyedShellMontage();
}
bool AShellsCharacter::ServerPlayExplorerDead_Validate(AExplorerCharacter* E)
{
	return true;
}

// Swipe Attack //
void AShellsCharacter::SwipeAttack()
{
	bWantsToGrip = true;
	if (State == ActionType::Idle)
	{
		if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 0.9f, 1.0f, FLinearColor(0.1f, 0.0f, 0.0f));
			ServerSwipeAttack();
		}
		else
		{
			// ChangeState here?
			State = ActionType::Swipe;
			IsPreparingSwipeVR = true;
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 0.9f, 1.0f, FLinearColor(0.1f, 0.0f, 0.0f));
			GetController()->SetIgnoreMoveInput(true);
			GetWorldTimerManager().SetTimer(VRSwipeHandle, this, &AShellsCharacter::SwipeCancelVR, 3);
			//FVector Forward = VRMesh->GetForwardVector() * 100;
			//FVector TargetLocation = FVector(Forward.X, 50, 30);
			//VRSwipeTarget->SetRelativeLocation(TargetLocation);
			VRSwipeTargetMesh->SetVisibility(true);
			VRSwipeTargetParticle->Activate();
		}
	}
}

void AShellsCharacter::SwipeReleased()
{
	bWantsToGrip = false;
}

void AShellsCharacter::SwipeCancelVR()
{
	IsPreparingSwipeVR = false;
	IsSwippingVR = false;
	State = ActionType::Idle;
	VRSwipeParticle->Deactivate();
	VRSwipeTargetParticle->Deactivate();
	VRSwipeTargetMesh->SetVisibility(false);
	GetController()->SetIgnoreMoveInput(false);
}

void AShellsCharacter::ServerSwipeAttack_Implementation()
{
	State = ActionType::Swipe;
	MulticastPlayAnimMontage(SwipeMontage);
}

bool AShellsCharacter::ServerSwipeAttack_Validate()
{
	return true;
}

void AShellsCharacter::ServerBeginInsanityAttack_Implementation(bool VisibilityValue, bool AttachmentValue, UAnimMontage* Montage)
{
	MulticastSetCableVisibility(VisibilityValue, AttachmentValue);
	MulticastPlayAnimMontage(Montage);
}

bool AShellsCharacter::ServerBeginInsanityAttack_Validate(bool VisibilityValue, bool AttachmentValue, UAnimMontage* Montage)
{
	return true;
}
void AShellsCharacter::ServerEndInsanityAttack_Implementation(bool VisibilityValue, bool AttachmentValue)
{
	MulticastSetCableVisibility(VisibilityValue, AttachmentValue);
	MulticastPlayAnimMontage(InsanityMontage, 1, FName("InsanityEnd"));
}

bool AShellsCharacter::ServerEndInsanityAttack_Validate(bool VisibilityValue, bool AttachmentValue)
{
	return true;
}

void AShellsCharacter::ServerJumpAttack_Implementation()
{
	MulticastPlayAnimMontage(JumpMontage);
}
bool AShellsCharacter::ServerJumpAttack_Validate()
{
	return true;
}

void AShellsCharacter::ServerPrepareJumpAttack_Implementation()
{
	State = ActionType::Jump;
}
bool AShellsCharacter::ServerPrepareJumpAttack_Validate()
{
	return true;
}

void AShellsCharacter::ServerLandAttack_Implementation()
{
	GetController()->SetIgnoreLookInput(false);
	GetController()->SetIgnoreMoveInput(false);
	MulticastPlayAnimMontage(LandMontage);
}
bool AShellsCharacter::ServerLandAttack_Validate()
{
	return true;
}

void AShellsCharacter::PlaySwipeCameraShake()
{
	if (SwipeAttackCameraShake != NULL)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(SwipeAttackCameraShake, 1.0f);
	}
}

void AShellsCharacter::EnableClawCollision()
{
	SwipeCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwipeCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	UE_LOG(LogTemp, Warning, TEXT("Enabled"));
}

void AShellsCharacter::DisableClawCollision()
{
	SwipeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwipeCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	UE_LOG(LogTemp, Warning, TEXT("Disabled"));
}

void AShellsCharacter::SetStateIdle()
{ 
	State = ActionType::Idle;
	GetCharacterMovement()->MaxCustomMovementSpeed = 600;
}

void AShellsCharacter::CheckIfSwipeHit()
{
	if (SuccessfulSwipe)
	{
		SuccessfulSwipe = false;
	}
	else
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, .75f, 1.0f, TeleportFadeColor);
		MulticastPlayAnimMontage(RecoveryMontage);
	}
}

void AShellsCharacter::CheckExplorersInRange(float Radius, float HalfHeight, TArray<AActor*> &ExplorersFound)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> Actors;
	Actors.Add(EObjectTypeQuery::ObjectTypeQuery_MAX);
	TArray<AActor*> IgnoredSelf;
	IgnoredSelf.Add(this);
	TArray<AActor*> HitActors;

	if (UKismetSystemLibrary::CapsuleOverlapActors(GetWorld(), GetActorLocation(), Radius, HalfHeight, Actors, AExplorerCharacter::StaticClass(), IgnoredSelf, HitActors))
		ExplorersFound = HitActors;
}

bool AShellsCharacter::IsExplorerInSight(AActor* ExplorerToCheck)
{
	TEnumAsByte<ETraceTypeQuery> Actor = ETraceTypeQuery::TraceTypeQuery_MAX;
	TArray<AActor*> IgnoredSelf;
	IgnoredSelf.Add(this);
	FHitResult f(ForceInit);
	bool IsExplorerInSight = UKismetSystemLibrary::LineTraceSingle (GetWorld(), GetActorLocation(), TargetExplorer->GetActorLocation(), Actor, false, IgnoredSelf, EDrawDebugTrace::None, f, true);
	if (ExplorerToCheck == f.Actor) return true;

	return false;
}

void AShellsCharacter::CheckVisibilityForInsanity(float DeltaTime)
{
	TArray<AActor*> HitActors;
	CheckExplorersInRange(2000, 20000, HitActors);
	if (HitActors.IsValidIndex(0))
	{
		bool FoundExplorer = false;
		TargetExplorer = NULL;
		float Dist = 20000;
		for (auto a : HitActors)
		{
			float CurrentDistance = FVector::Dist(GetActorLocation(), a->GetActorLocation());
			if (CurrentDistance < Dist)
			{
				Dist = CurrentDistance;
				TargetExplorer = a;
			}
		}

		FVector ActorDirection = TargetExplorer->GetActorLocation() - GetActorLocation();
		ActorDirection.Normalize();
		float d = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ActorDirection));
		if (d < 1)
		{
			if (IsExplorerInSight(TargetExplorer))
			{
				FoundExplorer = true;
				auto Explorer = Cast<AExplorerCharacter>(TargetExplorer);
				Explorer->GetCharacterMovement()->MaxWalkSpeed = ExplorerInsanitySpeed;
				ExplorerInsanitySpeed -= (120 * DeltaTime);
				Explorer->InsanityTimerMax -= DeltaTime;
				if (Explorer->InsanityTimerMax < 0)
				{
					ServerPlayExplorerInsane(Explorer);
					InsanityReleased();
					UGameplayStatics::PlaySoundAtLocation(this, SuccessInsanitySound, GetActorLocation(), 1, InsanitySoundPitch);
				}

				if (!PlayingDazzedEmitter)
				{
					PlayingDazzedEmitter = true;
					MulticastInsanityDazzedEmitter(Explorer);
					ServerBeginInsanityAttack(true, true, InsanityMontage);
					UpdateHeadRotation(true);
				}
			}
		}
		if (!FoundExplorer)
			InsanityReleased();
	}
	else
	{
		ExplorerInsanitySpeed = 600;
		InsanityReleased();
	}
}

void AShellsCharacter::ServerPlayExplorerInsane_Implementation(AExplorerCharacter* E)
{
	E->MulticastPlayInsanityMontage();
}
bool AShellsCharacter::ServerPlayExplorerInsane_Validate(AExplorerCharacter* E)
{
	return true;
}

void AShellsCharacter::OnClawOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		AExplorerCharacter* Explorer = Cast<AExplorerCharacter>(OtherActor);
		if (Explorer != NULL && !Explorer->IsInsane)
		{
			// TODO - Call do damage here
			SuccessfulSwipe = true;
			Explorer->PlayHurtMontage();
		}
	}
}

void AShellsCharacter::OnMotionControllerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsPreparingSwipeVR && OtherComp == VRSwipeTarget)
	{
		IsPreparingSwipeVR = false;
		IsSwippingVR = true;

		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, .8f, .5f, FColor::Black, false, true);
		FTimerHandle HandleSwipeTimer;
		GetWorldTimerManager().SetTimer(HandleSwipeTimer, this, &AShellsCharacter::HandleSuccessfulSwipe, .5f);

		VRSwipeParticle->Activate();
	}
	else if (IsSwippingVR)
	{
		auto E = Cast<AExplorerCharacter>(OtherActor);
		if (E)
		{
			SwipeCancelVR();
			GetWorldTimerManager().ClearTimer(VRSwipeHandle);
			E->PlayHurtMontage();
		}
	}

}

void AShellsCharacter::HandleSuccessfulSwipe()
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.8f, 0, 1, FColor::Black);

	FTimerHandle ResetSwipeHandle;
	GetWorldTimerManager().SetTimer(ResetSwipeHandle, this, &AShellsCharacter::ResetSwipeWindow, 1);
}

void AShellsCharacter::ResetSwipeWindow()
{
	IsSwippingVR = false;

}

// Jump Attack //
void AShellsCharacter::PrepareJump()
{
	bCanUpdateSpline = false;
	bCanClearArc = false;
	bIsTeleporterActive = false;
	FirstPersonCameraComponent->SetPostProcessBlendWeight(1);
	//DisableInput(PlayerController);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 0.65f, FadeOutDuration, TeleportFadeColor);
}

void AShellsCharacter::StartJump()
{
	bStartedJumping = true;
	CurrentJumpDistance = 0;
	SetJumpEndPoint();

	ServerFallingIdle();
}

float AShellsCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0;
}

// Checkout --
// this->CallFunctionByNameWithArguments(TEXT("AddDestructible"), Ar, NULL, true);

void AShellsCharacter::DealDamage(int Damage)
{
	ServerDealDamage(Damage);
	if (Health <= 0)
	{
		auto PlayerState = Cast<AMainPlayerState>(GetPlayerState());
		if (PlayerState)
		{
			PlayerState->AddPlayerBeastDeath();

		}

		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0, 1, .5f, FColor::White, true);
		ServerToggleMobility(false);
		FirstPersonCameraComponent->bUsePawnControlRotation = false;
		MulticastPlayAnimMontage(DyingMontage);
		FTimerHandle DyingHandle;
		FTimerHandle FadeOutHandle;

		// Timers
		GetWorldTimerManager().SetTimer(FadeOutHandle, this, &AShellsCharacter::FadeOutScreen, .55f);
		GetWorldTimerManager().SetTimer(DissolveHandle, this, &AShellsCharacter::DissolveMaterial, 0.05f, true);
		GetWorldTimerManager().SetTimer(DyingHandle, this, &AShellsCharacter::StopAnim, 2);
	}
}

void AShellsCharacter::ServerDealDamage_Implementation(int Damage)
{
	if (Health > 0)
	{
		Health -= Damage;
	}
}
bool AShellsCharacter::ServerDealDamage_Validate(int Damage)
{
	if (Damage > 30)
	{
		return false;
	}
	return true;
}

void AShellsCharacter::DissolveMaterial()
{
	MulticastDissolveChanges();
	if (InitialDissolveValue < -1.0f)
	{
		GetWorldTimerManager().ClearTimer(DissolveHandle);
		InitialDissolveValue = 1.5f;
	}
}

void AShellsCharacter::MulticastDissolveChanges_Implementation(bool IsDefault = false)
{
	if (!IsDefault)
	{
		InitialDissolveValue -= (GetWorld()->GetDeltaSeconds() * 1.5f);
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, FName("Dissolve"), InitialDissolveValue);
	}
	else
	{
		InitialDissolveValue = 1.5f;
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, FName("Dissolve"), InitialDissolveValue);
	}
}
bool AShellsCharacter::MulticastDissolveChanges_Validate(bool IsDefault = false)
{
	return true;
}

void AShellsCharacter::MulticastSetCableMaterial_Implementation(UMaterialInstance* Mat)
{
	InsanityCable->SetMaterial(0, Mat);
}

void AShellsCharacter::FadeOutScreen()
{
	auto GI = Cast<ULobbyGameInstance>(GetGameInstance());
	if (GI != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found GI"));
		if (IsLocallyControlled() && GI->DeathScreenClass != nullptr)
		{
			DeathScreen = CreateWidget<UMenuWidget>(GI, GI->DeathScreenClass);
			DeathScreen->Setup();
			DeathScreen->AddToViewport();
		
			auto DSClass = Cast<URespawnScreen>(DeathScreen);
			if (DSClass != nullptr)
				DeathScreen->PlayAnimation(DSClass->FadeIn);
		}
	}

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 1.0f, 4.0f, TeleportFadeColor, false, true);
}

void AShellsCharacter::StopAnim()
{
	MulticastStopAnim();
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AShellsCharacter::Respawn, 6);
}
void AShellsCharacter::MulticastStopAnim_Implementation()
{
	UBeastAnimInstance* AnimInstance = Cast<UBeastAnimInstance>(GetMesh()->GetAnimInstance());
	if (!ensure(AnimInstance)) return;
	AnimInstance->Montage_Pause(DyingMontage);
}
bool AShellsCharacter::MulticastStopAnim_Validate()
{
	return true;
}

void AShellsCharacter::Respawn()
{
	DeathScreen->Teardown();

	TArray<AActor*> Portals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APortalBox::StaticClass(), Portals);
	int RandomPortalNumber = UKismetMathLibrary::RandomIntegerInRange(0, 2);
	SetActorLocation(Portals[RandomPortalNumber]->GetActorLocation());
	MulticastDissolveChanges(true);
	DeathPostProcessComponent->BlendWeight = 0;
	
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1.0f, 0.0f, 2.5f, TeleportFadeColor, false, true);
	MulticastPlayAnimMontage(GettingUpMontage);
}

void AShellsCharacter::ServerFallingIdle_Implementation()
{
	MulticastPlayAnimMontage(FallingMontage);
}

bool AShellsCharacter::ServerFallingIdle_Validate()
{
	return true;
}

void AShellsCharacter::UpdateJumpPosition(float DeltaTime)
{
	CurrentJumpDistance += DeltaTime * JumpSpeed;

	if (CurrentJumpDistance > JumpEndPoint)
	{
		bCanClearArc = true;
		bIsTeleporterActive = true;
		bCanUpdateSpline = true;
		TeleportReleased();
		bStartedJumping = false;

		MulticastPlayLandEmitter();

		ServerLandAttack();
	}
	else
	{
		FHitResult HitActor;
		
		FVector TmpLocation = ArcSpline->GetLocationAtDistanceAlongSpline(CurrentJumpDistance, ESplineCoordinateSpace::Local);
		FVector NewLocation = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()/2) + TmpLocation;
		FRotator NewRotation = ArcSpline->GetRotationAtDistanceAlongSpline(CurrentJumpDistance, ESplineCoordinateSpace::Local);
		FVector InterLocation = UKismetMathLibrary::VInterpTo
		(GetActorLocation(),
			NewLocation,
			DeltaTime,
			JumpInterpSpeed);
		FRotator InterRotation = UKismetMathLibrary::RInterpTo
		(GetActorRotation(),
			NewRotation,
			DeltaTime,
			JumpInterpSpeed);

		// DO NOT USE THIS IN VR? -- VERY EASY TO EXPERIENCE MOTION SICKNESS BY SETTING ROTATION OF THE CAMERA
		if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			FirstPersonCameraComponent->bUsePawnControlRotation = false;
			FirstPersonCameraComponent->SetWorldRotation(InterRotation.Quaternion());
		}

		ServerUpdateJumpPosition(InterLocation, NewRotation);
	}
}

void AShellsCharacter::ServerUpdateJumpPosition_Implementation(FVector Position, FRotator Rotation)
{
	SetActorLocation(Position);
}

bool AShellsCharacter::ServerUpdateJumpPosition_Validate(FVector Position, FRotator Rotation)
{
	return true;
}

void AShellsCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);


	AExplorerCharacter* ExplorerCharacter = Cast<AExplorerCharacter>(OtherActor);
	if (ExplorerCharacter != nullptr && State == ActionType::Jump && !ExplorerCharacter->IsInsane)
	{
		// TODO - Call add damage here

		GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, TEXT("Overlapped with explorer"));
		ExplorerCharacter->PlayHurtMontage();
	}
}

void AShellsCharacter::FinishJump()
{
	bCanClearArc = true;
	bIsTeleporterActive = true;
	bCanUpdateSpline = true;
	TeleportReleased();
	bStartedJumping = false;

	ServerLandAttack();
}

void AShellsCharacter::OnRep_bStartedJumping()
{
	if (!bStartedJumping)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(FinishJumpCameraShake, 1.0f);
		FirstPersonCameraComponent->PostProcessSettings.MotionBlurAmount = 0;
		// Check later
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		UE_LOG(LogTemp, Warning, TEXT("Started jumping false"));
	}
	else
	{
		FirstPersonCameraComponent->PostProcessSettings.MotionBlurAmount = 3;
		UE_LOG(LogTemp, Warning, TEXT("Started jumping TRUE"));

	}
}

bool AShellsCharacter::TraceTeleportDestination(TArray<FVector>& TracePoints, FVector& NavMeshLocation, FVector& TraceLocation)
{
	FVector StartPos;
	FVector LaunchVelocity;
	//if (RightHandController != nullptr) {
	//	StartPos = RightHandController->GetActorLocation();
	//	LaunchVelocity = RightHandController->GetActorForwardVector() * TeleportLaunchVelocity;
	//}
	//else {
		StartPos = ArcDirection->GetComponentLocation();
		LaunchVelocity = ArcDirection->GetForwardVector() * TeleportLaunchVelocity;

	//}

	
	FPredictProjectilePathParams PredictParams(0.0f, StartPos, LaunchVelocity, 2.0f, UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	FPredictProjectilePathResult PredictResult;

	// Simulate throwing a projectile to find a teleport location.
	const bool bSuccessPredictProjectilePath = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	
	if (!bSuccessPredictProjectilePath)
	{
		return false;
	}

	UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FVector Point = PredictResult.HitResult.Location;

	FNavLocation ProjectedLocation;
	FVector ProjectNavExtends = FVector(500.0f, 500.0f, 500.0f);

	// Project our hit against the NavMesh to find a place for player to stand.
	const bool bSuccessProjectToNav = NaviSystem->ProjectPointToNavigation(Point, ProjectedLocation, ProjectNavExtends);

	if (!bSuccessProjectToNav) {
		return false;
	}

	TracePoints.Empty();

	for (FPredictProjectilePathPointData PathPositions : PredictResult.PathData) {
		TracePoints.Push(PathPositions.Location); // TracePoints.Add(PathPositions.Location);
	}

	TraceLocation = Point;
	NavMeshLocation = ProjectedLocation.Location;

	return true;
}

void AShellsCharacter::ClearArc()
{
	for (USplineMeshComponent* MySpline : SplineMeshes)
	{
		MySpline->DestroyComponent();
	}

	SplineMeshes.Empty();
	ArcSpline->ClearSplinePoints();
}

void AShellsCharacter::UpdateArcSpline(bool FoundValidLocation, TArray<FVector> SplinePoints)
{
	// Create Small Stub line when we failed to find a teleport location
	if (!FoundValidLocation)
	{
		FVector ArcLocation = ArcDirection->GetComponentLocation();
		FVector ArcForward = ArcDirection->GetForwardVector();

		SplinePoints.Empty();
		SplinePoints.Add(ArcLocation);

		FVector NextVector = ArcLocation + (ArcForward * 40.0f);
		SplinePoints.Add(NextVector);
	}

	for (FVector Points : SplinePoints)
	{
		// Build a spline from all trace points. This generates tangents we can use to build a smoothly curved spline mesh
		ArcSpline->AddSplinePoint(Points, ESplineCoordinateSpace::Local);
	}

	// Update the point type to create the curve
	ArcSpline->SetSplinePointType(SplinePoints.Num() - 1, ESplinePointType::CurveClamped);

	FVector StartPos;
	FVector StartTangent;
	FVector EndPos;
	FVector EndTangent;

	for (int i = 0; i <= ArcSpline->GetNumberOfSplinePoints() - 2; i++)
	{
		// Add new cylinder mesh
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), FName(*FString::Printf(TEXT("MySpline %d"), i)));
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->SetStaticMesh(BeamMesh);
		SplineMeshComponent->SetMaterial(0, BeamMaterial);
		SplineMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SplineMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		SplineMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::X);
		SplineMeshComponent->SetStartScale(FVector2D(4.0f, 4.0f));
		SplineMeshComponent->SetEndScale(FVector2D(4.0f, 4.0f));
		SplineMeshComponent->SetBoundaryMax(1.0f);
		SplineMeshComponent->RegisterComponent();
		FString ss = SplineMeshComponent->GetName();

		SplineMeshes.Add(SplineMeshComponent);

		StartPos = SplinePoints[i];
		StartTangent = ArcSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		EndPos = SplinePoints[i + 1];

		EndTangent = ArcSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		// Set the tangents and position to build slightly bend the cylinder. All cylinders combined create a smooth arc.
		SplineMeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	}
}

void AShellsCharacter::UpdateArcEndPoint(bool ValidLocationFound, FVector NewLocation)
{
	bool bIsArcEndPointVisible = ValidLocationFound && bIsTeleporterActive;

	ArcEndPoint->SetVisibility(bIsArcEndPointVisible);
	ArcEndPoint->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

	//FRotator ArrowRotation = UKismetMathLibrary::ComposeRotators(TeleportRotation, FRotator(0.0f, DeviceRotation.Yaw, 0.0f)); // = TeleportRotation + FRotator(0.0f, DeviceRotation.Yaw, 0.0f);
	//Arrow->SetWorldRotation(ArrowRotation);
}

void AShellsCharacter::GetTeleportDestination(FVector& MyLocation, FRotator& MyRotation)
{
	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

	//  Ignore relative Height difference
	FVector RotateResult = TeleportRotation.RotateVector(FVector(DevicePosition.X, DevicePosition.Y, 0.0f));

	MyLocation = TeleportCylinder->GetComponentLocation() - RotateResult;
	MyRotation = TeleportRotation;
}

void AShellsCharacter::SetJumpEndPoint()
{
	JumpEndPoint = ArcSpline->GetSplineLength() - (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2);
}

void AShellsCharacter::ExecuteTeleportation()
{
	if (!bIsTeleporting)
	{
		if (GetIsValidTeleportDestination())
		{
			bIsTeleporting = true;
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 1.0f, FadeOutDuration, TeleportFadeColor, false, true);
			//FTimerDelegate FadeDelegate;// = FTimerDelegate::CreateUObject(this, &ATP_VirtualRealityPawn_Motion::TeleportActor, MotionController);
			//FadeDelegate.BindUFunction(this, FName("TeleportActor"), MotionController);
			//GetWorldTimerManager().SetTimer(FadeTimerHandle, FadeDelegate, FadeOutDuration, false);
			TeleportActor();
		}
		else
		{
			DisableTeleporter();
		}
	}
}

void AShellsCharacter::TeleportPressed()
{
	if (State == ActionType::Idle)
	{
		State = ActionType::Jump;
		ActivateTeleporter();
	}
}

void AShellsCharacter::TeleportReleased()
{
	ExecuteTeleportation();
}

void AShellsCharacter::MulticastPlayAnimMontage_Implementation(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

bool AShellsCharacter::MulticastPlayAnimMontage_Validate(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	if (AnimMontage != NULL)
		return true;
	return false;
}

void AShellsCharacter::MulticastStopAnimMontage_Implementation(UAnimMontage* AnimMontage, ACharacter* Target)
{
	Target->StopAnimMontage(AnimMontage);
}

bool AShellsCharacter::MulticastStopAnimMontage_Validate(UAnimMontage* AnimMontage, ACharacter* Target)
{
	if (AnimMontage != NULL)
		return true;
	return false;
}

void AShellsCharacter::MulticastInsanityDazzedEmitter_Implementation(AExplorerCharacter* Explorer, bool Destroy = false)
{
	if (!Destroy)
	{
		//FVector Location = Explorer->GetMesh()->GetSocketLocation(FName )
		DazzedComponent->AttachToComponent(Explorer->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Head"));
		DazzedComponent->SetVisibility(true);
		DazzedComponent->Activate();
		//DazzedComponent->ActivateSystem()
	}
	else
	{
		DazzedComponent->ResetToDefaults();
		DazzedComponent->SetVisibility(false);
	}
		
}
bool AShellsCharacter::MulticastInsanityDazzedEmitter_Validate(AExplorerCharacter* Explorer, bool Destroy = false)
{
	return true;
}

void AShellsCharacter::MulticastPlayLandEmitter_Implementation()
{
	FVector ActorLocation = GetActorLocation();
	FVector SpawnLocation = FVector(ActorLocation.X, ActorLocation.Y, ActorLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LandEffect, SpawnLocation, FRotator::ZeroRotator, FVector(1), true, EPSCPoolMethod::AutoRelease);
}

bool AShellsCharacter::MulticastPlayLandEmitter_Validate()
{
	return true;
}

void AShellsCharacter::MulticastSetCableVisibility_Implementation(bool VisibilityValue, bool AttachmentValue)
{
	if (AttachmentValue)
		ServerSetCableProperties();
		//InsanityCable->SetAttachEndTo(TargetExplorer, FName("Mesh"), FName("Head"));
	else
	{
		InsanityCable->SetVisibility(false);
		if (TargetExplorer)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InsanityBurst,
				TargetExplorer->GetActorLocation() + GetActorUpVector() * 50,
				FRotator::ZeroRotator, FVector(0.5f), true, EPSCPoolMethod::AutoRelease);
		}
	}

	InsanityCable->SetVisibility(VisibilityValue);
}
bool AShellsCharacter::MulticastSetCableVisibility_Validate(bool VisibilityValue, bool AttachmentValue)
{
	return true;
}

void AShellsCharacter::ServerSetCableProperties_Implementation()
{
	InsanityCable->SetAttachEndTo(TargetExplorer, FName("Mesh"), FName("Head"));
	InsanityCable->SetVisibility(true);

}
bool AShellsCharacter::ServerSetCableProperties_Validate()
{
	return true;
}

void AShellsCharacter::JumpReleased()
{
	if (bIsValidTeleportDestination && State == ActionType::Jump)
	{
		ServerJumpAttack();
		int32 LastSplinePoint = ArcSpline->GetNumberOfSplinePoints() - 1;
		//UGameplayStatics::SpawnDecalAtLocation(GetWorld(), JumpDecalMaterial, FVector(256, 256, 256),
			//ArcSpline->GetLocationAtSplinePoint(LastSplinePoint, FRotator::ZeroRotator, ESplineCoordinateSpace::Local, ));
		//\->SetWorldLocation(ArcSpline->GetLocationAtSplinePoint(LastSplinePoint, ESplineCoordinateSpace::Local));
		GetController()->SetIgnoreLookInput(true);
		GetController()->SetIgnoreMoveInput(true);
	}
	else if (!bIsValidTeleportDestination)
	{
		State = ActionType::Idle;	
		DisableTeleporter();
	}
}

void AShellsCharacter::TeleportActor()
{
	DisableTeleporter();

	FVector DestLocation;
	FRotator DestRotation;
	GetTeleportDestination(DestLocation, DestRotation);

	TeleportTo(DestLocation, DestRotation);

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1.0f, 0.0f, FadeOutDuration, TeleportFadeColor, false, false);

	bIsTeleporting = false;
	GetWorldTimerManager().ClearTimer(FadeTimerHandle);
}

// TODO: Rename to ShowJumpSpline or something
void AShellsCharacter::ActivateTeleporter()
{
	bIsTeleporterActive = true;
}

// TODO: Rename to HideJumpSpline or something
void AShellsCharacter::DisableTeleporter()
{
	bIsTeleporterActive = false;
	TeleportCylinder->SetVisibility(false, true);
	ArcEndPoint->SetVisibility(false);
}

void AShellsCharacter::SetTeleportRotation(FRotator& NewTeleportRotation)
{
	TeleportRotation = NewTeleportRotation;
}

// Insanity Attack //
void AShellsCharacter::InsanityTimelineCallback(float InterpolatedValue)
{
	FirstPersonCameraComponent->SetPostProcessBlendWeight(InterpolatedValue);
	PostProcessComponent->BlendWeight = InterpolatedValue;
	MulticastUpdateInsanityCollection(InterpolatedValue);

	auto E = Cast<AExplorerCharacter>(TargetExplorer);
	if (E) E->GetCameraComponent()->SetPostProcessBlendWeight(InterpolatedValue);
}

void AShellsCharacter::MulticastUpdateInsanityCollection_Implementation(float InterpolatedValue)
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), InsanityParameterCollection, FName("Amount"), InterpolatedValue);
}

void AShellsCharacter::PlayInsanitySound()
{
	InsanityBuzzLoopAudio->Play();
}

void AShellsCharacter::InsanityPressed()
{
	if (State == ActionType::Idle)
	{
		TArray<AActor*> HitActors;
		CheckExplorersInRange(2000, 20000, HitActors);
		if (!HitActors.IsValidIndex(0)) return;
		TargetExplorer = NULL;
		float Dist = 20000;
		for (auto a : HitActors)
		{
			float CurrentDistance = FVector::Dist(GetActorLocation(), a->GetActorLocation());
			if (CurrentDistance < Dist)
			{
				Dist = CurrentDistance;
				TargetExplorer = a;
			}
		}
		if (TargetExplorer == NULL) return;

		FVector ActorDirection = TargetExplorer->GetActorLocation() - GetActorLocation();
		ActorDirection.Normalize();
		float d = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ActorDirection));
		auto Explorer = Cast<AExplorerCharacter>(TargetExplorer);
		if (d > 1 || Explorer->IsInsane) return;
		
		if (IsExplorerInSight(TargetExplorer))
		{
			State = ActionType::Insanity;
			bChannelling = true;
			MulticastSetCableMaterial(InsanityCableMaterial);
			if (!ensure(InsanityTimeline)) return;
			InsanityTimeline->SetPlayRate(0.4f);
			InsanityTimeline->PlayFromStart();
			FirstPersonCameraComponent->bUsePawnControlRotation = false;
			bUseControllerRotationYaw = false;
			ServerToggleMobility(false);
			auto E = Cast<AExplorerCharacter>(TargetExplorer);
			if (E)
			{
				E->InsanityTimerMax = 5;
				//E->DazzedComponent->SetVisibility(true);
				GetWorldTimerManager().SetTimer(PlayInsanitySoundHandle, this, &AShellsCharacter::PlayInsanitySound, FirstDelayTime);
			}
			PlayingDazzedEmitter = false;
		}
	}
}

void AShellsCharacter::InsanityReleased()
{
	if (State == ActionType::Insanity)
	{
		State = ActionType::Idle;

		bChannelling = false;
		InsanityTimeline->SetPlayRate(2.5f);
		InsanityTimeline->Reverse();
		InsanityCable->SetVisibility(false);

		GetWorldTimerManager().ClearTimer(PlayInsanitySoundHandle);
		InsanityBuzzLoopAudio->Stop();	
	
		if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
			FirstPersonCameraComponent->bUsePawnControlRotation = true;
		
		bUseControllerRotationYaw = true;

		UpdateHeadRotation(true);

		ServerToggleMobility(true);

		ServerEndInsanityAttack(false, false);
		MulticastInsanityDazzedEmitter(NULL, true);
	}
}

// Box Teleport // -- Change name?
void AShellsCharacter::OnOverlapPortal(FVector portalLocation, bool onPortal)
{
	PortalLocation = portalLocation;
	OnPortal = onPortal;
}

void AShellsCharacter::EnableBodyPhysics()
{
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	VRInvisibleMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	VRInvisibleMesh->SetAllBodiesSimulatePhysics(true);

	FTimerHandle DisableBodyPhysicsHandle;
	GetWorldTimerManager().SetTimer(DisableBodyPhysicsHandle, this, &AShellsCharacter::DisableBodyPhysics, 7.5f);
}

void AShellsCharacter::DisableBodyPhysics()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	VRInvisibleMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	VRInvisibleMesh->SetAllBodiesSimulatePhysics(false);
	MulticastPlayAnimMontage(GettingUpMontage);
}

void AShellsCharacter::ServerTeleport_Implementation()
{

	TeleportTo(PortalLocation, GetActorRotation());

}

bool AShellsCharacter::ServerTeleport_Validate()
{
	return true;
}

void AShellsCharacter::ServerToggleMobility_Implementation(bool Value)
{
	MulticastToggleMobility(Value);
}
bool AShellsCharacter::ServerToggleMobility_Validate(bool Value)
{
	return true;
}

void AShellsCharacter::MulticastToggleMobility_Implementation(bool Value)
{
	if (GetController())
	{
		if (Value)
		{
			GetController()->ResetIgnoreLookInput();
			GetCharacterMovement()->MaxWalkSpeed = 600;
		}
		else
		{
			GetController()->SetIgnoreLookInput(true);
			GetCharacterMovement()->MaxWalkSpeed = 0;
		}

	}
}
bool AShellsCharacter::MulticastToggleMobility_Validate(bool Value)
{
	return true;
}

// NOTIFIES
void AShellsCharacter::EnableCollisions()
{
	if (HasAuthority())
	{
		GetCharacterMovement()->AddImpulse(GetCapsuleComponent()->GetForwardVector() * ImpulseForce, true);
	}
	//PlaySwipeCameraShake();
	EnableClawCollision();

	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		FirstPersonCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform);
}

void AShellsCharacter::DisableCollisions()
{
	DisableClawCollision();

	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		FirstPersonCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("Head"));
		FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 20));
	}
}

void AShellsCharacter::EnablePawnControl()
{
	//ChangeVR
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		FirstPersonCameraComponent->bUsePawnControlRotation = true;
		UE_LOG(LogTemp, Warning, TEXT("pawn control true"));
	}

	GetCharacterMovement()->MaxWalkSpeed = 600; 
	State = ActionType::Idle;
}

void AShellsCharacter::DisablePawnControl()
{
	FirstPersonCameraComponent->bUsePawnControlRotation = false;
	SetActorRotation(FQuat::Identity);
}

// INPUT
void AShellsCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			AddMovementInput(FirstPersonCameraComponent->GetForwardVector(), Value);
			//const FRotator Rotation = Controller->GetControlRotation();
			//const FRotator YawRotation(0, Rotation.Yaw, 0);

			//// get forward vector
			//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			//AddMovementInput(Direction, Value);

		}
		else
		{
			AddMovementInput(GetActorForwardVector(), Value);
		}
	}
}

void AShellsCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			AddMovementInput(FirstPersonCameraComponent->GetRightVector(), Value);
			//// find out which way is right
			//const FRotator Rotation = Controller->GetControlRotation();
			//const FRotator YawRotation(0, Rotation.Yaw, 0);

			//// get right vector 
			//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			//// add movement in that direction
			//AddMovementInput(Direction, Value);
		}
		else
		{
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void AShellsCharacter::TurnAtRate(float Rate)
{
	float NewRate = UKismetMathLibrary::Clamp(Rate, -6, 6);
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		AddActorWorldRotation(FRotator(0, NewRate * GetWorld()->GetDeltaSeconds(), 0));
	}
	else
	{
		AddControllerYawInput(NewRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AShellsCharacter::LookUpAtRate(float Rate)
{	
	float NewRate = UKismetMathLibrary::Clamp(Rate, -6, 6);
	AddControllerPitchInput(NewRate * GetWorld()->GetDeltaSeconds());
}

void AShellsCharacter::Turn(float Rate)
{
	float NewRate = UKismetMathLibrary::Clamp(Rate, -6, 6);
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		AddActorWorldRotation(FRotator(0, NewRate * GetWorld()->GetDeltaSeconds(), 0));
	}
	else
	{
		AddControllerYawInput(NewRate * GetWorld()->GetDeltaSeconds());
	}
}

void AShellsCharacter::Look(float Rate)
{
	float NewRate = UKismetMathLibrary::Clamp(Rate, -6, 6);
	AddControllerPitchInput(NewRate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShellsCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AShellsCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AShellsCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

//void AShellsCharacter::OnConstruction(const FTransform & Transform)
//{
//	Super::OnConstruction(Transform);
//
//	MotionController->SetTrackingSource(Hand); // 4.19 Code
//}

void AShellsCharacter::UpdateHeadRotation(bool DefaultRotation)
{
	if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->IsHeadTrackingAllowed()) return;
	UBeastAnimInstance* AnimI = Cast<UBeastAnimInstance>(GetMesh()->GetAnimInstance());
	if (!DefaultRotation)
	{
		float CPitch = APawn::GetControlRotation().Pitch;
		float CYaw = APawn::GetControlRotation().Yaw;
		float AYaw = GetActorRotation().Yaw;
		HeadRotation = FRotator(0, CYaw - AYaw, 0 - CPitch);
		if (HasAuthority())
		{
			OnRep_HeadRotation();
		}
	}
	else
	{
		//HeadRotation = FRotator(0, 0, 0);
		FRotator DesiredCameraRotation = UKismetMathLibrary::FindLookAtRotation(FirstPersonCameraComponent->GetComponentLocation(), GetCapsuleComponent()->GetForwardVector());
		HeadRotation = DesiredCameraRotation;
		if (HasAuthority())
		{
			OnRep_HeadRotation();
		}
	}
}

void AShellsCharacter::OnRep_HeadRotation()
{
	UBeastAnimInstance* BeastAnimInstance = Cast<UBeastAnimInstance>(GetMesh()->GetAnimInstance());
	if (!ensure(BeastAnimInstance)) return;
	BeastAnimInstance->HeadRotation = HeadRotation;
}

void AShellsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShellsCharacter, InsanityCable);
	DOREPLIFETIME(AShellsCharacter, InsanityTimeline);
	DOREPLIFETIME(AShellsCharacter, State);
	DOREPLIFETIME(AShellsCharacter, HeadRotation);
	DOREPLIFETIME_CONDITION(AShellsCharacter, Health, COND_OwnerOnly);
}

