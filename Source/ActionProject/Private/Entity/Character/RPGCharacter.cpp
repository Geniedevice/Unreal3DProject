#include "Entity/Character/RPGCharacter.h"

#include "AbilitySystemComponent.h"
#include "Entity/Attribute/RPGAttributeSet.h" 
#include "Entity/Attribute/RPGGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Components/CapsuleComponent.h"
#include "Entity/Weapon/RPGWeapon.h"
#include "Entity/Weapon/RPGItemTypes.h"
#include "Manager/RPGDataSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ARPGCharacter::ARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 여기서 미리 만들어둡니다. 클래스 타입이 고정이라면 이게 가장 확실합니다.
	AttributeSet = CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet"));

}

UAbilitySystemComponent* ARPGCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }

	if (AttributeSet)
	{
		// 중요: ASC에게 "Health 값이 변하면 이 함수를 실행해!"라고 등록하는 과정
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute())
			.AddUObject(this, &ARPGCharacter::OnHealthChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute())
			.AddUObject(this, &ARPGCharacter::OnMaxHealthChanged);
	}

    // 어빌리티는 서버/클라이언트 모두 확인 필요할 수 있음
	InitializeAttributes();
    AddCharacterAbilities();
}

void ARPGCharacter::InitializeAttributes()
{
	if (!AbilitySystemComponent || !AttributeSet) return;

	URPGDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<URPGDataSubsystem>();
	if (DataSubsystem)
	{
		const FCharacterAttributeRow* AttrRow = DataSubsystem->GetCharacterAttributeRow(MyCharacterTag);

		if (AttrRow && AttrRow->DefaultAttributeGE)
		{
			FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(AttrRow->DefaultAttributeGE, 1.0f, ContextHandle);
			if (SpecHandle.IsValid())
			{
				// [서버 전용] GE 적용
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				// 중요: GE 적용 후 데이터가 갱신되었는지 확인하기 위해 수동으로 값을 다시 읽어옴
				float CurrentHP = AttributeSet->GetHealth();
				float MaxHP = AttributeSet->GetMaxHealth();

				// 만약 여기서 여전히 생성자 값(0)이 나온다면, GE 내부의 Modifier 설정(Override/Additive)을 확인해야 함
				OnHealthChangedDelegate.Broadcast(CurrentHP, MaxHP, (MaxHP > 0.f) ? CurrentHP / MaxHP : 0.f);
			}
		}
	}
}

// --- 무기 장착 로직 수정 핵심 ---

void ARPGCharacter::EquipWeaponByName(FName WeaponRowName)
{
	// 클라이언트에서 호출하더라도 서버 RPC를 통해 서버에서 실행하도록 유도
	ServerEquipWeaponByName(WeaponRowName);
}

