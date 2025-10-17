// Copyright Epic Games, Inc. All Rights Reserved.

#include "YellPointAndPrayCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Interfaces/Interactable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "YellPointAndPray.h"

AYellPointAndPrayCharacter::AYellPointAndPrayCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	InventoryComponent = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
}

void AYellPointAndPrayCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AYellPointAndPrayCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::LookInput);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::Interact);
	}
	else
	{
		UE_LOG(LogYellPointAndPray, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AYellPointAndPrayCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AYellPointAndPrayCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AYellPointAndPrayCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AYellPointAndPrayCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AYellPointAndPrayCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AYellPointAndPrayCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AYellPointAndPrayCharacter::ServerInteract_Implementation(AActor* hitObject, AYellPointAndPrayCharacter* character)
{
	if (!HasAuthority()) return;

	if (hitObject->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PickedUpItem! Inventory"));
		InventoryComponent->SetInventory(2);
		UE_LOG(LogTemp, Warning, TEXT("PickedUpItem! Interacting"));
		IInteractable::Execute_Interact(hitObject, character);
	}	
}

void AYellPointAndPrayCharacter::Interact() {
	UE_LOG(LogTemp, Warning, TEXT("YOU CALLED INTERACT"));

	//Get vector to do the ray
	FVector start;
	FRotator dir;
	GetController()->GetPlayerViewPoint(start, dir);

	FVector end = start + (dir.Vector() * 300);

	//Not hit player
	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	//ray

	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params)) {
		if (AActor* hitObject = hit.GetActor()) {
			this->ServerInteract(hitObject, this);
		}
	}
}