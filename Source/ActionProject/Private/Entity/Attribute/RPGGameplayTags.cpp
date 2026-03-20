#include "Entity/Attribute/RPGGameplayTags.h"
#include "GameplayTagsManager.h"

FRPGGameplayTags FRPGGameplayTags::GameplayTags;

void FRPGGameplayTags::InitializeNativeTags()
{
    // 여기서 실제 변수와 이름을 연결하며 등록합니다.
    GameplayTags.AddTag(GameplayTags.Data_Damage, "Data.Damage", "Damage Amount Data");

    GameplayTags.AddTag(GameplayTags.State_Dead, "State.Dead", "Character Death State");
    GameplayTags.AddTag(GameplayTags.State_Attacking, "State.Attacking", "Character Attack State");
    GameplayTags.AddTag(GameplayTags.State_Dashing, "State.Dashing", "Character Dash State");
    GameplayTags.AddTag(GameplayTags.State_Airborne, "State.Airborne", "Character Airborne State");

    GameplayTags.AddTag(GameplayTags.Event_Combo_Check, "Event.Combo.Check", "Combo Window Check Event");
    GameplayTags.AddTag(GameplayTags.Event_Action_Attack, "Event.Action.Attack", "Attacking");
    GameplayTags.AddTag(GameplayTags.Event_Action_Dodge, "Event.Action.Dodge", "Dodge");
    GameplayTags.AddTag(GameplayTags.Event_Action_DashAttack, "Event.Action.DashAttack", "DashAttack");

    GameplayTags.AddTag(GameplayTags.Event_Input_Attack, "Event.Input.Attack", "Input Attack");
    GameplayTags.AddTag(GameplayTags.Event_Input_Dodge, "Event.Input.Dodge", "Input Dodge");
    GameplayTags.AddTag(GameplayTags.Event_Input_DashAttack, "Event.Input.DashAttack", "Input DashAttack");
    GameplayTags.AddTag(GameplayTags.Event_Input_Jump, "Event.Input.Jump", "Input Jump");
    GameplayTags.AddTag(GameplayTags.Event_Input_Special, "Event.Input.Special", "Input Special");

    GameplayTags.AddTag(GameplayTags.GameplayCue_Character_DamageText, "GameplayCue.Character.DamageText", "Spawn Damaged Text");
    GameplayTags.AddTag(GameplayTags.GameplayCue_Character_DamageEffect, "GameplayCue.Character.DamageEffect", "Spawn Damaged Effect");

    //Character Type
    GameplayTags.AddTag("Character.Player", "Player Type");
    GameplayTags.AddTag("Character.Enemy", "Enemy Type");
    GameplayTags.AddTag("Character.NPC", "NPC Type");
}

void FRPGGameplayTags::AddTag(FGameplayTag& OutTag, const FName& TagName, const FString& Comment)
{
    // Native 태그로 등록하고 변수에 할당
    OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(TagName, Comment);

    // 맵에도 저장 (나중에 이름으로 찾기 위해)
    GameplayTagMap.Add(TagName, OutTag);
}

void FRPGGameplayTags::AddTag(const FName& TagName, const FString& Comment)
{
    // 변수 할당 없이 매니저에 등록하고 맵에만 넣음
    FGameplayTag RegisteredTag = UGameplayTagsManager::Get().AddNativeGameplayTag(TagName, Comment);
    GameplayTagMap.Add(TagName, RegisteredTag);
}

FGameplayTag FRPGGameplayTags::FindTagByString(FString TagString) const
{
    const FName TagName = FName(*TagString);
    const FGameplayTag* FoundTag = GameplayTagMap.Find(TagName);
    if (FoundTag) return *FoundTag;

    return UGameplayTagsManager::Get().RequestGameplayTag(TagName, false);
}