void ARPGCharacter::ServerEquipWeaponByName_Implementation(FName WeaponRowName)
{
	if (!HasAuthority()) return;

	URPGDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<URPGDataSubsystem>();
	if (!DataSubsystem) return;

	const FWeaponData* WeaponData = DataSubsystem->GetWeaponDataRow(WeaponRowName);

	// 데이터가 존재하는지(null이 아닌지) 먼저 확인
	if (WeaponData && WeaponData->WeaponClass)
	{
		// 기존 무기 제거
		if (CurrentWeapon)
		{
			CurrentWeapon->Destroy();
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// WeaponData가 포인터이므로 . 대신 -> 를 사용합니다.
		CurrentWeapon = GetWorld()->SpawnActor<ARPGWeapon>(WeaponData->WeaponClass, SpawnParams);

		if (CurrentWeapon)
		{
			// 무기 액터에 RowName을 저장합니다.
			CurrentWeapon->InitializeWeapon(WeaponRowName);

			// 이제 AttachWeaponToMesh를 호출할 때 RowName을 같이 넘겨줍니다.
			AttachWeaponToMesh(CurrentWeapon, CurrentWeapon->WeaponData.AttachSocketName, WeaponRowName);

			CurrentWeapon->SetOwner(this);
		}
	}
}

void ARPGCharacter::OnRep_CurrentWeapon(ARPGWeapon* OldWeapon)
{
	// 서버에서 CurrentWeapon 변수가 복제되어 내려오면 실행됨
	if (CurrentWeapon)
	{
		// 2. 메시와 애님 인스턴스가 유효한지 확인
		if (GetMesh() && GetMesh()->GetAnimInstance())
		{
			// 무기 장착 비주얼/애니메이션 업데이트
			AttachWeaponToMesh(CurrentWeapon, CurrentWeapon->WeaponData.AttachSocketName, CurrentWeapon->WeaponData.WeaponName);
		}
	}
}

void ARPGCharacter::AttachWeaponToMesh(ARPGWeapon* Weapon, FName SocketName, FName RowName)
{
	if (!Weapon || !GetMesh()) return;

	// 1. 물리적 부착
	const FName TargetSocket = SocketName.IsNone() ? FName("Equipment_R_Pistol") : SocketName;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	Weapon->AttachToComponent(GetMesh(), AttachRules, TargetSocket);

	// 2. 애니메이션 레이어 링크 
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		URPGDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<URPGDataSubsystem>();
		if (DataSubsystem)
		{
			const FWeaponData* WeaponData = DataSubsystem->GetWeaponDataRow(RowName);
			if (WeaponData && WeaponData->WeaponAnimLayer)
			{
				AnimInstance->LinkAnimClassLayers(WeaponData->WeaponAnimLayer);
			}
		}
	}

	// [서버 전용] 무기 데이터 기반 어빌리티 부여 및 데미지 이펙트 설정
	if (HasAuthority())
	{
		URPGDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<URPGDataSubsystem>();
		if (DataSubsystem)
		{
			const FWeaponData* WeaponData = DataSubsystem->GetWeaponDataRow(RowName);
			if (WeaponData)
			{
				if (WeaponData->DamageEffect)
				{
					DamageEffectClass = WeaponData->DamageEffect;
					UE_LOG(LogTemp, Log, TEXT("Weapon DamageEffect Updated: %s"), *DamageEffectClass->GetName());
				}

				// 어빌리티 부여 로직 (기존 코드 유지)
				if (AbilitySystemComponent && WeaponData->AttackAbility)
				{
					if (CurrentAttackAbilityHandle.IsValid())
					{
						AbilitySystemComponent->CancelAbilityHandle(CurrentAttackAbilityHandle);
						AbilitySystemComponent->ClearAbility(CurrentAttackAbilityHandle);
					}

					FGameplayAbilitySpec AttackSpec(WeaponData->AttackAbility);
					AttackSpec.SourceObject = Weapon;
					AttackSpec.InputID = static_cast<int32>(ERPGAbilityInputID::Attack);

					CurrentAttackAbilityHandle = AbilitySystemComponent->GiveAbility(AttackSpec);
				}
			}
		}
	}

}

// -------------------------------------------

void ARPGCharacter::ApplyDamageToTarget(AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (!HasAuthority() || !TargetActor || !DamageEffectClass) 
		return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent();
	IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(TargetActor);
	UAbilitySystemComponent* TargetASC = TargetInterface ? TargetInterface->GetAbilitySystemComponent() : nullptr;

	if (TargetASC && TargetASC->HasMatchingGameplayTag(FRPGGameplayTags::Get().State_Dead))
		return;
	

	if (SourceASC && TargetASC)
	{
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddInstigator(this, this);

		// [중요] 이 한 줄이 있어야 이펙트(GC)가 타격 좌표에 정확히 생성됩니다.
		ContextHandle.AddHitResult(HitResult);

		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(FRPGGameplayTags::Get().Data_Damage, DamageAmount);
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}

		
	}
}

void ARPGCharacter::AddCharacterAbilities()
{
	GiveStartupAbilities();
}

void ARPGCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(FRPGGameplayTags::Get().State_Dead)) return;

	float CurrentHealth = Data.NewValue;
	float MaxHealth = AttributeSet->GetMaxHealth();
	OnHealthChangedDelegate.Broadcast(CurrentHealth, MaxHealth, (CurrentHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f);

	if (CurrentHealth <= 0.f) 
		Die();
}

void ARPGCharacter::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float CurrentHealth = AttributeSet->GetHealth();
	float MaxHealth = Data.NewValue;
	OnHealthChangedDelegate.Broadcast(CurrentHealth, MaxHealth, (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f);
}

void ARPGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGCharacter, CurrentWeapon);
	DOREPLIFETIME(ARPGCharacter, CurrentAttackAbilityHandle);
}

void ARPGCharacter::RequestAttack()
{
	if (AbilitySystemComponent && CurrentAttackAbilityHandle.IsValid())
	{
		// 부여된 무기 어빌리티를 실행 시도
		AbilitySystemComponent->TryActivateAbility(CurrentAttackAbilityHandle);
	}
}

void ARPGCharacter::GiveStartupAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
		}
	}
}

void ARPGCharacter::Die()
{
	if (AbilitySystemComponent)
	{
		// 1. 상태 태그 부여 및 모든 어빌리티 중단
		AbilitySystemComponent->AddLooseGameplayTag(FRPGGameplayTags::Get().State_Dead);
		AbilitySystemComponent->CancelAllAbilities();
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
}