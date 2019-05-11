// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_ItemHolder.h"
#include "Components/BoxComponent.h"
#include "VR_MotionController.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "VR_Rifle.h"

// Sets default values
AVR_ItemHolder::AVR_ItemHolder()
{
	ItemHolderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemHolder"));
	//ItemHolderCollision->InitBoxExtent(FVector(10.0f, 10.0f, 10.0f));
	ItemHolderCollision->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");

	RootComponent = ItemHolderCollision;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TargetItemClass = nullptr;
	HoldingItem = nullptr;
	HoldedWithVisible = true;
	ItemIsUnique = true;
	ItemRegenTime = 2.0;
}

// Called when the game starts or when spawned
void AVR_ItemHolder::BeginPlay()
{
	Super::BeginPlay();
	makeItem();
}

//// Called every frame
//void AVR_ItemHolder::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

//void AVR_ItemHolder::SetTargetItemClass(UClass * Target)
//{
//	TargetItemClass = Target;
//}

void AVR_ItemHolder::makeItem()
{
	AActor* spawnActor;
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f)); // = FTransform::Identity;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	FActorSpawnParameters parameter;
	parameter.Owner = this;

	if (TargetItemClass->IsValidLowLevel())
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TargetItemClass->GetName(), true, FVector2D(10.0f, 10.0f));
		spawnActor = GetWorld()->SpawnActorDeferred<AActor>(TargetItemClass->GetClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		ItemIn_Implementation(spawnActor, nullptr);
	}
}

bool AVR_ItemHolder::ItemIn_Implementation(AActor* Actor, class USceneComponent* Component)
{
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	
	
	if (Actor->GetClass() == TargetItemClass)
	{
		/*Sequence 0*/
		if (HoldingItem == nullptr)
		{
			IIN_CatchableItem* CatchableActor = Cast<IIN_CatchableItem>(Actor);
			if (Component!=nullptr)
			{
				if (CatchableActor && Component == CatchableActor->GetBaseCatchingComp())
				{
					if (CatchableActor->GetHoldingOwner() != nullptr)
					{
						IIN_ItemOwner* ItemOwner = Cast<IIN_ItemOwner>(CatchableActor->GetHoldingOwner());
						if (ItemOwner)
						{
							ItemOwner->ItemOut(Actor);
						}
					}
				}
			}

			/*Sequence 1*/
			FTimerManager& RegenTimer = GetWorld()->GetTimerManager();
			RegenTimer.ClearTimer(RegenTimerHandle);
			HoldingItem = Actor;
			HoldingItemVisible(HoldedWithVisible);

			/*Sequence 2*/
			if (CatchableActor)
			{
				CatchableActor->Catched_Implementation(Component, this, ItemHolderCollision, "", HoldedWithVisible);
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool AVR_ItemHolder::ItemOut_Implementation(AActor* Actor)
{
	FTimerManager& RegenTimer = GetWorld()->GetTimerManager();
	if (Actor == HoldingItem && HoldingItem != nullptr)
	{
		/*Sequence 0*/
		HoldingItemVisible(true);
		IIN_CatchableItem* CatchableActor = Cast<IIN_CatchableItem>(Actor);
		if (CatchableActor)
		{
			CatchableActor->Dropped(this);
		}
		HoldingItem = nullptr;
		
		/*Sequence 1*/
		if (!ItemIsUnique)
		{
			RegenTimer.SetTimer(RegenTimerHandle,this, &AVR_ItemHolder::makeItem, ItemRegenTime, false);
		}
		return true;
	}
	else
	{
		return false;
	}
}

USceneComponent* AVR_ItemHolder::Catched_Implementation(USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible)
{
	AActor* temp;
	USceneComponent* RealCathed = nullptr;


	if (HoldingItem != nullptr)
	{
		/*Sequence 0*/
		temp = HoldingItem;
		ItemOut_Implementation(temp);

		/*Sequence 1*/
		IIN_CatchableItem* CatchableActor = Cast<IIN_CatchableItem>(temp);
		if (CatchableActor)
		{
			RealCathed = CatchableActor->Catched_Implementation(CatchableActor->GetBaseCatchingComp(), Owner, OwnerComponent, "", false);
		}
		if (RealCathed != nullptr)
		{
			return RealCathed;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}
void AVR_ItemHolder::HoldingItemVisible(bool Visible)
{
	if (HoldingItem != nullptr)
	{
		/*Sequence 0*/
		HoldingItem->SetActorHiddenInGame(!Visible);
		if (HoldedWithVisible)
		{
			ItemHolderCollision->SetVisibility(!Visible,false);
		}

		/*Sequence 1*/
		TArray<AActor*> ChildrenActors;
		HoldingItem->GetAttachedActors(ChildrenActors);
		for (AActor* actor : ChildrenActors)
		{
			actor->SetActorHiddenInGame(Visible);
		}
	}
}

void AVR_ItemHolder::SetDropWhenReleased(bool val)
{
	DropWhenReleased = val;
}

USceneComponent* AVR_ItemHolder::GetBaseCatchingComp_Implementation()
{
	return ItemHolderCollision;
}