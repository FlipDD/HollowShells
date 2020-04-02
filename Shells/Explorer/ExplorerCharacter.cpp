#include "Explorer/ExplorerCharacter.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Beast/ShellsCharacter.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

#include "Kismet/KismetMathLibrary.h"

#include "MotionControllerComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HeadMountedDisplay.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "ExplorerAnimInstance.h"
#include "World/Pickup.h"
#include "World/ActionPickup.h"
#include "World/BeaconActor.h"
#include "MainPlayerState.h"

#include "Engine/Engine.h"

AExplorerCharacter::AExplorerCharacter()
{
	SetReplicates(true);
	SetReplicateMovement(true);

	// Set size for character collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Input turn rates
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	// Usual distance to the character	
	CameraBoom->TargetArmLength = 300.0f;
	// Rotate the arm based on the controller
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Camera does not rotate relative to arm
	FollowCamera->bUsePawnControlRotation = false;

	// Item Collection
	CollectionSphereRadius = 300.0f;
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(CollectionSphereRadius);

	// Default offset from the character location for projectiles to spawn
	LauncherOffset = FVector(100.0f, 0.0f, 10.0f);

	// Launcher
	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GetMesh(), FName("RightHand"));
	MuzzleLocation->SetRelativeLocation(FVector(0, 0, 0));

	LauncherComponent = CreateDefaultSubobject<ULauncherComponent>(TEXT("LauncherComponent"));
	LauncherComponent->SetupAttachment(RootComponent);

	// Drill
	DrillComponent = CreateDefaultSubobject<UDrillComponent>(TEXT("DrillComponent"));
	DrillComponent->SetupAttachment(RootComponent);

	// Power
	PowerComponent = CreateDefaultSubobject<UPowerComponent>(TEXT("PowerComponent"));
	PowerComponent->OnPowerChanged.AddDynamic(this, &AExplorerCharacter::OnPowerChanged);
	PowerComponent->SetupAttachment(RootComponent);

	DazzedComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DazzedParticleComponent"));
	DazzedComponent->SetIsReplicated(true);
	DazzedComponent->SetupAttachment(RootComponent);

	DestroyShellRangeComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DestroyShellRangeParticleComponent"));
	DestroyShellRangeComponent->SetIsReplicated(true);
	DestroyShellRangeComponent->SetupAttachment(RootComponent);

	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->SetTrackingSource(EControllerHand::Right);
	R_MotionController->SetupAttachment(RootComponent);

	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetTrackingSource(EControllerHand::Left);
	L_MotionController->SetupAttachment(RootComponent);

	DrillMesh = CreateDefaultSubobject<UStaticMeshComponent>("DrillMesh");
	DrillMesh->SetupAttachment(DrillComponent);
	DrillMesh->SetIsReplicated(true);

	DrillMesh->OnComponentBeginOverlap.AddDynamic(this, &AExplorerCharacter::OnDrillOverlapBegin);
	DrillMesh->OnComponentEndOverlap.AddDynamic(this, &AExplorerCharacter::OnDrillOverlapEnd);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	HandMesh->SetupAttachment(R_MotionController);
	HandMesh->SetCollisionProfileName(TEXT("NoCollision"));
	HandMesh->CastShadow = false;

	VRShootPS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VRShootPS"));
	VRShootPS->SetIsReplicated(true);
	VRShootPS->SetupAttachment(HandMesh);
	VRShootPS->bAutoActivate = false;

	VRDrillSparksPS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VRDrillSparksPS"));
	VRDrillSparksPS->SetIsReplicated(true);
	VRDrillSparksPS->SetupAttachment(DrillMesh);
	VRDrillSparksPS->bAutoActivate = false;

	VRDrillSmokePS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VRDrillSmokePS"));
	VRDrillSmokePS->SetIsReplicated(true);
	VRDrillSmokePS->SetupAttachment(DrillMesh);
	VRDrillSmokePS->bAutoActivate = false;

	VelocityPostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>("VelocityPostProcessComponent");

	DrillAudioLoopComponent = CreateDefaultSubobject<UAudioComponent>("DrillLoopSound");
	DrillAudioLoopComponent->SetupAttachment(RootComponent);
	DrillAudioLoopComponent->bAutoActivate = false;
}

void AExplorerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Movement
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AExplorerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AExplorerCharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AExplorerCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AExplorerCharacter::EndCrouch);

	PlayerInputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &AExplorerCharacter::ToggleCrouch);

	PlayerInputComponent->BindAxis("MoveForward", this, &AExplorerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AExplorerCharacter::MoveRight);

	// Handle devices that provide an absolute delta, such as a mouse.
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Handle devices that that provide a rate of change, such as an analog joystic k
	PlayerInputComponent->BindAxis("TurnRate", this, &AExplorerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AExplorerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AExplorerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AExplorerCharacter::StopFire);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AExplorerCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AExplorerCharacter::StopRun);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AExplorerCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AExplorerCharacter::EndZoom);

	// Collecting Pickups
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AExplorerCharacter::CollectPickups);
}

void AExplorerCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = FollowCamera->FieldOfView;

	if (BlinkerMaterialBase != nullptr)
	{
		BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
		VelocityPostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);

		BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), .2f);
	}

	
	IsDrilling = false;

	DrillAudioLoopComponent->AttachToComponent(DrillComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	FTimerHandle WaitHeadsetHandle;
	GetWorldTimerManager().SetTimer(WaitHeadsetHandle, this, &AExplorerCharacter::WaitForHeadset, 0.2f);
}

void AExplorerCharacter::WaitForHeadset()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		LauncherComponent->AttachToComponent(HandMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
		DrillComponent->AttachToComponent(L_MotionController, FAttachmentTransformRules::SnapToTargetIncludingScale);
		FollowCamera->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
	else
	{
		DrillComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LeftHand"));
		DrillComponent->AddRelativeLocation(FVector(10, 0, 0));
		//DrillMesh->SetVisibility(false);
		HandMesh->SetVisibility(false);
		VelocityPostProcessComponent->DestroyComponent();
	}
}

void AExplorerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	const auto NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	FollowCamera->SetFieldOfView(NewFOV);

	if (IsDrilling)
	{
		float RotationSpeed = DeltaTime * -500;
		//float RotationYaw = UKismetMathLibrary::FInterpTo(DrillMesh->GetComponentRotation().Yaw, DrillMesh->GetComponentRotation().Yaw + RotationSpeed, DeltaTime, DrillRotationInterp);
		//DrillMesh->SetRelativeRotation(FRotator(0, 0, RotationYaw));
		DrillMesh->AddLocalRotation(FRotator(0, 0, RotationSpeed));
	}

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (RadiusVsVelocity == nullptr) return;
		float Radius = RadiusVsVelocity->GetFloatValue(GetCharacterMovement()->Velocity.Size());
		if (BlinkerMaterialInstance != nullptr)
			BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), Radius / 3);
	}
}

void AExplorerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExplorerCharacter, CollectionSphereRadius);
	DOREPLIFETIME(AExplorerCharacter, bDied);
	DOREPLIFETIME(AExplorerCharacter, InsanityTimerMax);
	DOREPLIFETIME(AExplorerCharacter, DestroyShellTimer); 
}

void AExplorerCharacter::PlayHurtMontage() const
{
	if (HurtMontage != nullptr) {
		auto AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance->Montage_IsPlaying(HurtMontage)) {
			AnimInstance->Montage_Play(HurtMontage);
		}
	}
}

void AExplorerCharacter::MulticastPlayInsanityMontage_Implementation()
{
	IsInsane = true;

	if (GetController() != nullptr)
	{
		GetController()->SetIgnoreLookInput(true);
		GetController()->SetIgnoreMoveInput(true);
	}

	if (InsaneMontage != nullptr) {
		auto AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance->Montage_IsPlaying(InsaneMontage)) {
			AnimInstance->Montage_Play(InsaneMontage);
		}
	}
}

void AExplorerCharacter::MulticastPlayDestroyedShellMontage_Implementation()
{
	IsInsane = false;
	if (FallingMontage != nullptr) {
		auto AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance->Montage_IsPlaying(FallingMontage)) {
			AnimInstance->Montage_Play(FallingMontage);
		}
	}
}

void AExplorerCharacter::MulticastPlayAnimMontage_Implementation(class UAnimMontage* AnimMontage, float InPlayRate /* = 1 */, FName StartSectionName /* = NAME_None */)
{
	if (AnimMontage != nullptr) {
		auto AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance->Montage_IsPlaying(AnimMontage)) {
			AnimInstance->Montage_Play(AnimMontage);
		}
	}
}

void AExplorerCharacter::MulticastStopAnimMontage_Implementation(class UAnimMontage* AnimMontage)
{
	StopAnimMontage(AnimMontage);
}

