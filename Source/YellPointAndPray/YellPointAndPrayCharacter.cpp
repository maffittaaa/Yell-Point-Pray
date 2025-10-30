// Copyright Epic Games, Inc. All Rights Reserved.

#include "YellPointAndPrayCharacter.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "Animation/AnimInstance.h"
#include "Items/PickableItemsParent/PickableItem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "Interfaces/Interactable.h"
#include "Interfaces/Knockable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "YellPointAndPray.h"
#include "Blueprint/UserWidget.h"
#include <Net/UnrealNetwork.h>

#include "WhiteBoard.h"
#include <Kismet/GameplayStatics.h>

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "YellPointAndPrayPlayerController.h"

using namespace std;

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


	//Cesar Stuff -------------------------------------------------------

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickableItem::StaticClass(), FoundActors);
	for (auto& FoundActor : FoundActors)
	{
		if (FoundActor->GetClass()->ImplementsInterface(UInventorySubject::StaticClass()))
		{
			IInventorySubject::Execute_AddObserver(FoundActor, this);
		}
	}
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

		//Drop
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::Drop);

		//Release
		EnhancedInputComponent->BindAction(MouseRelease, ETriggerEvent::Completed, this, &AYellPointAndPrayCharacter::CallDuck);

		//Drawing
		auto& teste = EnhancedInputComponent->BindAction(DrawAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::CharacterDrawing);
		GEngine->AddOnScreenDebugMessage(1, 10.0f, FColor::Red, teste.IsBoundToObject(this) && teste.GetAction() != nullptr ? "yay bound properly!" : "oh noes failed to bind the draw :(");
			
	}
	else
		UE_LOG(LogYellPointAndPray, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
}

void AYellPointAndPrayCharacter::BeginPlay() {
	Super::BeginPlay();

	if (interactWidgetClass)
		interactWidget = CreateWidget<UUserWidget>(GetWorld(), interactWidgetClass, FName("Interact"));

	if (KnockGuardWidgetClass)
		KnockGuardWidget = CreateWidget<UUserWidget>(GetWorld(), KnockGuardWidgetClass, FName("Knock"));

	enumVariable = InGame;
}

void AYellPointAndPrayCharacter::AddTraceAndWidget() 
{
	float distance = 300.0f;

	FVector start;
	FRotator direction;
	
	AController* controller = GetController();
	
	if (!controller) return;

	controller->GetPlayerViewPoint(start, direction);
	GetController()->GetPlayerViewPoint(start, direction);

	FVector end = start + (direction.Vector() * distance);

	ECollisionChannel traceChannel = ECC_Visibility;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);

	// DrawDebugLine(GetWorld(), startTrace, endTrace, FColor::Red, false, -1.0f, 0, 1.0f);

	bHit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		start,
		end,
		traceChannel,
		RV_TraceParams
	);

	AActor* hitComponent = RV_Hit.GetActor();
	if (bHit && hitComponent->GetClass()->ImplementsInterface(UInteractable::StaticClass()) && !interactWidget->IsInViewport() && TimesWidgetCreated == 0) {
		interactWidget->AddToViewport();
		TimesWidgetCreated = 1;
	}
	else if (!bHit || !hitComponent->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		interactWidget->RemoveFromParent();
		TimesWidgetCreated = 0;
	}
}

