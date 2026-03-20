// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ComboInputWindow.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API UANS_ComboInputWindow : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	// 구간이 시작될 때 호출
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	// 구간이 끝날 때 호출
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
