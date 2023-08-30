// Fill out your copyright notice in the Description page of Project Settings.


#include "ContinueAttack_C_AN.h"
#include "CharacterBase.h"
#include "Engine.h"

//void UContinueAttack_C_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
//{
//    if (MeshComp->GetClass()->ImplementsInterface(UCombat_CI::StaticClass())) 
//    {
//        UE_LOG(LogTemp, Warning, TEXT("CASTED TO ANS CONTINUE ATTACK"))
//        ICombat_CI* CI = Cast<ICombat_CI>(MeshComp);
//        CI->ContinueAttack();
//    }
//
//
//
//
//}

//bool UContinueAttack_C_AN::Received_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
//{
//    if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr) 
//    {
//        ACharacterBase* Player = Cast<ACharacterBase>(MeshComp->GetOwner());
//        Player->ContinueAttack();
//        UE_LOG(LogTemp, Warning, TEXT("Casted to Character from AN"))
//    }
//
//
//    return true;
//}

void UContinueAttack_C_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//UE_LOG(LogTemp, Warning, TEXT("ContinueAttack from AN"));
	ICombat_CI::Execute_ContinueAttack(MeshComp->GetOwner());
}