void AYellPointAndPrayCharacter::AddKnockGuardWidget()
{
	float distance = 150.0f;

	FVector start;
	FRotator direction;

	AController* controller = GetController();

	if (!controller) return;

	controller->GetPlayerViewPoint(start, direction);
	GetController()->GetPlayerViewPoint(start, direction);

	FVector end = start + (direction.Vector() * distance);

	ECollisionChannel traceChannel = ECC_Visibility;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);

	bool hitted = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		start,
		end,
		traceChannel,
		RV_TraceParams
	);

	AActor* hitComponent = RV_Hit.GetActor();

	if (hitComponent) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Name: %d"), hitComponent->GetClass());
		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *hitComponent->GetClass()->GetName());
	}

	if (hitted && hitComponent->GetClass()->ImplementsInterface(UKnockable::StaticClass()) && !KnockGuardWidget->IsInViewport() && TimesKnockWidgetCreated == 0) {
		KnockGuardWidget->AddToViewport();
		TimesKnockWidgetCreated = 1;
	}
	else if (!hitted || !hitComponent->GetClass()->ImplementsInterface(UKnockable::StaticClass()))
	{
		KnockGuardWidget->RemoveFromParent();
		TimesKnockWidgetCreated = 0;
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

void AYellPointAndPrayCharacter::Duck() {
	// signal the character to crouch
	ACharacter::Crouch(false);	
}

void AYellPointAndPrayCharacter::StopDuck() {
	// signal the character to stop crouching
	ACharacter::UnCrouch(false);
}

void AYellPointAndPrayCharacter::ThrowDuck(ARubberDuckUsable* Ducksent)
{
	FVector start;
	FRotator dir;
	GetController()->GetPlayerViewPoint(start, dir);
	if (InventoryComponent->GetSlotID(InventoryComponent->CurrentItemSelected) != -1)
	{
		if (InventoryComponent->GetSlotObj(InventoryComponent->CurrentItemSelected)->Obj->IsChildOf(APickableItem::StaticClass()))
		{
			APickableItem* PickableItem = Cast<APickableItem>(InventoryComponent->GetSlotObj(InventoryComponent->CurrentItemSelected)->Obj->GetDefaultObject());

			TSubclassOf<AActor> NewHoldingItemClass = PickableItem->GetClass();

			if (NewHoldingItemClass)
			{
				FVector NewPosition = start + (dir.Vector() * 150);
				DuckUsing->Throw(this, GetWorld(), NewHoldingItemClass, NewPosition, dir);
			}
		}
	}
	//InventoryComponent->DeleteInventorySlot(InventoryComponent->CurrentItemSelected);
	//this->ServerDeleteItem();
}

void AYellPointAndPrayCharacter::GetDuck(ARubberDuckUsable* Duck)
{
	DuckUsing = Duck;
}

void AYellPointAndPrayCharacter::CallDuck()
{
	if (DuckUsing) {
		DuckUsing->ChangeAdd();
	}
}

void AYellPointAndPrayCharacter::Drop() 
{	FVector start;
	FRotator dir;
	GetController()->GetPlayerViewPoint(start, dir);


	ServerOnItemDroped(InventoryComponent->CurrentItemSelected, start, dir);
	InventoryComponent->DeleteInventorySlot(InventoryComponent->CurrentItemSelected);
	this->ServerDeleteItem();
}

void AYellPointAndPrayCharacter::ServerOnItemDroped_Implementation(int SlotID, FVector start, FRotator dir)
{
	if (!HasAuthority()) return;

	if (InventoryComponent->GetSlotID(SlotID) != -1)
	{
		if (InventoryComponent->GetSlotObj(SlotID)->Obj->IsChildOf(APickableItem::StaticClass())) 
		{
			APickableItem* PickableItem = Cast<APickableItem>(InventoryComponent->GetSlotObj(SlotID)->Obj->GetDefaultObject());

			TSubclassOf<AActor> NewHoldingItemClass = PickableItem->GetClass();

			if (NewHoldingItemClass)
			{
				FVector NewPosition = start + (dir.Vector() * 150);
				FActorSpawnParameters SpawnParams;
				GetWorld()->SpawnActor<AActor>(NewHoldingItemClass, NewPosition, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}
}

void AYellPointAndPrayCharacter::Use()
{
	if (InventoryComponent->GetSlotID(InventoryComponent->CurrentItemSelected) != -1)
	{
		FString name = InventoryComponent->GetSlotName(InventoryComponent->CurrentItemSelected);

		if (InventoryComponent->GetSlotObj(InventoryComponent->CurrentItemSelected)->GetClass()->ImplementsInterface(UUsable::StaticClass()))
		{
			IUsable::Execute_Use(InventoryComponent->GetSlotObj(InventoryComponent->CurrentItemSelected), this);
		}
	}
}

void AYellPointAndPrayCharacter::ServerOnItemSelected_Implementation(int SlotID)
{
	if (!HasAuthority()) return;

	if (InventoryComponent->GetSlotID(SlotID) != -1)
	{
		if (!ItemCreated)
		{
			TSubclassOf<AActor> NewHoldingItemClass = InventoryComponent->GetSlotObj(SlotID)->GetClass();

			if (NewHoldingItemClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Cast<APawn>(this);

				// Only spawn on server - this will replicate to clients
				HoldingItem = GetWorld()->SpawnActor<AActor>(NewHoldingItemClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

				if (HoldingItem)
				{
					HoldingItem->SetActorEnableCollision(false);
					// Don't attach here - let OnRep_HoldingItem handle attachment based on ownership
					ItemCreated = true;
				}

				OnRep_HoldingItem();
			}
		}
	}
}

void AYellPointAndPrayCharacter::ServerDeleteItem_Implementation()
{
	if (HoldingItem != nullptr)
	{
		HoldingItem->Destroy();
		HoldingItem = nullptr;
	}
	ItemCreated = false;
}

void AYellPointAndPrayCharacter::OnItemSelected(int SlotID)
{
	// If the slot changed
	if (OldItemSelected != InventoryComponent->CurrentItemSelected)
	{
		OldItemSelected = InventoryComponent->CurrentItemSelected;
		this->ServerDeleteItem();
	}

	this->ServerOnItemSelected(SlotID);
}

void AYellPointAndPrayCharacter::DeleteItem()
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

void AYellPointAndPrayCharacter::OnRep_HoldingItem()
{
	if (HoldingItem)
	{
		// Attach to the appropriate mesh based on ownership
		if (IsLocallyControlled())
		{
			// Local player sees it on first person mesh
			HoldingItem->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_r"));
			//HoldingItem->SetActorRelativeLocation(FVector(20, 0, 0)); // Replace X, Y, Z with your values			
			//HoldingItem->SetActorRelativeRotation(FRotator(Pitch, Yaw, Roll)); // In degrees
		}
		else
		{
			// Other players see it on the regular mesh
			HoldingItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_r"));
		}
		HoldingItem->SetActorEnableCollision(false);

	}
}

void AYellPointAndPrayCharacter::OnRepState() {
	UE_LOG(LogTemp, Warning, TEXT("STATE: %d"), static_cast<int>(enumVariable));
	
	playerController = Cast<APlayerController>(GetController());
	if (!playerController) return;
	
	if (enumVariable == InWhiteboard)
	{
		
		float blendTime = 0.0f;
		ACameraActor* camera = whiteboardCamera.LoadSynchronous();//to get the actual camera object
		
		playerController->SetViewTargetWithBlend(camera, blendTime, VTBlend_EaseIn);
		
		UE_LOG(LogTemp, Warning, TEXT("Camera and movement locked"));
		
		SetActorHiddenInGame(true);
		GetCharacterMovement()->DisableMovement();
		SetActorEnableCollision(false);
		
		playerController->bShowMouseCursor = true;
		playerController->SetShowMouseCursor(true);
		UE_LOG(LogTemp, Warning, TEXT("Cursor visibility after set: %d"), playerController->bShowMouseCursor);
	
		FInputModeGameOnly inputMode;
		//inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		playerController->SetInputMode(inputMode);
		
		paintBrushWidget = CreateWidget<UUserWidget>(GetWorld(), paintBrushWidgetClass, FName("PaintBrush"));
		playerController->SetMouseCursorWidget(EMouseCursor::Type::Default ,paintBrushWidget);
		isDrawing = true;
		UE_LOG(LogTemp, Warning, TEXT("Drawing %d"), isDrawing);
		
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer())) { //adding the map context for drawing
			AYellPointAndPrayPlayerController* yellPlayerController = Cast<AYellPointAndPrayPlayerController>(playerController);
			if (yellPlayerController){
				for (UInputMappingContext* DefaultContexts : yellPlayerController->DefaultMappingContexts) {
					if (DefaultContexts) {
						subsystem->RemoveMappingContext(DefaultContexts);

						for (UInputMappingContext* drawingContexts : yellPlayerController->DrawingContexts) {
							if (drawingContexts) {
								FString ContextName = drawingContexts->GetName();
								UE_LOG(LogTemp, Warning, TEXT("Adding Drawing Mapping Context: %s"), *ContextName);
								subsystem->AddMappingContext(drawingContexts, 2);
							}
						}
					}
				}
			}
		}
	} else {
		if (UEnhancedInputLocalPlayerSubsystem* subsystem2 = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))  {
			AYellPointAndPrayPlayerController* yellPlayerController = Cast<AYellPointAndPrayPlayerController>(playerController);
			if (yellPlayerController) {
				for (UInputMappingContext* drawingContext : yellPlayerController->DrawingContexts) {
					if (drawingContext) {
						subsystem2->RemoveMappingContext(drawingContext);
						for (UInputMappingContext* DefaultContexts : yellPlayerController->DefaultMappingContexts){
							if (DefaultContexts)
								subsystem2->AddMappingContext(DefaultContexts, 1);
						}
					}
				}
			}
				
			SetActorHiddenInGame(false);
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			SetActorEnableCollision(true);
			playerController->bShowMouseCursor = false;
			playerController->SetShowMouseCursor(false);
			isDrawing = false;
		}
	}
}

void AYellPointAndPrayCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AYellPointAndPrayCharacter, HoldingItem);
	DOREPLIFETIME(AYellPointAndPrayCharacter, ItemCreated);
	DOREPLIFETIME(AYellPointAndPrayCharacter, enumVariable);
}

