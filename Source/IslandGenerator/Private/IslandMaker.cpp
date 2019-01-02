// Copyright 2018 Jay Stevens

#include "IslandMaker.h"

void AIslandMaker::OnRiverGenerationComplete_Implementation()
{
	const UNamedIslandRivers* namedRivers = Cast<UNamedIslandRivers>(Rivers);
	if (namedRivers == NULL)
	{
		return;
	}

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
		TArray<FVector> splinePoints = namedRivers->ProcessRiver(r_elevation, Mesh, river, t_elevation, s_flow, maxFlow, ZScale);
		URiverSplineComponent* riverSpline = namedRivers->CreateRiverSpine(this, river, splinePoints);
		RiverSplines.Add(riverSpline);
	}

	AIslandMapMesh::OnRiverGenerationComplete_Implementation();
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
