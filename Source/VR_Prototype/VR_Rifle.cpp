// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_Rifle.h"
#include "VR_Player.h"
#include "VR_MotionController.h"
#include "VR_RifleAnimInstance.h"
#include "VR_RifleMagazine.h"
#include "VR_HandAnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"


// Sets default values
AVR_Rifle::AVR_Rifle()
{
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Rifle(TEXT("SkeletalMesh'/Game/Graphics/Rifle/Rifle.Rifle'"));

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleMesh"));
	RifleMesh->SetupAttachment(RootComponent);
	RifleMesh->AttachTo(RootComponent);
	RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//RifleMesh->SetRelativeLocation(FVector(20.0f, 00.0f, 0.0f));
	RifleMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	RifleMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f));
	RifleMesh->SetCollisionProfileName(TEXT("NoCollision"));
	RifleMesh->CastShadow = false;

	if (SK_Rifle.Succeeded())
	{
		RifleMesh->SetSkeletalMesh(SK_Rifle.Object);
	}

	MainHandBox = CreateDefaultSubobject<UBoxComponent>("MainHandBox");
	MainHandBox->SetupAttachment(RifleMesh);

	MainHandLocation = CreateDefaultSubobject<USceneComponent>("MainHandLocation");
	MainHandLocation->SetupAttachment(RifleMesh);

	SubHandBox = CreateDefaultSubobject<UBoxComponent>("SubHandBox");
	SubHandBox->SetupAttachment(RifleMesh);

	SubHandLocation = CreateDefaultSubobject<USceneComponent>("SubHandLocation");
	SubHandLocation->SetupAttachment(RifleMesh);

	MagazineBox = CreateDefaultSubobject<UBoxComponent>("MagazineBox");
	MagazineBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MagazineBox->SetGenerateOverlapEvents(true);
	MagazineBox->OnComponentBeginOverlap.AddDynamic(this, &AVR_Rifle:OnComponentBeginOverlap);
	MagazineBox->SetupAttachment(RifleMesh);

	ProjectileClass = AVR_Projectile::StaticClass();

	ProjSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn"));
	ProjSpawn->SetupAttachment(RifleMesh,"Spawner");

	ReloadParent = CreateDefaultSubobject<USceneComponent>(TEXT("ReloadParent"));
	ReloadParent->SetupAttachment(RootComponent);

	ReLoadTracker = CreateDefaultSubobject<USphereComponent>(TEXT("ReloadTracker"));
	ReLoadTracker->SetupAttachment(ReloadParent);
	ReLoadTracker->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ReLoadTracker->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	ReLoadTracker->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	//ReLoadTracker->OnComponentBeginOverlap.AddDynamic(this,&AVR_Rifle::OnBeginOverlap);
	ReLoadTracker->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	ReLoadTarget = CreateDefaultSubobject<UBoxComponent>(TEXT("ReloadTarget"));
	ReLoadTarget->SetupAttachment(ReloadParent);
	ReLoadTarget->SetRelativeLocation(FVector(0.0f, 0.0f, -10.0f));
	ReLoadTarget->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	ReLoadTarget->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	TargetMark = CreateDefaultSubobject<USceneComponent>(TEXT("TargetMark"));
	TargetMark->SetupAttachment(RootScene);
	TargetMark->bVisible = true;
	TargetMarkImg = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReloadTargetImg"));
	TargetMarkImg->SetupAttachment(TargetMark);

	IsReadyToFire = false;
	IsRecharging = false;
	BottomButtonPressed = false;
	GripState = ERifleGripState::NoGrip;
}

// Called when the game starts or when spawned
void AVR_Rifle::BeginPlay()
{
	Super::BeginPlay();

	/*Then 0*/
	TickEventHandle = TickEvent.AddUObject(this, &AVR_Rifle::LateUpdate);

	/*Then 1*/
	TickEventHandle = TickEvent.AddUObject(this, &AVR_Rifle::ClassifyState);
	FVector MainHandRelativeLocation = GetTransform().InverseTransformPosition(MainHandLocation->GetComponentLocation());
	FVector SubHandRelativeLocation = GetTransform().InverseTransformPosition(SubHandLocation->GetComponentLocation());
	HandRotDiff = UKismetMathLibrary::MakeRotFromX(SubHandRelativeLocation - MainHandRelativeLocation);
	TickEventHandle = TickEvent.AddUObject(this, &AVR_Rifle::TargetMarkMatching);

	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f));
	AVR_RifleMagazine* Magazine = GetWorld()->SpawnActorDeferred<AVR_RifleMagazine>(AVR_RifleMagazine::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);//라이플 스폰

}

