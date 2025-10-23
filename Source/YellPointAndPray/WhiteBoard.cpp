#include "WhiteBoard.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"

AWhiteBoard::AWhiteBoard() {
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	
	dynamicMaterialInstance = nullptr;
}

void AWhiteBoard::BeginPlay() {
	Super::BeginPlay();

	dynamicMaterialInstance->Parent = markerP1Material;
	renderTargetWhiteboard->ClearColor = FColor::Black;
}

void AWhiteBoard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AWhiteBoard::DrawOnWhiteboard(FVector2D* locationToDraw) {
	FLinearColor drawLocationColor = FLinearColor(locationToDraw->X, locationToDraw->Y, 0.0f, 1.0f);
	dynamicMaterialInstance->SetVectorParameterValue(FName("DrawLocation"), drawLocationColor);

UKismetRenderingLibrary::DrawMaterialToRenderTarget(
		GetWorld(),
		renderTargetWhiteboard,
		dynamicMaterialInstance
		);
	
}

void AWhiteBoard::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Show Board"));

	ACharacter* PlayerChar = Cast<ACharacter>(Interactor);
	if (!PlayerChar) return;

	APlayerController* Controller = Cast<APlayerController>(PlayerChar->GetController());

	if (!Controller) return;

	Controller->bShowMouseCursor = true;
	Controller->bEnableClickEvents = true;
	Controller->bEnableMouseOverEvents = true;

	Controller->SetIgnoreLookInput(true);
	Controller->SetIgnoreMoveInput(true);

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr);
	Controller->SetInputMode(InputMode);


	if (PlayerChar)
		PlayerChar->GetCharacterMovement()->DisableMovement();
}

void AWhiteBoard::CloseBoard() {
	UE_LOG(LogTemp, Warning, TEXT("Close Board"));
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	Controller->bShowMouseCursor = false;
	Controller->bEnableClickEvents = false;
	Controller->bEnableMouseOverEvents = false;
	Controller->SetIgnoreLookInput(false);
	Controller->SetIgnoreMoveInput(false);

	FInputModeGameOnly InputMode;
	Controller->SetInputMode(InputMode);

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerChar) {
		PlayerChar->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

