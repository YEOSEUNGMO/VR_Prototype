// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_CrossBow.h"
#include "VR_MotionController.h"
#include "VR_HandAnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AVR_CrossBow::AVR_CrossBow()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CrossBow(TEXT("SkeletalMesh'/Game/Graphics/New/Objects/_CrossBow/Object/Mesh/Crossgun.Crossgun'"));


	CrossBowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrossBowMesh"));
	CrossBowMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	CrossBowMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CrossBowMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CrossBowMesh->CastShadow = false;

	RootComponent = CrossBowMesh;

	if (SK_CrossBow.Succeeded())
	{
		CrossBowMesh->SetSkeletalMesh(SK_CrossBow.Object);
	}

	MainHandBox = CreateDefaultSubobject<UBoxComponent>("MainHandBox");
	//MainHandBox->SetupAttachment(RifleMesh);
	MainHandBox->AttachTo(CrossBowMesh);
	MainHandBox->SetRelativeLocation(FVector(3.08f, 0.0f, -0.21f));
	MainHandBox->SetRelativeScale3D(FVector(0.28f, 0.06f, 0.21f));
	MainHandBox->BodyInstance.SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MainHandBox->SetGenerateOverlapEvents(true);


	MainHandLocation = CreateDefaultSubobject<USceneComponent>("MainHandLocation");
	//MainHandLocation->SetupAttachment(RifleMesh);
	MainHandLocation->AttachTo(CrossBowMesh);

	SubHandBox = CreateDefaultSubobject<UBoxComponent>("SubHandBox");
	SubHandBox->AttachTo(CrossBowMesh);
	SubHandBox->SetRelativeLocation(FVector(51.63f, 0.0f, 2.0f));
	SubHandBox->SetRelativeScale3D(FVector(0.28f, 0.06f, 0.09f));
	SubHandBox->BodyInstance.SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SubHandBox->SetGenerateOverlapEvents(true);

	SubHandLocation = CreateDefaultSubobject<USceneComponent>("SubHandLocation");
	SubHandLocation->SetRelativeLocation(FVector(50.05f, 0.0f, 1.0f));
	SubHandLocation->AttachTo(CrossBowMesh);

	ProjectileClass = AVR_Projectile::StaticClass();

	ProjSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn"));
	ProjSpawn->SetRelativeLocation(FVector(139.13f, -0.1f, 11.87f));
	ProjSpawn->AttachTo(CrossBowMesh,"Spawner");

	IsReadyToShot = true;
	SubHandIsInRange = false;
	RechargingTime_Now = 0.0f;
	RechargingTime_Need = 0.125f;
	GripState = EWeaponGripState::NoGrip;

}
void AVR_CrossBow::BeginPlay()
{
	Super::BeginPlay();
	TickEventHandle_ClassifyState = TickEvent.AddUObject(this, &AVR_CrossBow::ClassifyState);
	TickEventHandle_AutoRecharge = TickEvent.AddUObject(this, &AVR_CrossBow::AutoRecharge);
	TickEventHandle_DrawAimLine = TickEvent.AddUObject(this, &AVR_CrossBow::DrawAimLine);
	FVector MainHandRelativeLocation = GetTransform().InverseTransformPosition(MainHandLocation->GetComponentLocation());
	FVector SubHandRelativeLocation = GetTransform().InverseTransformPosition(SubHandLocation->GetComponentLocation());
	HandRotDiff = UKismetMathLibrary::MakeRotFromX(SubHandRelativeLocation - MainHandRelativeLocation);
}

void AVR_CrossBow::AutoRecharge(float DeltaTime)
{
	if (!IsReadyToShot)
	{
		RechargingTime_Now += DeltaTime;
		if (RechargingTime_Need <= RechargingTime_Now)
		{
			Recharged();
		}
	}
}

void AVR_CrossBow::Recharged()
{
	IsReadyToShot = true;
}

void AVR_CrossBow::ClassifyState(float DeltaTime)
{
	if (MainHand != nullptr || SubHand != nullptr)
	{
		if (MainHand != nullptr && SubHand != nullptr)
		{
			SetGripState(EWeaponGripState::BothGrip);
		}
		else
		{
			if (MainHand != nullptr)
			{
				SetGripState(EWeaponGripState::MainGrip);
			}
			else
			{
				SetGripState(EWeaponGripState::SubGrip);
			}
		}
	}
	else
	{
		SetGripState(EWeaponGripState::NoGrip);
	}
}

