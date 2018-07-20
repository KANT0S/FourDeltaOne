#include "StdInc.h"
#include "Save.h"

void Save_XSurfaceCT(XSurfaceCT* var, int num)
{
	XSurfaceCT* varCT;
	Save_Allocate(sizeof(XSurfaceCT) * num, var, &varCT);

	for (int i = 0; i < num; i++)
	{
		if (varCT[i].entry)
		{
			if (!Save_GetPointer(varCT[i].entry, &varCT[i].entry))
			{
				void* outVar;
				XSurfaceCTEntry* varEntry;
				Save_Allocate(sizeof(XSurfaceCTEntry), varCT[i].entry, &varEntry);

				if (varEntry->node)
				{
					Save_Align(15);
					Save_Allocate(16 * varEntry->numNode, varEntry->node, &outVar);

					varEntry->node = (char*)-1;
				}

				if (varEntry->leaf)
				{
					Save_Allocate(2 * varEntry->numLeaf, varEntry->leaf, &outVar);

					varEntry->leaf = (short*)-1;
				}

				varCT[i].entry = (XSurfaceCTEntry*)-1;
			}
		}
	}
}

void Save_XSurface(XSurface* var)
{
	// blend info
	if (var->blendInfo)
	{
		if (!Save_GetPointer(var->blendInfo, &var->blendInfo))
		{
			void* outVar;
			Save_Allocate(2 * (var->blendNum1 + (3 * var->blendNum2) + (5 * var->blendNum3) + (7 * var->blendNum4)), var->blendInfo, &outVar);

			var->blendInfo = (short*)-1;
		}
	}

	Save_PushStream(6);

	if (var->vertexBuffer)
	{
		if (!Save_GetPointer(var->vertexBuffer, &var->vertexBuffer))
		{
			void* outVar;

			Save_Align(15);
			Save_Allocate(32 * var->numVertices, var->vertexBuffer, &outVar);

			var->vertexBuffer = (void*)-1;
		}
	}

	Save_PopStream();

	if (var->ct)
	{
		if (!Save_GetPointer(var->ct, &var->ct))
		{
			Save_XSurfaceCT(var->ct, var->numCT);

			var->ct = (XSurfaceCT*)-1;
		}
	}

	Save_PushStream(7);

	if (var->indexBuffer)
	{
		if (!Save_GetPointer(var->indexBuffer, &var->indexBuffer))
		{
			void* outVar;

			Save_Allocate(6 * var->numPrimitives, var->indexBuffer, &outVar);

			var->indexBuffer = (void*)-1;
		}
	}

	Save_PopStream();
}

void Save_XModelSurfs(XModelSurfs** var)
{
	char* model = (char*)*var;

	if (!Save_GetPointer(model, (DWORD*)var))
	{
		XModelSurfs* varModel;
		Save_PushStream(0);
		Save_Allocate(sizeof(XModelSurfs), model, &varModel);
		Save_PopStream();

		Save_XString(&varModel->name);

		if (varModel->surfaces)
		{
			XSurface* surfaces;
			Save_Allocate(sizeof(XSurface) * varModel->numSurfaces, varModel->surfaces, &surfaces);

			for (int i = 0; i < varModel->numSurfaces; i++)
			{
				Save_XSurface(&surfaces[i]);
			}

			varModel->surfaces = (XSurface*)-1;
		}

		*(DWORD*)var = -2;
	}
}

void Save_XModelStuffs(char* var, int num)
{
	char* outVar;

	Save_Allocate(44 * num, var, &outVar);

	if (num > 0)
	{
		for (int i = 0; i < num; i++)
		{
			if (*(DWORD*)(outVar + 0))
			{
				void* v;
				Save_Allocate(48 * *(int*)(outVar + 4), *(void**)(outVar + 0), &v);

				*(DWORD*)(outVar + 0) = -1;
			}

			outVar += 44;
		}
	}
}

