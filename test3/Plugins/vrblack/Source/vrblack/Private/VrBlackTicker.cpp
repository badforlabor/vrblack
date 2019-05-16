#include "VrBlackTicker.h"

#include "TickableObjectRenderThread.h"
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "openvr.h"
#include "VrBlackPrivate.h"
#include "Rendercore.h"
#include "Engine/Texture.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <d3d11.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

extern void ShowOverlay(UTexture * tex);
extern void HideOverlay();

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

		vr::Texture_t vrTex;
		vrTex.handle = RenderTargetTexture;
		vrTex.eType = vr::TextureType_DirectX;
		vrTex.eColorSpace = vr::ColorSpace_Auto;

		vr::VRTextureBounds_t LeftBounds;
		LeftBounds.uMin = 0.0f;
		LeftBounds.uMax = 0.5f;
		LeftBounds.vMin = 0.0f;
		LeftBounds.vMax = 1.0f;

		vr::EVRCompositorError Error = VRCompositor->Submit(vr::Eye_Left, &vrTex, &LeftBounds);

		vr::VRTextureBounds_t RightBounds;
		RightBounds.uMin = 0.5f;
		RightBounds.uMax = 1.0f;
		RightBounds.vMin = 0.0f;
		RightBounds.vMax = 1.0f;

		vrTex.handle = RenderTargetTexture;
		Error = VRCompositor->Submit(vr::Eye_Right, &vrTex, &RightBounds);

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
static bool BringCompositorToFront()
{
#if 0//!STEAMVR_SUPPORTED_PLATFORM
	UE_LOG(LogVrBlack, Warning, TEXT("Not SteamVR Supported Platform!!"));
	return false;
#else
	vr::HmdError HmdErr;
	vr::IVRCompositor * VRCompositor = (vr::IVRCompositor*)vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &HmdErr);

	if (!VRCompositor)
	{
		UE_LOG(LogVrBlack, Warning, TEXT("VR Compositor InterfaceErrorCode %i"), (int32)HmdErr);
		return false;
	}
	//VRCompositor->FadeToColor(0, 0, 0, 0, 0);
	VRCompositor->CompositorBringToFront();
	//VRCompositor->SuspendRendering(false);

	return true;

#endif
}
static bool BringCompositorToBack()
{
#if 0//!STEAMVR_SUPPORTED_PLATFORM
	UE_LOG(LogVrBlack, Warning, TEXT("Not SteamVR Supported Platform!!"));
	return false;
#else
	vr::HmdError HmdErr;
	vr::IVRCompositor * VRCompositor = (vr::IVRCompositor*)vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &HmdErr);

	if (!VRCompositor)
	{
		UE_LOG(LogVrBlack, Warning, TEXT("VR Compositor InterfaceErrorCode %i"), (int32)HmdErr);
		return false;
	}

	//VRCompositor->FadeToColor(0.001f, 0, 0, 0, 1);
	VRCompositor->CompositorGoToBack();
	//VRCompositor->SuspendRendering(true);

	return true;

