// Fill out your copyright notice in the Description page of Project Settings.


#include "WhiteBoard.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AWhiteBoard::AWhiteBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
}

// Called when the game starts or when spawned
void AWhiteBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWhiteBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWhiteBoard::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Show Board"));


	ACharacter* PlayerChar = Cast<ACharacter>(Interactor);
	APlayerController* Controller = Cast<APlayerController>(PlayerChar->GetController());


	Controller->bShowMouseCursor = true;
	Controller->bEnableClickEvents = true;
	Controller->bEnableMouseOverEvents = true;

	Controller->SetIgnoreLookInput(true);
	Controller->SetIgnoreMoveInput(true);

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Controller->SetInputMode(InputMode);


	if (PlayerChar)
	{
		PlayerChar->GetCharacterMovement()->DisableMovement();
	}
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
	if (PlayerChar)
	{
		PlayerChar->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

