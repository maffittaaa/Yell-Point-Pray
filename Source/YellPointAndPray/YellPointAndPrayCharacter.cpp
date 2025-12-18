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
#include <Items/Treasure/TreasurePickable.h>
#include <Players/YPPCustomGameMode.h>
#include <UI/Menus/MenusLevelScript.h>
#include <Lobby/LobbyLevelScript.h>
#include "Items/Keys/KeyPickable.h"
#include "Items/Keys/KeyUsable.h"

#include "Players/YPPCustomGameInstance.h"
#include <NavigationSystem.h>
#include "NavigationPath.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

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

	Hands = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hands Mesh"));
	Hands2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hands2 Mesh"));
	Hands2->SetupAttachment(Hands);
	HandsPos = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands Position"));
	HandsPos->SetupAttachment(FirstPersonCameraComponent);

	PlayerLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("PlayerLight"));
	PlayerLight->SetupAttachment(GetRootComponent());
	PlayerLight->Intensity = 7000.f;
	PlayerLight->SetVisibility(false);

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	InventoryComponent = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
	
	cheatsComponent = CreateDefaultSubobject<UACCheats>(TEXT("Cheats"));
	
	bReplicates = true;

	FartAudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("FartAudioPlayer"));
	BonkAudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("BonkAudioPlayer"));
	EatAudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("EatAudioPlayer"));

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
		EnhancedInputComponent->BindAction(DrawAction, ETriggerEvent::Triggered, this, &AYellPointAndPrayCharacter::CharacterDrawing);
		EnhancedInputComponent->BindAction(DrawAction, ETriggerEvent::Completed, this, &AYellPointAndPrayCharacter::CharacterStopDrawing);

		//turnOffDetectionFromGuards
		EnhancedInputComponent->BindAction(TurnOffDetectionAction, ETriggerEvent::Started, this , &AYellPointAndPrayCharacter::TurnOffDetection);

		//TeleportToLaserRoom
		EnhancedInputComponent->BindAction(TeleportToLaserRoomAction, ETriggerEvent::Started, this , &AYellPointAndPrayCharacter::TeleportToLaserRoom);

		//TeleportToElectricalRoom
		EnhancedInputComponent->BindAction(TeleportToElectricalRoomAction, ETriggerEvent::Started, this , &AYellPointAndPrayCharacter::TeleportToElectricalRoom);

		//EmoteWheel
		EnhancedInputComponent->BindAction(EmoteWheelAction, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::TurnOnEmoteWheel);
		EnhancedInputComponent->BindAction(EmoteWheelAction, ETriggerEvent::Completed, this, &AYellPointAndPrayCharacter::TurnOffEmoteWheel);

		//GoBack
		EnhancedInputComponent->BindAction(GoBackToAppartment, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::GoToAppartment);

		//GoBack
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AYellPointAndPrayCharacter::Pause);
		
	}
	else
		UE_LOG(LogYellPointAndPray, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
}

void AYellPointAndPrayCharacter::BeginPlay() {
	Super::BeginPlay();

	GoToAppartment();

	if (interactWidgetClass)
		interactWidget = CreateWidget<UUserWidget>(GetWorld(), interactWidgetClass, FName("Interact"));

	if (KnockGuardWidgetClass)
		KnockGuardWidget = CreateWidget<UUserWidget>(GetWorld(), KnockGuardWidgetClass, FName("Knock"));

	if (GameOverWidgetClass)
		GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass, FName("GameOver"));

	StartLocation = GetActorLocation();
	StartRotation = GetActorTransform().GetRotation();
	
	Hands2->SetWorldLocation(HandsPos->GetComponentLocation());
	Hands2->SetWorldRotation(HandsPos->GetComponentRotation());
	OriginalDiff = (FirstPersonCameraComponent->GetComponentLocation() - Hands2->GetComponentLocation()).Length();

	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (LevelName == "Lvl_Lobby")
	{
		return;
	}

	if (HasAuthority())
	{
		AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->StoreAllItemsInMap();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Player is going to Start Timer"));

	//RestoreTravelInventory();
	//FTimerHandle TimerHandle; 
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AYellPointAndPrayCharacter::RestoreTravelInventory, 0.5f, false);
}

void AYellPointAndPrayCharacter::Reset_Implementation()
{
	FRotator rotation = StartRotation.Rotator();
	TeleportTo(StartLocation, rotation);

	if (HoldingItem != nullptr)
	{
		ServerDeleteItem();
	}	

	KebabEffect = false;
	GetWorld()->GetTimerManager().ClearTimer(KebabTimerHandle);

	RestoreTravelInventory();

	Client_HideGameOver();
}

