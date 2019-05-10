// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VR_Projectile.generated.h"

UCLASS()
class VR_PROTOTYPE_API AVR_Projectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "Projetile")
		class USphereComponent* ProjCollision;
public:	
	// Sets default values for this actor's properties
	AVR_Projectile();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile", meta = (AllowPrivateAccess = "ture"))
		class UParticleSystemComponent* ProjParticle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile", meta = (AllowPrivateAccess = "ture"))
		class UStaticMeshComponent* ProjMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "ture"))
		class UProjectileMovementComponent* ProjMoveMent;
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuls, const FHitResult& Hit);

	UFUNCTION()
		void OnDestroyedProjectile(AActor* DestroyingActor);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
