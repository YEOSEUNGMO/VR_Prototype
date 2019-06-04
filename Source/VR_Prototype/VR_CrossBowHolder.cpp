// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_CrossBowHolder.h"
#include "VR_CrossBow.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

AVR_CrossBowHolder::AVR_CrossBowHolder()
{
	ItemIsUnique = true;
	HoldedWithVisible = true;
	HoldingItem = nullptr;
	ItemHolded = true;
}

void AVR_CrossBowHolder::BeginPlay()
{
	Super::BeginPlay();
	//GetWorldTimerManager().SetTimer(makeItemdelay, this, &AVR_ItemHolder::makeItem, 0.5f);
	makeItem();
}

void AVR_CrossBowHolder::makeItem()
{
	AActor* spawnActor;
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(0.5f, 0.5f, 0.5f)); // = FTransform::Identity;
	FActorSpawnParameters SpawnParams;

	spawnActor = GetWorld()->SpawnActor<AVR_CrossBow>(AVR_CrossBow::StaticClass(), SpawnTransform, SpawnParams);
	if (spawnActor)
		ItemIn_Implementation(spawnActor, nullptr);
	else
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, TEXT("Spawn error"), true, FVector2D(10.0f, 10.0f));

}