// Called every frame
void AVR_Rifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickEvent.Broadcast(DeltaTime);

	
}

void AVR_Rifle::OnFire()
{
	
}

void AVR_Rifle::ReloadTrackerTracing(float time)
{
	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("Tracing"), true, FVector2D(10.0f, 10.0f));
	//ReLoadTracker->SetRelativeLocation(FMath::Lerp(MainHand->GetGrabSphere()->GetComponentLocation(), Tracker_oldLocation, time));
	//Tracker_oldLocation = FMath::Lerp(MainHand->GetGrabSphere()->GetComponentLocation(), Tracker_oldLocation, time);
	DrawDebugSphere(ReLoadTracker->GetWorld(), ReLoadTracker->GetComponentLocation(), 0.1, 12, FColor::Red, false,0.1);

}

void AVR_Rifle::setMainHand(AVR_MotionController* val)
{
	MainHand = val;
}

void AVR_Rifle::LateUpdate(float DeltaTime)
{
	RifleAnim = Cast< UVR_RifleAnimInstance>(RifleMesh->GetAnimInstance());
	//RifleAnim->setRifle(this);
	TickEvent.Remove(TickEventHandle);
	TickEvent.AddUObject(this, &AVR_Rifle::SetTriggerPosition);
}

void AVR_Rifle::SetTriggerPosition(float DeltaTime)
{
	if (IsValid(MainHand))
	{
		RifleAnim->setTriggerPulled(MainHand->GetTriggerValue());
	}
}

void AVR_Rifle::ClassifyState(float DeltaTime)
{
	if (MainHand != nullptr || SubHand != nullptr)
	{
		if (MainHand != nullptr && SubHand != nullptr)
		{
			SetGripState(ERifleGripState::BothGrip);
		}
		else
		{
			if (MainHand != nullptr)
			{
				SetGripState(ERifleGripState::MainGrip);
			}
			else
			{
				SetGripState(ERifleGripState::SubGrip);
			}
		}
	}
	else
	{
		SetGripState(ERifleGripState::NoGrip);
	}
}

void AVR_Rifle::TargetMarkMatching(float DeltaTime)
{
	if (MainHand != nullptr && IsReadyToFire)
	{
		/*Then 0*/
		if (!TargetMark->bVisible)
		{
			TargetMark->SetVisibility(true, true);
		}

		/*Then 1*/
		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit,ProjSpawn->GetComponentLocation(),	(ProjSpawn->GetComponentLocation() + ProjSpawn->GetForwardVector() * 1000),ECollisionChannel::ECC_Visibility))
		{
			TargetMark->SetWorldLocation(hit.Location);
		}
		else
		{
			TargetMark->SetVisibility(false, true);
		}

		/*Then 2*/
		AVR_Player* player = Cast<AVR_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		TargetMark->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(TargetMark->GetComponentLocation() - player->GetCamera()->GetComponentLocation()));
		TargetMark->AddRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	else
	{
		if (TargetMark->bVisible)
		{
			TargetMark->SetVisibility(false, true);
		}
	}
}

