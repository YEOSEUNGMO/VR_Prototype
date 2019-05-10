// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleMagazine.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
AVR_RifleMagazine::AVR_RifleMagazine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RifleMagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RifleMagazineMesh"));

	BulletCount = 5;
}

// Called when the game starts or when spawned
void AVR_RifleMagazine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVR_RifleMagazine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent* AVR_RifleMagazine::GetStaticMesh()
{
	return RifleMagazineMesh;
}

int AVR_RifleMagazine::GetBulletCount()
{
	return BulletCount;
}

USceneComponent* AVR_RifleMagazine::Catched_Implementation(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible)
{
	//�ӽ� ����
	return nullptr;
}

AActor* AVR_RifleMagazine::Dropped_Implementation(AActor* OldOwner)
{
	//�ӽ� ����
	return nullptr;
}

bool AVR_RifleMagazine::IsCatchableComp_Implementation(class USceneComponent* SelectedComponent)
{
	//�ӽ� ����
	return false;
}

AActor* AVR_RifleMagazine::GetHoldingOwner_Implementation()
{
	//�ӽ� ����
	return nullptr;
}

bool AVR_RifleMagazine::SetHoldingOwner_Implementation(AActor* Owner)
{
	//�ӽ� ����
	return false;
}

USceneComponent* AVR_RifleMagazine::GetBaseCatchingComp()
{
	//�ӽ� ����
	return nullptr;
}

bool AVR_RifleMagazine::IsDroppedWhenRelease_Implementation(class USceneComponent* targetComp)
{
	//�ӽ� ����
	return false;
}