#endif

}
static vr::Texture_t CreateOpenVRTexture_t(UTexture * Texture)
{
	vr::Texture_t VRTexture;

	//FRHITexture2D* Texture2d = Texture->Resource->TextureRHI->GetTexture2D();
	//ID3D11Texture2D* RenderTargetTexture = Texture2d ? (ID3D11Texture2D*)Texture2d->GetNativeResource() : NULL;
	//VRTexture.handle = RenderTargetTexture;

	if (Texture)
		VRTexture.handle = Texture->Resource->TextureRHI->GetNativeResource();
	else
		VRTexture.handle = NULL;

	VRTexture.eType = vr::TextureType_DirectX;
	VRTexture.eColorSpace = vr::ColorSpace_Auto;

	return VRTexture;
}
static bool SetSkyboxOverride(UTexture * tFront, UTexture * tBack, UTexture * tLeft, UTexture * tRight, UTexture * tTop, UTexture * tBottom)
{
#if 0//!STEAMVR_SUPPORTED_PLATFORM
	UE_LOG(LogVrBlack, Warning, TEXT("Not SteamVR Supported Platform!!"));
	return false;
#else
	if (!tFront || !tBack || !tLeft || !tRight || !tTop || !tBottom)
	{
		UE_LOG(LogVrBlack, Warning, TEXT("Bad texture passed in to SetSkyBoxOverride"));
		return false;
	}

	vr::HmdError HmdErr;
	vr::IVRCompositor * VRCompositor = (vr::IVRCompositor*)vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &HmdErr);

	if (!VRCompositor)
	{
		UE_LOG(LogVrBlack, Warning, TEXT("VR Compositor InterfaceErrorCode %i"), (int32)HmdErr);
		return false;
	}

	vr::Texture_t TextureArray[6];

	TextureArray[0] = CreateOpenVRTexture_t(tFront);
	TextureArray[1] = CreateOpenVRTexture_t(tBack);
	TextureArray[2] = CreateOpenVRTexture_t(tLeft);
	TextureArray[3] = CreateOpenVRTexture_t(tRight);
	TextureArray[4] = CreateOpenVRTexture_t(tTop);
	TextureArray[5] = CreateOpenVRTexture_t(tBottom);

	vr::EVRCompositorError CompositorError;
	CompositorError = VRCompositor->SetSkyboxOverride(TextureArray, 6);

	if (CompositorError != vr::VRCompositorError_None)
	{
		UE_LOG(LogVrBlack, Warning, TEXT("VR Compositor Error %i"), (int32)CompositorError);
		return false;
	}

	return true;
#endif
}

