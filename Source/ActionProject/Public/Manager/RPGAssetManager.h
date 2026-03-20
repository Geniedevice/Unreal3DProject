// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "RPGAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API URPGAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	// 싱글톤처럼 접근하기 위한 헬퍼 함수
	static URPGAssetManager& Get();

	// 엔진이 초기 로딩을 시작할 때 자동으로 호출되는 핵심 함수
	virtual void StartInitialLoading() override;
};
