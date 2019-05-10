// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_CrossBow.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AVR_CrossBow::AVR_CrossBow()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CrossBow(TEXT("SkeletalMesh'/Game/Graphics/CrossBosw/Crossbow.Crossbow'"));


	CrossBowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrossBowMesh"));
	//CrossBowMesh->SetupAttachment(RootComponent);
	CrossBowMesh->SetRelativeRotation(FRotator(90.0f, -90.0f, 0.0f));
	CrossBowMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	CrossBowMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CrossBowMesh->CastShadow = false;

	RootComponent = CrossBowMesh;

	if (SK_CrossBow.Succeeded())
	{
		CrossBowMesh->SetSkeletalMesh(SK_CrossBow.Object);
	}

	ProjectileClass = AVR_Projectile::StaticClass();

	ProjSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn"));
	//ProjSpawn->SetRelativeLocation(FVector(139.13f, -0.1f, 11.87f));
	ProjSpawn->AttachTo(CrossBowMesh,"Spawner");

	OnFireReady = false;
	OnFireDelayTime = 0.1f;

}

void AVR_CrossBow::SetFireReady(bool set)
{
	FTimerManager& OnFireTimer = GetWorld()->GetTimerManager();
	OnFireReady = set;

	if (OnFireReady)
	{
		OnFireTimer.SetTimer(OnFireTimerHandle, this, &AVR_CrossBow::OnFire, OnFireDelayTime,true);
	}
	else
	{
		OnFireTimer.ClearTimer(OnFireTimerHandle);
	}
}

void AVR_CrossBow::OnFire()
{
	if (ProjectileClass != nullptr)
	{
		if (GetWorld() != nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, FString::SanitizeFloat(ProjSpawn->GetComponentLocation().X), true, FVector2D(10.0f, 10.0f));
			GetWorld()->SpawnActor<AVR_Projectile>(ProjectileClass, ProjSpawn->GetComponentLocation(), ProjSpawn->GetComponentRotation());
		}
	}
}