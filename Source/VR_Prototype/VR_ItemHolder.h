// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define PRIVATE_MEMBER(Type,Name)\
private:\
	Type m_##Name;\
public:\
	inline const Type& Get##Name() const\
	{\
		return this->m_##Name;\
	}\
	inline void Set##Name(const Type& in##Name)\
	{\
		this->m_##Name=in##Name;\
	}\
private:\

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IN_ItemOwner.h"
#include "IN_CatchableItem.h"
#include "VR_ItemHolder.generated.h"

UCLASS()
class VR_PROTOTYPE_API AVR_ItemHolder : public AActor ,public IIN_ItemOwner,public IIN_CatchableItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "ItemHolder")
		TSubclassOf<UClass> TargetItemClass;
	UPROPERTY(EditDefaultsOnly, Category = "ItemHolder")
		AActor* HoldingItem;
	UPROPERTY(VisibleDefaultsOnly, Category = "ItemHolder")
		class UBoxComponent* ItemHolderCollision;
	UPROPERTY(VisibleDefaultsOnly, Category = "ItemHolder")
		bool HoldedWithVisible;
	UPROPERTY(VisibleDefaultsOnly, Category = "ItemHolder")
		bool DropWhenReleased;
	UPROPERTY(VisibleDefaultsOnly, Category = "ItemHolder")
		bool ItemIsUnique;
	UPROPERTY(VisibleDefaultsOnly, Category = "ItemHolder")
		FTimerHandle RegenTimerHandle;
	UPROPERTY(VisibleDefaultsOnly, Category = "ItemHolder")
		float ItemRegenTime;
	PRIVATE_MEMBER(TSubclassOf<UClass>, TargetItemClass)
public:	
	// Sets default values for this actor's properties
	AVR_ItemHolder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void makeItem();
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void HoldingItemVisible(bool Visible);
	UFUNCTION(BlueprintCallable, Category = "ItemHoler")
		void SetDropWhenReleased(bool val);

	virtual bool ItemIn_Implementation(AActor* Actor, class USceneComponent* Component);
	virtual bool ItemOut_Implementation(AActor* Actor);
	virtual USceneComponent* Catched_Implementation(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible);
	virtual USceneComponent* GetBaseCatchingComp_Implementation();
};