void AYellPointAndPrayCharacter::RestoreTravelInventory()
{
	if (!HasAuthority()) return;

	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player: Game Instance Exists"));

		if (UYPPCustomGameInstance* CustomGameInstance = Cast<UYPPCustomGameInstance>(GameInstance))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player: Custom Game Instance Exists"));

			if (AYPPCustomPlayerState* CustomPlayerState = Cast<AYPPCustomPlayerState>(GetPlayerState()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Player: Custom Player State Exists"));

				FPlayerInventoryInfo TravelInventory = CustomGameInstance->GetPlayerInventory(CustomPlayerState);

				if (TravelInventory.InventorySlots.Num() > 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player: TravelInventory is > 0"));

					InventoryComponent->RestoreInventoryWithTravelData(TravelInventory.InventorySlots);
					//UE_LOG(LogTemp, Warning, TEXT("Player: Restored travel inventory with %d slots"), TravelInventory.InventorySlots.Num());
				}
			}
		}
	}
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

	ECollisionChannel traceChannel = ECC_Pawn;
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

	if (hitted && hitComponent->GetClass()->ImplementsInterface(UKnockable::StaticClass()) && !KnockGuardWidget->IsInViewport() && TimesKnockWidgetCreated == 0) 
	{
		KnockGuardWidget->AddToViewport();
		KnockableActor = hitComponent;
		TimesKnockWidgetCreated = 1;
	}
	else if (!hitted || !hitComponent->GetClass()->ImplementsInterface(UKnockable::StaticClass()))
	{
		KnockGuardWidget->RemoveFromParent();
		KnockableActor = nullptr;
		TimesKnockWidgetCreated = 0;
	}
}

void AYellPointAndPrayCharacter::Client_KnockGuard_Implementation(AActor* CurrentKnockableActor)
{
	// This runs on the client that owns this character
	Server_KnockGuard(CurrentKnockableActor);
}

void AYellPointAndPrayCharacter::Pause()
{
	Cast<AYellPointAndPrayPlayerController>(GetController())->PauseMenu(GetWorld(), this);
}

void AYellPointAndPrayCharacter::Server_KnockGuard_Implementation(AActor* CurrentKnockableActor)
{
	if (CurrentKnockableActor && CurrentKnockableActor->GetClass()->ImplementsInterface(UKnockable::StaticClass()))
	{
		//SOUND
		BonkAudioPlayer->Sound = BonkSound;
		BonkAudioPlayer->Play();
		IKnockable::Execute_Knock(CurrentKnockableActor);
		UE_LOG(LogTemp, Warning, TEXT("Server knocking guard via player RPC"));
	}
}

bool AYellPointAndPrayCharacter::Server_KnockGuard_Validate(AActor* CurrentKnockableActor)
{
	return true; // Add validation logic if needed
}