void AVR_CrossBow::SetGripState(EWeaponGripState state)
{
	/*Then 0*/
	if (GripState != state)
	{
		switch (state)
		{
		case EWeaponGripState::MainGrip:
			MainGrip_Enter();
			break;
		case EWeaponGripState::BothGrip:
			BothGrip_Enter();
			break;
		}
		GripState = state;
	}

	/*Then 1*/
	switch (state)
	{
	case EWeaponGripState::MainGrip:
		MainGrip_Tick();
		break;
	case EWeaponGripState::BothGrip:
		BothGrip_Tick();
		break;
	}
}

void AVR_CrossBow::MainGrip_Enter()
{
	SetActorRelativeTransform(InvertTransform(MainHandLocation->GetRelativeTransform()));
	MainHand->GetHandMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
}

void AVR_CrossBow::BothGrip_Enter()
{

}

void AVR_CrossBow::MainGrip_Tick()
{

}

void AVR_CrossBow::BothGrip_Tick()
{
	FVector temp1;
	FVector temp2;
	FVector temp3;
	FRotator result;

	temp1 = SubHand->GetDirectionPoint()->GetComponentLocation() - MainHand->GetDirectionPoint()->GetComponentLocation();
	temp2 = temp1.RotateAngleAxis(MainHand->GetDirectionPoint()->GetComponentRotation().Roll, MainHand->GetDirectionPoint()->GetForwardVector());
	temp3 = temp2.RotateAngleAxis(MainHand->GetDirectionPoint()->GetComponentRotation().Yaw + HandRotDiff.Yaw, FVector(0.0f, 0.0f, -1.0f));
	result = temp3.RotateAngleAxis(MainHand->GetDirectionPoint()->GetComponentRotation().Pitch + HandRotDiff.Pitch, FVector(0.0f, 1.0f, 0.0f)).Rotation();

	result.Roll = 0.0f;
	MainHand->GetHandMesh()->SetRelativeRotation(result);
}

FTransform AVR_CrossBow::InvertTransform(FTransform transform)
{
	FTransform result;

	result.SetLocation((FVector(0.0f, 0.0f, 0.0f) - transform.GetLocation()));
	result.SetRotation((FQuat(0.0f, 0.0f, 0.0f, 0.0f) - transform.GetRotation()));
	result.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

	return result;
}

// Called every frame
void AVR_CrossBow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickEvent.Broadcast(DeltaTime);
}

void AVR_CrossBow::TriggerPulled()
{
	TickEventHandle_TryShot= TickEvent.AddUObject(this, &AVR_CrossBow::TryShot);
}

void AVR_CrossBow::TryShot(float DeltaTime)
{
	if (IsReadyToShot)
	{
		Shot();
	}
}

void AVR_CrossBow::Shot()
{
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	/*Then 0*/

	GetWorld()->SpawnActor<AVR_Projectile>(AVR_Projectile::StaticClass(), ProjSpawn->GetComponentTransform(), SpawnParams);
	
	/*사운드 출력 추가!!*/

	/*Then 1*/
	IsReadyToShot = false;
	RechargingTime_Now = 0.0f;

	/*Then 2*/
	if (GetAttachParentActor() != nullptr)
	{
		AVR_MotionController* MotionController = Cast< AVR_MotionController>(GetAttachParentActor());
		MotionController->StartRumbleController(1.0f, false);
	}
}

void AVR_CrossBow::TriggerReleased()
{
	TickEvent.Remove(TickEventHandle_TryShot);
}

void AVR_CrossBow::DrawAimLine(float DeltaTime)
{

}

