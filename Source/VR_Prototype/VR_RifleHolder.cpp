// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleHolder.h"
#include "VR_Rifle.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"


AVR_RifleHolder::AVR_RifleHolder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	TargetItemClass = AVR_Rifle::StaticClass();
	HoldedWithVisible = false;
	ItemRegenTime = 2.0f;
	ItemHolded = true;
	ItemIsUnique = true;
}

// Called when the game starts or when spawned
void AVR_RifleHolder::BeginPlay()
{
	Super::BeginPlay();
	//GetWorldTimerManager().SetTimer(makeItemdelay, this, &AVR_ItemHolder::makeItem, 0.5f);
	makeItem();
}

void AVR_RifleHolder::makeItem()
{
	AActor* spawnActor;
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f)); // = FTransform::Identity;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	FActorSpawnParameters parameter;
	parameter.Owner = this;

	//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, FString::SanitizeFloat(ItemRegenTime) , true, FVector2D(10.0f, 10.0f));

	spawnActor = GetWorld()->SpawnActorDeferred<AVR_Rifle>(AVR_Rifle::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (spawnActor)
		ItemIn_Implementation(spawnActor, nullptr);
	else
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, TEXT("Spawn error"), true, FVector2D(10.0f, 10.0f));

}