void AYellPointAndPrayCharacter::MoveInput(const FInputActionValue& Value)
{
	if (!bMovementInputEnabled) return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AYellPointAndPrayCharacter::LookInput(const FInputActionValue& Value)
{
	if (!bLookInputEnabled) return;
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AYellPointAndPrayCharacter::SetMovementInputEnabled(bool bEnabled){
	bMovementInputEnabled = bEnabled;
}

void AYellPointAndPrayCharacter::SetLookInputEnabled(bool bEnabled){
	bLookInputEnabled = bEnabled;
}

void AYellPointAndPrayCharacter::SetUseActive(bool state) {
	UseActive = state;
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

void AYellPointAndPrayCharacter::OnRepAnimationState() {
	
}

void AYellPointAndPrayCharacter::TurnOnEmoteWheel(){
	emoteWheelWidget = CreateWidget<UUserWidget>(GetWorld(), emoteWheelWidgetClass, FName("WEmoteWheelMenu"));
	emoteWheelWidget->AddToViewport();
	
	playerController = Cast<APlayerController>(GetController());
	if (!playerController) return;

	FInputModeGameAndUI inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	inputMode.SetHideCursorDuringCapture(false);
	playerController->SetInputMode(inputMode);

	playerController->bShowMouseCursor = true;
	playerController->SetShowMouseCursor(true);
}

void AYellPointAndPrayCharacter::TurnOffEmoteWheel() {
	emoteWheelWidget->RemoveFromParent();

	playerController = Cast<APlayerController>(GetController());
	if (!playerController) return;

	FInputModeGameOnly inputMode;
	playerController->SetInputMode(inputMode);

	playerController->bShowMouseCursor = false;
	playerController->SetShowMouseCursor(false);
}

void AYellPointAndPrayCharacter::ThrowDuck_Implementation(ARubberDuckUsable* Ducksent, int ItemSelect)
{
	if (!HasAuthority()) return;
	FVector start;
	FRotator dir;
	GetController()->GetPlayerViewPoint(start, dir);

	FVector end = start + (dir.Vector() * 150);

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

	FVector_NetQuantize HitPoint = RV_Hit.ImpactPoint;
	FVector HitPointVector = HitPoint;

	if (InventoryComponent->GetSlotID(ItemSelect) != -1)
	{
		AUsableItem* UsableItem = Cast<AUsableItem>(InventoryComponent->GetSlotObj(ItemSelect)->GetDefaultObject());

		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (UYPPCustomGameInstance* CustomGameInstance = Cast<UYPPCustomGameInstance>(GameInstance))
			{
				if (CustomGameInstance->ItemsPickableArray[InventoryComponent->GetSlotID(ItemSelect)]->IsChildOf(APickableItem::StaticClass()))
				{
					TSubclassOf<AActor> NewHoldingItemClass = CustomGameInstance->ItemsPickableArray[InventoryComponent->GetSlotID(ItemSelect)];

					if (NewHoldingItemClass)
					{
						FVector NewPosition;
						if (HitPointVector != FVector::Zero())
						{
							NewPosition = HitPointVector;
						}
						else
						{
							NewPosition = end;
						}
						DuckUsing->Throw(this, GetWorld(), NewHoldingItemClass, NewPosition, dir);
					}
				}
			}
		}
	}
	InventoryComponent->DeleteInventorySlot(ItemSelect);
	this->ServerDeleteItem();
	DuckUsing = nullptr;
}

void AYellPointAndPrayCharacter::GetDuck_Implementation(ARubberDuckUsable* Duck)
{
	DuckUsing = Duck;
}

void AYellPointAndPrayCharacter::CallDuck()
{
	if (DuckUsing) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Duck using exists"));
		DuckUsing->ChangeAdd(InventoryComponent->CurrentItemSelected);
	}
}

void AYellPointAndPrayCharacter::Drop() 
{	
	if (!UseActive) return;

	FVector start;
	FRotator dir;
	GetController()->GetPlayerViewPoint(start, dir);

	FVector end = start + (dir.Vector() * 150);

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

	FVector_NetQuantize hitPoint = RV_Hit.ImpactPoint;

	ServerOnItemDroped(InventoryComponent->CurrentItemSelected, hitPoint, end);

	InventoryComponent->DeleteInventorySlot(InventoryComponent->CurrentItemSelected);
	this->ServerDeleteItem();
}

void AYellPointAndPrayCharacter::ServerOnItemDroped_Implementation(int SlotID, FVector HitPoint, FVector SpacePoint)
{
	if (!HasAuthority()) return;

	if (InventoryComponent->GetSlotID(SlotID) != -1)
	{
		AUsableItem* UsableItem = Cast<AUsableItem>(InventoryComponent->GetSlotObj(SlotID)->GetDefaultObject());

		//if (!UsableItem) return;

		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (UYPPCustomGameInstance* CustomGameInstance = Cast<UYPPCustomGameInstance>(GameInstance))
			{
				if (CustomGameInstance->ItemsPickableArray[SlotID]->IsChildOf(APickableItem::StaticClass()))
				{
					TSubclassOf<AActor> NewHoldingItemClass = CustomGameInstance->ItemsPickableArray[InventoryComponent->GetSlotID(SlotID)];

					if (NewHoldingItemClass)
					{
						//FVector NewPosition = start + (dir.Vector() * 150);
						FVector NewPosition;
						if (HitPoint != FVector::Zero())
						{
							NewPosition = HitPoint;
						}
						else
						{
							NewPosition = SpacePoint;
						}
						FActorSpawnParameters SpawnParams;
						if (InventoryComponent->GetSlotID(SlotID) == 6)
						{
							AActor* Key = GetWorld()->SpawnActor<AActor>(NewHoldingItemClass, NewPosition, FRotator::ZeroRotator, SpawnParams);

							AKeyPickable* KeyItem = Cast<AKeyPickable>(Key);

							KeyItem->KeyID = InventoryComponent->GetSlotKeyID(SlotID);
							KeyItem->KeyName = InventoryComponent->GetSlotKeyName(SlotID);

							UE_LOG(LogTemp, Warning, TEXT("KeyID AAAAAAAAAAAAAset to: %d"), Cast<AKeyPickable>(Key)->KeyID);
							return;
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("No Key"));
							UE_LOG(LogTemp, Warning, TEXT("Key Class %s"), *NewHoldingItemClass->GetName());
						}
						Multicast_TurnOffLight();
						GetWorld()->SpawnActor<AActor>(NewHoldingItemClass, NewPosition, FRotator::ZeroRotator, SpawnParams);
					}
				}
			}
		}
	}
}

