// Copyright 2018 Jay Stevens

#include "IslandMaker.h"

AIslandMaker::AIslandMaker()
{
	RiverErosionFactor = 0.1f;
}

void AIslandMaker::OnRiverGenerationComplete_Implementation()
{
	int32 maxFlow = -1;
	for (FSideIndex s = 0; s < s_flow.Num(); s++)
	{
		if (s_flow[s] > maxFlow)
		{
			maxFlow = s_flow[s];
		}
	}
	for (int i = 0; i < CreatedRivers.Num(); i++)
	{
		UNamedRiver* river = Cast<UNamedRiver>(CreatedRivers[i]);
		if (river == NULL)
		{
			continue;
		}
		TArray<FVector> splinePoints = ProcessRiver(river, maxFlow);
		CreateRiverSpine(river, splinePoints);
	}
}

URiverSplineComponent* AIslandMaker::CreateRiverSpine(UNamedRiver* River, TArray<FVector> SplinePositions)
{
	FName objectName(*(River->RiverName));

	URiverSplineComponent* riverSpline = NewObject<URiverSplineComponent>(this, objectName, RF_Transient);
	riverSpline->RegisterComponent();
	riverSpline->SetMobility(EComponentMobility::Movable);
	riverSpline->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	riverSpline->SetSplineLocalPoints(SplinePositions);

	riverSpline->PointScale = PointScale;
	riverSpline->RiverMesh = RiverMesh;

	riverSpline->CreateRiverMeshes(River, this, RiverInterface, RiverTextures);

	RiverSplines.Add(riverSpline);
	return riverSpline;
}

TArray<FVector> AIslandMaker::ProcessRiver(UNamedRiver* River, int32 MaxFlow)
{
	TArray<FVector> splinePositions;
	TSet<FPointIndex> processedRegions;
	River->RiverFlow.SetNum(River->RiverTriangles.Num());
	for (int j = 0; j < River->RiverTriangles.Num(); j++)
	{
		FTriangleIndex triangle = River->RiverTriangles[j];
		FVector2D first2D = Mesh->t_pos(triangle);
		float z1 = t_elevation.IsValidIndex(triangle) ? t_elevation[triangle] : -1000.0f;
		splinePositions.Add(FVector(first2D.X, first2D.Y, z1 * 10000));

		TArray<FPointIndex> trianglePoints = Mesh->t_circulate_r(triangle);
		for (FPointIndex r : trianglePoints)
		{
			if (processedRegions.Contains(r))
			{
				continue;
			}
			int32 flow = s_flow[River->Downslopes[j]];
			River->RiverFlow[j] = flow;

			// Let river erode part of the landscape
			float originalElevation = r_elevation[r];
			float flowPercent = (float)flow / (float)MaxFlow;
			flowPercent = 1.0f - FMath::Clamp(flowPercent, 0.1f, 0.9f);
			r_elevation[r] -= RiverErosionFactor * flowPercent;
			r_elevation[r] = FMath::Clamp(r_elevation[r], originalElevation * 0.1f, originalElevation * 0.9f);
			processedRegions.Add(r);
		}
	}
	return splinePositions;
}

void AIslandMaker::OnIslandGenComplete_Implementation()
{
	Regions.Empty(r_elevation.Num());
	Regions.SetNum(r_elevation.Num());
	Triangles.Empty(t_elevation.Num());
	Triangles.SetNum(t_elevation.Num());

	for (FPointIndex r = 0; r < Regions.Num(); r++)
	{
		Regions[r].Value = r.Value;
		Regions[r].Elevation = r_elevation[r];
		Regions[r].bIsCoast = r_coast[r];
		Regions[r].bIsOcean = r_ocean[r];
		Regions[r].bIsWater = r_ocean[r];
		Regions[r].DebugColor = r_biome[r].DebugColor;
		Regions[r].Moisture = r_moisture[r];
		Regions[r].Temperature = r_temperature[r];
		Regions[r].WaterDistance = r_waterdistance[r];
		Regions[r].Biome = FGameplayTag::RequestGameplayTag(r_biome[r].Tag);
	}

	for (FTriangleIndex t = 0; t < Triangles.Num(); t++)
	{
		Triangles[t].Value = t.Value;
		Triangles[t].CoastDistance = t_coastdistance[t];
		Triangles[t].Downslope = t_downslope_s[t];
		Triangles[t].Elevation = t_elevation[t];
	}

	for (FTriangleIndex t : spring_t)
	{
		Triangles[t].bIsSpring = true;
	}
	for (FTriangleIndex t : river_t)
	{
		Triangles[t].bIsRiver = true;
	}

	AIslandMapMesh::OnIslandGenComplete_Implementation();
}
