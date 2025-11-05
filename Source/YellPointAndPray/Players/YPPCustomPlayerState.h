#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "YPPCustomPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	None    UMETA(DisplayName = "None"),
	Blind   UMETA(DisplayName = "Blind"),
	Mute    UMETA(DisplayName = "Mute"),
	Deaf    UMETA(DisplayName = "Deaf")
};

UCLASS()
class YELLPOINTANDPRAY_API AYPPCustomPlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	float TimePassed = 0;
	bool LevelLoaded = false;
	void ChangeToLevel(FName LevelName);

public:
	AYPPCustomPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere)
	bool IsHost = false;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerType, BlueprintReadOnly, Category = "Role")
	EPlayerType PlayerType = EPlayerType::None;

	UFUNCTION()
	void OnRep_PlayerType();

	// Server-side settEPlayerType
	void SetPlayerType(EPlayerType NewType);

	virtual void Tick(float DeltaTime) override;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};