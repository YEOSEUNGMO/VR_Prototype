// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_Projectile.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Actor.h"

// Sets default values

AVR_Projectile::AVR_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_ProjectileMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	ProjCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	ProjCollision->InitSphereRadius(5.0f);
	ProjCollision->BodyInstance.SetCollisionProfileName("BlockAll");
	ProjCollision->OnComponentHit.AddDynamic(this, &AVR_Projectile::OnHit);
	ProjCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	ProjCollision->CanCharacterStepUpOn = ECB_No;// �߻�ü������ �̵� �Ұ�.

	RootComponent = ProjCollision;

	ProjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	ProjMesh->AttachTo(ProjCollision);
	ProjMesh->SetCollisionProfileName(TEXT("NoCollision"));

	if (SK_ProjectileMesh.Succeeded())
	{
		ProjMesh->SetStaticMesh(SK_ProjectileMesh.Object);
		ProjMesh->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	}

	ProjParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ProjParticle->bAutoActivate = false;
	ProjParticle->AttachTo(ProjCollision);

	ProjMoveMent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjMoveMent->UpdatedComponent = ProjCollision;// �θ� ������Ʈ�� ������Ʈ�ϹǷ� ����� ��� �ڽ�������Ʈ�� ������Ʈ�ȴ�.
	ProjMoveMent->InitialSpeed = 3000.0f;//�߻�ü�� �����ҋ� �ӵ�
	ProjMoveMent->MaxSpeed = 3000.0f;//�߻�ü�� �ְ� �ӵ�
	ProjMoveMent->bRotationFollowsVelocity = true;
	//ProjMoveMent->bShouldBounce = true;
	//ProjMoveMent->bIsHomingProjectile = false;//�߻�ü�� ���� �߻�ü ó�� �ൿ�ϰ� ����.
	//ProjMoveMent->HomingAccelerationMagnitude = 10000.0f;

	InitialLifeSpan = 3.0f;// 3�ʵ� �����.
}

// Called when the game starts or when spawned
void AVR_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVR_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVR_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuls, const FHitResult& Hit)
{
	
}

void AVR_Projectile::OnDestroyedProjectile(AActor* DestroyingActor)
{

}