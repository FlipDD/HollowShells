#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandController.generated.h"

UENUM(BlueprintType)
enum class EGripState : uint8
{
	Open,
	CanGrab,
	Grab
};

UCLASS()
class SHELLS_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	AHandController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* GrabSphere;

	FRotator InitialControllerRotation;
	TArray<class USplineMeshComponent*> SplineMeshes;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		class UHapticFeedbackEffect_Base* HapticEffect;

	/** Is this the left or right hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		EControllerHand Hand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		FVector Extents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		bool WantsToGrip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		EGripState Grip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		class AActor* AttachedActor;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void RumbleController(float intensity);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void GrabActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void ReleaseActor();

	AActor* GetActorNearHand();
	void UpdateAnimationGripState();

	FRotator GetControllerRelativeRotation();
};