void Save_PhysCollmap(PhysCollmap** var)
{
	static_assert(sizeof(PhysCollmap) == 72, "PhysCollmap size != 72");
	static_assert(sizeof(PhysGeomInfo) == 68, "PhysGeomInfo size != 68");
	static_assert(sizeof(BrushWrapper) == 68, "BrushWrapper size != 68");

	char* phys = (char*)*var;

	if (!Save_GetPointer(phys, var))
	{
		PhysCollmap* varPhys;
		Save_PushStream(0);
		Save_Allocate(sizeof(PhysCollmap), phys, &varPhys);
		Save_PopStream();

		Save_XString(&varPhys->name);

		if (varPhys->info) // should always be the case actually
		{
			PhysGeomInfo* varGeomInfo;

			Save_Allocate(sizeof(PhysGeomInfo) * varPhys->numInfo, varPhys->info, &varGeomInfo);

			for (int i = 0; i < varPhys->numInfo; i++)
			{
				if (varGeomInfo->brush)
				{
					char* outVar;
					BrushWrapper* varBrush;
					Save_Allocate(sizeof(BrushWrapper), varGeomInfo->brush, &varBrush);

					if (varBrush->side)
					{
						cbrushside_t* varSide;
						Save_Allocate(sizeof(cbrushside_t) * varBrush->numPlaneSide, varBrush->side, &varSide);

						for (int j = 0; j < varBrush->numPlaneSide; j++)
						{
							if (varSide->plane)
							{
								if (!Save_GetPointer(varSide->plane, &varSide->plane))
								{
									Save_Allocate(sizeof(cplane_t), varSide->plane, &outVar);

									varSide->plane = (cplane_t*)-1;
								}
							}

							varSide++;
						}

						varBrush->side = (cbrushside_t*)-1;
					}

					if (varBrush->edge)
					{
						Save_Allocate(varBrush->numEdge, varBrush->edge, &outVar);

						varBrush->edge = (char*)-1;
					}

					if (varBrush->plane)
					{
						if (!Save_GetPointer(varBrush->plane, &varBrush->plane))
						{
							Save_Allocate(sizeof(cplane_t) * varBrush->numPlaneSide, varBrush->plane, &outVar);

							varBrush->plane = (cplane_t*)-1;
						}
					}

					varGeomInfo->brush = (BrushWrapper*)-1;
				}

				varGeomInfo++;
			}

			varPhys->info = (PhysGeomInfo*)-1;
		}

		*(DWORD*)var = -2;
	}
}

void Save_PhysPreset(PhysPreset** var)
{
	static_assert(sizeof(PhysPreset) == 44, "sizeof(physpreset) != 44");

	char* phys = (char*)*var;

	if (!Save_GetPointer(phys, var))
	{
		PhysPreset* varPhys;
		Save_PushStream(0);
		Save_Allocate(sizeof(PhysPreset), phys, &varPhys);
		Save_PopStream();

		Save_XString(&varPhys->name);

		Save_XString(&varPhys->unkString);

		*(DWORD*)var = -2;
	}
}

