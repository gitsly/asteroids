#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "hash.h"

CHNode::CHNode()
{
	data = NULL;
}

CHNode::~CHNode()
{
	if(data) delete data; data = NULL;
}

CHashTable::CHashTable()
{
	key = NULL;
	value = NULL;
	length = 0;
}

CHashTable::~CHashTable()
{
	if(key) delete key; key = NULL;
	if(value) delete value; value = NULL;
}

void CHashTable::Add(char *k, char *val)
{
	CHNode *node = key;
	CHNode *to = NULL, *nv = NULL;

	// Check if key already exists (owerwrite it if that is the case).
	while(node)
	{
		if(!strcmp(node->data, k))
		{
			to = node;
			break;
		}

		node = node->next;
	}


	if(!to) // create new.
	{
		to = new CHNode;
		to->next = key;
		key = to;

		to->data = new char[strlen(k) + 1];
		strcpy(to->data, k);

		nv = new CHNode;
		nv->data = new char[strlen(val) + 1];
		strcpy(nv->data, val);
		nv->next = value;
		value = nv;
		nv->other = to;
		to->other = nv;
		length ++;
	}
	else // overwrite
	{
		delete to->other->data;
		to->other->data = new char[strlen(val) + 1];
		strcpy(to->other->data, val);
	}
}

// Returns value for a given key.
char *CHashTable::GetValue(char *k)
{
	CHNode *node = key;
	while(node)
	{
		if(!strcmp(node->data, k))
			return node->other->data;
		node = node->next;
	}
	return NULL;
}

// Returns key for a given value.
char *CHashTable::GetKey(char *val)
{
	CHNode *node = value;
	while(node)
	{
		if(!strcmp(node->data, val))
			return node->other->data;
		node = node->next;
	}
	return NULL;
}

