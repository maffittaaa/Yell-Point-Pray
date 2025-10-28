#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CesarClass/InventoryObserver.h"
#include "InventorySubject.generated.h"

// Unreal reflection wrapper
UINTERFACE(MinimalAPI, Blueprintable)
class UInventorySubject : public UInterface
{
	GENERATED_BODY()
};


class IInventorySubject
{
	GENERATED_BODY()

public:
	TArray<TScriptInterface<IInventoryObserver>> Observers;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void AddObserver(const TScriptInterface<IInventoryObserver>& Observer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void RemoveObserver(const TScriptInterface<IInventoryObserver>& Observer);

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void NotifyObservers(const FString& Name);
};