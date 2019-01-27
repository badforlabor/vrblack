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
	
};
