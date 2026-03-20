// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entity/Weapon/RPGWeapon.h"
#include "GunWeapon.generated.h"

class ABulletActor;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API AGunWeapon : public ARPGWeapon
{
	GENERATED_BODY()
public:
    // --- ÃÑ±â »ç¾ç ---
    UPROPERTY(EditAnywhere, Category = "Weapon Stats")
    float FireRate = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Weapon Stats")
    int32 MaxAmmo = 30;

    UPROPERTY(EditAnywhere, Category = "Effects")
    TSubclassOf<ABulletActor> BulletClass;

    UPROPERTY(EditAnywhere, Category = "Effects")
    FName MuzzleSocketName = TEXT("Muzzle");

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual class ABulletActor* SpawnProjectile(const FGameplayEffectSpecHandle& InSpecHandle);

    bool CanFire() const { return CurrentAmmo > 0; }
    void ConsumeAmmo() { CurrentAmmo--; }

    virtual void InitializeWeapon(FName InRowName) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    TObjectPtr<APlayerController> CachedPC;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    int32 CurrentAmmo;
};
