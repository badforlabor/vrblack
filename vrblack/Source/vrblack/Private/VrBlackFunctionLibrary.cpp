// Fill out your copyright notice in the Description page of Project Settings.

#include "VrBlackFunctionLibrary.h"
#include "VrBlackTicker.h"

void UVrBlackFunctionLibrary::BeforeOpenLevel(class UTexture* Texture)
{
	if (!Texture)
	{
		return;
	}

	Texture->AddToRoot();

	extern void RegBlack(UTexture* Texture);
	RegBlack(Texture);
}