AActor* AVR_CrossBow::Dropped_Implementation(AActor* OldOwner)
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	if (SubHand==OldOwner)
	{
		/*Then 0*/
		/*Then 0-0*/
		UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(SubHand->GetHandMesh()->GetAnimInstance());
		HandAnimation->setRifleSubGripped(false);

		/*Then 0-1*/
		SubHand->GetHandMesh()->DetachFromComponent(DetachRules);
		SubHand->GetHandMesh()->AttachToComponent(SubHand->GetRotateDummy(), AttachRules);

		/*Then 0-2*/
		SubHand = nullptr;

		/*Then 1*/
		return this;
	}
	else
	{
		HoldingOwner = nullptr;
		if (MainHand == OldOwner)
		{
			/*Then 0*/
			UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(MainHand->GetHandMesh()->GetAnimInstance());
			if (HandAnimation == nullptr)
				return nullptr;
			HandAnimation->setRifleGripped(false);

			/*Then 1*/
			RootComponent->DetachFromComponent(DetachRules);
			MainHand->GetHandMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

			MainHand->TriggerPullActions.Clear();
			MainHand->TriggerReleaseActions.Clear();

			/*Then 2*/
			MainHand = nullptr;

			/*Then 3-0*/
			if (SubHand != nullptr)
			{
				UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(SubHand->GetHandMesh()->GetAnimInstance());
				if (HandAnimation == nullptr)
					return nullptr;
				HandAnimation->setRifleSubGripped(false);

				SubHand->GetHandMesh()->DetachFromComponent(DetachRules);
				SubHand->GetHandMesh()->AttachToComponent(SubHand->GetRotateDummy(), AttachRules);

				SubHand = nullptr;
			}
			/*Tehn 3-1*/
			TriggerReleased();
			return this;
		}

		else
		{
			return nullptr;
		}
	}
}

USceneComponent* AVR_CrossBow::Catched_Implementation(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible)
{
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	AVR_MotionController* MotionController = Cast< AVR_MotionController>(Owner);
	if (MotionController != nullptr)
	{
		if (MainHand == nullptr&&SubHand == nullptr)
		{
			MainHand = MotionController;
			HoldingOwner = MainHand;
			/*Then 0*/
			MainHand->TriggerPullActionsHandle = MainHand->TriggerPullActions.AddUObject(this, &AVR_CrossBow::TriggerPulled);
			MainHand->TriggerPullActionsHandle = MainHand->TriggerReleaseActions.AddUObject(this, &AVR_CrossBow::TriggerReleased);

			/*Then 1*/
			RootComponent->AttachToComponent(MainHand->GetAttachingPoint(), AttachRules);
			RootComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

			/*Then 2*/
			UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(MainHand->GetHandMesh()->GetAnimInstance());
			HandAnimation->setRifleGripped(true);

			/*Then 3*/
			return MainHandBox;
		}
		else
		{
			if (SubHandBox == ItemComponent)
			{
				SubHand = MotionController;
				/*Then 0*/
				SubHand->GetHandMesh()->AttachToComponent(RootComponent, AttachRules, TEXT("SubHandPosition"));

				/*Then 1*/
				UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(SubHand->GetHandMesh()->GetAnimInstance());
				HandAnimation->setRifleSubGripped(true);

				/*Then 2*/
				return SubHandBox;
			}
			else
			{
				if (MainHandBox == ItemComponent)
				{
					MainHand = MotionController;
					HoldingOwner = MainHand;
					/*Then 0*/
					MainHand->TriggerPullActionsHandle = MainHand->TriggerPullActions.AddUObject(this, &AVR_CrossBow::TriggerPulled);
					MainHand->TriggerPullActionsHandle = MainHand->TriggerReleaseActions.AddUObject(this, &AVR_CrossBow::TriggerReleased);

					/*Then 1*/
					RootComponent->AttachToComponent(MainHand->GetAttachingPoint(), AttachRules);
					RootComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

					/*Then 2*/
					UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(MainHand->GetHandMesh()->GetAnimInstance());
					HandAnimation->setRifleGripped(true);

					/*Then 3*/
					return MainHandBox;
				}

				else
				{
					return nullptr;
				}
			}
		}
	}
	else
	{
		HoldingOwner = Owner;
		RootComponent->AttachToComponent(OwnerComponent, AttachRules);
		return nullptr;
	}
}

USceneComponent* AVR_CrossBow::GetBaseCatchingComp_Implementation()
{
	return MainHandBox;
}

AActor* AVR_CrossBow::GetHoldingOwner_Implementation()
{
	return HoldingOwner;
}

bool AVR_CrossBow::SetHoldingOwner_Implementation(AActor* Owner)
{
	if (HoldingOwner)
	{
		HoldingOwner = Owner;
		return true;
	}
	else
	{
		return false;
	}
}

bool AVR_CrossBow::IsDroppedWhenRelease_Implementation(USceneComponent* targetComp)
{
	if (SubHandBox == targetComp)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AVR_CrossBow::IsCatchableComp_Implementation(USceneComponent* SelectedComponent)
{
	if (SubHandBox == SelectedComponent)
	{
		if (SubHand == nullptr)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (MainHandBox == SelectedComponent)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}