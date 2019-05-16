// Fill out your copyright notice in the Description page of Project Settings.

#include "VrBlackFunctionLibrary.h"
#include "VrBlackTicker.h"
#include "Engine/Texture.h"
#include "HeadMountedDisplayFunctionLibrary.h"

void UVrBlackFunctionLibrary::BeforeOpenLevel(class UTexture* Texture)
{
	if (!Texture)
	{
		return;
	}

	Texture->AddToRoot();

	extern void RegBlack(UTexture* Texture);
	RegBlack(Texture);
}


void UVrBlackFunctionLibrary::ManualDisableRender(const UObject* WorldContextObject, class UTexture* Texture)
{
	if (!Texture)
	{
		return;
	}

	extern void EnableManualBlack(class UWorld* World, UTexture* Texture);
	EnableManualBlack(WorldContextObject->GetWorld(), Texture);

	//UHeadMountedDisplayFunctionLibrary::EnableHMD(false);
}
void UVrBlackFunctionLibrary::ManualEnableRender(const UObject* WorldContextObject)
{
	extern void DisableManualBlack(class UWorld* World);
	DisableManualBlack(WorldContextObject->GetWorld());

	//UHeadMountedDisplayFunctionLibrary::EnableHMD(true);
}

void UVrBlackFunctionLibrary::MySleep(float t)
{
	FPlatformProcess::Sleep(t);
}