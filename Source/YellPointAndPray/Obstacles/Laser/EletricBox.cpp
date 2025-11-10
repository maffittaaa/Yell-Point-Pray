#include "Obstacles/Laser/EletricBox.h"

AEletricBox::AEletricBox()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	eletricBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Electric Box"));
	eletricBoxMesh->SetupAttachment(RootComponent);

}

void AEletricBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEletricBox::Interact_Implementation(AActor* Interactor) {
	for (ALaser* laser : lasers)
		laser->niagaraLaserImpact->SetActive(false);

	UE_LOG(LogTemp, Warning, TEXT("Laser is deactivated"));
}

void AEletricBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

