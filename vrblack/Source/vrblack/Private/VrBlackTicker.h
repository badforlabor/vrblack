#pragma once

#include "TickableObjectRenderThread.h"
//#include "IXRTrackingSystem.h"
//#include "IHeadMountedDisplay.h"
//#include "openvr.h"
//#include "VrBlackPrivate.h"
//#include "Rendercore.h"
//
//#if PLATFORM_WINDOWS
//#include "Windows/WindowsHWrapper.h"
//#include "Windows/AllowWindowsPlatformTypes.h"
//#include <d3d11.h>
//#include "Windows/HideWindowsPlatformTypes.h"
//#endif

class FVrBlackTicker : public FTickableObjectRenderThread, public TSharedFromThis<FVrBlackTicker>
{
public:
	FVrBlackTicker();

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

public:
	void Init();
	void OnDestroy();
	class UTexture* Texture;

private:
	void RegisterForMapLoad();
	void UnregisterForMapLoad();
	void OnPreLoadMap(const FString&);
	void OnPostLoadMap(UWorld*);
};
