#include "StdInc.h"
#include <tinyxml.h>

typedef enum
{
	STRUCTURED_DATA_INT = 0,
	STRUCTURED_DATA_BYTE = 1,
	STRUCTURED_DATA_BOOL = 2,
	STRUCTURED_DATA_STRING = 3,
	STRUCTURED_DATA_ENUM = 4,
	STRUCTURED_DATA_STRUCT = 5,
	STRUCTURED_DATA_INDEXEDARR = 6,
	STRUCTURED_DATA_ENUMARR = 7,
	STRUCTURED_DATA_FLOAT = 8,
	STRUCTURED_DATA_SHORT = 9
} structuredDataType_t;

typedef struct
{
	structuredDataType_t type;
	union
	{
		int index;
	};
	int offset;
} structuredDataItem_t;

typedef struct
{
	const char* name;
	structuredDataItem_t item;
	int unknown;
} structuredDataChild_t;

typedef struct
{
	int numChildren;
	structuredDataChild_t* children;
	int unknown1;
	int unknown2;
} structuredDataStruct_t;

typedef struct
{
	int enumIndex;
	structuredDataItem_t item;
} structuredDataEnumArray_t;

typedef struct
{
	const char* key;
	int index;
} structuredDataEnumIndex_t;

typedef struct
{
	int numIndices;
	int unknown;
	structuredDataEnumIndex_t* indices;
} structuredDataEnum_t;

typedef struct
{
	int numItems;
	structuredDataItem_t item;
} structuredDataIndexedArray_t;

typedef struct
{
	int version;
	unsigned int hash;
	int numEnums;
	structuredDataEnum_t* enums;
	int numStructs;
	structuredDataStruct_t* structs;
	int numIndexedArrays;
	structuredDataIndexedArray_t* indexedArrays;
	int numEnumArrays;
	structuredDataEnumArray_t* enumArrays;
	structuredDataItem_t rootItem;
} structuredData_t;

typedef struct
{
	const char* name;
	int unknown;
	structuredData_t* data;
} structuredDataDef_t;

typedef struct
{
	structuredData_t* data;
	structuredDataItem_t* item;
	int offset;
	int error;
} structuredDataFindState_t;


static bool isPlayerData = false;

void StructuredData_GetEnumName(char* buffer, size_t size, int enumIndex)
{
	if (!isPlayerData)
	{
		_snprintf(buffer, size, "enum%d", enumIndex);
		return;
	}

	switch (enumIndex)
	{
	case 0:
		_snprintf(buffer, size, "feature");
		break;
	case 1:
		_snprintf(buffer, size, "weapon");
		break;
	case 2:
		_snprintf(buffer, size, "attachment");
		break;
	case 3:
		_snprintf(buffer, size, "challenge");
		break;
	case 4:
		_snprintf(buffer, size, "camo");
		break;
	case 5:
		_snprintf(buffer, size, "specialty");
		break;
	case 6:
		_snprintf(buffer, size, "killstreak");
		break;
	case 7:
		_snprintf(buffer, size, "award");
		break;
	case 8:
		_snprintf(buffer, size, "cardicon");
		break;
	case 9:
		_snprintf(buffer, size, "cardtitle");
		break;
	case 10:
		_snprintf(buffer, size, "cardnameplate");
		break;
	case 11:
		_snprintf(buffer, size, "team");
		break;
	case 12:
		_snprintf(buffer, size, "gametype");
		break;
	default:
		_snprintf(buffer, size, "enum%d", enumIndex);
		break;
	}
}

void StructuredData_GetStructName(char* buffer, size_t size, int structIndex)
{
	if (!isPlayerData)
	{
		_snprintf(buffer, size, "struct%d", structIndex);
		return;
	}

	switch (structIndex)
	{
	case 0:
		_snprintf(buffer, size, "playerdata");
		break;
	case 1:
		_snprintf(buffer, size, "weaponsetup");
		break;
	case 2:
		_snprintf(buffer, size, "customclass");
		break;
	case 3:
		_snprintf(buffer, size, "awarditem");
		break;
	case 4:
		_snprintf(buffer, size, "rounddata");
		break;
	default:
		_snprintf(buffer, size, "struct%d", structIndex);
		break;
	}
}

