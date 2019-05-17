// Fill out your copyright notice in the Description page of Project Settings.

#include "VrBlackFunctionLibrary.h"
#include "VrBlackTicker.h"
#include "Engine/Texture.h"
#include "HeadMountedDisplayFunctionLibrary.h"

extern void EnableAutoVrBlack(UTexture* Texture, bool Auto);
extern void EnableVrBlack(UTexture* Texture);
extern void DisableVrBlack();

void UVrBlackFunctionLibrary::BeforeOpenLevel(class UTexture* Texture)
{
	EnableAutoVrBlack(Texture, true);
	//if (!Texture)
	//{
	//	return;
	//}

	//Texture->AddToRoot();

	//extern void RegBlack(UTexture* Texture);
	//RegBlack(Texture);
}

void UVrBlackFunctionLibrary::EnableAutoBlack(class UTexture* Texture)
{
	EnableAutoVrBlack(Texture, true);
}

void UVrBlackFunctionLibrary::DisableAutoBlack()
{
	EnableAutoVrBlack(NULL, false);
}
void UVrBlackFunctionLibrary::EnableVrBlack(class UTexture* Texture)
{
	::EnableVrBlack(Texture);
}
void UVrBlackFunctionLibrary::DisableVrBlack()
{
	::DisableVrBlack();
}

void UVrBlackFunctionLibrary::ManualDisableRender(const UObject* WorldContextObject, class UTexture* Texture)
{
	EnableVrBlack(Texture);
	//if (!Texture)
	//{
	//	return;
	//}

	//extern void EnableManualBlack(class UWorld* World, UTexture* Texture);
	//EnableManualBlack(WorldContextObject->GetWorld(), Texture);

	//UHeadMountedDisplayFunctionLibrary::EnableHMD(false);
}
void UVrBlackFunctionLibrary::ManualEnableRender(const UObject* WorldContextObject)
{
	DisableVrBlack();
	//extern void DisableManualBlack(class UWorld* World);
	//DisableManualBlack(WorldContextObject->GetWorld());

	//UHeadMountedDisplayFunctionLibrary::EnableHMD(true);
}

void UVrBlackFunctionLibrary::MySleep(float t)
{
	FPlatformProcess::Sleep(t);
}