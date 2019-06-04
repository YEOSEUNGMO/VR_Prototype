// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleAnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include"VR_Rifle.h"

UVR_RifleAnimInstance::UVR_RifleAnimInstance()
{
	TriggerPulled = 1.0f;
	//TriggerWillBePushed = false;
	useOneHandReloading = false;
	useTwoHandReloading = false;
}

void UVR_RifleAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	/*Then 0*/
	//if (TriggerPulled >= 0.0f)
	//{
	//	TriggerPulled = TriggerPulled + (DeltaTime*(-100.0f));
	//	if (TriggerPulled <= 0.0f)
	//	{
	//		TriggerPulled = 0.0f;
	//		//TriggerWillBePushed = false;
	//		//if(Rifle!=nullptr)
	//			//Rifle->Shot();
	//	}
	//}
}

void UVR_RifleAnimInstance::setRifle(AVR_Rifle* val)
{
	Rifle = val;
}

void UVR_RifleAnimInstance::setTriggerPulled(float val)
{
	TriggerPulled = val;
}

void UVR_RifleAnimInstance::setuseOneHandReloading(bool val)
{
	useOneHandReloading = val;
}

void UVR_RifleAnimInstance::setuseTwoHandReloading(bool val)
{
	useTwoHandReloading = val;
}