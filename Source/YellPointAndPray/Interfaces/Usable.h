#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Usable.generated.h"

UINTERFACE(BlueprintType)
class UUsable : public UInterface
{
	GENERATED_BODY()
};

class IUsable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Userection")
	void Use(AActor* User);
};
