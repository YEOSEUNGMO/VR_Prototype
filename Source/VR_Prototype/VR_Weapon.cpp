// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_Weapon.h"

// Sets default values
AVR_Weapon::AVR_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVR_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVR_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVR_Weapon::OnFire()
{

}