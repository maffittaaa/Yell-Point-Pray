// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WhiteBoard.h"
#include "GameFramework/Character.h"
#include "UI/Inventory/Inventory.h"
#include "Interfaces/Caughtable.h"
#include "Interfaces/Usable.h"
#include "Logging/LogMacros.h"
#include "CesarClass/InventoryObserver.h"
#include "Items/RubberDuck/RubberDuckUsable.h"
#include <Obstacles/Guard/Guard.h>
#include "Blueprint/UserWidget.h"
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

UCLASS(abstract)
class AYellPointAndPrayCharacter : public ACharacter, public ICaughtable, public IInventoryObserver
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

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
		
	public:
		AYellPointAndPrayCharacter();

		UUserWidget* GetGameOverWidget() const { return GameOverWidget; }

		UFUNCTION(Client, Reliable)
		void Client_KnockGuard(AGuard* GuardToKnock);

		UFUNCTION(Server, Reliable, WithValidation)
		void Server_KnockGuard(AGuard* GuardToKnock);

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory")
		class UInventory* InventoryComponent;

		void Duck();
		void StopDuck();

		// void CharacterStartDrawing(const FInputActionValue& value);
		void CharacterDrawing(const FInputActionValue& value);
		void CharacterStopDrawing(const FInputActionValue& value);
	
		bool isDrawing = false;

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CurrentGuard")
		AGuard* CurrentGuard = nullptr;

		UPROPERTY()
		UTexture2D* brushTexture;

		UPROPERTY()
		UUserWidget* interactWidget;

		UPROPERTY()
		UUserWidget* KnockGuardWidget;

		UPROPERTY()
		UUserWidget* GameOverWidget;

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

		UPROPERTY()
		UUserWidget* paintBrushWidget;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> paintBrushWidgetClass;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		TSoftObjectPtr<AWhiteBoard> whiteboard;

		UPROPERTY(EditAnywhere, Category = "Whiteboard")
		UTexture2D* whiteboardBrushTexture;

		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Camera")
		TSoftObjectPtr<ACameraActor> whiteboardCamera;
	
		FHitResult RV_Hit;
		bool bHit;
		void AddTraceAndWidget();
		void AddKnockGuardWidget();

		UPROPERTY(ReplicatedUsing=OnRepState)
		TEnumAsByte<EGameStates> enumVariable;

		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Player Controller")
		APlayerController* playerController;

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

	protected:
		
		virtual void BeginPlay() override;
		/** Set up input action bindings */
		virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
		
		UClass* HoldingItemClass = nullptr;


		UPROPERTY(ReplicatedUsing = OnRep_HoldingItem)
		AActor* HoldingItem;

		UFUNCTION()
		void OnRep_HoldingItem();

		UPROPERTY(Replicated)
		bool ItemCreated = false;

		int OldItemSelected = 0;

		UPROPERTY(Replicated)
		ARubberDuckUsable* DuckUsing;

		virtual void Tick(float DeltaTime) override;

	public:
		UFUNCTION(Client, Reliable)
		void Client_ShowGameOver();

		UFUNCTION(BlueprintCallable)
		void OnItemSelected(int SlotID);

		/** Returns the first person mesh **/
		USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

		/** Returns first person camera component **/
		UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

		virtual void Caught_Implementation() override;
	
		UFUNCTION()
		void OnRepState();

		UFUNCTION(Server, Reliable)
		void ThrowDuck(ARubberDuckUsable* Duck, int ItemSelect);

	private:
		ACharacter* ItemOwner;

		UFUNCTION()
		void Interact();
		
		UFUNCTION()
		void Use();

		UFUNCTION()
		void Drop();

		UFUNCTION(Server, Reliable)
		void ServerOnItemDroped(int SlotID, FVector start, FRotator dir);

		UFUNCTION(Server, Reliable)
		void ServerInteract(AActor* hitObject, AYellPointAndPrayCharacter* character);

		UFUNCTION(Server, Reliable)
		void ServerOnItemSelected(int SlotID);

		UFUNCTION(Server, Reliable)
		void ServerDeleteItem();

		void DeleteItem();

		UFUNCTION(Server, Reliable)
		void Server_UseItem(int SlotID);

		virtual void Server_UseItem_Implementation(int SlotID);

		void CallDuck();

		public:


		UFUNCTION(Server, Reliable)
		void GetDuck(ARubberDuckUsable* Duck);


		//Cesar Stuff -----------------------------------------
		virtual  void OnItemAdded_Implementation(const FString& Name) override;
};
