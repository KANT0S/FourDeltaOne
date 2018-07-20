// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Console variable system include file.
//
// Initial author: NTAuthority
// Started: 2013-03-02
// ==========================================================

#include <gbh.h>
#include <unordered_map>

static ConVar* com_cheats;

class gbhConVarManagerLocal : public gbhConVarManager
{
private:
	typedef std::unordered_map<std::string, ConVar>::iterator convar_iterator;
	std::unordered_map<std::string, ConVar> conVars;

protected:
	inline int getEnumCount(const char** enumValues)
	{
		if (!enumValues)
		{
			return 0;
		}

		int c = 0;

		for (; *enumValues; enumValues++)
		{
			c++;
		}

		return c;
	}

	ConVar* reregisterConVar(const char* name, ConVarType type, ConVarValue defaultValue, const char** enumValues, const char* description, uint32_t flags, ConVar* var)
	{
		// convert the convar
		const char* value = var->toString();
		var->type = type;
		var->setValue(value, true);

		// set other details
		var->default = defaultValue;
		var->description = description;
		var->enumValues = enumValues;
		var->enumValueCount = getEnumCount(enumValues);
		var->flags = flags;

		return var;
	}

	ConVar* registerNewConVar(const char* name, ConVarType type, ConVarValue defaultValue, const char** enumValues, const char* description, uint32_t flags)
	{
		ConVar var;
		var.name = stringList->duplicateWithRef(name);
		var.current = defaultValue;
		var.default = defaultValue;
		var.description = description;
		var.enumValues = enumValues;
		var.enumValueCount = getEnumCount(enumValues);
		var.flags = flags;
		var.type = type;

		// special type handling (string duplication; for instance)
		switch (type)
		{
			case CONVAR_STRING:
				var.current.string = stringList->duplicateWithRef(var.current.string);
				var.default.string = stringList->duplicateWithRef(var.default.string);
				break;
		}

		conVars[name] = var;

		return &conVars[name];
	}

	ConVar* registerConVar(const char* name, ConVarType type, ConVarValue defaultValue, const char** enumValues, const char* description, uint32_t flags)
	{
		ConVar* existingVar = find(name);

		if (existingVar != nullptr)
		{
			return reregisterConVar(name, type, defaultValue, enumValues, description, flags, existingVar);
		}
		else
		{
			return registerNewConVar(name, type, defaultValue, enumValues, description, flags);
		}
	}

public:
	virtual void initialize() 
	{
		com_cheats = registerVar("cheats", 0, "Enable cheats", CONVAR_NONE);
	}

	virtual ConVar* find(const char* name) 
	{
		const char* lowerName = strtolower(name);
		convar_iterator it = conVars.find(lowerName);

		// we don't need the lowercase name anymore
		free(const_cast<char*>(lowerName));
		
		// check if the convar was actually found
		if (it != conVars.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	virtual ConVar* registerVar(const char* name, const char* defaultValue, const char* description, uint32_t flags) 
	{
		ConVarValue value;
		value.string = defaultValue;

		return registerConVar(name, CONVAR_STRING, value, nullptr, description, flags);
	}

	virtual ConVar* registerVar(const char* name, float defaultValue, const char* description, uint32_t flags) 
	{
		ConVarValue value;
		value.number = defaultValue;

		return registerConVar(name, CONVAR_FLOAT, value, nullptr, description, flags);
	}

	virtual ConVar* registerVar(const char* name, int defaultValue, const char* description, uint32_t flags) 
	{
		ConVarValue value;
		value.integer = defaultValue;

		return registerConVar(name, CONVAR_INT, value, nullptr, description, flags);
	}

	virtual ConVar* registerVar(const char* name, int defaultValue, const char** enumValues, const char* description, uint32_t flags) 
	{
		ConVarValue value;
		value.integer = defaultValue;

		return registerConVar(name, CONVAR_ENUM, value, enumValues, description, flags);
	}

	virtual ConVar* setValue(const char* name, const char* value) 
	{
		return setValue(name, value, true);
	}

	virtual ConVar* setValue(const char* name, const char* value, bool force) 
	{
		ConVar* existingVar = find(name);

		if (existingVar)
		{
			existingVar->setValue(value, force);
		}
		else
		{
			existingVar = registerVar(name, value, "External console variable", (force) ? 0 : CONVAR_USER_CREATED);
		}

		return existingVar;
	}

};

static gbhConVarManagerLocal conVarManagerLocal;
gbhConVarManager* conVarManager = &conVarManagerLocal;

// --- ConVar members ---------------------------------------------------------
void ConVar::setValue(const char* string, bool force)
{
	if (!force)
	{
		if (flags & CONVAR_READ_ONLY)
		{
			printf("%s is read only.", name);
			return;
		}

		if (flags & CONVAR_WRITE_PROTECTED)
		{
			printf("%s is write protected.", name);
			return;
		}

		if ((flags & CONVAR_CHEAT) && !com_cheats->getValue()->integer)
		{
			printf("%s is read only.", name);
			return;
		}
	}

	switch (type)
	{
		case CONVAR_INT:
			current.integer = atoi(string);
			break;
		case CONVAR_FLOAT:
			current.number = (float)atof(string);
			break;
		case CONVAR_STRING:
			if (current.string)
			{
				stringList->freeDuplicate(current.string);
			}

			current.string = stringList->duplicateWithRef(string);
			break;
		case CONVAR_ENUM:
			{
				int i = 0;

				for (const char** p = enumValues; *p; p++)
				{
					if (!_stricmp(*p, string))
					{
						current.integer = i;
						return;
					}

					i++;
				}

				printf("%s is an invalid value for %s.\n", string, name);
			}

			break;
	}
}

const char* ConVar::toString()
{
	switch (type)
	{
		case CONVAR_INT:
			_snprintf(stringRep, sizeof(stringRep), "%i", current.integer);
			break;
		case CONVAR_FLOAT:
			_snprintf(stringRep, sizeof(stringRep), "%g", current.number);
			break;
		case CONVAR_STRING:
			return current.string;
		case CONVAR_ENUM:
			return (enumValues[current.integer]);
	}

	stringRep[sizeof(stringRep) - 1] = 0;
	return stringRep;
}