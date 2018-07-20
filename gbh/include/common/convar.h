// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Console variable system include file.
//
// Initial author: NTAuthority
// Started: 2013-03-02
// ==========================================================

#ifndef _CONVAR_H
#define _CONVAR_H

enum ConVarFlags
{
	CONVAR_NONE = 0,
	CONVAR_ARCHIVED = 1,
	CONVAR_READ_ONLY = 2,
	CONVAR_WRITE_PROTECTED = 4,
	CONVAR_CHEAT = 8,
	CONVAR_USER_CREATED = 16
};

union ConVarValue
{
	int integer;
	const char* string;
	float number;
};

enum ConVarType
{
	CONVAR_INT,
	CONVAR_STRING,
	CONVAR_FLOAT,
	CONVAR_ENUM,
};

class ConVar
{
friend class gbhConVarManagerLocal;

protected:
	const char* name;
	const char* description;
	uint32_t flags;
	ConVarType type;
	ConVarValue current;
	ConVarValue default;

	const char** enumValues;
	int enumValueCount;

	char stringRep[16];

public:
	ConVar() {}

	// getters
	inline const char* getName() { return name; }
	inline ConVarType getType() { return type; }
	inline ConVarValue* getValue() { return &current; }

	// setters
	void setValue(const char* string) { setValue(string, true); }
	void setValue(const char* string, bool force);

	// string conversion for display
	const char* toString();
};

class gbhConVarManager
{
public:
	// initializes the convar system; should only be called by Game::initialize()/friends
	virtual void initialize() = 0;

	// returns a specific convar, or nullptr if none was found
	virtual ConVar* find(const char* name) = 0;

	// registers a console variable of various types
	virtual ConVar* registerVar(const char* name, const char* defaultValue, const char* description, uint32_t flags) = 0;
	virtual ConVar* registerVar(const char* name, int defaultValue, const char* description, uint32_t flags) = 0;
	virtual ConVar* registerVar(const char* name, float defaultValue, const char* description, uint32_t flags) = 0;
	virtual ConVar* registerVar(const char* name, int defaultValue, const char** enumValues, const char* description, uint32_t flags) = 0;

	// sets a console variable to a specified (string) value
	virtual ConVar* setValue(const char* name, const char* value) = 0;
	virtual ConVar* setValue(const char* name, const char* value, bool force) = 0;
};

extern gbhConVarManager* conVarManager;

#endif