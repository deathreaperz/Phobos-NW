#include "BuildSpeedBonus.h"

#include <TechnoTypeClass.h>
#include <Ext/Building/Body.h>
#include <Ext/House/Body.h>

double BuildingTypeExt::GetExternalFactorySpeedBonus(TechnoTypeClass* pWhat, HouseClass* pOwner)
{
	double fFactor = 1.0;

	if (!pWhat || !pOwner || pOwner->Defeated || pOwner->IsNeutral() || pOwner->IsObserver())
		return fFactor;

	auto const pHouseExt = HouseExt::Fetch(pOwner);
	if (pHouseExt->BuildSpeedBonusBuildings.empty())
		return fFactor;

	const auto what = pWhat->WhatAmI();
	for (auto const pBuilding : pHouseExt->BuildSpeedBonusBuildings)
	{
		auto const pExt = BuildingTypeExt::Fetch(pBuilding->Type);

		if (!pExt->SpeedBonus.AffectedType.empty() && !pExt->SpeedBonus.AffectedType.Contains(pWhat))
			continue;

		double nBonus = 0.000;
		switch (what)
		{
		case AircraftTypeClass::AbsID:
			nBonus = pExt->SpeedBonus.SpeedBonus_Aircraft;
			break;
		case BuildingTypeClass::AbsID:
			nBonus = pExt->SpeedBonus.SpeedBonus_Building;
			break;
		case UnitTypeClass::AbsID:
			nBonus = pExt->SpeedBonus.SpeedBonus_Unit;
			break;
		case InfantryTypeClass::AbsID:
			nBonus = pExt->SpeedBonus.SpeedBonus_Infantry;
			break;
		default:
			continue;
		}

		if (nBonus != 0.000)
			fFactor *= nBonus;
	}

	return fFactor;
}

double BuildingTypeExt::GetExternalFactorySpeedBonus(TechnoClass* pWhat, HouseClass* pOwner)
{
	return pWhat ? GetExternalFactorySpeedBonus(pWhat->GetTechnoType(), pOwner) : 1.0;
}

double BuildingTypeExt::GetExternalFactorySpeedBonus(TechnoClass* pWhat)
{
	return pWhat ? GetExternalFactorySpeedBonus(pWhat, pWhat->GetOwningHouse()) : 1.0;
}

void BuildSpeedBonus::Read(INI_EX& parser, const char* pSection)
{
	Nullable<double> nBuff {};
	nBuff.Read(parser, pSection, "BuildSpeedBonus.Aircraft");

	if (nBuff.isset() && nBuff.Get() != 0.000)
	{
		Enabled = true;
		SpeedBonus_Aircraft = nBuff.Get();
	}

	nBuff.Reset();
	nBuff.Read(parser, pSection, "BuildSpeedBonus.Building");

	if (nBuff.isset() && nBuff.Get() != 0.000)
	{
		Enabled = true;
		SpeedBonus_Building = nBuff.Get();
	}

	nBuff.Reset();
	nBuff.Read(parser, pSection, "BuildSpeedBonus.Infantry");

	if (nBuff.isset() && nBuff.Get() != 0.000)
	{
		Enabled = true;
		SpeedBonus_Infantry = nBuff.Get();
	}

	nBuff.Reset();
	nBuff.Read(parser, pSection, "BuildSpeedBonus.Unit");

	if (nBuff.isset() && nBuff.Get() != 0.000)
	{
		Enabled = true;
		SpeedBonus_Unit = nBuff.Get();
	}

	if (Enabled)
		AffectedType.Read(parser, pSection, "BuildSpeedBonus.AffectedTypes");
}

bool BuildSpeedBonus::Load(PhobosStreamReader& stm, bool registerForChange)
{
	return Serialize(stm);
}

bool BuildSpeedBonus::Save(PhobosStreamWriter& stm) const
{
	return const_cast<BuildSpeedBonus*>(this)->Serialize(stm);
}

template <typename T>
bool BuildSpeedBonus::Serialize(T& stm)
{
	return stm
		.Process(Enabled)
		.Process(SpeedBonus_Aircraft)
		.Process(SpeedBonus_Building)
		.Process(SpeedBonus_Infantry)
		.Process(SpeedBonus_Unit)
		.Process(AffectedType)
		.Success()
		//&& stm.RegisterChange(this)
		; // announce this type
}
