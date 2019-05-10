// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_CrossBowHolder.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"


AVR_CrossBowHolder::AVR_CrossBowHolder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CrossBow(TEXT("SkeletalMesh'/Game/Graphics/CrossBosw/Crossbow.Crossbow'"));

	CrossBowHolderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RifleHolder"));
	CrossBowHolderCollision->InitBoxExtent(FVector(2.0f, 2.0f, 2.0f));
	CrossBowHolderCollision->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");

	RootComponent = CrossBowHolderCollision;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	Mesh->AttachTo(CrossBowHolderCollision);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	Mesh->CastShadow = false;
	Mesh->SetOnlyOwnerSee(true);
	//CrossBowMesh->SetRelativeRotation(FRotator(90.0f, -90.0f, 0.0f));
	//CrossBowMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	if (SK_CrossBow.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_CrossBow.Object);
		Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
	
}
