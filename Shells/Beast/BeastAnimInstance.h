#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BeastAnimInstance.generated.h"

UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class SHELLS_API UBeastAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaTimeX) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsMoving;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsInAir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float RightwardSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float ForwardSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	FRotator HeadRotation;

	APawn *OwningPawn;
};