void AExplorerCharacter::ServerSetInsanityMode_Implementation()
{
	MulticastPlayInsanityMontage();
}
bool AExplorerCharacter::ServerSetInsanityMode_Validate()
{
	return true;
}

void AExplorerCharacter::PlayDazzedEffect() const
{
	DazzedComponent->Activate(true);
}

void AExplorerCharacter::CollectPickups()
{
	ServerCollectPickups();
}

bool AExplorerCharacter::ServerCollectPickups_Validate()
{
	return true;
}

void AExplorerCharacter::ServerCollectPickups_Implementation()
{
	if (!HasAuthority())
		return;

	float TotalPower = 0.0f;

	TArray<AActor*> Found;
	CollectionSphere->GetOverlappingActors(Found);

	for (auto A : Found) {
		auto Pickup = Cast<APickup>(A);
		if (Pickup && !Pickup->IsPendingKill() && Pickup->IsActive()) {
			if (const auto Battery = Cast<AActionPickup>(Pickup)) {
				TotalPower += Battery->GetPower();
			}

			Pickup->PickedUpBy(this);
			Pickup->SetActive(false);
		}
	}
}

void AExplorerCharacter::OnPlayerDeath_Implementation()
{
	DetachFromControllerPendingDestroy();

	if (GetMesh()) {
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}

	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AExplorerCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			AddMovementInput(FollowCamera->GetForwardVector(), Value);
		}
		else
		{
			const FRotator Rotation = CameraBoom->GetTargetRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void AExplorerCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			AddMovementInput(FollowCamera->GetRightVector(), Value);
		}
		else
		{
			const FRotator Rotation = CameraBoom->GetTargetRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Value);
		}
	}
}

void AExplorerCharacter::TurnAtRate(float Rate)
{
	float NewRate = UKismetMathLibrary::Clamp(Rate, -6, 6);
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		AddActorWorldRotation(FRotator(0, NewRate * GetWorld()->GetDeltaSeconds(), 0));
	}
	else
	{
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AExplorerCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AExplorerCharacter::Jump()
{
	if (ClimbComponent != NULL) {
		if (ClimbComponent->bIsHanging) {
			ClimbComponent->ClimbLedge();
		}
	}
	else {
		Super::Jump();
		MulticastPlayAnimMontage(JumpMontage);
	}
}

void AExplorerCharacter::StopJumping()
{
	Super::StopJumping();
}

void AExplorerCharacter::BeginCrouch()
{
	Crouch();
	const auto Anim = Cast<UExplorerAnimInstance>(GetMesh()->GetAnimInstance());
	Anim->bIsSneaking = true;
}

void AExplorerCharacter::EndCrouch()
{
	UnCrouch();
	const auto Anim = Cast<UExplorerAnimInstance>(GetMesh()->GetAnimInstance());
	Anim->bIsSneaking = false;
}

void AExplorerCharacter::ToggleCrouch()
{
	const bool WasCrouched = bIsCrouched;
	WasCrouched ? UnCrouch() : Crouch();
	const auto Anim = Cast<UExplorerAnimInstance>(GetMesh()->GetAnimInstance());
	Anim->bIsSneaking = !WasCrouched;
}

void AExplorerCharacter::Fire() const
{
	if (FireSound) {
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	const auto SpawnRotation = GetActorRotation();
	/*const auto SpawnLocation = (MuzzleLocation ? MuzzleLocation->RelativeLocation : GetActorLocation());*/
	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 100) + (GetActorUpVector() * 50);

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		SpawnLocation = HandMesh->GetComponentLocation() + (HandMesh->GetForwardVector() * 150);
		VRShootPS->Activate(true);

		LauncherComponent->Fire(SpawnLocation, SpawnRotation, HandMesh);
	}
	else
	{
		/*if (FireAnimation) {
			auto AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance != nullptr) {
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}*/
		LauncherComponent->Fire(SpawnLocation, SpawnRotation, FollowCamera);
	}
}

void AExplorerCharacter::RumbleController()
{
	auto PC = Cast<APlayerController>(GetController());
	if (PC)
		PC->PlayHapticEffect(HapticEffect, HandL, 1);
}

void AExplorerCharacter::BeginZoom()
{
	bWantsToZoom = true;
	ServerSetWalkSpeed(0);
	GetCharacterMovement()->MaxWalkSpeed = 0;

	bWantsToGrip = true; 

	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		CameraBoom->TargetOffset = FVector(0, 0, 75);
		const auto Anim = Cast<UExplorerAnimInstance>(GetMesh()->GetAnimInstance());
		Anim->bIsAiming = true;
	}
}


