// Copyright 2018 Jay Stevens

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

#include "NamedIslandRivers.h"

#include "RiverSplineComponent.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDGENERATOR_API URiverSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "River")
	float PointScale;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "River")
	UStaticMesh* RiverMesh;

public:
	URiverSplineComponent();

private:
	USplineMeshComponent* CreateRiver(UNamedRiver* River, int32 SplineIndex, AActor* Parent, UMaterialInterface* RiverMaterial, TMap<FName, UTexture*> TextureMap);
	void CreateTangentWaterfalls();

	UMaterialInstanceDynamic* CreateMaterial(USplineMeshComponent* SplineMesh, UMaterialInterface* RiverMaterial, TMap<FName, UTexture*> TextureMap);
	void SetupMaterialParameters(USplineMeshComponent* SplineMesh);

public:
	TArray<USplineMeshComponent*> CreateRiverMeshes(UNamedRiver* River, AActor* Parent, UMaterialInterface* RiverMaterial, TMap<FName, UTexture*> TextureMap);
};
