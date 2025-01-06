#ifndef	LIST_H
#define LIST_H

class CFileName
{
	CFileName(char* pszName);
	CFileName(CFileName* pCopy);	
}

template <class T>
class CList
{

private:

	T *m_pHead;
	T *m_pTail;

	int m_nSize;

public:
	
	CList();
	virtual ~CList();
	virtual void operator << (T& pNode); 
	virtual void operator << (T* pNode);
	virtual void operator -- (int);
	virtual int Size();
	virtual T* operator [] ( int nIndex );
	virtual void Clear();
};


#endif
