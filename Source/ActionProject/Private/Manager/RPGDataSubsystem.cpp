// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/RPGDataSubsystem.h"

// 경로 관리
const FString URPGDataSubsystem::WeaponDataTablePath = TEXT("/Game/Component/Weapon/DT_WeaponData.DT_WeaponData");
const FString URPGDataSubsystem::CharacterAttributeTablePath = TEXT("/Game/Component/Attribute/DT_CharacterStatRow.DT_CharacterStatRow");

URPGDataSubsystem::URPGDataSubsystem()
{
	// 무기 테이블 로드
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Weapon(*WeaponDataTablePath);
	if (DT_Weapon.Succeeded()) WeaponDataTable = DT_Weapon.Object;

	// 캐릭터 속성 테이블 로드
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Attribute(*CharacterAttributeTablePath);
	if (DT_Attribute.Succeeded()) CharacterAttributeTable = DT_Attribute.Object;
}

const FWeaponData* URPGDataSubsystem::GetWeaponDataRow(FName RowName) const
{
	if (!WeaponDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("URPGDataSubsystem: WeaponDataTable is null!"));
		return nullptr;
	}

	// FindRow의 결과를 포인터로 반환하여 구조체 전체 복사를 피합니다 (성능 이득)
	return WeaponDataTable->FindRow<FWeaponData>(RowName, TEXT("WeaponDataContext"), true);
}

const FCharacterAttributeRow* URPGDataSubsystem::GetCharacterAttributeRow(FGameplayTag CharacterTag) const
{
	if (!CharacterAttributeTable || !CharacterTag.IsValid()) return nullptr;

	// 태그의 이름(예: "Character.Hero.Warrior")을 FName으로 가져옵니다.
	// 데이터 테이블의 Row Name이 태그 풀네임과 일치해야 합니다.
	const FName RowName = CharacterTag.GetTagName();

	return CharacterAttributeTable->FindRow<FCharacterAttributeRow>(RowName, TEXT("AttributeContext"));
}
