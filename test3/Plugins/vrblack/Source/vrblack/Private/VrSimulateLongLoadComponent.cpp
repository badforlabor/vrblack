// Fill out your copyright notice in the Description page of Project Settings.

#include "VrSimulateLongLoadComponent.h"


// Sets default values for this component's properties
UVrSimulateLongLoadComponent::UVrSimulateLongLoadComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	// ...
}


// Called when the game starts
void UVrSimulateLongLoadComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVrSimulateLongLoadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVrSimulateLongLoadComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// FPlatformProcess::Sleep(5);
}
void UVrSimulateLongLoadComponent::PostInitProperties()
{
	Super::PostInitProperties();

	FPlatformProcess::Sleep(5);
}