#include "Ability/ANS_ComboInputWindow.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Ability/RPGComboAbility.h"
#include "Entity/Character/RPGPlayerCharacter.h"

void UANS_ComboInputWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ARPGPlayerCharacter* RPGChar = Cast<ARPGPlayerCharacter>(MeshComp->GetOwner());
	if (!RPGChar) return;

	UAbilitySystemComponent* ASC = RPGChar->GetAbilitySystemComponent();
	if (ASC && RPGChar->CurrentAttackAbilityHandle.IsValid())
	{
		// [핵심] 캐릭터가 가진 핸들로 스펙을 직접 찾습니다.
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(RPGChar->CurrentAttackAbilityHandle);
		if (Spec && Spec->IsActive())
		{
			if (URPGComboAbility* ComboAbility = Cast<URPGComboAbility>(Spec->GetPrimaryInstance()))
			{
				ComboAbility->SetInputWindow(true);
				UE_LOG(LogTemp, Log, TEXT("ANS: Window Opened via Character Handle!"));
			}
		}
	}
}

void UANS_ComboInputWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ARPGPlayerCharacter* RPGChar = Cast<ARPGPlayerCharacter>(MeshComp->GetOwner());
	if (!RPGChar) return;

	UAbilitySystemComponent* ASC = RPGChar->GetAbilitySystemComponent();
	if (ASC && RPGChar->CurrentAttackAbilityHandle.IsValid())
	{
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(RPGChar->CurrentAttackAbilityHandle);
		if (Spec && Spec->IsActive())
		{
			if (URPGComboAbility* ComboAbility = Cast<URPGComboAbility>(Spec->GetPrimaryInstance()))
			{
				// 1. 윈도우 닫기
				ComboAbility->SetInputWindow(false);

				// 2. 콤보 체크 이벤트 발사
				FGameplayEventData Payload;
				Payload.Instigator = RPGChar;
				FGameplayTag CheckTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Check"));
				ASC->HandleGameplayEvent(CheckTag, &Payload);

				UE_LOG(LogTemp, Log, TEXT("ANS: Window Closed and Event Sent!"));
			}
		}
	}
}