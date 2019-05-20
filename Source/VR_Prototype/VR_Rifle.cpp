// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_Rifle.h"
#include "VR_Player.h"
#include "VR_MotionController.h"
#include "VR_HandAnimInstance.h"
#include "VR_Projectile.h"
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
	static ConstructorHelpers::FObjectFinder<UClass> AnimBP_RifleAnimation(TEXT("AnimBlueprint'/Game/Graphics/Rifle/RifleBP.RifleBP_C'"));
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleMesh"));
	RifleMesh->SetupAttachment(RootComponent);
	RifleMesh->AttachTo(RootComponent);
	RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//RifleMesh->SetRelativeLocation(FVector(20.0f, 00.0f, 0.0f));
	RifleMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	RifleMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	RifleMesh->SetCollisionProfileName(TEXT("NoCollision"));
	RifleMesh->CastShadow = false;

	if (SK_Rifle.Succeeded())
	{
		RifleMesh->SetSkeletalMesh(SK_Rifle.Object);
		RifleMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		if (AnimBP_RifleAnimation.Succeeded())
		{
			RifleMesh->SetAnimInstanceClass(AnimBP_RifleAnimation.Object);
			UVR_RifleAnimInstance* RifleAnim = Cast<UVR_RifleAnimInstance>(RifleMesh->GetAnimInstance());
			if (RifleAnim->IsValidLowLevel())
			{
				RifleAnim->setRifle(this);
			}
		}
	}

	MainHandBox = CreateDefaultSubobject<UBoxComponent>("MainHandBox");
	MainHandBox->SetupAttachment(RifleMesh);

	MainHandLocation = CreateDefaultSubobject<USceneComponent>("MainHandLocation");
	MainHandLocation->SetupAttachment(RifleMesh);

	SubHandBox = CreateDefaultSubobject<UBoxComponent>("SubHandBox");
	SubHandBox->SetupAttachment(RifleMesh);

	SubHandLocation = CreateDefaultSubobject<USceneComponent>("SubHandLocation");
	SubHandLocation->SetupAttachment(RifleMesh);

	/*MagazineBox = CreateDefaultSubobject<UBoxComponent>("MagazineBox");
	MagazineBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MagazineBox->SetGenerateOverlapEvents(true);
	MagazineBox->OnComponentBeginOverlap.AddDynamic(this, &AVR_Rifle::OnComponentBeginOverlap);
	MagazineBox->SetupAttachment(RifleMesh);*/

	ProjectileClass = AVR_Projectile::StaticClass();

	ProjSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn"));
	ProjSpawn->SetupAttachment(RifleMesh,"Spawner");

	ReloadTracker = CreateDefaultSubobject<USphereComponent>(TEXT("ReloadTracker"));
	ReloadTracker->SetupAttachment(MainHandLocation);
	ReloadTracker->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ReloadTracker->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	ReloadTracker->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	ReloadTracker->OnComponentBeginOverlap.AddDynamic(this,&AVR_Rifle::OnComponentBeginOverlap);
	ReloadTracker->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	ReloadTargets = CreateDefaultSubobject<USceneComponent>(TEXT("ReloadTargets"));
	ReloadTargets->SetupAttachment(MainHandLocation);

	ReloadTarget1 = CreateDefaultSubobject<UBoxComponent>(TEXT("ReloadTarget1"));
	ReloadTarget1->SetupAttachment(ReloadTargets);
	ReloadTarget1->SetRelativeLocation(FVector(19.2f, 0.0f, 64.6f));
	ReloadTarget1->SetRelativeRotation(FRotator(0.0f, 15.0f, 0.0f));
	ReloadTarget1->SetRelativeScale3D(FVector(0.6f, 1.0f, 1.0f));
	ReloadTarget1->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));

	ReloadTarget2 = CreateDefaultSubobject<UBoxComponent>(TEXT("ReloadTarget2"));
	ReloadTarget2->SetupAttachment(ReloadTargets);
	ReloadTarget2->SetRelativeLocation(FVector(-29.1f, 0.0f, 209.5f));
	ReloadTarget2->SetRelativeRotation(FRotator(0.0f, -40.0f, 0.0f));
	ReloadTarget2->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	ReloadTarget2->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));

	TrakcerBound = CreateDefaultSubobject<UBoxComponent>(TEXT("ReloadTarget"));
	TrakcerBound->SetupAttachment(MainHandLocation);
	TrakcerBound->SetRelativeLocation(FVector(-4.6f, 0.0f, -1.3f));
	TrakcerBound->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	TrakcerBound->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	TrakcerBound->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));

	TargetMark = CreateDefaultSubobject<USceneComponent>(TEXT("TargetMark"));
	TargetMark->SetupAttachment(RootScene);
	TargetMark->bVisible = true;
	TargetMarkImg = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReloadTargetImg"));
	TargetMarkImg->SetupAttachment(TargetMark);

	IsReadyToShot = false;
	IsReloading = false;
	BottomButtonPressed = false;
	GripState = ERifleGripState::NoGrip;
	HoldingOwner = nullptr;
}

