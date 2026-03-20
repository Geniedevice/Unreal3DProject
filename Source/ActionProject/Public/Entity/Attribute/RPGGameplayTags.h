#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FRPGGameplayTags
{
public:
    static const FRPGGameplayTags& Get() { return GameplayTags; }
    static void InitializeNativeTags();

    // 1. 외부에서 태그 이름(FName)만으로 태그를 찾아야 할 때 사용
    FGameplayTag FindTagByString(FString TagString) const;

    // --- 2. 실제 변수들 (자동 완성을 위해 public 선언) ---
    FGameplayTag Data_Damage;

    FGameplayTag State_Dead;
    FGameplayTag State_Attacking;
    FGameplayTag State_Dashing;
    FGameplayTag State_Airborne;

    FGameplayTag Event_Combo_Check;
    FGameplayTag Event_Action_Attack;
    FGameplayTag Event_Action_Dodge;
    FGameplayTag Event_Action_DashAttack;

    FGameplayTag Event_Input_Attack;
    FGameplayTag Event_Input_Dodge;
    FGameplayTag Event_Input_DashAttack;
    FGameplayTag Event_Input_Jump;
    FGameplayTag Event_Input_Special;

    // --- 상황별 세부 태그 (필요 시) ---
    // 만약 '꾹 누르기(Charge)'나 '연타'를 구분해야 한다면 아래처럼 확장합니다.
    FGameplayTag Input_Status_Charging;

    FGameplayTag GameplayCue_Character_DamageText;
    FGameplayTag GameplayCue_Character_DamageEffect;


    // -----------------------------------------------

private:
    static FRPGGameplayTags GameplayTags;

    // 외부에서 가져올 변수 등록
    void AddTag(FGameplayTag& OutTag, const FName& TagName, const FString& Comment);

    // (단순 등록용)
    void AddTag(const FName& TagName, const FString& Comment = TEXT(""));

    // 전체 태그 보관용 맵 (이름으로 찾기용)
    TMap<FName, FGameplayTag> GameplayTagMap;
};