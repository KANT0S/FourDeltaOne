#include "StdInc.h"
#include "Save.h"

static XAnimParts* varXAnim;

void Save_XAnimDeltaTrans(XAnimDeltaTrans** varTrans)
{
	XAnimDeltaTrans* var = *varTrans;
	XAnimDeltaTrans* trans;

	Save_Allocate(4, var, &trans);

	if (trans->deltaBase)
	{
		XAnimDeltaTransData* transData;

		Save_Allocate(sizeof(XAnimDeltaTransData), &var->data, &transData);

		char* outVar;

		if (varXAnim->numIndices > 255)
		{
			Save_Allocate((2 * trans->deltaBase) + 2, (((char*)&var->data) + sizeof(XAnimDeltaTransData)), &outVar);
		}
		else
		{
			Save_Allocate(trans->deltaBase + 1, (((char*)&var->data) + sizeof(XAnimDeltaTransData)), &outVar);
		}

		if (trans->deltaBase2 & 0xFF)
		{
			if (transData->unk)
			{
				Save_Allocate(3 * trans->deltaBase2, transData->unk, &outVar);

				transData->unk = (char*)-1;
			}
		}
		else
		{
			if (transData->unk)
			{
				Save_Allocate(6 * trans->deltaBase2, transData->unk, &outVar);

				transData->unk = (char*)-1;
			}
		}
	}
	else
	{
		char* outVar;
		Save_Allocate(12, &var->rawData, &outVar);
	}

	*varTrans = (XAnimDeltaTrans*)-1;
}

void Save_XAnimDeltaQuat(XAnimDeltaQuat** varTrans)
{
	XAnimDeltaQuat* var = *varTrans;
	XAnimDeltaQuat* trans;

	Save_Allocate(4, var, &trans);

	if (trans->deltaBase)
	{
		XAnimDeltaQuatData* transData;

		Save_Allocate(sizeof(XAnimDeltaQuatData), &var->data, &transData);

		char* outVar;

		if (varXAnim->numIndices > 255)
		{
			Save_Allocate((2 * trans->deltaBase) + 2, (((char*)&var->data) + sizeof(XAnimDeltaQuatData)), &outVar);
		}
		else
		{
			Save_Allocate(trans->deltaBase + 1, (((char*)&var->data) + sizeof(XAnimDeltaQuatData)), &outVar);
		}

		if (transData->data)
		{
			Save_Allocate(trans->deltaBase2 * 4, transData->data, &outVar);

			transData->data = (char*)-1;
		}
	}
	else
	{
		char* outVar;
		Save_Allocate(4, &var->rawData, &outVar);
	}

	*varTrans = (XAnimDeltaQuat*)-1;
}

void Save_XAnimDeltaUnk(XAnimDeltaUnk** varTrans)
{
	XAnimDeltaUnk* var = *varTrans;
	XAnimDeltaUnk* trans;

	Save_Allocate(4, var, &trans);

	if (trans->deltaBase)
	{
		XAnimDeltaUnkData* transData;

		Save_Allocate(sizeof(XAnimDeltaUnkData), &var->data, &transData);

		char* outVar;

		if (varXAnim->numIndices > 255)
		{
			Save_Allocate((2 * trans->deltaBase) + 2, (((char*)&var->data) + sizeof(XAnimDeltaUnkData)), &outVar);
		}
		else
		{
			Save_Allocate(trans->deltaBase + 1, (((char*)&var->data) + sizeof(XAnimDeltaUnkData)), &outVar);
		}

		if (transData->data)
		{
			Save_Allocate(trans->deltaBase2 * 8, transData->data, &outVar);

			transData->data = (char*)-1;
		}
	}
	else
	{
		char* outVar;
		Save_Allocate(8, &var->rawData, &outVar);
	}

	*varTrans = (XAnimDeltaUnk*)-1;
}

