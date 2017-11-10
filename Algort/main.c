#include <stdio.h>
#include "cJSON.h"

int main()
{
	cJSON * pJsonRoot = NULL;

	pJsonRoot = cJSON_CreateObject();
	if(NULL == pJsonRoot)
	{
		//error happend here
		return 0;
	}
	cJSON_AddStringToObject(pJsonRoot, "hello", "hello world");
	cJSON_AddNumberToObject(pJsonRoot, "number", 10010);
	cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
	cJSON * pSubJson = NULL;
	pSubJson = cJSON_CreateObject();
	if(NULL == pSubJson)
	{
		// create object faild, exit
		cJSON_Delete(pJsonRoot);
		return 0;
	}
	cJSON_AddStringToObject(pSubJson, "subjsonobj", "a sub json string");
	cJSON_AddItemToObject(pJsonRoot, "subobj", pSubJson);

	char * p = cJSON_Print(pJsonRoot);
	if(NULL == p)
	{
		//convert json list to string faild, exit
		//because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
		cJSON_Delete(pJsonRoot);
		return 0;
	}
	printf("%s\n", p);

	return 0;
}

