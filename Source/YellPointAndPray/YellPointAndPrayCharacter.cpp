// Copyright Epic Games, Inc. All Rights Reserved.

#include "YellPointAndPrayCharacter.h"
#include "Animation/AnimInstance.h"
#include "Items/PickUpItems/Test/PickableItem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Interfaces/Interactable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "YellPointAndPray.h"
#include "Blueprint/UserWidget.h"

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
		
		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::Duck);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AYellPointAndPrayCharacter::StopDuck);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::LookInput);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::Interact);

		//Use
		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::Use);
	}
	else
	{
		UE_LOG(LogYellPointAndPray, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AYellPointAndPrayCharacter::BeginPlay() {
	Super::BeginPlay();

	if (widgetClass)
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), widgetClass, FName("HUD"));
}

void AYellPointAndPrayCharacter::AddTrace() {
	float distance = 150.0f;
	FVector startTrace = GetActorLocation();
	FVector endTrace = startTrace + (GetActorForwardVector() * distance);

	ECollisionChannel traceChannel = ECC_Visibility;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);

	// DrawDebugLine(GetWorld(), startTrace, endTrace, FColor::Red, false, -1.0f, 0, 1.0f);

	bHit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		startTrace,
		endTrace,
		traceChannel,
		RV_TraceParams
	);
}

void AYellPointAndPrayCharacter::AddAndRemoveWidget() {
	AddTrace();
	UPrimitiveComponent* hitComponent = RV_Hit.GetComponent();
	if (bHit && hitComponent->ComponentHasTag(FName("CanInteract")) && !HUDWidget->IsInViewport())
		HUDWidget->AddToViewport();
	else if (!bHit || !hitComponent->ComponentHasTag(FName("CanInteract")))
		HUDWidget->RemoveFromParent();
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

void AYellPointAndPrayCharacter::Duck() {
	// signal the character to crouch
	ACharacter::Crouch(false);	
}

void AYellPointAndPrayCharacter::StopDuck() {
	// signal the character to stop crouching
	ACharacter::UnCrouch(false);
}

void AYellPointAndPrayCharacter::Use() 
{
	if (InventoryComponent->GetSlotID(0) != -1)
	{
		if (InventoryComponent->GetSlotObj(0)->GetClass()->ImplementsInterface(UUsable::StaticClass())) 
		{
			IUsable::Execute_Use(InventoryComponent->GetSlotObj(0), this);
		}
	}
}

void AYellPointAndPrayCharacter::ServerOnItemSelected_Implementation(int SlotID) 
{
	if (!HasAuthority()) return;
	//UE_LOG(LogTemp, Warning, TEXT("OnItemCalled"));

	//If Slot has a valid Item
	if (InventoryComponent->GetSlotID(SlotID) != -1)
	{
		//If Item has not been created
		if (!ItemCreated)
		{
			UE_LOG(LogTemp, Warning, TEXT("Slot Item has not been Created"));

			HoldingItemClass = InventoryComponent->GetSlotObj(SlotID);

			if (HoldingItemClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;

				if (HoldingItem == nullptr || HoldingItem != GetWorld()->SpawnActor<AActor>(HoldingItemClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
				{
					HoldingItem = GetWorld()->SpawnActor<AActor>(HoldingItemClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
					HoldingItem->SetActorEnableCollision(false);
					
					if (HoldingItem)
					{
						HoldingItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_r"));
						UE_LOG(LogTemp, Warning, TEXT("Slot Item has been Created"));
					}
				}
			}

			ItemCreated = true;
		}
	}
}

void AYellPointAndPrayCharacter::ServerDeleteItem_Implementation() 
{
	if (HoldingItem != nullptr)
	{
		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		HoldingItem->DetachFromActor(DetachRules);
		HoldingItem->Destroy();
		HoldingItem = nullptr;
	}

	ItemCreated = false;
	HoldingItemClass = nullptr;
}

void AYellPointAndPrayCharacter::OnItemSelected(int SlotID)
{
	//If the slot changed
	if (OldItemSelected != InventoryComponent->CurrentItemSelected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot has Changed"));
		OldItemSelected = InventoryComponent->CurrentItemSelected;

		this->ServerDeleteItem();
	}
	
	this->ServerOnItemSelected(SlotID);
}

void AYellPointAndPrayCharacter::ServerInteract_Implementation(AActor* hitObject, AYellPointAndPrayCharacter* character)
{
	if (!HasAuthority()) return;

	if (hitObject->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		if (InventoryComponent->IsInventoryFull()) 
		{
			UE_LOG(LogTemp, Warning, TEXT("INVENTORY IS FULL"));
		}
		else 
		{
			APickableItem* PickableItem = Cast<APickableItem>(hitObject);

			if (PickableItem)
			{
				InventoryComponent->SetInventory(PickableItem);
			}

			IInteractable::Execute_Interact(hitObject, character);
			UE_LOG(LogTemp, Warning, TEXT("PickedUpItem!"));
		}
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

void AYellPointAndPrayCharacter::Caught_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("YOU GOT CAUGTH NOOB L"));
}

void AYellPointAndPrayCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (widgetClass)
		AddAndRemoveWidget();
}