void AYellPointAndPrayCharacter::Use()
{
	if (!UseActive) return;

	if (InventoryComponent->GetSlotID(InventoryComponent->CurrentItemSelected) != -1)
	{
		FString name = InventoryComponent->GetSlotName(InventoryComponent->CurrentItemSelected);

		if (HoldingItem)
		{

			if (HoldingItem->GetClass()->ImplementsInterface(UUsable::StaticClass()))
			{
				IUsable::Execute_Use(HoldingItem, this);
				if (GetNetMode() == NM_Client)
				{
					Server_UseItem(InventoryComponent->CurrentItemSelected);
				}
			}
		}
	}
}

void AYellPointAndPrayCharacter::Server_UseItem_Implementation(int SlotID)
{
	if (HoldingItem && HoldingItem->GetClass()->ImplementsInterface(UUsable::StaticClass()))
	{
		IUsable::Execute_Use(HoldingItem, this);
	}
}

void AYellPointAndPrayCharacter::Multicast_TurnOffLight_Implementation()
{
	PlayerLight->SetVisibility(false);
}

void AYellPointAndPrayCharacter::ServerOnItemSelected_Implementation(int SlotID)
{
	if (!HasAuthority()) return;

	if (InventoryComponent->GetSlotID(SlotID) != -1)
	{
		if (!ItemCreated)
		{
			if (!InventoryComponent->GetSlotObj(SlotID)) 
			{
				//UE_LOG(LogTemp, Warning, TEXT("Player: No slot Obj"));
				return;
			}
			
			TSubclassOf<AActor> NewHoldingItemClass = InventoryComponent->GetSlotObj(SlotID);
			//UE_LOG(LogTemp, Warning, TEXT("Player: KeyID set to: %d"), Cast<AKeyUsable>(HoldingItem)->KeyID);

			if (NewHoldingItemClass)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Player: Has NewHoldingItem"));

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Cast<APawn>(this);

				HoldingItem = GetWorld()->SpawnActor<AActor>(NewHoldingItemClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

				if (AKeyUsable* KeyItem = Cast<AKeyUsable>(HoldingItem))
				{
					KeyItem->KeyID = InventoryComponent->GetSlotKeyID(SlotID);
					UE_LOG(LogTemp, Warning, TEXT("Player: KeyID set to: %d"), Cast<AKeyUsable>(HoldingItem)->KeyID);
				}

				if (HoldingItem)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Player: Has HoldingItem"));

					HoldingItem->SetActorEnableCollision(false);
					HoldingItem->SetReplicates(true);
					ItemCreated = true;
				}

				OnRep_HoldingItem();
			}
			else 
			{
				//UE_LOG(LogTemp, Warning, TEXT("Player: No NewHoldingItem"));
			}
		}
	}
}

void AYellPointAndPrayCharacter::ServerDeleteItem_Implementation()
{
	Multicast_TurnOffLight();
	if (HoldingItem != nullptr)
	{
		HoldingItem->Destroy();
		HoldingItem = nullptr;
	}
	ItemCreated = false;
}

