// Fill out your copyright notice in the Description page of Project Settings.

#include "MySimulateLongLoadComponent.h"


// Sets default values for this component's properties
UMySimulateLongLoadComponent::UMySimulateLongLoadComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	// ...
}


// Called when the game starts
void UMySimulateLongLoadComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMySimulateLongLoadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMySimulateLongLoadComponent::InitializeComponent()
{
	Super::InitializeComponent();

	FPlatformProcess::Sleep(5);
}
void UMySimulateLongLoadComponent::PostInitProperties()
{
	Super::PostInitProperties();

	FPlatformProcess::Sleep(5);
}