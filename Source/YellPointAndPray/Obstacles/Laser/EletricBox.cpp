#include "Obstacles/Laser/EletricBox.h"

#include "Kismet/GameplayStatics.h"

AEletricBox::AEletricBox() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	eletricBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Electric Box"));
	eletricBoxMesh->SetupAttachment(RootComponent);

}

void AEletricBox::BeginPlay() {
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALaser::StaticClass(), laserActors);
}

void AEletricBox::Interact_Implementation(AActor* Interactor) {
	if (!HasAuthority())
		return;
	
	for (AActor* laserActor : laserActors) {
		ALaser* laser = Cast<ALaser>(laserActor);

		if (laser) {
			laser->DeactivateLaserTemporarily(3.0f);
		} else
			UE_LOG(LogTemp, Error, TEXT("Failed to cast to ALaser: %s"), *laserActor->GetName());
	}
}

void AEletricBox::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

