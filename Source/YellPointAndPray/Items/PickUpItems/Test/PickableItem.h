#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>
#include "Interfaces/Interactable.h"
#include <string>
#include "PickableItem.generated.h"

using namespace std;

UCLASS()
class YELLPOINTANDPRAY_API APickableItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableItem();
	
	UPROPERTY()
	int ID = 0;

	FString Name = "Item";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void Interact_Implementation(AActor* Interactor) override;
};