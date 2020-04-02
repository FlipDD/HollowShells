#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ExplorerAnimInstance.generated.h"

UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class SHELLS_API UExplorerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaTimeX) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool IsMoving;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float RightwardSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float ForwardSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsSneaking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsCrawling;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsClimbing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsVaulting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsHanging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsAiming;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsShooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsDrilling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsRagdoll;

	APawn *OwningPawn;
};