// Called when the game starts or when spawned
void AVR_Rifle::BeginPlay()
{
	Super::BeginPlay();

	/*Then 0*/
	TickEventHandle_LateUpdate = TickEvent.AddUObject(this, &AVR_Rifle::LateUpdate);

	/*Then 1*/
	TickEventHandle_ClassifyState = TickEvent.AddUObject(this, &AVR_Rifle::ClassifyState);
	FVector MainHandRelativeLocation = GetTransform().InverseTransformPosition(MainHandLocation->GetComponentLocation());
	FVector SubHandRelativeLocation = GetTransform().InverseTransformPosition(SubHandLocation->GetComponentLocation());
	HandRotDiff = UKismetMathLibrary::MakeRotFromX(SubHandRelativeLocation - MainHandRelativeLocation);
	TickEventHandle_TargetMarkMatching = TickEvent.AddUObject(this, &AVR_Rifle::TargetMarkMatching);

	///*Then 2*/
	//const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f));
	//AVR_RifleMagazine* Magazine = GetWorld()->SpawnActorDeferred<AVR_RifleMagazine>(AVR_RifleMagazine::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);//탄창

}

// Called every frame
void AVR_Rifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickEvent.Broadcast(DeltaTime);
}


void AVR_Rifle::setMainHand(AVR_MotionController* val)
{
	MainHand = val;
}

void AVR_Rifle::LateUpdate(float DeltaTime)
{
	RifleAnim = Cast< UVR_RifleAnimInstance>(RifleMesh->GetAnimInstance());
	//RifleAnim->setRifle(this);
	TickEvent.Remove(TickEventHandle_LateUpdate);
	TickEventHandle_SetTriggerPosition = TickEvent.AddUObject(this, &AVR_Rifle::SetTriggerPosition);
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

void AVR_Rifle::Tracker_Initialize()
{
	ReloadTracker->SetWorldLocation(MainHand->GetGrabSphere()->GetComponentLocation());
	Tracker_oldLocation = MainHand->GetGrabSphere()->GetComponentLocation();
}

void AVR_Rifle::MaindHand_CheckStart()
{
	Tracker_Initialize();
	TickEventHandle_OneHand_Checking = TickEvent.AddUObject(this, &AVR_Rifle::OneHand_Checking);
}

void AVR_Rifle::MaindHand_CheckFinish()
{
	TickEvent.Remove(TickEventHandle_OneHand_Checking);
}

void AVR_Rifle::OneHand_Checking(float DeltaTime)
{
	if (GripState == ERifleGripState::BothGrip)
	{

	}
	else
	{
		ReloadTrackerTracing(DeltaTime);
	}
}

void AVR_Rifle::ReloadTrackerTracing(float time)
{
	if (MainHand->IsValidLowLevel())
	{
		/*Then 0*/
		float Alpha = (1.0f / UKismetMathLibrary::Exp(time)) / 1.05f;
		FVector Lerp_Location = FMath::Lerp(MainHand->GetGrabSphere()->GetComponentLocation(), Tracker_oldLocation, Alpha);
		ReloadTracker->SetWorldLocation(Lerp_Location);

		/*Then 1*/
		DrawDebugSphere(GetWorld(), Lerp_Location, ReloadTracker->GetScaledSphereRadius(), 12, FColor::White, false, 0.0f, 0, 0.2f);

		/*Then 1-0*/
		DrawDebugBox(GetWorld(), TrakcerBound->GetComponentLocation(), TrakcerBound->GetScaledBoxExtent(), FColor::Red, false, 0.0f, 0, 0.2f);
	
		/*Then 1-1*/
		TArray<USceneComponent*> TargetComponents;
		ReloadTargets->GetChildrenComponents(false, TargetComponents);
		for (USceneComponent* Components : TargetComponents)
		{
			UBoxComponent* ReloadTarget = Cast<UBoxComponent>(Components);
			DrawDebugBox(GetWorld(), Components->GetComponentLocation(), ReloadTarget->GetScaledBoxExtent(), FColor::Cyan, false, 0.0f, 0, 0.2f);
		}
	}
}

void AVR_Rifle::OneHandReloadTriggerSet()
{
	MaindHand_CheckStart();
	ReloadTracker->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ReloadTracker->OnComponentBeginOverlap.AddDynamic(this, &AVR_Rifle::OnComponentBeginOverlap);
	ReloadTracker->OnComponentEndOverlap.AddDynamic(this, &AVR_Rifle::OnComponentEndOverlap);
}

void AVR_Rifle::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->GetAttachParent() == ReloadTargets)
	{
		StartOneHandReloading();
	}
}

void AVR_Rifle::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp == TrakcerBound)
	{
		Tracker_Initialize();
	}
}

