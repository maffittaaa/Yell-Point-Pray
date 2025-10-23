// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UI/Inventory/Inventory.h"
#include "Interfaces/Caughtable.h"
#include "Interfaces/Usable.h"
#include "Logging/LogMacros.h"
#include "YellPointAndPrayCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AYellPointAndPrayCharacter : public ACharacter, public ICaughtable
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
		
	public:
		AYellPointAndPrayCharacter();

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory")
		class UInventory* InventoryComponent;

		void Duck();
		void StopDuck();

		UPROPERTY()
		UUserWidget* HUDWidget;

		UPROPERTY()
		int TimesWidgetCreated = 0;

		UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<UUserWidget> widgetClass;
	
			
		FHitResult RV_Hit;
		bool bHit;
		void AddTraceAndWidget();

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

		virtual void Tick(float DeltaTime) override;

	public:

		UFUNCTION(BlueprintCallable)
		void OnItemSelected(int SlotID);

		/** Returns the first person mesh **/
		USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

		/** Returns first person camera component **/
		UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

		virtual void Caught_Implementation() override;

	private:
		ACharacter* ItemOwner;

		UFUNCTION()
		void Interact();
		
		UFUNCTION()
		void Use();

		UFUNCTION(Server, Reliable)
		void ServerInteract(AActor* hitObject, AYellPointAndPrayCharacter* character);

		UFUNCTION(Server, Reliable)
		void ServerOnItemSelected(int SlotID);

		UFUNCTION(Server, Reliable)
		void ServerDeleteItem();

		void DeleteItem();
};


