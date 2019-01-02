// Copyright 2018 Jay Stevens

#include "NamedIslandRivers.h"
#include "ProceduralNameGenerator.h"

UNamedIslandRivers::UNamedIslandRivers()
{
	RiverErosionFactor = 0.1f;
	FlowScale = 10.0f;
	MinRiverWidth = 30.0f;
}

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
		if (Rivers[i] == NULL)
		{
			UE_LOG(LogWorldGen, Error, TEXT("River at index %d was null!"), i);
			continue;
		}
		FString riverName;
		do
		{
			riverName = UProceduralNameGenerator::GenerateRandomWord(textArray, RiverRNG, randomOrder, minLength, maxLength, bConvertToTitleCase, bUseNamePattern);
		} while (riverLookup.Contains(riverName));
		riverLookup.Add(riverName);
		UNamedRiver* newRiver = NewObject<UNamedRiver>();
		if (newRiver == NULL)
		{
			UE_LOG(LogWorldGen, Error, TEXT("Could not create a new river!"));
			continue;
		}
		newRiver->Downslopes = Rivers[i]->Downslopes;
		newRiver->RiverTriangles = Rivers[i]->RiverTriangles;
		newRiver->RiverName = riverName;
		Rivers[i] = newRiver;
		UE_LOG(LogWorldGen, Log, TEXT("Created the %s river, of length %d."), *riverName, newRiver->RiverTriangles.Num());
	}
}

URiverSplineComponent* UNamedIslandRivers::CreateRiverSpine(AActor* Parent, UNamedRiver* River, const TArray<FVector>& SplinePositions) const
{
	FName objectName(*(River->RiverName));

	URiverSplineComponent* riverSpline = NewObject<URiverSplineComponent>(Parent, objectName, RF_Transient);
	riverSpline->RegisterComponent();
	riverSpline->SetMobility(EComponentMobility::Movable);
	riverSpline->AttachToComponent(Parent->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	riverSpline->SetSplineLocalPoints(SplinePositions);

	riverSpline->CreateRiverMeshes(River, Parent, this);

	return riverSpline;
}

TArray<FVector> UNamedIslandRivers::ProcessRiver(TArray<float>& r_elevation, UTriangleDualMesh* Mesh, UNamedRiver* River, const TArray<float>& t_elevation, const TArray<int32>& s_flow, int32 MaxFlow, float ZScale) const
{
	TArray<FVector> splinePositions;

	TSet<FPointIndex> processedRegions;
	River->RiverFlow.SetNum(River->RiverTriangles.Num());
	for (int j = 0; j < River->RiverTriangles.Num(); j++)
	{
		FTriangleIndex triangle = River->RiverTriangles[j];
		FVector2D first2D = Mesh->t_pos(triangle);
		float z1 = t_elevation.IsValidIndex(triangle) ? t_elevation[triangle] : -1000.0f;
		splinePositions.Add(FVector(first2D.X, first2D.Y, z1 * ZScale) + RiverOffset);

		int32 flow = s_flow[River->Downslopes[j]];
		River->RiverFlow[j] = flow;

		TArray<FPointIndex> trianglePoints = Mesh->t_circulate_r(triangle);
		for (FPointIndex r : trianglePoints)
		{
			if (processedRegions.Contains(r))
			{
				continue;
			}
			processedRegions.Add(r);

			// Let river erode part of the landscape
			// We skip the first point so the river comes "out of" the ground
			if (j == 0)
			{
				continue;
			}
			float originalElevation = r_elevation[r];
			float flowPercent = (float)flow / (float)MaxFlow;
			flowPercent = 1.0f - FMath::Clamp(flowPercent, 0.1f, 0.9f);
			r_elevation[r] -= RiverErosionFactor * flowPercent;
			r_elevation[r] = FMath::Clamp(r_elevation[r], originalElevation * 0.1f, originalElevation * 0.9f);
		}
	}
	return splinePositions;
}