void AYellPointAndPrayCharacter::OnItemSelected(int SlotID)
{
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

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		Subsystem->RequestRebuildControlMappings();
	
	if (enumVariable == InWhiteboard)
	{
		float blendTime = 0.0f;
		AActor* camera = whiteboardCamera.LoadSynchronous();//to get the actual camera object
		
		playerController->SetViewTargetWithBlend(camera, blendTime, VTBlend_EaseIn);
		
		SetLookInputEnabled(false);

		UE_LOG(LogTemp, Warning, TEXT("Camera and movement locked"));
		
		FInputModeGameAndUI inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		inputMode.SetHideCursorDuringCapture(false);
		playerController->SetInputMode(inputMode);
		
		playerController->bShowMouseCursor = true;
		playerController->SetShowMouseCursor(true);

		interactWidget->RemoveFromParent();

		// whiteboard->CloseWidget();
		
		UE_LOG(LogTemp, Warning, TEXT("Cursor visibility after set: %d"), playerController->bShowMouseCursor);
		
		paintBrushWidget = CreateWidget<UUserWidget>(GetWorld(), paintBrushWidgetClass, FName("WPaintBrush"));
		playerController->SetMouseCursorWidget(EMouseCursor::Type::Default ,paintBrushWidget);
		closingBoardWidget = CreateWidget<UUserWidget>(GetWorld(), closingBoardWidgetClass, FName("WClosingBoard"));
		
		if (UButton* closeButton = Cast<UButton>(closingBoardWidget->GetWidgetFromName(TEXT("CloseButton"))))
			closeButton->OnClicked.AddDynamic(this, &AYellPointAndPrayCharacter::OnCloseButtonClicked);
		
		clearBoardWidget = CreateWidget<UUserWidget>(GetWorld(), clearBoardWidgetClass, FName("WClearBoard"));

		if (UButton* clearButton = Cast<UButton>(clearBoardWidget->GetWidgetFromName(TEXT("ClearButton"))))
			clearButton->OnClicked.AddDynamic(this, &AYellPointAndPrayCharacter::OnClearButtonClicked);
		
		closingBoardWidget->AddToViewport();
		clearBoardWidget->AddToViewport();
		
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
				subsystem->RequestRebuildControlMappings();
			}
		}
	} else {
		playerController->SetViewTargetWithBlend(this, 0.0f, VTBlend_EaseIn);

		SetLookInputEnabled(true);
			
		FInputModeGameOnly inputMode;
		playerController->SetInputMode(inputMode);
		
		playerController->bShowMouseCursor = false;
		playerController->SetShowMouseCursor(false);

		if (paintBrushWidget && closingBoardWidget && clearBoardWidget) {
			paintBrushWidget->RemoveFromParent();
			closingBoardWidget->RemoveFromParent();
			clearBoardWidget->RemoveFromParent();
			paintBrushWidget = nullptr;
			closingBoardWidget = nullptr;
			clearBoardWidget = nullptr;
		}
		
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))  {
	 		AYellPointAndPrayPlayerController* yellPlayerController = Cast<AYellPointAndPrayPlayerController>(playerController);
	 		if (yellPlayerController) {
	 			for (UInputMappingContext* drawingContext : yellPlayerController->DrawingContexts) {
	 				if (drawingContext) {
	 					subsystem->RemoveMappingContext(drawingContext);
	 					
	 					for (UInputMappingContext* DefaultContexts : yellPlayerController->DefaultMappingContexts){
	 						if (DefaultContexts) {
	 							UE_LOG(LogTemp, Warning, TEXT("default context: %s"), *DefaultContexts->GetName());
	 							subsystem->AddMappingContext(DefaultContexts, 2);
	 						}
	 					}
	 				}
	 			}
	 		}
	 	}
	}
}

void AYellPointAndPrayCharacter::OnClearButtonClicked() {
	whiteboard = Cast<AWhiteBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AWhiteBoard::StaticClass()));

	if (whiteboard) {
		confirmationWidget= CreateWidget<UUserWidget>(GetWorld(), confirmationWidgetClass, FName("WConfirmation"));
		clearBoardWidget->RemoveFromParent();

		if (UButton* confirmationButton = Cast<UButton>(confirmationWidget->GetWidgetFromName(TEXT("ConfirmationButton"))))
			confirmationButton->OnClicked.AddDynamic(this, &AYellPointAndPrayCharacter::OnConfirmationButtonClicked);

		if (UButton* refuseButton = Cast<UButton>(confirmationWidget->GetWidgetFromName(TEXT("RefuseButton"))))
			refuseButton->OnClicked.AddDynamic(this, &AYellPointAndPrayCharacter::OnRefuseButtonClicked);
		
		confirmationWidget->AddToViewport();
	}
}

void AYellPointAndPrayCharacter::OnConfirmationButtonClicked() {
	whiteboard = Cast<AWhiteBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AWhiteBoard::StaticClass()));
	if (whiteboard)
		Server_ClearBoard(true);
	confirmationWidget->RemoveFromParent();
	clearBoardWidget->AddToViewport();
}

void AYellPointAndPrayCharacter::OnRefuseButtonClicked() {
	whiteboard = Cast<AWhiteBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AWhiteBoard::StaticClass()));
	if (whiteboard)
		Server_ClearBoard(false);
	confirmationWidget->RemoveFromParent();
	clearBoardWidget->AddToViewport();
}

void AYellPointAndPrayCharacter::OnRep_ClearBoard() {
	if (bClearBoard && whiteboard->renderTarget2D) {
		
		UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), whiteboard->renderTarget2D, FLinearColor::White);
        
		if (whiteboard->canvasTexture)
			whiteboard->InitializeBackground();
		
		bClearBoard = false;
	}
}