void Save_XAnim(XAnimParts** var)
{
	static_assert(sizeof(XAnimParts) == 88, "n");

	char* anim = (char*)*var;

	if (!Save_GetPointer(anim, (DWORD*)var))
	{
		varXAnim = *var;

		XAnimParts* varAnim;
		Save_PushStream(0);
		Save_Allocate(sizeof(XAnimParts), anim, &varAnim);
		Save_PopStream();

		Save_XString(&varAnim->name);

		void* outVar;

		if (varAnim->tagList)
		{
			short* boneNames;

			Save_Allocate(2 * varAnim->numTags1, varAnim->tagList, &boneNames);

			for (int i = 0; i < varAnim->numTags1; i++)
			{
				boneNames[i] = Save_ScriptString(SL_ConvertToString(boneNames[i]));
			}

			varAnim->tagList = (short*)-1;
		}

		if (varAnim->tagList2)
		{
			XAnimTL* boneNames;

			//Save_Allocate(8 * (varAnim->pad2 & 0xFF), varAnim->tagList2, &boneNames);
			Save_Allocate(8 * varAnim->numTags2, varAnim->tagList2, &boneNames);
			//Save_Allocate(8 * varAnim->numTags2, varAnim->tagList2, &boneNames);

			for (int i = 0; i < (varAnim->numTags2); i++)
			{
				boneNames[i].tagName = Save_ScriptString(SL_ConvertToString(boneNames[i].tagName));
			}

			varAnim->tagList2 = (XAnimTL*)-1;
		}

		if (varAnim->deltaParts)
		{
			XAnimDeltaPart* parts;

			Save_Allocate(sizeof(XAnimDeltaPart), varAnim->deltaParts, &parts);

			if (parts->trans)
			{
				Save_XAnimDeltaTrans(&parts->trans);
			}

			if (parts->quat)
			{
				Save_XAnimDeltaQuat(&parts->quat);
			}

			if (parts->unk)
			{
				Save_XAnimDeltaUnk(&parts->unk);
			}

			varAnim->deltaParts = (XAnimDeltaPart*)-1;
		}

		if (varAnim->unk1)
		{
			Save_Allocate(varAnim->numUnk1, varAnim->unk1, &outVar);

			varAnim->unk1 = (char*)-1;
		}

		if (varAnim->unk2)
		{
			Save_Allocate(varAnim->numUnk2 * 2, varAnim->unk2, &outVar);

			varAnim->unk2 = (char*)-1;
		}

		if (varAnim->unk3)
		{
			Save_Allocate(varAnim->numUnk3 * 4, varAnim->unk3, &outVar);

			varAnim->unk3 = (char*)-1;
		}

		if (varAnim->unk6)
		{
			Save_Allocate(varAnim->numUnk6 * 2, varAnim->unk6, &outVar);

			varAnim->unk6 = (char*)-1;
		}

		if (varAnim->unk4)
		{
			Save_Allocate(varAnim->numUnk4, varAnim->unk4, &outVar);

			varAnim->unk4 = (char*)-1;
		}

		if (varAnim->unk5)
		{
			Save_Allocate(varAnim->numUnk5 * 4, varAnim->unk5, &outVar);

			varAnim->unk1 = (char*)-1;
		}

		if (varAnim->indices)
		{
			if (varAnim->numIndices > 255)
			{
				Save_Allocate(varAnim->numIndices * 2, varAnim->indices, &outVar);
			}
			else
			{
				Save_Allocate(varAnim->numIndices, varAnim->indices, &outVar);
			}

			varAnim->indices = (char*)-1;
		}

		*(DWORD*)var = -2;
	}
}

void SL_MarkXAnim(XAnimParts* varAnim)
{
	if (varAnim->tagList)
	{
		for (int i = 0; i < varAnim->numTags1; i++)
		{
			Save_ScriptString(SL_ConvertToString(varAnim->tagList[i]));
		}
	}

	if (varAnim->tagList2)
	{
		for (int i = 0; i < (varAnim->numTags2); i++)
		{
			Save_ScriptString(SL_ConvertToString(varAnim->tagList2[i].tagName));
		}
	}
}