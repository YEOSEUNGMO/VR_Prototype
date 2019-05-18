// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_ItemHolder.h"
#include "Components/BoxComponent.h"
#include "VR_MotionController.h"
#include "IN_CatchableItem.h"
#include "IN_ButtonUsableItem.h"
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
	PrimaryActorTick.bCanEverTick = false;
	//TargetItemClass = nullptr;
	HoldingItem = nullptr;
	HoldedWithVisible = false;
	ItemIsUnique = false;
	ItemRegenTime = 0.0f;
}

// Called when the game starts or when spawned
void AVR_ItemHolder::BeginPlay()
{
	Super::BeginPlay();
	//GetWorldTimerManager().SetTimer(makeItemdelay, this, &AVR_ItemHolder::makeItem, 0.5f);
}

//// Called every frame
//void AVR_ItemHolder::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AVR_ItemHolder::makeItem()
{
	AActor* spawnActor;
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	FActorSpawnParameters parameter;
	parameter.Owner = this;

	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, FString::SanitizeFloat(ItemRegenTime) , true, FVector2D(10.0f, 10.0f));
	if (TargetItemClass->IsValidLowLevel())
	{
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Yellow, TargetItemClass->GetName(), true, FVector2D(10.0f, 10.0f));
		spawnActor = GetWorld()->SpawnActorDeferred<AActor>(TargetItemClass->StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if(spawnActor)
			if(ItemIn_Implementation(spawnActor, nullptr))
			{
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Yellow, TEXT("ItemIn!"), true, FVector2D(10.0f, 10.0f));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, TEXT("In error"), true, FVector2D(10.0f, 10.0f));
			}
		else
			GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, TEXT("Spawn error"), true, FVector2D(10.0f, 10.0f));
	}
}

bool AVR_ItemHolder::ItemIn_Implementation(AActor* Actor, class USceneComponent* Component)
{
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	
	
	/*Sequence 0*/
	if (HoldingItem == nullptr)
	{
		bool bHas_CatchableItem_Interface = Actor->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
		if (Component != nullptr)
		{
			if (bHas_CatchableItem_Interface&&Component == IIN_CatchableItem::Execute_GetBaseCatchingComp(Actor))
			{
				AActor* HoldingOwner = IIN_CatchableItem::Execute_GetHoldingOwner(Actor);
				if (HoldingOwner != nullptr)
				{
					bool bHasItemOwnerInterface = HoldingOwner->GetClass()->ImplementsInterface(UIN_ItemOwner::StaticClass());
					if (bHasItemOwnerInterface)
					{
						IIN_ItemOwner::Execute_ItemOut(HoldingOwner, Actor);
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
		if (bHas_CatchableItem_Interface)
		{
			IIN_CatchableItem::Execute_Catched(Actor, Component, this, ItemHolderCollision, "", HoldedWithVisible);
		}
		return true;
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
		bool bHas_CatchableItem_Interface = Actor->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
		if (bHas_CatchableItem_Interface)
		{
			IIN_CatchableItem::Execute_Dropped(Actor,this);
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
	AActor* tempActor;
	USceneComponent* RealCathed = nullptr;


	if (HoldingItem != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, HoldingItem->GetName(), true, FVector2D(10.0f, 10.0f));
		/*Sequence 0*/
		tempActor = HoldingItem;
		ItemOut_Implementation(tempActor);

		/*Sequence 1*/
		bool bHas_CatchableItem_Interface = tempActor->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
		if (bHas_CatchableItem_Interface)
		{
			RealCathed = IIN_CatchableItem::Execute_Catched(tempActor, IIN_CatchableItem::Execute_GetBaseCatchingComp(tempActor), Owner, OwnerComponent, "", false);

			if (RealCathed->IsValidLowLevel())
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
			actor->SetActorHiddenInGame(!Visible);
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

void AVR_ItemHolder::SetItemRegenTime(float val)
{
	ItemRegenTime = val;
}

void AVR_ItemHolder::SetTargetItemClass(UClass* val)
{
	TargetItemClass = val;
}

void AVR_ItemHolder::SetItemIsUnique(float val)
{
	ItemIsUnique = val;
}

void AVR_ItemHolder::SetHoldedWithVisible(float val)
{
	HoldedWithVisible = val;
}

void AVR_ItemHolder::SetItemHolded(float val)
{
	ItemHolded = val;
}
AActor* AVR_ItemHolder::Dropped_Implementation(AActor* OldOwner)
{
	return nullptr;
}
AActor* AVR_ItemHolder::GetHoldingOwner_Implementation()
{
	return nullptr;
}
bool AVR_ItemHolder::SetHoldingOwner_Implementation(AActor* Owner)
{
	return false;
}
bool AVR_ItemHolder::IsDroppedWhenRelease_Implementation(USceneComponent* targetComp)
{
	return false;
}
bool AVR_ItemHolder::IsCatchableComp_Implementation(USceneComponent* SelectedComponent)
{
	return true;
}