void AVR_Rifle::OneHandReloadingTriggerRemove()
{
	MaindHand_CheckFinish();
	ReloadTracker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ReloadTracker->OnComponentBeginOverlap.RemoveAll(this);
	ReloadTracker->OnComponentEndOverlap.RemoveAll(this);
}

void AVR_Rifle::StartOneHandReloading()
{
	OneHandReloadingTriggerRemove();
	IsReloading = true;
	RifleAnim->setuseOneHandReloading(true);
	
	UVR_HandAnimInstance* Hand_Anim = Cast<UVR_HandAnimInstance>(MainHand->GetHandMesh()->GetAnimInstance());
	Hand_Anim->setuseOneHandReload(true);
}


void AVR_Rifle::TargetMarkMatching(float DeltaTime)
{
	if (MainHand != nullptr && IsReadyToShot)
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
		HandAnimation->setRifleSubGrabed(false);

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
			HandAnimation->setRifleGrabed(false);

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
				HandAnimation->setRifleSubGrabed(false);

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
//bool AVR_Rifle::ItemIn_Implementation(AActor* Actor, class USceneComponent* Component)
//{
//	AVR_RifleMagazine* magazine = Cast<AVR_RifleMagazine>(Actor);
//	if (magazine->GetBulletCount() > 0)
//	{
//		/*Then 0*/
//		if (Component->IsValidLowLevel())
//		{
//			bool bHas_CatchableItem_Interface = Actor->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
//			if (bHas_CatchableItem_Interface)
//			{
//				if (Component == IIN_CatchableItem::Execute_GetBaseCatchingComp(Actor))
//				{
//					if (IIN_CatchableItem::Execute_GetHoldingOwner(Actor)->IsValidLowLevel())
//					{
//						bool bHasItemOwnerInterface = HoldingOwner->GetClass()->ImplementsInterface(UIN_ItemOwner::StaticClass());
//						if (bHasItemOwnerInterface)
//						{
//							IIN_ItemOwner::Execute_ItemOut(HoldingOwner, Actor);
//						}
//					}
//				}
//			}
//		}
//
//		/*Then 1*/
//		AttachedMagazine = magazine;
//		AttachedMagazine->Catched_Implementation(AttachedMagazine->GetStaticMesh(), this, RifleMesh,"",false);
//		
//		/*Then 2*/
//		MagazineAttacheEvenet.Broadcast();
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//bool AVR_Rifle::ItemOut_Implementation(AActor* Actor)
//{
//	if (!IsReloading)
//	{
//		if (AttachedMagazine != nullptr&&AttachedMagazine == Actor)
//		{
//			bool bHas_CatchableItem_Interface = Actor->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
//			if (bHas_CatchableItem_Interface)
//			{
//				if (IIN_CatchableItem::Execute_Dropped(Actor, this) == nullptr)
//				{
//					AttachedMagazine = nullptr;
//					return true;
//				}
//				else
//				{
//					return false;
//				}
//			}
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	//임시 리턴
//	return false;
//}

USceneComponent* AVR_Rifle::Catched_Implementation(USceneComponent* ItemComponent, AActor* Owner, USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible)
{
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	AVR_MotionController* MotionController = Cast< AVR_MotionController>(Owner);

	if (MotionController != nullptr) 
	{
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
			HandAnimation->setRifleGrabed(true);

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
				HandAnimation->setRifleGrabed(true);
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
					HandAnimation->setRifleGrabed(true);

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
	TryShot();
}

void AVR_Rifle::TryShot()
{
	if (IsReadyToShot)
	{

	}
}

void AVR_Rifle::Shot()
{
	AActor* spawnBullet;
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
	//FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	/*Then 0*/
	spawnBullet = GetWorld()->SpawnActorDeferred<AVR_Projectile>(AVR_Projectile::StaticClass(), ProjSpawn->GetComponentTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	//사운드 출력

	/*Then 1*/
	IsReadyToShot = false;

	/*Then 2*/
	if (GetAttachParentActor()->IsValidLowLevel())
	{
		AVR_MotionController* MotionController = Cast<AVR_MotionController>(GetAttachParentActor());
		MotionController->StartRumbleController(0.7f, false);
	}

}

void AVR_Rifle::TriggerReleased()
{

}



//
//void AVR_Rifle::BottomButton_Implementation(bool T_Dwon_F_Up)
//{
//	BottomButtonPressed = T_Dwon_F_Up;
//	if (BottomButtonPressed)
//	{
//		ItemOut_Implementation(AttachedMagazine);
//	}
//}

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

FTransform AVR_Rifle::InvertTransform(FTransform transform)
{
	FTransform result;

	result.SetLocation((FVector(0.0f, 0.0f, 0.0f) - transform.GetLocation()));
	result.SetRotation((FQuat(0.0f, 0.0f, 0.0f, 0.0f) - transform.GetRotation()));
	result.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

	return result;
}