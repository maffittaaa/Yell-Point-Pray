#include "ServerButton.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include <YellPointAndPrayPlayerController.h>

UServerButton::UServerButton()
{
    OnClicked.AddDynamic(this, &UServerButton::OnClick);
}

void UServerButton::SetSessionInfo(const FMatchSessionInfo& inInfo)
{
    SessionInfo = inInfo;
}

void UServerButton::OnClick()
{
    if (GetOuter() && GetOuter()->GetWorld())
    {
        if (APlayerController* PC = GetOuter()->GetWorld()->GetFirstPlayerController())
        {
            UE_LOG(LogTemp, Warning, TEXT("Connecting to %s:%d"), *SessionInfo.ServerIp, SessionInfo.ServerPort);
            FString Cmd = FString::Printf(TEXT("open %s:%d"), *SessionInfo.ServerIp, SessionInfo.ServerPort);
            PC->ConsoleCommand(*Cmd);
        }
    }
}