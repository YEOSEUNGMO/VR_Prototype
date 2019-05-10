// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VR_RifleCase.generated.h"

UCLASS()
class VR_PROTOTYPE_API AVR_RifleCase : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "RifleCase")
		class USphereComponent* RifleCaseCollision;
public:	
	// Sets default values for this actor's properties
	AVR_RifleCase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile", meta = (AllowPrivateAccess = "ture"))
		class UStaticMeshComponent* Mesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
