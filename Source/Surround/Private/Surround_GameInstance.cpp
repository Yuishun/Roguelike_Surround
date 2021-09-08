// Fill out your copyright notice in the Description page of Project Settings.


#include "Surround_GameInstance.h"

USurround_GameInstance* USurround_GameInstance::GetInstance()
{
    USurround_GameInstance* instance = nullptr;

    if (GEngine)
    {
        FWorldContext* context = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
        instance = Cast<USurround_GameInstance>(context->OwningGameInstance);
    }

    return instance;
}