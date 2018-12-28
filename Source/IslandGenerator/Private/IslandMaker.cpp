// Copyright 2018 Jay Stevens

#include "IslandMaker.h"

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

	Super::OnIslandGenComplete_Implementation();
}
