#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Reset.generated.h"

UINTERFACE(BlueprintType)
	class YELLPOINTANDPRAY_API UReset : public UInterface
{
	GENERATED_BODY()
};

class IReset {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Reset")
	void Reset();
};