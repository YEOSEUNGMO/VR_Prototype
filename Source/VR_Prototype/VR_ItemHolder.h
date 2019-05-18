// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IN_ItemOwner.h"
#include "IN_CatchableItem.h"
#include "VR_ItemHolder.generated.h"

UCLASS()
class VR_PROTOTYPE_API AVR_ItemHolder : public AActor ,public IIN_ItemOwner,public IIN_CatchableItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* ItemHolderCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		bool DropWhenReleased;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		FTimerHandle RegenTimerHandle;


public:	
	// Sets default values for this actor's properties
	AVR_ItemHolder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		class AActor* HoldingItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UClass> TargetItemClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		bool HoldedWithVisible;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		float ItemRegenTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		bool ItemHolded;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemHolder", meta = (AllowPrivateAccess = "true"))
		bool ItemIsUnique;
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		virtual void makeItem();
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void HoldingItemVisible(bool Visible);
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void SetDropWhenReleased(bool val);
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void SetTargetItemClass(UClass* val);
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void SetItemIsUnique(float val);
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void SetHoldedWithVisible(float val);
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void SetItemHolded(float val);
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void SetItemRegenTime(float val);
	/*UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		TArray<AActor> GetAllAttahcedActors(AActor* RootActor);*/
	virtual bool ItemIn_Implementation(AActor* Actor, class USceneComponent* Component);
	virtual bool ItemOut_Implementation(AActor* Actor);
	virtual USceneComponent* Catched_Implementation(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible);
	virtual USceneComponent* GetBaseCatchingComp_Implementation();
	virtual AActor* Dropped_Implementation(AActor* OldOwner);
	virtual AActor* GetHoldingOwner_Implementation();
	virtual bool SetHoldingOwner_Implementation(AActor* Owner);
	virtual bool IsDroppedWhenRelease_Implementation(USceneComponent* targetComp);
	virtual bool IsCatchableComp_Implementation(USceneComponent* SelectedComponent);
};