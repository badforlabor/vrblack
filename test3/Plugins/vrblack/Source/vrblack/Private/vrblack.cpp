// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "vrblack.h"
#include "VrBlackPrivate.h"
#include "VrBlackTicker.h"

DEFINE_LOG_CATEGORY(LogVrBlack);

#define LOCTEXT_NAMESPACE "FvrblackModule"


TSharedPtr<FVrBlackTicker>	SplashTicker = NULL;
TSharedPtr<FVrBlackTickerManual>	ManualSplashTicker = NULL;

extern void RegBlack(UTexture* Texture)
{
	if (!SplashTicker.IsValid())
	{
		SplashTicker = MakeShareable(new FVrBlackTicker());
		SplashTicker->Init();
	}
	SplashTicker->Texture = Texture;
}
extern void EnableManualBlack(class UWorld* World, UTexture* Texture)
{
	if (!ManualSplashTicker.IsValid())
	{
		ManualSplashTicker = MakeShareable(new FVrBlackTickerManual());
		ManualSplashTicker->Init();
	}
	ManualSplashTicker->BeginBlack(Texture, World);
}
extern void DisableManualBlack(class UWorld* World)
{
	if (ManualSplashTicker.IsValid())
	{
		ManualSplashTicker->EndBlack(World);
	}
}

static FVrBlackAuto AutoVrBlack;
extern void EnableAutoVrBlack(UTexture* Texture, bool Auto)
{
	AutoVrBlack.SetEnableAuto(Texture, Auto);
}
extern void EnableVrBlack(UTexture* Texture)
{
	AutoVrBlack.DoBlack(Texture, true);
}
extern void DisableVrBlack()
{
	AutoVrBlack.DoBlack(NULL, false);
}

void FvrblackModule::StartupModule()
{
	UE_LOG(LogVrBlack, Display, TEXT("FvrblackModule::StartupModule"));
	AutoVrBlack.Init();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FvrblackModule::ShutdownModule()
{
	UE_LOG(LogVrBlack, Display, TEXT("FvrblackModule::ShutdownModule"));

	if (SplashTicker.IsValid())
	{
		SplashTicker->OnDestroy();
		SplashTicker = NULL;
	}
	if (ManualSplashTicker.IsValid())
	{
		ManualSplashTicker->OnDestroy();
		ManualSplashTicker = NULL;
	}
	AutoVrBlack.Destroy();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FvrblackModule, vrblack)