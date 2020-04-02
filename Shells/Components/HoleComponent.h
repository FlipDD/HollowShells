
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHELLS_API UHoleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHoleComponent();

protected:
	void BeginPlay() override;

public:	
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
