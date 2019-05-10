// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleCase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/SkeletalMesh.h"

// Sets default values
AVR_RifleCase::AVR_RifleCase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Sphere(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	RifleCaseCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RifleCaseCollision->InitSphereRadius(5.0f);
	RifleCaseCollision->BodyInstance.SetCollisionProfileName("BlockAll");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	Mesh->AttachTo(RifleCaseCollision);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	RootComponent = RifleCaseCollision;

	if (SK_Sphere.Succeeded())
	{
		Mesh->SetStaticMesh(SK_Sphere.Object);
	}
}

// Called when the game starts or when spawned
void AVR_RifleCase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVR_RifleCase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