void AYellPointAndPrayCharacter::Server_ClearBoard_Implementation(bool bClear) {
	bClearBoard = bClear;
}

void AYellPointAndPrayCharacter::OnCloseButtonClicked() {
	UE_LOG(LogTemp, Warning, TEXT("Closing whiteboard"));
	Server_SetEnumVariable(InGame);
	enumVariable = InGame;
	OnRepState();
	
	ForceNetUpdate();

	whiteboard = Cast<AWhiteBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AWhiteBoard::StaticClass()));
	if (whiteboard) {
		whiteboard->CloseBoard();
		interactWidget->AddToViewport();
	}
		
	whiteboard = nullptr;
}

void AYellPointAndPrayCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AYellPointAndPrayCharacter, HoldingItem);
	DOREPLIFETIME(AYellPointAndPrayCharacter, ItemCreated);
	DOREPLIFETIME(AYellPointAndPrayCharacter, enumVariable);
	DOREPLIFETIME(AYellPointAndPrayCharacter, DuckUsing);
	DOREPLIFETIME(AYellPointAndPrayCharacter, replicatedMouseUV);
	DOREPLIFETIME(AYellPointAndPrayCharacter, bReplicatedIsDrawing);
	DOREPLIFETIME(AYellPointAndPrayCharacter, bClearBoard); 
	DOREPLIFETIME(AYellPointAndPrayCharacter, IsOutSide);
	DOREPLIFETIME(AYellPointAndPrayCharacter, PlayerLight);
}

void AYellPointAndPrayCharacter::Client_ShowGameOver_Implementation(bool State)
{
	UE_LOG(LogTemp, Warning, TEXT("Client_ShowGameOver called on client"));

	if (GameOverWidget && !GameOverWidget->IsInViewport())
	{
		GameOverWidget->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("Game Over widget added to viewport"));

		APlayerController* PC = Cast<APlayerController>(GetController());

		UWorld* World = GetWorld();
		if (World)
		{
			AMenusLevelScript* LevelScript = Cast<AMenusLevelScript>(World->GetLevelScriptActor());

			if (LevelScript && PC)
			{
				LevelScript->SetLocalPlayerController(PC);
				LevelScript->RegisterPlayerGameOverWidget(GameOverWidget, State);
			}
		}

		//Pause game and show cursor
		if (PC)
		{
			PreviousInputMode = FInputModeGameOnly();
			PC->SetPause(true);
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

void AYellPointAndPrayCharacter::Client_HideGameOver_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Client_HideGameOver called on client"));

	if (GameOverWidget && GameOverWidget->IsInViewport())
	{
		GameOverWidget->RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("Game Over widget hideen to viewport"));

		APlayerController* PC = Cast<APlayerController>(GetController());

		//Pause game and show cursor
		if (PC)
		{
			PC->SetPause(false);
			FInputModeGameOnly GameOnly;
			PC->SetInputMode(GameOnly);
			PC->bShowMouseCursor = false;
		}
	}
}

void AYellPointAndPrayCharacter::ServerInteract_Implementation(AActor* hitObject, AYellPointAndPrayCharacter* character)
{
	if (!HasAuthority()) return;

	if (hitObject->GetClass()->ImplementsInterface(UInteractable::StaticClass())) 
	{
		//ATreasurePickable* Treasure = Cast<ATreasurePickable>(hitObject);

		//if (Treasure)
		//{
		//	AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());

		//	if (GameMode)
		//	{
		//		UE_LOG(LogTemp, Warning, TEXT("Called Game won"));
		//		GameMode->GameOver(true);
		//	}
		//}

		if (hitObject->GetClass()->GetName().Contains("BP_WhiteBoard") && !(enumVariable == InWhiteboard)) {
			Server_SetEnumVariable(InWhiteboard);
			whiteboard = Cast<AWhiteBoard>(hitObject);
			OnRepState();
		}
		
		APickableItem* PickableItem = Cast<APickableItem>(hitObject);

		if (PickableItem) 
		{
			if (InventoryComponent->IsInventoryFull())
			{
				UE_LOG(LogTemp, Warning, TEXT("INVENTORY IS FULL"));
				return;
			}

			InventoryComponent->SetInventory(PickableItem);
		}
			
		IInteractable::Execute_Interact(hitObject, character);
	}
}

void AYellPointAndPrayCharacter::Server_SetEnumVariable_Implementation(EGameStates gameState) {
	enumVariable = gameState;
}

