// Pointerlist.cpp: implementation of the Pointerlist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pointerlist.h"

PointerList::PointerList(int sorting) 
{
//	this->wSorting=SO_NONE;//sorting;
	this->wSorting=sorting;

	this->lSortOffset=0;
	this->lPointerCount=0;

	this->pPointers=NULL;

	for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++) this->lCursorPos[aCount]=0;
	mEnumerator=0;
}

PointerList::~PointerList() 
{
	if (this->pPointers!=NULL) free(this->pPointers);
	this->pPointers=NULL;
}

//////////////////////////////////////////////////////////////////////
// Clear Operations
//////////////////////////////////////////////////////////////////////

void PointerList::Clear(void) 
{
	if (this->pPointers!=NULL) free(this->pPointers);
	this->pPointers=NULL;
	this->lPointerCount=0;
}

//////////////////////////////////////////////////////////////////////
// Add Operations
//////////////////////////////////////////////////////////////////////

void PointerList::Add(void *pointer) 
{
	if (this->wSorting==SO_NONE) 
	{
		if (this->lPointerCount==0) 
		{
			this->lPointerCount=1;
			this->pPointers=(long*)malloc(4);
			for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++) this->lCursorPos[aCount]=0;
			*(long*)this->pPointers=(long)pointer;
			return;
		}

		long *pPtr;
		this->lPointerCount++;
		this->pPointers=(long*)realloc(this->pPointers,(this->lPointerCount)<<2);

		pPtr=this->pPointers+(this->lPointerCount-1);
		*pPtr=(long)pointer;

		return;
	}

	AddSort(pointer);
}

int PointerList::GetEnumerator()
{
	mEnumerator++;
	if (mEnumerator>255) mEnumerator=1;

	return mEnumerator;
}


void PointerList::AddSort(void *pointer) 
{
	char *pNewCast;
	char *pOldCast;
	long *newdata;
	long *source,*dest;
	bool placed;
	int count;


	if (this->lPointerCount==0) 
	{
		this->lPointerCount=1;
		this->pPointers=(long*)malloc(4);
		for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++) this->lCursorPos[aCount]=0;
		*(long*)this->pPointers=(long)pointer;
		return;
	}

	pNewCast=(char*)pointer;
	pNewCast+=this->lSortOffset;

	this->lPointerCount++;

	placed=false;
	source=this->pPointers;
	newdata=dest=(long*)malloc((this->lPointerCount)<<2);

	for (count=0;count<this->lPointerCount-1;count++) 
	{
		pOldCast=(char*)(*source);
		pOldCast+=this->lSortOffset;

		if (this->wSorting==SO_ASCENDING)
		 {
			if ((*(int*)pOldCast>*(int*)pNewCast) && placed==false) 
			{
				placed=true;
				for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++)
				{
					if (this->lCursorPos[aCount]>count) this->lCursorPos[aCount]++;
				}
				*dest=(long)pointer;
				dest++;
			}
		}
		else 
		{
			if ((*(int*)pOldCast<*(int*)pNewCast) && placed==false) 
			{
				placed=true;
				for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++)
				{
					if (this->lCursorPos[aCount]>count) this->lCursorPos[aCount]++;
				}
				*dest=(long)pointer;
				dest++;
			}
		}
		*dest=*source;
		dest++;
		source++;
	}
	if (placed==false) 
	{
		placed=true;
		*dest=(long)pointer;
		dest++;
	}

	free(this->pPointers);
	this->pPointers=newdata;
}

//////////////////////////////////////////////////////////////////////
// Subtract Operations
//////////////////////////////////////////////////////////////////////

