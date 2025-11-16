#include "Obstacles/Laser/EletricBox.h"

#include "Kismet/GameplayStatics.h"

AEletricBox::AEletricBox() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	eletricBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Electric Box"));
	eletricBoxMesh->SetupAttachment(RootComponent);

	wiresMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wires"));
	wiresMesh->SetupAttachment(RootComponent);

	wiresMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wires 2"));
	wiresMesh2->SetupAttachment(RootComponent);

	wiresMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wires 3"));
	wiresMesh3->SetupAttachment(RootComponent);

	buttonsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Buttons"));
	buttonsMesh->SetupAttachment(RootComponent);

	buttonsMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Buttons 2"));
	buttonsMesh2->SetupAttachment(RootComponent);

	buttonsMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Buttons 3"));
	buttonsMesh3->SetupAttachment(RootComponent);

	buttonsMesh4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Buttons 4"));
	buttonsMesh4->SetupAttachment(RootComponent);

	buttonsMesh5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Buttons 5"));
	buttonsMesh5->SetupAttachment(RootComponent);

	leverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lever"));
	leverMesh->SetupAttachment(RootComponent);

	leverMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lever 2"));
	leverMesh2->SetupAttachment(RootComponent);

	cableHolderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable Holder"));
	cableHolderMesh->SetupAttachment(RootComponent);

	cableHolderMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable Holder 2"));
	cableHolderMesh2->SetupAttachment(RootComponent);

	stickers = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stickers"));
	stickers->SetupAttachment(RootComponent);
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