void AYellPointAndPrayCharacter::ServerInteract_Implementation(AActor* hitObject, AYellPointAndPrayCharacter* character)
{
	if (!HasAuthority()) return;

	if (hitObject->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {
		if (InventoryComponent->IsInventoryFull()){
			UE_LOG(LogTemp, Warning, TEXT("INVENTORY IS FULL"));
		} else {
			APickableItem* PickableItem = Cast<APickableItem>(hitObject);

			if (PickableItem) 
				InventoryComponent->SetInventory(PickableItem);

			if (hitObject->GetClass()->GetName().Contains("BP_WhiteBoard")) {
				enumVariable = InWhiteboard;
				OnRepState();
			}
			
			IInteractable::Execute_Interact(hitObject, character);
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
	if (GetWorld()->LineTraceSingleByChannel( hit, start, end, ECC_Visibility, params)) {
		//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1.0f, 0, 1.0f);
		if (AActor* hitObject = hit.GetActor())
			this->ServerInteract(hitObject, this);
	}
}

void AYellPointAndPrayCharacter::Caught_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("YOU GOT CAUGHT NOOB L"));
}

void AYellPointAndPrayCharacter::CharacterDrawing(const FInputActionValue& value) {
	if (isDrawing) {
		float distance = 1000.0f;
		
		FVector start;
		FVector direction;
		
		FVector2D viewportSize;
		
		if (GEngine && GEngine->GameViewport)
			GEngine->GameViewport->GetViewportSize(viewportSize);

		FVector2D mousePosition;
		playerController->GetMousePosition(mousePosition.X, mousePosition.Y);

		float normalizedX = mousePosition.X / viewportSize.X;
		float normalizedY = mousePosition.Y / viewportSize.Y;
		
		playerController->DeprojectScreenPositionToWorld(normalizedX * viewportSize.X, normalizedY * viewportSize.Y, start, direction);
		FVector end = start + (direction * distance);
	
		ECollisionChannel traceChannel = ECC_Visibility;
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
		RV_TraceParams.bTraceComplex = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		RV_TraceParams.AddIgnoredActor(this);

		DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1.0f, 0, 1.0f);
	
		bool bHit2 = GetWorld()->LineTraceSingleByChannel(
			RV_Hit,
			start,
			end,
			traceChannel,
			RV_TraceParams
		);

		AActor* hitActor = RV_Hit.GetActor();
		if (bHit2) {
			whiteboard = Cast<AWhiteBoard>(hitActor);
		
			if (whiteboard) {
				UE_LOG(LogTemp, Warning, TEXT("Successfully cast to whiteboard - calling Draw()"));
				float whiteboardBrushSize = 50.0f;
				FVector2D UVCoordinates;
				FVector LocalImpact = RV_Hit.GetComponent()->GetComponentTransform().InverseTransformPosition(RV_Hit.ImpactPoint);
				FBoxSphereBounds Bounds = RV_Hit.GetComponent()->CalcBounds(FTransform());
			
				UVCoordinates.X = FMath::GetMappedRangeValueUnclamped(
					FVector2D(-Bounds.BoxExtent.X, Bounds.BoxExtent.X), 
					FVector2D(0, 1), 
					LocalImpact.X
				);
				UVCoordinates.Y = FMath::GetMappedRangeValueUnclamped(
					FVector2D(-Bounds.BoxExtent.Y, Bounds.BoxExtent.Y), 
					FVector2D(0, 1), 
					LocalImpact.Y
				);
				
				whiteboard->Draw(whiteboardBrushTexture, whiteboardBrushSize, UVCoordinates);
			}
		}
	}
}


void AYellPointAndPrayCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (interactWidgetClass)
		AddTraceAndWidget();

	if (KnockGuardWidgetClass && InventoryComponent->GetSlotName(InventoryComponent->CurrentItemSelected) == "Toy Hammer Usable")
	{
		AddKnockGuardWidget();
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Slot Item Name: %s"), *InventoryComponent->GetSlotName(InventoryComponent->CurrentItemSelected))
	}
}

//Cesar Stuff -----------------------------------------------------------------------

void AYellPointAndPrayCharacter::OnItemAdded_Implementation(const FString& Name) {
	UE_LOG(LogTemp, Warning, TEXT("Code Works im the best :D!"));
}