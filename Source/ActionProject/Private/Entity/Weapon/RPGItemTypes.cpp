// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Weapon/RPGItemTypes.h"

FWeaponData::FWeaponData()
	: WeaponName(NAME_None)
	, WeaponClass(nullptr)
	, AttachSocketName(NAME_None)
	, BaseDamage(0.f)
	, WeaponIcon(nullptr)
{
}
