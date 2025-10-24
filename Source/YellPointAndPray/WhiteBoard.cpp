#include "WhiteBoard.h"
#include "YellPointAndPrayCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AWhiteBoard::AWhiteBoard() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetupAttachment(RootComponent);
}

void AWhiteBoard::BeginPlay() {
	Super::BeginPlay();
}

void AWhiteBoard::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Show Board"));

	AYellPointAndPrayCharacter* playerCharacter = Cast<AYellPointAndPrayCharacter>(Interactor);
	if (!playerCharacter) return;

	APlayerController* playerController = Cast<APlayerController>(playerCharacter->GetController());
	if (!playerController) return;
}

void AWhiteBoard::CloseBoard() {
	UE_LOG(LogTemp, Warning, TEXT("Close Board"));
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController) return;
}

void AWhiteBoard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

