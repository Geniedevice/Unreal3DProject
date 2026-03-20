// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/RPGAssetManager.h"
#include "Entity/Attribute/RPGGameplayTags.h" // 본인의 태그 헤더 경로에 맞게 수정하세요
#include "AbilitySystemGlobals.h"

URPGAssetManager& URPGAssetManager::Get()
{
	URPGAssetManager* This = Cast<URPGAssetManager>(GEngine->AssetManager);
	if (This)
	{
		return *This;
	}

	// 에디터 설정이 안 되어 있을 경우를 대비한 로그
	UE_LOG(LogTemp, Fatal, TEXT("Please Project Settings Asset Manager Class -> RPGAssetManager"));
	return *NewObject<URPGAssetManager>();
}

void URPGAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// [중요] 네이티브 태그 초기화
	FRPGGameplayTags::InitializeNativeTags();

	// [참고] GAS를 사용할 때 필수적으로 호출해야 하는 초기화 함수입니다.
	UAbilitySystemGlobals::Get().InitGlobalData();

	UE_LOG(LogTemp, Warning, TEXT("RPGAssetManager: Success Tag Init"));
}