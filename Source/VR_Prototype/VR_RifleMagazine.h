// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IN_CatchableItem.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VR_RifleMagazine.generated.h"

UCLASS()
class VR_PROTOTYPE_API AVR_RifleMagazine : public AActor, public IIN_CatchableItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVR_RifleMagazine();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RifleMagazine", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RifleMagazine", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* RifleMagazineMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RifleMagazine", meta = (AllowPrivateAccess = "true"))
		int BulletCount;//BulletLeft
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "RifleMagazine")
		UStaticMeshComponent* GetStaticMesh();
	UFUNCTION(BlueprintCallable, Category = "RifleMagazine")
		int GetBulletCount();

	
	virtual USceneComponent* Catched_Implementation(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible);
	virtual AActor* Dropped_Implementation(AActor* OldOwner);
	virtual bool IsCatchableComp_Implementation(class USceneComponent* SelectedComponent);
	virtual	AActor* GetHoldingOwner_Implementation();
	virtual	bool SetHoldingOwner_Implementation(AActor* Owner);
	virtual	USceneComponent* GetBaseCatchingComp();
	virtual	bool IsDroppedWhenRelease_Implementation(class USceneComponent* targetComp);
};
