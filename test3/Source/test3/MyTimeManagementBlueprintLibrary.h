// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimeManagementBlueprintLibrary.h"
#include "MyTimeManagementBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TEST3_API UMyTimeManagementBlueprintLibrary : public UTimeManagementBlueprintLibrary
{
	GENERATED_BODY()
	

public:
	/** Converts an FrameRate to a float ie: 1/30 returns 0.0333333 */
	UFUNCTION(BlueprintCallable)
		static void MyTimeSleep(float t);
	
	
};