void AExplorerCharacter::EndZoom()
{
	bWantsToZoom = false;
	ServerSetWalkSpeed(300);
	GetCharacterMovement()->MaxWalkSpeed = 300;

	bWantsToGrip = false;

	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		CameraBoom->TargetOffset = FVector(0, 0, 0);
		const auto Anim = Cast<UExplorerAnimInstance>(GetMesh()->GetAnimInstance());
		Anim->bIsAiming = false;
	}
}

void AExplorerCharacter::StartPlayingDrillSound()
{
	DrillAudioLoopComponent->Play();
}

void AExplorerCharacter::StartFire()
{
	
	if (bWantsToZoom) {
		Fire();
		MulticastPlayAnimMontage(FireAnimation, 1);

		auto PC = Cast<APlayerController>(GetController());
		if (PC)
			PC->PlayHapticEffect(HapticEffect, HandR, 1);
	}
	else {
		IsDrilling = true;
		VRDrillSmokePS->Activate(true);
		DrillComponent->StartDrilling();
		MulticastPlayAnimMontage(DrillingMontage);
		
		UGameplayStatics::PlaySoundAtLocation(this, DrillBeginSound, GetActorLocation());

		GetWorldTimerManager().SetTimer(StartDrillSoundHandle, this, &AExplorerCharacter::StartPlayingDrillSound, .5f);
		GetWorldTimerManager().SetTimer(HapticTimerHandle, this, &AExplorerCharacter::RumbleController, .7f, true, 0);
	}
}

void AExplorerCharacter::StopFire()
{
	if (bWantsToZoom) {
		LauncherComponent->StopFire();
	}
	else {
		IsDrilling = false;
		VRDrillSmokePS->Deactivate();
		VRDrillSparksPS->Deactivate();
		DrillComponent->StopDrilling();
		MulticastPlayAnimMontage(EndDrillingMontage);

		UGameplayStatics::PlaySoundAtLocation(this, DrillEndSound, GetActorLocation());
		
		DrillAudioLoopComponent->Stop();
		GetWorldTimerManager().ClearTimer(StartDrillSoundHandle);
		GetWorldTimerManager().ClearTimer(HapticTimerHandle);
	}
}

void AExplorerCharacter::StartRun()
{
	ServerSetWalkSpeed(600);
	GetCharacterMovement()->MaxWalkSpeed = 600;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 400;
	GetCharacterMovement()->JumpZVelocity = 600.f;
}

void AExplorerCharacter::StopRun()
{
	ServerSetWalkSpeed(300);
	GetCharacterMovement()->MaxWalkSpeed = 300;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
	GetCharacterMovement()->JumpZVelocity = 400.f;
}

void AExplorerCharacter::ServerSetWalkSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

bool AExplorerCharacter::ServerSetWalkSpeed_Validate(float Speed)
{
	return true;
}

void AExplorerCharacter::OnPowerChanged(UPowerComponent* OwningComponent, float Power, float Delta, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (PowerComponent && PowerComponent->GetPower() <= 0.0f && !bDied)
	{
		bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.0f);
	}
}

void AExplorerCharacter::AddDeath()
{
	AMainPlayerState* ThisPlayerState = Cast<AMainPlayerState>(GetPlayerState());
	ThisPlayerState->AddPlayerMechaDeath();
}

void AExplorerCharacter::OnDrillOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && IsDrilling)
	{
		FString s = OtherComp->GetName();
		UE_LOG(LogTemp, Warning, TEXT("COMP NAME IS : %s"), *s);
		
		auto Beacon = Cast<ABeaconActor>(OtherActor);
		auto BeaconComp = Cast<ABeaconActor>(OtherComp->GetOwner());
		if (Beacon != NULL || BeaconComp != NULL)
		{
			IsDrillOverlapping = true;
			VRDrillSparksPS->Activate(true);
			Beacon->ServerBeginDrilling();
		}
	}
}

void AExplorerCharacter::OnDrillOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		VRDrillSparksPS->Deactivate();
		
		auto Beacon = Cast<ABeaconActor>(OtherActor);
		if (Beacon)
		{
			IsDrillOverlapping = false;
			//Server here but not in the begin?
			if (IsDrilling)
			{
				UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 1.0f, .5f, FColor::Black);
				Beacon->ServerStopDrilling();
			}
		}
	}
}


FVector AExplorerCharacter::GetPawnViewLocation() const
{
	if (FollowCamera) {
		return FollowCamera->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}
