#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
//#include "eos_rtc_audio.h"
//#include "eos_rtc.h"
//#include "eos.h"
#include <ThirdParty/EOSSDK/SDK/Include/eos_base.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_init.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_sdk.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_rtc.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_types.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_rtc_audio.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_rtc_types.h>
#include "EOSVoiceSubsystem.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API UEOSVoiceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Call this when player joins/leaves game
    UFUNCTION(BlueprintCallable)
    void JoinVoiceRoom(const FString& RoomName);

    UFUNCTION(BlueprintCallable)
    void LeaveVoiceRoom();

    UFUNCTION(BlueprintCallable)
    void LoginToEOS();

    // Proximity management
    void UpdatePlayerPosition(const FString& PlayerId, const FVector& Position);
    void SetLocalPlayerPosition(const FVector& Position);

    // Volume control
    void SetPlayerVolume(const FString& PlayerId, float Volume);

    EOS_HPlatform EOSPlatformHandle;
    EOS_HRTC EOSRTCHandle;
    EOS_HRTCAudio EOSAudioHandle;  // Added separate audio handle
    EOS_ProductUserId LocalUserProductId;
    FString CurrentRoomName;

protected:
    TMap<FString, EOS_ProductUserId> PlayerIdToProductUserId;
    TMap<FString, FVector> PlayerPositions;
    FVector LocalPlayerPosition;
    float MaxHearingDistance = 5000.0f; // Adjust as needed

    void InitializeEOS();
    void OnRTCJoinRoomComplete(const EOS_RTC_JoinRoomCallbackInfo* Data);
    void UpdateProximityVolumes();

    static void EOS_CALL OnRTCJoinRoomCompleteFn(const EOS_RTC_JoinRoomCallbackInfo* Data);
};