AActor* AVR_Rifle::Dropped_Implementation(AActor* OldOwner)
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	if (SubHand == OldOwner)
	{
		/*Then 0*/
		UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(SubHand->GetHandMesh()->GetAnimInstance());
		HandAnimation->RifleSubGraped = false;

		SubHand->GetHandMesh()->DetachFromComponent(DetachRules);
		SubHand->GetHandMesh()->AttachToComponent(SubHand->GetRotateDummy(), AttachRules);

		SubHand = nullptr;

		/*Then 1*/
		return this;
	}
	else
	{
		HoldingOwner = nullptr;
		if (OldOwner == MainHand)
		{
			UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(MainHand->GetHandMesh()->GetAnimInstance());
			HandAnimation->RifleGraped = false;

			RootComponent->DetachFromComponent(DetachRules);
			MainHand->GetHandMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

			MainHand->TriggerPullActions.Remove(MainHand->TriggerPullActionsHandle);
			MainHand->TriggerReleaseActions.Remove(MainHand->TriggerReleaseActionsHandle);

			MainHand = nullptr;

			if (SubHand != nullptr)
			{
				UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(SubHand->GetHandMesh()->GetAnimInstance());
				if (!HandAnimation)
				{
					return nullptr;
				}
				HandAnimation->RifleSubGraped = false;

				SubHand->GetHandMesh()->DetachFromComponent(DetachRules);
				SubHand->GetHandMesh()->AttachToComponent(SubHand->GetRotateDummy(), AttachRules);

				SubHand = nullptr;
			}

			return this;
		}
		else
		{
			return nullptr;
		}
	}
}
bool AVR_Rifle::ItemIn_Implementation(AActor* Actor, class USceneComponent* Component)
{
	AVR_RifleMagazine* magazine = Cast<AVR_RifleMagazine>(Actor);
	if (magazine->GetBulletCount() > 0)
	{
		/*Then 0*/
		if (Component->IsValidLowLevel())
		{
			IIN_CatchableItem* CatchableItem = Cast<IIN_CatchableItem>(Actor);
			if (CatchableItem)
			{
				if (Component == CatchableItem->GetBaseCatchingComp())
				{
					if (CatchableItem->GetHoldingOwner()->IsValidLowLevel())
					{
						IIN_ItemOwner* ItemOwner = Cast<IIN_ItemOwner>(CatchableItem->GetHoldingOwner());
						ItemOwner->ItemOut(Actor);
					}
				}
			}
		}

		/*Then 1*/
		AttachedMagazine = magazine;
		AttachedMagazine->Catched_Implementation(AttachedMagazine->GetStaticMesh(), this, RifleMesh,"",false);
		
		/*Then 2*/
		MagazineAttacheEvenet.Broadcast();
		return true;
	}
	else
	{
		return false;
	}
}

