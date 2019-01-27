// Fill out your copyright notice in the Description page of Project Settings.

#include "MyTimeManagementBlueprintLibrary.h"

void UMyTimeManagementBlueprintLibrary::MyTimeSleep(float t)
{
	FPlatformProcess::Sleep(t);
}