void Save_XModel(XModel** var)
{
	char* model = (char*)*var;

	if (!Save_GetPointer(model, (DWORD*)var))
	{
		XModel* varModel;
		Save_PushStream(0);
		Save_Allocate(sizeof(XModel), model, &varModel);
		Save_PopStream();

		if (false && !strcmp(varModel->name, "t6_wpn_smg_peacekeeper_view"))
		{
			for (int i = 0; i < (varModel->numBones - varModel->numSubBones); i++)
			{
				short* boneShorts = (short*)(varModel->boneUnknown2 + (8 * i));
				float* boneFloats = (float*)(varModel->boneUnknown3 + (12 * i));

				float boneAngles[3];
				float angle90 = boneShorts[3];

				boneAngles[0] = (boneShorts[0] / angle90) * 90.0f;
				boneAngles[1] = (boneShorts[1] / angle90) * 90.0f;
				boneAngles[2] = (boneShorts[2] / angle90) * 90.0f;

				printf("%s (%s): %i %i %i %i (%g %g %g); %g %g %g\n", SL_ConvertToString(varModel->boneNames[i + 1]), SL_ConvertToString(varModel->boneNames[varModel->boneUnknown1[i + 1]]), 
					   boneShorts[0], boneShorts[1], boneShorts[2], boneShorts[3],
					   boneAngles[0], boneAngles[1], boneAngles[2],
					   boneFloats[0], boneFloats[1], boneFloats[2]);
			}

			for (int i = 0; i < varModel->numBones; i++)
			{
				DWORD* stuff = (DWORD*)(((char*)varModel->unknowns) + (28 * i));

				printf("\n%s:\n", SL_ConvertToString(varModel->boneNames[i]));

				for (int j = 0; j < 7; j++)
				{
					printf("%08x %g\n", stuff[j], *(float*)(&stuff[j]));
				}
			}

			ExitProcess(0);
		}

		Save_XString(&varModel->name);

		void* outVar;

		if (varModel->boneNames)
		{
			if (!Save_GetPointer(varModel->boneNames, &varModel->boneNames))
			{
				short* boneNames;

				Save_Allocate(2 * varModel->numBones, varModel->boneNames, &boneNames);

				for (int i = 0; i < varModel->numBones; i++)
				{
					boneNames[i] = Save_ScriptString(SL_ConvertToString(boneNames[i]));
				}

				varModel->boneNames = (short*)-1;
			}
		}

		if (varModel->boneUnknown1)
		{
			if (!Save_GetPointer(varModel->boneUnknown1, &varModel->boneUnknown1))
			{
				Save_Allocate((varModel->numBones - varModel->numSubBones), varModel->boneUnknown1, &outVar);

				varModel->boneUnknown1 = (char*)-1;
			}
		}

		if (varModel->boneUnknown2)
		{
			if (!Save_GetPointer(varModel->boneUnknown2, &varModel->boneUnknown2))
			{
				Save_Allocate((varModel->numBones - varModel->numSubBones) * 8, varModel->boneUnknown2, &outVar);

				varModel->boneUnknown2 = (char*)-1;
			}
		}

		if (varModel->boneUnknown3)
		{
			if (!Save_GetPointer(varModel->boneUnknown3, &varModel->boneUnknown3))
			{
				Save_Allocate((varModel->numBones - varModel->numSubBones) * 12, varModel->boneUnknown3, &outVar);

				varModel->boneUnknown3 = (char*)-1;
			}
		}

		if (varModel->boneUnknown4)
		{
			if (!Save_GetPointer(varModel->boneUnknown4, &varModel->boneUnknown4))
			{
				Save_Allocate((varModel->numBones), varModel->boneUnknown4, &outVar);

				varModel->boneUnknown4 = (char*)-1;
			}
		}

		if (varModel->boneUnknown5)
		{
			if (!Save_GetPointer(varModel->boneUnknown5, &varModel->boneUnknown5))
			{
				Save_Allocate((varModel->numBones) * 32, varModel->boneUnknown5, &outVar);

				varModel->boneUnknown5 = (char*)-1;
			}
		}

		if (varModel->materials)
		{
			Material** outMaterials;
			Save_Allocate(sizeof(Material*) * varModel->numMaterials, varModel->materials, &outMaterials);

			for (int i = 0; i < varModel->numMaterials; i++)
			{
				Save_Material(&outMaterials[i]);
			}

			varModel->materials = (Material**)-1;
		}

		for (int i = 0; i < 4; i++)
		{
			if (varModel->lods[i].surfaces)
			{
				Save_XModelSurfs(&varModel->lods[i].surfaces);
			}
		}

		if (varModel->stuffs)
		{
			Save_XModelStuffs(varModel->stuffs, varModel->numStuffs);

			varModel->stuffs = (char*)-1;
		}

		if (varModel->unknowns)
		{
			Save_Allocate(28 * varModel->numBones, varModel->unknowns, &outVar);

			varModel->unknowns = (char*)-1;
		}

		if (varModel->physPreset)
		{
			Save_PhysPreset(&varModel->physPreset);
		}

		if (varModel->physCollmap)
		{
			Save_PhysCollmap(&varModel->physCollmap);
		}

		*(DWORD*)var = -2;
	}
}

void SL_MarkXModel(XModel* varModel)
{
	if (varModel->boneNames)
	{
		for (int i = 0; i < varModel->numBones; i++)
		{
			Save_ScriptString(SL_ConvertToString(varModel->boneNames[i]));
		}
	}
}