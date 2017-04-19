#ifndef __Hash_Table_H__
#define __Hash_Table_H__


class CHashTable;

class CHNode
{
	friend CHashTable;
	public:
	private:
      CHNode();
      ~CHNode();

		char *data;
		CHNode *other; // Points to key / value depending on own type.
		CHNode *next;
};

class CHashTable
{
	public:
      CHashTable();
      ~CHashTable();

		void Add(char *k, char *val);

		char *GetValue(char *);
		char *GetKey(char *);
		int length;

	private:
		CHNode *key;
		CHNode *value;

};


#endif