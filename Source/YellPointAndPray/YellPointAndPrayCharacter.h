// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WhiteBoard.h"
#include "GameFramework/Character.h"
#include "UI/Inventory/Inventory.h"
#include "Interfaces/Reset.h"
#include "Interfaces/Caughtable.h"
#include "Interfaces/Usable.h"
#include "Logging/LogMacros.h"
#include "CesarClass/InventoryObserver.h"
#include "Items/RubberDuck/RubberDuckUsable.h"
#include <Obstacles/Guard/Guard.h>
#include "Blueprint/UserWidget.h"
#include "Players/ACCheats.h"
#include "Components/AudioComponent.h"
#include <Components/SpotLightComponent.h>
#include "YellPointAndPrayCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum EGameStates {
	InWhiteboard,
	InGame,
};

UENUM(BlueprintType, Category = "Animation")
enum EAnimationState : uint8
{
	Idle,
	ThumbsUp,
	Stop,
	No,
	Pointing,
	Emote5,
	Emote6
};

UCLASS(abstract)
class AYellPointAndPrayCharacter : public ACharacter, public ICaughtable, public IInventoryObserver, public IReset
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Hands;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Hands2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HandsPos;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* FartAudioPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EatAudioPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* BonkAudioPlayer;

	protected:

		/** Jump Input Action */
		UPROPERTY(EditAnywhere, Category ="Input")
		UInputAction* JumpAction;

		/** Move Input Action */
		UPROPERTY(EditAnywhere, Category ="Input")
		UInputAction* MoveAction;

		UPROPERTY(EditAnywhere, Category="Input")
		UInputAction* CrouchAction;

		/** Look Input Action */
		UPROPERTY(EditAnywhere, Category ="Input")
		class UInputAction* LookAction;

		/** Mouse Look Input Action */
		UPROPERTY(EditAnywhere, Category ="Input")
		class UInputAction* MouseLookAction;

		//Interact Input
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		class UInputAction* InteractAction;

		//Use Input
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		class UInputAction* UseAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		class UInputAction* DropAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* DrawAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* MouseRelease;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* TurnOffDetectionAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* TeleportToLaserRoomAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* TeleportToElectricalRoomAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* EmoteWheelAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* GoBackToAppartment;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* PauseAction;

		//Animation State
		UPROPERTY(ReplicatedUsing = OnRepAnimationState, BlueprintReadWrite, Category = "Animations")
		TEnumAsByte<EAnimationState> animationState = Idle;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<USoundWave*> KebabFartSoundsList;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundWave* BonkSound;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundWave* EatSound;

		bool KebabEffect = false;
		FTimerHandle KebabTimerHandle;

		bool UseActive = true;

	public:
		UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Light")
		USpotLightComponent* PlayerLight;

		void SetUseActive(bool Active);

		void Pause();

		UFUNCTION(Server, Reliable)
		void ChangeKebabEffect(bool state);

		void PlayKebabEffect();
		void CallGuard();

		UFUNCTION(NetMulticast, Reliable)
		void Multicast_TurnOffLight();

		FInputModeGameOnly PreviousInputMode;

		AYellPointAndPrayCharacter();

		UUserWidget* GetGameOverWidget() const { return GameOverWidget; }

		UFUNCTION(Client, Reliable)
		void Client_KnockGuard(AActor* CurrentKnockableActor);

		UFUNCTION(Server, Reliable, WithValidation)
		void Server_KnockGuard(AActor* CurrentKnockableActor);

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory")
		class UInventory* InventoryComponent;

		void Duck();
		void StopDuck();

		void TurnOffDetection();
		void TeleportToLaserRoom();
		void TeleportToElectricalRoom();

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CurrentGuard")
		AActor* KnockableActor = nullptr;

		UPROPERTY()
		UTexture2D* brushTexture;

		UPROPERTY()
		UUserWidget* interactWidget;

		UPROPERTY()
		UUserWidget* KnockGuardWidget;

		UPROPERTY()
		UUserWidget* GameOverWidget;

		UPROPERTY()
		UUserWidget* ReadyWidget;

		UPROPERTY()
		int TimesWidgetCreated = 0;

		UPROPERTY()
		int TimesKnockWidgetCreated = 0;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> interactWidgetClass;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> KnockGuardWidgetClass;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> GameOverWidgetClass;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> ReadyWidgetClass;
	
		//Whiteboard interaction
		UPROPERTY()
		UUserWidget* paintBrushWidget;

		UPROPERTY()
		UUserWidget* closingBoardWidget;

		UPROPERTY()
		UUserWidget* clearBoardWidget;

		UPROPERTY()
		UUserWidget* confirmationWidget;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> paintBrushWidgetClass;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> closingBoardWidgetClass;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> clearBoardWidgetClass;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> confirmationWidgetClass;
	
		UFUNCTION()
		void OnCloseButtonClicked();
	
		UFUNCTION()
		void OnClearButtonClicked();

		UFUNCTION()
		void OnConfirmationButtonClicked();

		UFUNCTION()
		void OnRefuseButtonClicked();
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		AWhiteBoard* whiteboard;

		UPROPERTY(EditAnywhere, Category = "Whiteboard")
		UTexture2D* whiteboardBrushTexture;

		UPROPERTY(EditAnywhere, Category = "Whiteboard")
		UMaterial* whiteboardBrushMaterial;

		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Camera")
		TSoftObjectPtr<AActor> whiteboardCamera;

		UPROPERTY(ReplicatedUsing = OnRep_DrawingData)
		FVector2D replicatedMouseUV;

		UPROPERTY(Replicated)
		bool bReplicatedIsDrawing;

		void CharacterDrawing(const FInputActionValue& value);
		void CharacterStopDrawing(const FInputActionValue& value);
			
		bool isDrawing = false;

		UFUNCTION()
		void OnRep_DrawingData();
		
		UFUNCTION(Server, Reliable)
		void Server_UpdateDrawingData(FVector2D mouseUV, bool bIsDrawingNow);
		//Whiteboard end of interaction

		UPROPERTY(BlueprintReadOnly)
		bool bMovementInputEnabled = true;
		    
		UPROPERTY(BlueprintReadOnly) 
		bool bLookInputEnabled = true;
	
		UFUNCTION(BlueprintCallable)
		void SetMovementInputEnabled(bool bEnabled);
	    
		UFUNCTION(BlueprintCallable)
		void SetLookInputEnabled(bool bEnabled);
	
		FHitResult RV_Hit;
		bool bHit;
		void AddTraceAndWidget();
		void AddKnockGuardWidget();

		UPROPERTY(ReplicatedUsing=OnRepState)
		TEnumAsByte<EGameStates> enumVariable = InGame;

		UFUNCTION(Server, Reliable)
		void Server_SetEnumVariable(EGameStates gameState);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Controller")
		APlayerController* playerController;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Cheats")
		class UACCheats* cheatsComponent;

		//Animations and Emote Wheel
		UFUNCTION()
		void OnRepAnimationState();

		UFUNCTION()
		void TurnOnEmoteWheel();

		UFUNCTION()
		void TurnOffEmoteWheel();

		UPROPERTY()
		UUserWidget* emoteWheelWidget;

		UPROPERTY(EditAnywhere, Category = "Animations")
		TSubclassOf<UUserWidget> emoteWheelWidgetClass;

		UPROPERTY(Replicated)
		bool IsOutSide = false;

		void GoToAppartment();

		UFUNCTION(Server, Reliable)
		void Server_GoToAppartment();

	protected:

		/** Called from Input Actions for movement input */
		void MoveInput(const FInputActionValue& Value);

		/** Called from Input Actions for looking input */
		void LookInput(const FInputActionValue& Value);

		/** Handles aim inputs from either controls or UI interfaces */
		UFUNCTION(BlueprintCallable, Category="Input")
		virtual void DoAim(float Yaw, float Pitch);

		/** Handles move inputs from either controls or UI interfaces */
		UFUNCTION(BlueprintCallable, Category="Input")
		virtual void DoMove(float Right, float Forward);

		/** Handles jump start inputs from either controls or UI interfaces */
		UFUNCTION(BlueprintCallable, Category="Input")
		virtual void DoJumpStart();

		/** Handles jump end inputs from either controls or UI interfaces */
		UFUNCTION(BlueprintCallable, Category="Input")
		virtual void DoJumpEnd();

		virtual void BeginPlay() override;
		/** Set up input action bindings */
		virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
		
		UClass* HoldingItemClass = nullptr;
	
		UPROPERTY(ReplicatedUsing = OnRep_HoldingItem)
		AActor* HoldingItem;

		UFUNCTION()
		void OnRep_HoldingItem();

		void HandMovement(float DeltaTime);

		float OriginalDiff;

		UPROPERTY(Replicated)
		bool ItemCreated = false;

		int OldItemSelected = 0;

		UPROPERTY(Replicated)
		ARubberDuckUsable* DuckUsing;

		virtual void Tick(float DeltaTime) override;

	public:
		FVector StartLocation;
		FQuat StartRotation;

		UFUNCTION(Client, Reliable)
		void Client_ShowGameOver(bool State);

		UFUNCTION(Client, Reliable)
		void Client_HideGameOver();

		UFUNCTION(BlueprintCallable)
		void OnItemSelected(int SlotID);

		/** Returns the first person mesh **/
		USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

		/** Returns first person camera component **/
		UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

		virtual void Reset_Implementation() override;

		void RestoreTravelInventory();

		virtual void Caught_Implementation() override;
	
		UFUNCTION()
		void OnRepState();

		UFUNCTION(Server, Reliable)
		void ThrowDuck(ARubberDuckUsable* Duck, int ItemSelect);

		void DeleteItem();

		UFUNCTION(Server, Reliable)
		void ServerDeleteItem();

	private:
		ACharacter* ItemOwner;

		UFUNCTION()
		void Interact();
		
		UFUNCTION()
		void Use();

		UFUNCTION()
		void Drop();

		UFUNCTION(Server, Reliable)
		void ServerOnItemDroped(int SlotID, FVector HitPoint, FVector SpacePoint);

		UFUNCTION(Server, Reliable)
		void ServerInteract(AActor* hitObject, AYellPointAndPrayCharacter* character);

		UFUNCTION(Server, Reliable)
		void ServerOnItemSelected(int SlotID);

		UFUNCTION(Server, Reliable)
		void Server_UseItem(int SlotID);

		virtual void Server_UseItem_Implementation(int SlotID);

		void CallDuck();

		UPROPERTY(ReplicatedUsing = OnRep_ClearBoard)
		bool bClearBoard = false;

		UFUNCTION()
		void OnRep_ClearBoard();

		UFUNCTION(Server, Reliable)
		void Server_ClearBoard(bool bClear);

	public:
		UFUNCTION(Server, Reliable)
		void GetDuck(ARubberDuckUsable* Duck);

		UFUNCTION(Server, Reliable)
		void Server_Caugth();

		//Cesar Stuff -----------------------------------------
		virtual  void OnItemAdded_Implementation(const FString& Name) override;
};
