#include "EOSVoiceSubsystem.h"
//#include "eos_init.h"
//#include "eos_sdk.h"
#include <ThirdParty/EOSSDK/SDK/Include/eos_base.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_init.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_sdk.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_rtc.h>
#include <ThirdParty/EOSSDK/SDK/Include/eos_rtc_audio.h>
#include "Engine/Engine.h"

void UEOSVoiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializeEOS();
}

void UEOSVoiceSubsystem::Deinitialize()
{
    LeaveVoiceRoom();

    if (EOSPlatformHandle)
    {
        EOS_Platform_Release(EOSPlatformHandle);
    }

    Super::Deinitialize();
}

void UEOSVoiceSubsystem::InitializeEOS()
{
    EOS_InitializeOptions InitializeOptions = {};
    InitializeOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
    InitializeOptions.ProductName = "CellPointAndPray";
    InitializeOptions.ProductVersion = "1.0";

    EOS_Initialize(&InitializeOptions);

    EOS_Platform_Options PlatformOptions = {};
    PlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
    PlatformOptions.ProductId = "ea0509ec50fa4f8eaf1064";
    PlatformOptions.SandboxId = "31d30a127fe743af8dd5f";
    PlatformOptions.ClientCredentials.ClientId = "xyzaf789192gHmv69ChJ";
    PlatformOptions.ClientCredentials.ClientSecret = "dtaxHHjDEsoPBWq3BTt4";
    PlatformOptions.bIsServer = false;

    EOSPlatformHandle = EOS_Platform_Create(&PlatformOptions);
    EOSRTCHandle = EOS_Platform_GetRTCInterface(EOSPlatformHandle);
    EOSAudioHandle = EOS_RTC_GetAudioInterface(EOSRTCHandle);
}

void UEOSVoiceSubsystem::JoinVoiceRoom(const FString& RoomName)
{
    if (!EOSRTCHandle) return;

    CurrentRoomName = RoomName;

    EOS_RTC_JoinRoomOptions JoinOptions = {};
    JoinOptions.ApiVersion = EOS_RTC_JOINROOM_API_LATEST;
    JoinOptions.LocalUserId = LocalUserProductId;
    JoinOptions.RoomName = TCHAR_TO_UTF8(*RoomName);
    JoinOptions.ClientBaseUrl = nullptr;

    EOS_RTC_JoinRoom(EOSRTCHandle, &JoinOptions, this, OnRTCJoinRoomCompleteFn);
}

void UEOSVoiceSubsystem::LeaveVoiceRoom()
{
    if (!EOSRTCHandle || CurrentRoomName.IsEmpty()) return;

    EOS_RTC_LeaveRoomOptions LeaveOptions = {};
    LeaveOptions.ApiVersion = EOS_RTC_LEAVEROOM_API_LATEST;
    LeaveOptions.LocalUserId = LocalUserProductId;
    LeaveOptions.RoomName = TCHAR_TO_UTF8(*CurrentRoomName);

    EOS_RTC_LeaveRoom(EOSRTCHandle, &LeaveOptions, nullptr, nullptr);
    CurrentRoomName = "";
}

void UEOSVoiceSubsystem::SetLocalPlayerPosition(const FVector& Position)
{
    LocalPlayerPosition = Position;
    UpdateProximityVolumes();
}

void UEOSVoiceSubsystem::UpdatePlayerPosition(const FString& PlayerId, const FVector& Position)
{
    PlayerPositions.Add(PlayerId, Position);
    UpdateProximityVolumes();
}

void UEOSVoiceSubsystem::UpdateProximityVolumes()
{
    for (const auto& PlayerPair : PlayerPositions)
    {
        const FString& PlayerId = PlayerPair.Key;
        const FVector& PlayerPos = PlayerPair.Value;

        float Distance = FVector::Distance(LocalPlayerPosition, PlayerPos);
        float Volume = 0.0f;

        if (Distance <= MaxHearingDistance)
        {
            Volume = 1.0f - (Distance / MaxHearingDistance);
            Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
        }

        SetPlayerVolume(PlayerId, Volume);
    }
}

void UEOSVoiceSubsystem::SetPlayerVolume(const FString& PlayerId, float Volume)
{
    if (!EOSRTCHandle || CurrentRoomName.IsEmpty()) return;

    if (EOS_ProductUserId* RemoteUserId = PlayerIdToProductUserId.Find(PlayerId))
    {
        // Enable/disable audio reception based on volume
        EOS_RTCAudio_UpdateReceivingOptions UpdateOptions = {};
        UpdateOptions.ApiVersion = EOS_RTCAUDIO_UPDATERECEIVING_API_LATEST;
        UpdateOptions.LocalUserId = LocalUserProductId;
        UpdateOptions.RoomName = TCHAR_TO_UTF8(*CurrentRoomName);
        UpdateOptions.ParticipantId = *RemoteUserId;
        UpdateOptions.bAudioEnabled = (Volume > 0.0f) ? EOS_TRUE : EOS_FALSE;

        EOS_RTCAudio_UpdateReceiving(EOSAudioHandle, &UpdateOptions, nullptr, nullptr);

        // Set volume level (0-100)
        EOS_RTCAudio_UpdateParticipantVolumeOptions VolumeOptions = {};
        VolumeOptions.ApiVersion = EOS_RTCAUDIO_UPDATEPARTICIPANTVOLUME_API_LATEST;
        VolumeOptions.LocalUserId = LocalUserProductId;
        VolumeOptions.RoomName = TCHAR_TO_UTF8(*CurrentRoomName);
        VolumeOptions.ParticipantId = *RemoteUserId;
        VolumeOptions.Volume = (int32_t)(Volume * 100.0f);

        EOS_RTCAudio_UpdateParticipantVolume(EOSAudioHandle, &VolumeOptions, nullptr, nullptr);
    }
}

void UEOSVoiceSubsystem::OnRTCJoinRoomComplete(const EOS_RTC_JoinRoomCallbackInfo* Data)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully joined EOS voice room: %s"), *CurrentRoomName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to join EOS voice room: %d"), Data->ResultCode);
    }
}

void UEOSVoiceSubsystem::LoginToEOS()
{
    EOS_HConnect ConnectHandle = EOS_Platform_GetConnectInterface(EOSPlatformHandle);

    EOS_Connect_Credentials Credentials = {};
    Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
    Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN;

    EOS_Connect_LoginOptions LoginOptions = {};
    LoginOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
    LoginOptions.Credentials = &Credentials;

    EOS_Connect_Login(ConnectHandle, &LoginOptions, this, [](const EOS_Connect_LoginCallbackInfo* Data) 
        {
        UEOSVoiceSubsystem* Subsystem = (UEOSVoiceSubsystem*)Data->ClientData;
        if (Data->ResultCode == EOS_EResult::EOS_Success)
        {
            UE_LOG(LogTemp, Warning, TEXT("EOS Login Successful!"));
            Subsystem->LocalUserProductId = Data->LocalUserId;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("EOS Login Failed: %d"), Data->ResultCode);
        }
        });
}

void UEOSVoiceSubsystem::OnRTCJoinRoomCompleteFn(const EOS_RTC_JoinRoomCallbackInfo* Data)
{
    if (Data->ClientData)
    {
        UEOSVoiceSubsystem* VoiceSubsystem = (UEOSVoiceSubsystem*)Data->ClientData;
        VoiceSubsystem->OnRTCJoinRoomComplete(Data);
    }
}