static char arrayName[64];

const char* StructuredData_GetEnumArrayName(structuredDataEnumArray_t* enumArray, int index)
{
	char enumName[64];
	StructuredData_GetEnumName(enumName, sizeof(enumName), enumArray->enumIndex);
	_snprintf(arrayName, sizeof(arrayName), "%slist%d", enumName, index);
	return arrayName;
}

static char typeName[64];

const char* StructuredData_TypeName(structuredDataItem_t* item, structuredData_t* data)
{
	switch (item->type)
	{
		case STRUCTURED_DATA_BOOL:
			return "bool";
		case STRUCTURED_DATA_BYTE:
			return "byte";
		case STRUCTURED_DATA_ENUM:
			StructuredData_GetEnumName(typeName, sizeof(typeName), item->index);
			return typeName;
			//return "enum";
		case STRUCTURED_DATA_ENUMARR:
			//StructuredData_GetEnumName(typeName, sizeof(typeName), data->enumArrays[item->index].enumIndex);
			return StructuredData_GetEnumArrayName(&data->enumArrays[item->index], item->index);
			//return typeName;
			//return "enumarr";
		case STRUCTURED_DATA_FLOAT:
			return "float";
		case STRUCTURED_DATA_INDEXEDARR:
			return "indexedarr";
		case STRUCTURED_DATA_INT:
			return "int";
		case STRUCTURED_DATA_SHORT:
			return "short";
		case STRUCTURED_DATA_STRING:
			return "string";
		case STRUCTURED_DATA_STRUCT:
			//return "struct";
			StructuredData_GetStructName(typeName, sizeof(typeName), item->index);
			return typeName;
		default:
			return "unknown";
	}
}

TiXmlElement* StructuredData_DumpToXml(structuredData_t* data, structuredDataItem_t* item, const char* name, int offset)
{
	offset += item->offset;

	TiXmlElement* element = new TiXmlElement(StructuredData_TypeName(item, data));

	if (name[0] == '\0')
	{
		element->SetAttribute("size", item->offset);
	}
	else
	{
		element->SetAttribute("name", name);
		element->SetAttribute("offset", offset);
	}

	if (item->type == STRUCTURED_DATA_STRUCT)
	{
		/*structuredDataStruct_t* structure = &data->structs[item->index];

		for (int i = 0; i < structure->numChildren; i++)
		{
			element->LinkEndChild(StructuredData_DumpToXml(data, &structure->children[i].item, structure->children[i].name, offset));
		}*/

		/*char structName[64];
		StructuredData_GetStructName(structName, sizeof(structName), item->index);

		element->SetAttribute("type", structName);*/
	}
	
	if (item->type == STRUCTURED_DATA_ENUM || item->type == STRUCTURED_DATA_ENUMARR)
	{
		/*char enumName[64];
		structuredDataEnum_t* enumeration;

		if (item->type == STRUCTURED_DATA_ENUMARR)
		{
			StructuredData_GetEnumName(enumName, sizeof(enumName), data->enumArrays[item->index].enumIndex);
			enumeration = &data->enums[data->enumArrays[item->index].enumIndex];
		}
		else
		{
			StructuredData_GetEnumName(enumName, sizeof(enumName), item->index);
			enumeration = &data->enums[item->index];
		}*/

		//element->SetAttribute("type", enumName);
		/*for (int i = 0; i < enumeration->numIndices; i++)
		{
			TiXmlElement* enumIndex = new TiXmlElement("index");
			enumIndex->SetAttribute("name", enumeration->indices[i].key);
			enumIndex->SetAttribute("i", enumeration->indices[i].index);
			element->LinkEndChild(enumIndex);
		}*/
	}

	if (/*item->type == STRUCTURED_DATA_ENUMARR || */item->type == STRUCTURED_DATA_INDEXEDARR)
	{
		structuredDataItem_t* subItem;

		if (item->type == STRUCTURED_DATA_ENUMARR)
		{
			subItem = &data->enumArrays[item->index].item;
		}
		else
		{
			subItem = &data->indexedArrays[item->index].item;
		}

		element->LinkEndChild(StructuredData_DumpToXml(data, subItem, "", offset));
	}

	if (item->type == STRUCTURED_DATA_INDEXEDARR)
	{
		element->SetAttribute("length", data->indexedArrays[item->index].numItems);
	}

	if (item->type == STRUCTURED_DATA_STRING)
	{
		element->SetAttribute("length", item->index);
	}

	return element;
}

