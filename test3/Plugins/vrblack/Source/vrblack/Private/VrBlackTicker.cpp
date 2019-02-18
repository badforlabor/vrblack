#include "VrBlackTicker.h"

#include "TickableObjectRenderThread.h"
//#include "IXRTrackingSystem.h"
//#include "IHeadMountedDisplay.h"
#include "openvr.h"
#include "VrBlackPrivate.h"
#include "Rendercore.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <d3d11.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

FVrBlackTicker::FVrBlackTicker()
	: FTickableObjectRenderThread(false, true)
{
	Texture = NULL;
}
TStatId FVrBlackTicker::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FVrBlackTicker, STATGROUP_Tickables);
}
bool FVrBlackTicker::IsTickable() const
{
	return true;
}
void FVrBlackTicker::RegisterForMapLoad()
{
	FCoreUObjectDelegates::PreLoadMap.AddSP(this, &FVrBlackTicker::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddSP(this, &FVrBlackTicker::OnPostLoadMap);
}

void FVrBlackTicker::UnregisterForMapLoad()
{
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
};

void FVrBlackTicker::OnPreLoadMap(const FString&)
{
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(RegisterAsyncTick,
		FTickableObjectRenderThread*, Ticker, this,
		{
			Ticker->Register();
		});
}
void FVrBlackTicker::OnPostLoadMap(UWorld*)
{
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(UnregisterAsyncTick,
		FTickableObjectRenderThread*, Ticker, this,
		{
			Ticker->Unregister();
		});
}

void FVrBlackTicker::Init()
{
	Texture = NULL;
	RegisterForMapLoad();
}
void FVrBlackTicker::OnDestroy()
{
	if (Texture)
	{
		Texture->RemoveFromRoot();
	}
	Texture = NULL;

	UnregisterForMapLoad();
}

void FVrBlackTicker::Tick(float DeltaTime)
{
	vr::HmdError HmdErr;
	vr::IVRCompositor * VRCompositor = (vr::IVRCompositor*)vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &HmdErr);

	if (!VRCompositor)
	{
		UE_LOG(LogVrBlack, Warning, TEXT("VR Compositor InterfaceErrorCode %i"), (int32)HmdErr);
		return;
	}

	if (!Texture || !Texture->Resource || !Texture->Resource->TextureRHI.IsValid())
	{
		UE_LOG(LogVrBlack, Warning, TEXT("VR Compositor InterfaceErrorCode %i"), (int32)HmdErr);
		return;
	}

	FRHITexture2D* Texture2d = Texture->Resource->TextureRHI->GetTexture2D();
	ID3D11Texture2D* RenderTargetTexture = Texture2d ? (ID3D11Texture2D*)Texture2d->GetNativeResource() : NULL;

	if (RenderTargetTexture)
	{
		vr::TrackedDevicePose_t Poses[vr::k_unMaxTrackedDeviceCount];
		VRCompositor->WaitGetPoses(Poses, ARRAYSIZE(Poses), NULL, 0);

		RenderTargetTexture->AddRef();

		vr::Texture_t Texture;
		Texture.handle = RenderTargetTexture;
		Texture.eType = vr::TextureType_DirectX;
		Texture.eColorSpace = vr::ColorSpace_Auto;

		vr::VRTextureBounds_t LeftBounds;
		LeftBounds.uMin = 0.0f;
		LeftBounds.uMax = 0.5f;
		LeftBounds.vMin = 0.0f;
		LeftBounds.vMax = 1.0f;

		vr::EVRCompositorError Error = VRCompositor->Submit(vr::Eye_Left, &Texture, &LeftBounds);

		vr::VRTextureBounds_t RightBounds;
		RightBounds.uMin = 0.5f;
		RightBounds.uMax = 1.0f;
		RightBounds.vMin = 0.0f;
		RightBounds.vMax = 1.0f;

		Texture.handle = RenderTargetTexture;
		Error = VRCompositor->Submit(vr::Eye_Right, &Texture, &RightBounds);

		static bool FirstError = true;
		if (FirstError && Error != vr::VRCompositorError_None)
		{
			UE_LOG(LogVrBlack, Log, TEXT("Warning:  SteamVR Compositor had an error on present (%d)"), (int32)Error);
			FirstError = false;
		}

		VRCompositor->PostPresentHandoff();

		RenderTargetTexture->Release();
	}
}

FVrBlackTickerManual::FVrBlackTickerManual()
	:FVrBlackTicker()
{
	Counter = 0;
}

void FVrBlackTickerManual::Init()
{
	Counter = 0;
	Texture = NULL;
}
void FVrBlackTickerManual::BeginBlack(UTexture* InTexture, class UWorld* World)
{
	if (!InTexture)
	{
		return;
	}

	if (Counter == 0)
	{
		APlayerController* playerController = World->GetFirstPlayerController();
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(playerController->Player);
		LocalPlayer->ViewportClient->EngineShowFlags.Rendering = 0;
		World->GetGameViewport()->bDisableWorldRendering = true;

		//Texture->GetWorld()->GetGameViewport()->bDisableWorldRendering = true;
		Texture = InTexture;
		Texture->AddToRoot();
		OnPreLoadMap("");
	}

	Counter++;
}
void FVrBlackTickerManual::EndBlack(class UWorld* World)
{
	if (Counter <= 0)
	{
		return;
	}

	Counter--;

	if (Counter == 0)
	{
		APlayerController* playerController = World->GetFirstPlayerController();
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(playerController->Player);
		LocalPlayer->ViewportClient->EngineShowFlags.Rendering = 1;
		World->GetGameViewport()->bDisableWorldRendering = false;

		OnPostLoadMap(NULL);
		if (Texture)
		{
			//Texture->GetWorld()->GetGameViewport()->bDisableWorldRendering = false;
			Texture->RemoveFromRoot();
		}
		Texture = NULL;
	}
}