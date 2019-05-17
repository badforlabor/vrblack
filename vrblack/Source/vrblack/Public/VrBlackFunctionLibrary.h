// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VrBlackFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VRBLACK_API UVrBlackFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		static void BeforeOpenLevel(class UTexture* Texture);

	UFUNCTION(BlueprintCallable)
		static void EnableAutoBlack(class UTexture* Texture);
	UFUNCTION(BlueprintCallable)
		static void DisableAutoBlack();
	UFUNCTION(BlueprintCallable)
		static void EnableVrBlack(class UTexture* Texture);
	UFUNCTION(BlueprintCallable)
		static void DisableVrBlack();
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void ManualDisableRender(const UObject* WorldContextObject, class UTexture* Texture);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void ManualEnableRender(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		static void MySleep(float t);
};
