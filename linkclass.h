/* Filename:	linkclass.h
 * Author:		 Martin Collberg
 * Email:		  comayog@swipnet.se
 * Created:		00-09-30
 * Modified:   00-09-30
 *
 * A linked list class
 */

#ifndef __LinkClass_Included__
#define __LinkClass_Included__

template <class T>
class CLinkNode
{
	public:
		CLinkNode() { next = NULL; data = NULL; }
		~CLinkNode() 
		{
			if(data) delete data; 
			if(next) delete next;
			next = NULL;
		}
		
		T *data;	
		CLinkNode<T> *next;
};

template <class T>
class CLink
{
	public:
		
		CLink() { cap = NULL; cur = NULL; }
		~CLink() { if(cap) delete cap; }

		CLinkNode<T> *cap; 
		CLinkNode<T> *cur;  

		void Add(T *item);
		void Remove(T *item);
		bool Loop(T &ret);
		void Disable(); // Disables the list so when it's destructed, none of the data is deleted.
		void Clear();
};

template <class T>
void CLink<T>::Clear()
{
	if(cap) 
		delete cap;
	cap = NULL;	
}

template <class T>
void CLink<T>::Disable()
{
	cap = NULL;	
}

template <class T>
void CLink<T>::Add(T *item)
{
	CLinkNode<T> *link = new CLinkNode<T>;	
	link->data = item;
	link->next = cap;
	cap = link;
}

template <class T>
void CLink<T>::Remove(T *item)
{
	CLinkNode<T> *t;	

	cur = cap;	
	if(cur->data == item)
	{
		cap = cur->next;
		cur->next = NULL;
		delete cur;
		cur = NULL;
		return;			
	}	
	while(cur)
	{
		if(cur->next && cur->next->data == item)
		{
			t = cur->next->next;
			cur->next->next = NULL;
			delete cur->next;
			cur->next = t;
			break;
		}
		cur = cur->next;
	}
	cur = NULL;
}

template <class T>
bool CLink<T>::Loop(T &ret)
{
	static bool ended = true;
	if(!cur && ended)
	{
		cur = cap;
		ended = false;
	}
	ended = true;
	if(cur)
	{
		ret = *cur->data;
		ended = false;
		cur = cur->next;
	}	
	return !ended;
}


#endif