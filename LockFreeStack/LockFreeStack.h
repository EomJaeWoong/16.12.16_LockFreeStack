#ifndef __LOCKFREESTACK__H__
#define __LOCKFREESTACK__H__

#include <malloc.h>

template <class DATA>
class CLockfreeStack
{
public:

	struct st_NODE
	{
		DATA	Data;
		st_NODE *pNext;
	};

	struct st_TOP_NODE
	{
		st_NODE *pTopNode;
		__int64 iUniqueNum;
	};

public:

	/////////////////////////////////////////////////////////////////////////
	// ������
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	CLockfreeStack()
	{
		_lUseSize = 0;

		-pTop = _aligned_malloc(st_TOP_NODE, 1);
		_pTop->pTopNode = NULL;
		_pTop->iUniqueNum = 0;
	}

	/////////////////////////////////////////////////////////////////////////
	// ������
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	virtual ~CLockfreeStack();

	/////////////////////////////////////////////////////////////////////////
	// ���� ������� �뷮 ���.
	//
	// Parameters: ����.
	// Return: (int)������� �뷮.
	/////////////////////////////////////////////////////////////////////////
	long	GetUseSize(void);

	/////////////////////////////////////////////////////////////////////////
	// �����Ͱ� ����°� ?
	//
	// Parameters: ����.
	// Return: (bool)true, false
	/////////////////////////////////////////////////////////////////////////
	bool	isEmpty(void)
	{
		if (_pTop->pTopNode == NULL)
			return true;

		return false;
	}


	/////////////////////////////////////////////////////////////////////////
	// CPacket ������ ����Ÿ ����.
	//
	// Parameters: (DATA)����Ÿ.
	// Return: (bool) true, false
	/////////////////////////////////////////////////////////////////////////
	bool	Push(DATA Data)
	{
		st_NODE *pNode = new st_NODE;
		st_TOP_NODE *pPreTopNode;

		do {
			pPreTopNode = _pTop;

			pNode->Data = Data;
			pNode->pNext = _pTop->pTopNode;
		} while (!InterlockedCompareExchange128(_pTop, ++(_pTop->iUniqueNum), pNode, pPreTopNode);

		iUseSize += sizeof(pNode);

		return true;
	}

	/////////////////////////////////////////////////////////////////////////
	// ����Ÿ ���� ������.
	//
	// Parameters: (DATA *) ���� ������ �־��� ������
	// Return: (bool) true, false
	/////////////////////////////////////////////////////////////////////////
	bool	Pop(DATA *pOutData)
	{
		st_TOP_NODE *pPreTopNode;;
		do
		{
			pPreTopNode = _pTop;
			*pOutData = pPreTopNode->pTopNode->Data;

			pNewTop = _pTop->pNext;

		} while (InterlockedCompareExchange(&_pTop, pNewTop, pPopNode);
	}

private:

	long			_lUseSize;

	st_TOP_NODE	*_pTop;
	__int64			_iUniqueNum;
};

#endif