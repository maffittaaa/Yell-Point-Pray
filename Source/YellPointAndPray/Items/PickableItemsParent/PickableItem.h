#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>
#include "Interfaces/Interactable.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "CesarClass/InventorySubject.h"
#include <string>
#include "PickableItem.generated.h"

using namespace std;

UCLASS()
class YELLPOINTANDPRAY_API APickableItem : public AActor, public IInteractable, public IInventorySubject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableItem();
	
	UPROPERTY()
	int ID = -1;

	FString Name = "DefaultItemName";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectToSpawn")
	UClass* Obj;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;


	//Cesar Stuff ----------------------------------------------------------------------

	UPROPERTY()
	TArray<TScriptInterface<IInventoryObserver>> Observers;

	virtual void AddObserver_Implementation(const TScriptInterface<IInventoryObserver>& Observer) override;

	virtual void RemoveObserver_Implementation(const TScriptInterface<IInventoryObserver>& Observer) override;

	virtual void NotifyObservers_Implementation(const FString& Name) override;


};