void AYellPointAndPrayCharacter::Interact() {
	UE_LOG(LogTemp, Warning, TEXT("YOU CALLED INTERACT"));

	if (enumVariable == InWhiteboard) {
		UE_LOG(LogTemp, Warning, TEXT("Closing whiteboard"));
		Server_SetEnumVariable(InGame);
		enumVariable = InGame;
		OnRepState();
	
		ForceNetUpdate();

		whiteboard = Cast<AWhiteBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AWhiteBoard::StaticClass()));
		if (whiteboard) {
			whiteboard->CloseBoard();
			interactWidget->AddToViewport();
		}
		
		whiteboard = nullptr;
		return;
	}
	
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
		{
			this->ServerInteract(hitObject, this);
		}
	}
}

void AYellPointAndPrayCharacter::Caught_Implementation() 
{
	if (!HasAuthority()) return;

	Server_Caugth();

	//AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());

	//if (GameMode) {
	//	UE_LOG(LogTemp, Warning, TEXT("Called Caught Game Over"));
	//	GameMode->GameOver(false);
	//}

	//UE_LOG(LogTemp, Warning, TEXT("YOU GOT CAUGHT NOOB L"));
}

void AYellPointAndPrayCharacter::Server_Caugth_Implementation()
{
	if (!HasAuthority()) return;

	AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode) {
		UE_LOG(LogTemp, Warning, TEXT("Called Caught Game Over"));
		GameMode->GameOver(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("YOU GOT CAUGHT NOOB L"));
}

void AYellPointAndPrayCharacter::Server_UpdateDrawingData_Implementation(FVector2D MouseUV, bool bIsDrawingNow) {
	replicatedMouseUV = MouseUV;
	bReplicatedIsDrawing = bIsDrawingNow;
	OnRep_DrawingData();
}

void AYellPointAndPrayCharacter::OnRep_DrawingData()
{
	whiteboard = Cast<AWhiteBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AWhiteBoard::StaticClass()));
	if (whiteboard) {
		UE_LOG(LogTemp, Warning, TEXT("Drawing?!: %d"), bReplicatedIsDrawing);
		float whiteboardBrushSize = 10.0f;
		
		AYPPCustomPlayerState* customPlayerState = Cast<AYPPCustomPlayerState>(this->GetPlayerState());
		whiteboard->Draw(whiteboardBrushTexture, whiteboardBrushSize, replicatedMouseUV, customPlayerState);
	}
}

void AYellPointAndPrayCharacter::CharacterDrawing(const FInputActionValue& value) {
	isDrawing = true;
	if (isDrawing && enumVariable == InWhiteboard) {
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

		FVector2D NormalizedMousePosition = FVector2D(normalizedX, normalizedY);
		
		playerController->DeprojectScreenPositionToWorld(NormalizedMousePosition.X * viewportSize.X, NormalizedMousePosition.Y * viewportSize.Y, start, direction);
		FVector end = start + (direction * distance);
	
		ECollisionChannel traceChannel = ECC_Visibility;
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
		RV_TraceParams.bTraceComplex = false;
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
			whiteboard = Cast<AWhiteBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AWhiteBoard::StaticClass()));
			whiteboard = Cast<AWhiteBoard>(hitActor);
		
		 	if (whiteboard) {
				float whiteboardBrushSize = 10.0f;
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
				
				Server_UpdateDrawingData(UVCoordinates, isDrawing);
			}
		}
	}
}

void AYellPointAndPrayCharacter::CharacterStopDrawing(const FInputActionValue& value) {
	isDrawing = false;
	Server_UpdateDrawingData(FVector2D::ZeroVector, isDrawing);
	UE_LOG(LogTemp, Warning, TEXT("Stopped drawing"));
}

void AYellPointAndPrayCharacter::TurnOffDetection() {
	cheatsComponent->NotDetectedByGuard();
}
void AYellPointAndPrayCharacter::TeleportToLaserRoom() {
	cheatsComponent->Server_TeleportToLaserRoom(this, FVector(-400.0f, -230.0f, 0.0f));
}
void AYellPointAndPrayCharacter::TeleportToElectricalRoom() {
	cheatsComponent->Server_TeleportToEletricalRoom(this, FVector(-2530.0f, -100.0f, 0.0f));
}

void AYellPointAndPrayCharacter::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
	if (interactWidgetClass)
		AddTraceAndWidget();

	if (KnockGuardWidgetClass && InventoryComponent->GetSlotID(InventoryComponent->CurrentItemSelected) == 1)
		AddKnockGuardWidget();

	HandMovement(DeltaTime);
}

