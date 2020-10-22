// Pointerlist.h: interface for the Pointerlist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTERLIST_H__5CC301C8_1E50_4D65_81B9_B2D96A72596A__INCLUDED_)
#define AFX_POINTERLIST_H__5CC301C8_1E50_4D65_81B9_B2D96A72596A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#define SO_NONE			0
#define SO_ASCENDING	1
#define SO_DESCENDING	2
#define MAX_ENUMERATORS	256



//#define EnumPointerlist(type,variable,list) for (int aEnum=list.GetEnumerator(),type variable=(type)list.First(aEnum); variable!=NULL;variable=(type)list.Next(aEnum))
//#define EnumPointerlistPtr(type,variable,list) for (type variable=(type)list->First(0); variable!=NULL;variable=(type)list->Next(0))
#define EnumPointerlist(type,variable,list) for (type *aEnum=(type *)list.GetEnumerator(), *variable=(type *)list.First((int)aEnum); variable!=NULL;variable=(type *)list.Next((int)aEnum))
#define EnumPointerlistPtr(type,variable,list) for (type *aEnum=(type *)list->GetEnumerator(), *variable=(type *)list->First((int)aEnum); variable!=NULL;variable=(type *)list->Next((int)aEnum))


class PointerList 
{
public:
	long			lPointerCount;					// How many pointers are stored
	long			lCursorPos[MAX_ENUMERATORS];	// Where the cursor is positioned.
	long			mEnumerator;					// Enumerator

	long			*pPointers;			// The pointers themselves
	long			lSortOffset;		// When sorting is turned on, the offset within a pointer
										// to a long integer that will be used for sorting purposes.

	int				wSorting;			// Whether sorting is on or off.

	void			AddSort(void *pointer);	// Adds a sorted pointer
	
public:
	PointerList(int sorting=SO_NONE);
	virtual ~PointerList();

	inline void		*Get(int pos)
					{
						if (pos<0) return NULL;
						if (pos>=lPointerCount) return NULL;
						return (void*)(*(this->pPointers+(pos)));
					}

	int				GetCount(void);
	int				GetCursorPos(int theEnumerator=0);
	void			SetCursorPos(int pos, int theEnumerator=0);

	void			Insert(void *pointer, int pos);
	void			Add(void *pointer);	
	bool			Subtract(void *pointer);	
	bool			Search(void *pointer);	
	void			Clear(void);
	
	// Sort functions allow you to set the sortoffset.  You can pass it as two pointers
	// for reference (i.e. &theclass,&theclass.member), or as an integer position.  The
	// sort number within a class or struct must be an integer.
	void			SetSortOffset(void *pointer1, int *pointer2);	
	void			SetSortOffset(long pos);						
	
	// First and Next are "Reset" and "Scan" designed for a for/next loop
	inline void		*First(int theEnumerator=0)
	{
		this->lCursorPos[theEnumerator]=0;
		return Next(theEnumerator);
	}
	inline void		*Next(int theEnumerator=0)
	{
		if (this->lCursorPos[theEnumerator]>=this->lPointerCount) return NULL;
		return (void*)(*(this->pPointers+(this->lCursorPos[theEnumerator]++)));
	}




	int				GetEnumerator();
	
	// Overloaded operators
	// Can use += and -= to add and subtract other pointerlists from this one.
	void operator+=	(PointerList*);
	void operator-=	(PointerList*);
	void operator+=	(void*);
	void operator-=	(void*);

};
	
#endif // !defined(AFX_POINTERLIST_H__666471E3_1C20_4D71_BCF4_FC1161F4B9F0__INCLUDED_)


