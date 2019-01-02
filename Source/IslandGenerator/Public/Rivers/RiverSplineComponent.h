// Copyright 2018 Jay Stevens

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

#include "RiverSplineComponent.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDGENERATOR_API URiverSplineComponent : public USplineComponent
{
	GENERATED_BODY()

private:
	USplineMeshComponent* CreateRiver(class UNamedRiver* River, int32 SplineIndex, AActor* Parent, const class UNamedIslandRivers* Rivers);
	void CreateTangentWaterfalls();

	UMaterialInstanceDynamic* CreateMaterial(USplineMeshComponent* SplineMesh, UMaterialInterface* RiverMaterial, TMap<FName, UTexture*> TextureMap);
	void SetupMaterialParameters(USplineMeshComponent* SplineMesh);

public:
	TArray<USplineMeshComponent*> CreateRiverMeshes(class UNamedRiver* River, AActor* Parent, const class UNamedIslandRivers* Rivers);
};
