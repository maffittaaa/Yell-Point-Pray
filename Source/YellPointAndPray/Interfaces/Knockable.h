#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Knockable.generated.h"

UINTERFACE(BlueprintType)
class UKnockable : public UInterface
{
	GENERATED_BODY()
};

class IKnockable {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Knocktion")
	void Knock(AActor* Interactor);
};
