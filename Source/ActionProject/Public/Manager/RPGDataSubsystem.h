// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Entity/Weapon/RPGItemTypes.h"
#include "Engine/DataTable.h"
#include "RPGDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API URPGDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	URPGDataSubsystem();

	/** 캐릭터 태그를 기반으로 속성(GE) 행 데이터를 찾아 반환합니다. */
	const FCharacterAttributeRow* GetCharacterAttributeRow(FGameplayTag CharacterTag) const;

	/** 기존 무기 데이터 획득 함수 */
	const FWeaponData* GetWeaponDataRow(FName RowName) const;

protected:
	static const FString WeaponDataTablePath;
	static const FString CharacterAttributeTablePath;

	UPROPERTY()
	TObjectPtr<UDataTable> WeaponDataTable;

	UPROPERTY()
	TObjectPtr<UDataTable> CharacterAttributeTable;
};
