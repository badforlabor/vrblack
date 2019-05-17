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
	virtual void Init();
	void OnDestroy();
	class UTexture* Texture;

protected:
	void RegisterForMapLoad();
	void UnregisterForMapLoad();
	void OnPreLoadMap(const FString&);
	void OnPostLoadMap(UWorld*);
};

// 主动模式
class FVrBlackTickerManual : public FVrBlackTicker
{
public:
	FVrBlackTickerManual();

	virtual void Init() override;
	void BeginBlack(UTexture* InTexture, class UWorld*);
	void EndBlack(class UWorld*);

private:
	int Counter;
};

class FVrBlackAuto
{
public:
	FVrBlackAuto();
	void Init();
	void SetEnableAuto(class UTexture* InTexture, bool b);
	void Destroy();
	void DoBlack(class UTexture* InTexture, bool b);
private:
	void OnPreLoadMap(const FString&);
	void OnPostLoadMap(UWorld*);
	void DoAction(bool b);
	void SetTexture(class UTexture* InTexture);
private:
	bool bAuto;
	bool bInitialized;
	class UTexture* Texture;
};