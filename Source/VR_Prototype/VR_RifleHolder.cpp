// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleHolder.h"
#include "VR_Rifle.h"
#include "Engine/Engine.h"
#include "Engine/World.h"


AVR_RifleHolder::AVR_RifleHolder()
{
	//ItemHolderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemHolder"));
	////ItemHolderCollision->InitBoxExtent(FVector(10.0f, 10.0f, 10.0f));
	//ItemHolderCollision->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");

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
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	
	spawnActor = GetWorld()->SpawnActor<AVR_Rifle>(AVR_Rifle::StaticClass(), SpawnTransform, SpawnParams);
	if (spawnActor)
		ItemIn_Implementation(spawnActor, nullptr);		
	else
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, TEXT("Spawn error"), true, FVector2D(10.0f, 10.0f));

}