bool PointerList::Subtract(void *pointer) 
{
	long *newdata;
	long *source,*dest;
	int count;
	bool ret;

	newdata=dest=(long*)malloc((this->lPointerCount)<<2);
	source=this->pPointers;

	ret=false;

	for (count=0;count<this->lPointerCount;count++) 
	{
		if ((long)(*source)!=(long)pointer) 
		{
			*dest=*source;
			dest++;
		}
		else 
		{
			for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++)
			{
				if (this->lCursorPos[aCount]>=count) this->lCursorPos[aCount]--;
			}
			ret=true;
		}
		source++;
	}

	if (ret==true) 
	{
		free(this->pPointers);
		newdata=(long*)realloc(newdata,(this->lPointerCount-1)<<2);
		this->pPointers=newdata;
		this->lPointerCount--;
		if (this->lPointerCount<=0) 
		{
			free(this->pPointers);
			this->pPointers=NULL;
		}
	} 
	else free(newdata);

	for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++)
	{
		if (this->lCursorPos[aCount]<0) this->lCursorPos[aCount]=0;
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////
// Search Operations
//////////////////////////////////////////////////////////////////////

bool PointerList::Search(void *pointer) 
{
	long *source;
	int count;
	short ret;

	source=this->pPointers;
	ret=0;

	for (count=0;count<this->lPointerCount;count++) 
	{
		if ((long)(*source)==(long)pointer) ret++;
		source++;
	}

	if (ret>=1) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////
// Sort Operations
//////////////////////////////////////////////////////////////////////

void PointerList::SetSortOffset(void *pointer1, int *pointer2) 
{
	this->lSortOffset=(long)pointer2-(long)pointer1;
}

void PointerList::SetSortOffset(long pos) 
{
	this->lSortOffset=pos;
}

//////////////////////////////////////////////////////////////////////
// Scan Operations -- Switched to inline
//////////////////////////////////////////////////////////////////////
/*
void *PointerList::First(int theEnumerator) 
{
	this->lCursorPos[theEnumerator]=0;
	return Next(theEnumerator);
}

void *PointerList::Next(int theEnumerator)
{
	if (this->lCursorPos[theEnumerator]>=this->lPointerCount) return NULL;
	return (void*)(*(this->pPointers+(this->lCursorPos[theEnumerator]++)));
}
*/



void PointerList::operator+=(void* ptr) 
{
	Add(ptr);
}

void PointerList::operator-=(void* ptr) 
{
	Subtract(ptr);
}

void PointerList::operator+=(PointerList* pl) 
{
	long lHoldCount;
	long *addpos;
	long *readpos;
	int count;

	if (this->wSorting==SO_NONE) 
	{
		lHoldCount=this->lPointerCount;

		this->lPointerCount+=pl->lPointerCount;
		this->pPointers=(long*)realloc(this->pPointers,this->lPointerCount<<2);

		addpos=(this->pPointers+lHoldCount);
		readpos=pl->pPointers;

		for (count=0;count<pl->lPointerCount;count++) 
		{
			*addpos=*readpos;
			addpos++;
			readpos++;
		}
	}
	else 
	{
		EnumPointerlistPtr(int,aItem,pl)
		{
			AddSort(aItem);
		}
	}
}

void PointerList::operator-=(PointerList* pl) 
{
	long *newdata;
	long *source,*dest;
	long data;
	int count;
	short removed;
	bool ret;

	newdata=dest=(long*)malloc((this->lPointerCount)<<2);
	source=this->pPointers;

	ret=false;
	removed=0;

	for (count=0;count<this->lPointerCount;count++) 
	{
		data=*source;
		if (pl->Search((void*)data)==false) 
		{
			*dest=*source;
			dest++;
		}
		else 
		{
			for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++)
			{
				if (this->lCursorPos[aCount]>=count) this->lCursorPos[aCount]--;
			}
			removed++;
			ret=true;
		}
		source++;
	}

	if (ret==true) 
	{
		free(this->pPointers);
		newdata=(long*)realloc(newdata,(this->lPointerCount-removed)<<2);
		this->pPointers=newdata;
		this->lPointerCount-=removed;
		if (this->lPointerCount<=0) 
		{
			free(this->pPointers);
			this->pPointers=NULL;
		}
	}
	else free(newdata);

	for (int aCount=0;aCount<MAX_ENUMERATORS;aCount++)
	{
		if (this->lCursorPos[aCount]<0) this->lCursorPos[aCount]=0;
	}
}

int PointerList::GetCount() 
{
	return lPointerCount;
}

int PointerList::GetCursorPos(int theEnumerator) 
{
	return lCursorPos[theEnumerator];
}

void PointerList::SetCursorPos(int pos,int theEnumerator) 
{
	lCursorPos[theEnumerator]=pos;
}

void PointerList::Insert(void *pointer, int pos) 
{
	if (pos>lPointerCount) pos=lPointerCount;

	this->lPointerCount++;

	long *newdata;
	long *source,*dest;

	source=this->pPointers;
	newdata=dest=(long*)malloc((this->lPointerCount)<<2);

	int count=0;
	while (count++<pos) 
	{
		*dest=*source;
		dest++;
		source++;
	}

	*dest=(long)pointer;dest++;

	while (count++<lPointerCount) 
	{
		*dest=*source;
		dest++;
		source++;
	}

	free(this->pPointers);
	this->pPointers=newdata;
}