bool AVR_Rifle::ItemOut_Implementation(AActor* Actor)
{
	if (!IsRecharging)
	{
		if (AttachedMagazine != nullptr&&AttachedMagazine == Actor)
		{
			IIN_CatchableItem* CatchableActor = Cast<IIN_CatchableItem>(AttachedMagazine);
			if (CatchableActor)
			{
				if (CatchableActor->Dropped_Implementation(this) == nullptr)
				{
					AttachedMagazine = nullptr;
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	//임시 리턴
	return false;
}
USceneComponent* AVR_Rifle::Catched_Implementation(USceneComponent* ItemComponent, AActor* Owner, USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible)
{
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	AVR_MotionController* MotionController = Cast< AVR_MotionController>(Owner);

	if (MotionController != nullptr) {
		if (MainHand == nullptr && SubHand == nullptr)
		{
			MainHand = MotionController;
			HoldingOwner = MainHand;

			/*Then 0*/
			MainHand->TriggerPullActionsHandle = MainHand->TriggerPullActions.AddUObject(this, &AVR_Rifle::TriggerPulled);
			MainHand->TriggerPullActionsHandle = MainHand->TriggerReleaseActions.AddUObject(this, &AVR_Rifle::TriggerReleased);
			
			/*Then 1*/
			RootComponent->AttachToComponent(MainHand->GetAttachingPoint(), AttachRules);
			RootComponent->SetRelativeRotation(FRotator(0, 0, 0));

			/*Then 2*/
			UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(MainHand->GetHandMesh()->GetAnimInstance());
			HandAnimation->RifleGraped = true;

			/*Then 3*/
			return MainHandBox;
		}
		else
		{
			if (SubHandBox == ItemComponent)
			{
				SubHand = MotionController;

				/*Then0*/
				SubHand->GetHandMesh()->AttachToComponent(RifleMesh, AttachRules);

				/*Then1*/
				UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(SubHand->GetHandMesh()->GetAnimInstance());
				HandAnimation->RifleGraped = true;
				/*Then2*/
				return SubHandBox;
			}
			else
			{
				if (MainHandBox == ItemComponent)
				{
					MainHand = MotionController;
					HoldingOwner = MainHand;

					/*Then 0*/
					MainHand->TriggerPullActionsHandle = MainHand->TriggerPullActions.AddUObject(this, &AVR_Rifle::TriggerPulled);
					MainHand->TriggerPullActionsHandle = MainHand->TriggerReleaseActions.AddUObject(this, &AVR_Rifle::TriggerReleased);

					/*Then 1*/
					RootComponent->AttachToComponent(MainHand->GetAttachingPoint(), AttachRules);
					RootComponent->SetRelativeRotation(FRotator(0, 0, 0));

					/*Then 2*/
					UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(MainHand->GetHandMesh()->GetAnimInstance());
					HandAnimation->RifleGraped = true;

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
	}

	//임시 리턴
	return nullptr;
}

USceneComponent* AVR_Rifle::GetBaseCatchingComp_Implementation()
{
	return MainHandBox;
}

AActor* AVR_Rifle::GetHoldingOwner_Implementation()
{
	return HoldingOwner;
}

bool AVR_Rifle::SetHoldingOwner_Implementation(AActor* Owner)
{
	HoldingOwner = Owner;
	if (HoldingOwner != nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AVR_Rifle::IsDroppedWhenRelease_Implementation(USceneComponent* targetComp)
{
	if (targetComp == SubHandBox)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AVR_Rifle::IsCatchableComp_Implementation(USceneComponent* SelectedComponent)
{
	if (SelectedComponent == SubHandBox)
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
		if (SelectedComponent == MainHandBox)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void AVR_Rifle::TriggerPulled()
{

}

void AVR_Rifle::TriggerReleased()
{

}

void AVR_Rifle::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AttachedMagazine == nullptr && !BottomButtonPressed)
	{
		AVR_RifleMagazine* magazine = Cast<AVR_RifleMagazine>(OtherActor);
		if (magazine->IsCatchableComp_Implementation(OtherComp))
		{
			ItemIn_Implementation(magazine, OtherComp);
		}
	}
}

void AVR_Rifle::BottomButton_Implementation(bool T_Dwon_F_Up)
{
	BottomButtonPressed = T_Dwon_F_Up;
	if (BottomButtonPressed)
	{
		ItemOut_Implementation(AttachedMagazine);
	}
}

void AVR_Rifle::SetGripState(ERifleGripState state)
{
	/*Then 0*/
	if (GripState != state)
	{
		switch (state)
		{
		case ERifleGripState::MainGrip:
			MainGrip_Enter();
			break;
		case ERifleGripState::BothGrip:
			BothGrip_Enter();
			break;
		}
		GripState = state;
	}

	/*Then 1*/
	switch (state)
	{
	case ERifleGripState::MainGrip:
		MainGrip_Tick();
		break;
	case ERifleGripState::BothGrip:
		BothGrip_Tick();
		break;
	}
}

void AVR_Rifle::MainGrip_Enter()
{
	SetActorRelativeTransform(InvertTransform(MainHandLocation->GetRelativeTransform()));
	MainHand->GetHandMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//MainHand->GetHandMesh()->SetRelativeRotation(Rotator);
}

void AVR_Rifle::BothGrip_Enter()
{

}

void AVR_Rifle::MainGrip_Tick()
{

}

void AVR_Rifle::BothGrip_Tick()
{
	FVector temp1;
	FVector temp2;
	FVector temp3;
	FRotator result;

	temp1 = MainHand->GetDirectionPoint()->GetComponentLocation() - SubHand->GetDirectionPoint()->GetComponentLocation();
	temp2 = temp1.RotateAngleAxis(MainHand->GetDirectionPoint()->GetComponentRotation().Roll, MainHand->GetDirectionPoint()->GetForwardVector());
	temp3 = temp2.RotateAngleAxis(MainHand->GetDirectionPoint()->GetComponentRotation().Yaw + HandRotDiff.Yaw, FVector(0.0f, 0.0f, -1.0f));
	result = temp3.RotateAngleAxis(MainHand->GetDirectionPoint()->GetComponentRotation().Pitch + HandRotDiff.Pitch, FVector(0.0f, 1.0f, 0.0f)).Rotation();
	
	result.Roll = 0.0f;
	MainHand->GetHandMesh()->SetRelativeRotation(result);
}

FTransform InvertTransform(FTransform transform)
{
	FTransform result;

	result.SetLocation = (FVector(0.0f, 0.0f, 0.0f) - transform.GetLocation());
	result.SetRotation = (FRotator(0.0f, 0.0f, 0.0f) - transform.GetRotation().Rotator());
	result.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

	return result;
}