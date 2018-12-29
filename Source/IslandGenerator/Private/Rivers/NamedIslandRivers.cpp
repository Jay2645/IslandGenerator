// Copyright 2018 Jay Stevens

#include "NamedIslandRivers.h"
#include "ProceduralNameGenerator.h"

void UNamedIslandRivers::AssignSideFlow_Implementation(TArray<int32>& s_flow, TArray<URiver*>& Rivers, UTriangleDualMesh* Mesh, const TArray<FSideIndex>& t_downslope_s, const TArray<FTriangleIndex>& river_t, FRandomStream& RiverRNG) const
{
	UIslandRivers::AssignSideFlow_Implementation(s_flow, Rivers, Mesh, t_downslope_s, river_t, RiverRNG);

	if (NameDataTable == NULL)
	{
		UE_LOG(LogWorldGen, Error, TEXT("No River Name Table found! Did you remember to set it in your Island Generation Settings?"));
		return;
	}

	TArray<FText> textArray;
	FString contextString;
	TArray<FName> rowNames;
	rowNames = NameDataTable->GetRowNames();

	for (auto& name : rowNames)
	{
		FMarkovData* row = NameDataTable->FindRow<FMarkovData>(name, contextString);
		if (row)
		{
			FText rowText = row->MarkovText;
			textArray.Add(rowText);
		}
	}

	const uint8 randomOrder = 2;
	const uint8 minLength = 3;
	const uint8 maxLength = 16;
	const bool bConvertToTitleCase = true;
	const bool bUseNamePattern = false;

	TSet<FString> riverLookup;
	for (int i = 0; i < Rivers.Num(); i++)
	{
		FString riverName;
		do
		{
			riverName = UProceduralNameGenerator::GenerateRandomWord(textArray, RiverRNG, randomOrder, minLength, maxLength, bConvertToTitleCase, bUseNamePattern);
		} while (riverLookup.Contains(riverName));
		riverLookup.Add(riverName);
		UNamedRiver* newRiver = NewObject<UNamedRiver>();
		newRiver->Downslopes = Rivers[i]->Downslopes;
		newRiver->RiverTriangles = Rivers[i]->RiverTriangles;
		newRiver->RiverName = riverName;
		Rivers[i] = newRiver;
		UE_LOG(LogWorldGen, Log, TEXT("Created the %s river, of length %d."), *riverName, newRiver->RiverTriangles.Num());
	}
}