void AYellPointAndPrayCharacter::ChangeKebabEffect(bool state)
{
	KebabEffect = state;
	InventoryComponent->DeleteInventorySlot(InventoryComponent->CurrentItemSelected);
	this->ServerDeleteItem();

	//Get random time
	int32 RandTime = FMath::RandRange(0, 120);
	UE_LOG(LogTemp, Warning, TEXT("FirstRandTime: %d"), RandTime);

	FTimerHandle NewTimerHandle;
	KebabTimerHandle = NewTimerHandle;

	EatAudioPlayer->Sound = EatSound;
	EatAudioPlayer->Play();

	GetWorld()->GetTimerManager().SetTimer(KebabTimerHandle, this, &AYellPointAndPrayCharacter::PlayKebabEffect, RandTime, false);
}

void AYellPointAndPrayCharacter::PlayKebabEffect()
{
	if (!KebabEffect || !KebabTimerHandle.IsValid()) { UE_LOG(LogTemp, Warning, TEXT("KEBAB EFFECT ENDED OR TIMER ISN'T VALID"));  return;}

	//Get random time
	int32 RandTime = FMath::RandRange(0, 120);
	//Get random fart
	int32 RandSound = FMath::RandRange(0, 4);

	UE_LOG(LogTemp, Warning, TEXT("RandTime: %d"), RandTime);
	UE_LOG(LogTemp, Warning, TEXT("RandSound: %d"), RandSound);
	
	GetWorld()->GetTimerManager().SetTimer(KebabTimerHandle, this, &AYellPointAndPrayCharacter::PlayKebabEffect, RandTime, false);

	FartAudioPlayer->Sound = KebabFartSoundsList[RandSound];
	FartAudioPlayer->Play();
	CallGuard();
}

void AYellPointAndPrayCharacter::CallGuard() {
	UE_LOG(LogTemp, Warning, TEXT("Fart Fart Bitch"));
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGuard::StaticClass(), FoundActors);
	for (auto& FoundActor : FoundActors)
	{
		if (!Cast<AGuard>(FoundActor))
			break;

		FVector Loc = FoundActor->GetActorTransform().GetLocation();

		//		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Loc.X);

		FVector Loc2 = GetMesh()->GetComponentLocation();

		UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), Loc, Loc2);
		float da = NavPath->GetPathLength();
		if (da < 4000) {
			UE_LOG(LogTemp, Warning, TEXT("Close Enough"));

			FVector duckLoc = GetActorTransform().GetLocation();
			Cast<AGuard>(FoundActor)->Called(duckLoc);
		}
	}
}

void AYellPointAndPrayCharacter::HandMovement(float DeltaTime)
{
	FVector TargetLocation = HandsPos->GetComponentLocation();
	FVector CurrentLocation = Hands2->GetComponentLocation();
	FVector Dir = TargetLocation - CurrentLocation;
	FRotator CurrentRotation = Hands2->GetComponentRotation();
	FRotator TargetRotation = HandsPos->GetComponentRotation();
	FRotator Rot = TargetRotation - CurrentRotation;

	if (Dir.Length() > 10) {
		CurrentLocation += Dir.GetSafeNormal() * DeltaTime * 400;
	}
	else {
		CurrentLocation = TargetLocation;
	}

	if (!Rot.IsNearlyZero(0.1f)) {
		CurrentRotation += Rot.GetNormalized() * DeltaTime * 50;
	}
	else {
		CurrentRotation = TargetRotation;
	}

	Hands2->SetWorldLocation(CurrentLocation);
	Hands2->SetWorldRotation(CurrentRotation);


	FVector ToBody = FirstPersonCameraComponent->GetComponentLocation() - Hands2->GetComponentLocation();
	if (ToBody.Length() > OriginalDiff + 10) {
		FVector Final = ToBody.GetSafeNormal();
		Final *= ToBody.Length() - OriginalDiff;
		Final += CurrentLocation;
		Hands2->SetWorldLocation(Final);
	}
}

void AYellPointAndPrayCharacter::GoToAppartment() {
	UE_LOG(LogTemp, Warning, TEXT("Code Works im the best :D!"));
	if (IsOutSide) {
		Server_GoToAppartment();
	}
}

void AYellPointAndPrayCharacter::Server_GoToAppartment_Implementation() {
	if (IsOutSide) {
		Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode())->BackToMainMenu();
	}
}

//Cesar Stuff -----------------------------------------------------------------------

void AYellPointAndPrayCharacter::OnItemAdded_Implementation(const FString& Name) {
	UE_LOG(LogTemp, Warning, TEXT("Code Works im the best :D!"));
}