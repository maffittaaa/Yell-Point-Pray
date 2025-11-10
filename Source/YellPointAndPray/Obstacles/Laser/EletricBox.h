#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Obstacles/Laser/Laser.h"
#include "EletricBox.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AEletricBox : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AEletricBox();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* eletricBoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	TArray<AActor*> laserActors;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* interactor) override;

};