void FVrBlackTickerManual::BeginBlack(UTexture* InTexture, class UWorld* World)
{
	SetSkyboxOverride(InTexture, InTexture, InTexture, InTexture, InTexture, InTexture);
	ShowOverlay(InTexture);
	//BringCompositorToBack();
	return;

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
	HideOverlay();
	//BringCompositorToFront();
	return;

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

/**************************************************************************/
/* overlay                                                              */
/**************************************************************************/
struct FBPOpenVRKeyboardHandle
{
	//GENERATED_BODY()
public:

	uint64_t VRKeyboardHandle;

	FBPOpenVRKeyboardHandle()
	{
		//static const VROverlayHandle_t k_ulOverlayHandleInvalid = 0;	
		VRKeyboardHandle = vr::k_ulOverlayHandleInvalid;
	}
	const bool IsValid()
	{
		return VRKeyboardHandle != vr::k_ulOverlayHandleInvalid;
	}

	//This is here for the Find() and Remove() functions from TArray
	FORCEINLINE bool operator==(const FBPOpenVRKeyboardHandle &Other) const
	{
		if (VRKeyboardHandle == Other.VRKeyboardHandle)
			return true;

		return false;
	}
	//#define INVALID_TRACKED_CAMERA_HANDLE
};

static vr::HmdMatrix34_t ToHmdMatrix34(const FMatrix& tm)
{
	// Rows and columns are swapped between vr::HmdMatrix34_t and FMatrix
	vr::HmdMatrix34_t out;

	out.m[0][0] = tm.M[0][0];
	out.m[1][0] = tm.M[0][1];
	out.m[2][0] = tm.M[0][2];

	out.m[0][1] = tm.M[1][0];
	out.m[1][1] = tm.M[1][1];
	out.m[2][1] = tm.M[1][2];

	out.m[0][2] = tm.M[2][0];
	out.m[1][2] = tm.M[2][1];
	out.m[2][2] = tm.M[2][2];

	out.m[0][3] = tm.M[3][0];
	out.m[1][3] = tm.M[3][1];
	out.m[2][3] = tm.M[3][2];

	return out;
}
static void TransformToSteamSpace(const FTransform& In, vr::HmdMatrix34_t& Out, float WorldToMeterScale)
{
	const FRotator InRot = In.Rotator();
	FRotator OutRot(InRot.Yaw, -InRot.Roll, -InRot.Pitch);

	const FVector InPos = In.GetTranslation();
	FVector OutPos(InPos.Y, InPos.Z, -InPos.X);
	OutPos /= WorldToMeterScale;

	const FVector InScale = In.GetScale3D();
	FVector OutScale(InScale.Y, InScale.Z, InScale.X);

	Out = ToHmdMatrix34(FTransform(OutRot, OutPos, OutScale).ToMatrixWithScale());
}

static FBPOpenVRKeyboardHandle KeyboardHandle1;
static FBPOpenVRKeyboardHandle KeyboardHandle2;
static FBPOpenVRKeyboardHandle KeyboardHandle3;
static FBPOpenVRKeyboardHandle KeyboardHandle4;
void ShowOverlay(UTexture * tex, FBPOpenVRKeyboardHandle& KeyboardHandle, FVector pos)
{
	if (KeyboardHandle.IsValid())
	{
		return;
	}

	vr::HmdError HmdErr;
	//vr::IVROverlay * VROverlay = (vr::IVROverlay*)(*UOpenVRExpansionFunctionLibrary::VRGetGenericInterfaceFn)(vr::IVROverlay_Version, &HmdErr);
	vr::IVROverlay * VROverlay = (vr::IVROverlay*)vr::VR_GetGenericInterface(vr::IVROverlay_Version, &HmdErr);
	if (!VROverlay)
	{
		return;
	}

	vr::EVROverlayError OverlayError;
	OverlayError = VROverlay->CreateOverlay("KeyboardOverlay", "Keyboard Overlay", &KeyboardHandle.VRKeyboardHandle);

	if (OverlayError != vr::EVROverlayError::VROverlayError_None || !KeyboardHandle.IsValid())
	{
		KeyboardHandle.VRKeyboardHandle = vr::k_ulOverlayHandleInvalid;
		return;
	}


	auto vrtex = CreateOpenVRTexture_t(tex);
	VROverlay->SetOverlayTexture(KeyboardHandle.VRKeyboardHandle, &vrtex);
	VROverlay->SetOverlaySortOrder(KeyboardHandle.VRKeyboardHandle, 0);
	VROverlay->SetOverlayFlag(KeyboardHandle.VRKeyboardHandle, vr::VROverlayFlags::VROverlayFlags_Panorama, true);
	VROverlay->SetOverlayWidthInMeters(KeyboardHandle.VRKeyboardHandle, 1000000);

	vr::HmdMatrix34_t HmdTransform;
	FTransform Translation(FQuat::Identity, pos, FVector(100, 100, 100));
	TransformToSteamSpace(Translation, HmdTransform, 10);
	VROverlay->SetOverlayTransformTrackedDeviceRelative(KeyboardHandle.VRKeyboardHandle, 0, &HmdTransform);

	VROverlay->ShowOverlay(KeyboardHandle.VRKeyboardHandle);
}
void HideOverlay(FBPOpenVRKeyboardHandle& KeyboardHandle)
{
	if (!KeyboardHandle.IsValid())
	{
		return;
	}

	vr::HmdError HmdErr;
	//vr::IVROverlay * VROverlay = (vr::IVROverlay*)(*UOpenVRExpansionFunctionLibrary::VRGetGenericInterfaceFn)(vr::IVROverlay_Version, &HmdErr);
	vr::IVROverlay * VROverlay = (vr::IVROverlay*)vr::VR_GetGenericInterface(vr::IVROverlay_Version, &HmdErr);
	if (!VROverlay)
	{
		return;
	}

	VROverlay->HideOverlay(KeyboardHandle.VRKeyboardHandle);
}
void ShowOverlay(UTexture * tex)
{
	//ShowOverlay(tex, KeyboardHandle1, FVector(500.0f, 0.0f, 100.0f));
	ShowOverlay(tex, KeyboardHandle1, FVector(100, 0, 0));
	ShowOverlay(tex, KeyboardHandle2, FVector(0, 100, 0));
	ShowOverlay(tex, KeyboardHandle3, FVector(-100, 0, 0));
	ShowOverlay(tex, KeyboardHandle4, FVector(0, -100, 0));
}
void HideOverlay()
{
	HideOverlay(KeyboardHandle1);
	HideOverlay(KeyboardHandle2);
	HideOverlay(KeyboardHandle3);
	HideOverlay(KeyboardHandle4);
}