void StructuredData_DumpDataDef_f()
{
	char rawFileName[255];
	const char* filename = "mp/matchdata.def";

	isPlayerData = false;

	//_mkdir("raw");
	//_mkdir("raw/mp");

	structuredDataDef_t* data = (structuredDataDef_t*)DB_FindXAssetHeader(42, filename, false);
	if (data)
	{
		TiXmlDocument xdoc;
		TiXmlElement* rootElement = new TiXmlElement("structureddatadef");
		rootElement->LinkEndChild(StructuredData_DumpToXml(data->data, &data->data->rootItem, "Root", 0));
		rootElement->SetAttribute("version", data->data->version);

		TiXmlElement* enums = new TiXmlElement("enums");

		// loop through enums
		for (int i = 0; i < data->data->numEnums; i++)
		{
			char enumName[64];
			StructuredData_GetEnumName(enumName, sizeof(enumName), i);

			TiXmlElement* element = new TiXmlElement("enum");
			element->SetAttribute("name", enumName);

			structuredDataEnum_t* enumeration = &data->data->enums[i];
			for (int j = 0; j < enumeration->numIndices; j++)
			{
				TiXmlElement* index = new TiXmlElement("index");
				index->SetAttribute("name", enumeration->indices[j].key);
				index->SetAttribute("index", enumeration->indices[j].index);
				element->LinkEndChild(index);
			}

			enums->LinkEndChild(element);
		}

		TiXmlElement* structs = new TiXmlElement("structs");

		// loop through structs
		for (int i = 0; i < data->data->numStructs; i++)
		{
			char structName[64];
			StructuredData_GetStructName(structName, sizeof(structName), i);

			TiXmlElement* element = new TiXmlElement("struct");
			element->SetAttribute("name", structName);

			structuredDataStruct_t* structure = &data->data->structs[i];
			for (int j = 0; j < structure->numChildren; j++)
			{
				element->LinkEndChild(StructuredData_DumpToXml(data->data, &structure->children[j].item, structure->children[j].name, 0));
			}

			structs->LinkEndChild(element);
		}

		TiXmlElement* enumArrays = new TiXmlElement("enumarrays");

		// loop through enumarrays
		for (int i = 0; i < data->data->numEnumArrays; i++)
		{
			char enumName[64];
			StructuredData_GetEnumName(enumName, sizeof(enumName), data->data->enumArrays[i].enumIndex);

			TiXmlElement* element = new TiXmlElement("enumarray");
			element->SetAttribute("name", StructuredData_GetEnumArrayName(&data->data->enumArrays[i], i));
			element->SetAttribute("enum", enumName);
			element->LinkEndChild(StructuredData_DumpToXml(data->data, &data->data->enumArrays[i].item, "", 0));

			enumArrays->LinkEndChild(element);
		}

		rootElement->LinkEndChild(structs);
		rootElement->LinkEndChild(enumArrays);
		rootElement->LinkEndChild(enums);
		xdoc.LinkEndChild(rootElement);

		sprintf(rawFileName, "raw/%s", filename);
		xdoc.SaveFile(rawFileName);

		return;
	}

	OutputDebugString("No such structured data definition.\n");
}