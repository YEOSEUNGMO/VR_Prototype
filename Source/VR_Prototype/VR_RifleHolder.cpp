// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleHolder.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/SkeletalMesh.h"


AVR_RifleHolder::AVR_RifleHolder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	RifleHolderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RifleHolder"));
	RifleHolderCollision->InitBoxExtent(FVector(10.0f, 10.0f, 10.0f));
	RifleHolderCollision->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");

	RootComponent = RifleHolderCollision;

	/*Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	Mesh->AttachTo(RifleHolderCollision);
	Mesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	if (SK_Sphere.Succeeded())
	{
		Mesh->SetStaticMesh(SK_Sphere.Object);
	}
	*/
}
