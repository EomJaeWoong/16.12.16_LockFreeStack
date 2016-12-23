#ifndef __LOCKFREESTACK__H__
#define __LOCKFREESTACK__H__

#include <malloc.h>
#include <Windows.h>

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
	// 생성자
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	CLockfreeStack()
	{
		_lUseSize = 0;

		_pTop = (st_TOP_NODE *)_aligned_malloc(sizeof(st_TOP_NODE), 16);
		_pTop->pTopNode = NULL;
		_pTop->iUniqueNum = 0;
	}

	/////////////////////////////////////////////////////////////////////////
	// 생성자
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	virtual ~CLockfreeStack()
	{
		st_NODE *pNode;
		while (_pTop->pTopNode != NULL)
		{
			pNode = _pTop->pTopNode;
			_pTop->pTopNode = _pTop->pTopNode->pNext;
			free(pNode);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// 현재 사용중인 용량 얻기.
	//
	// Parameters: 없음.
	// Return: (int)사용중인 용량.
	/////////////////////////////////////////////////////////////////////////
	long	GetUseSize(void);

	/////////////////////////////////////////////////////////////////////////
	// 데이터가 비었는가 ?
	//
	// Parameters: 없음.
	// Return: (bool)true, false
	/////////////////////////////////////////////////////////////////////////
	bool	isEmpty(void)
	{
		if (_pTop->pTopNode == NULL)
			return true;

		return false;
	}


	/////////////////////////////////////////////////////////////////////////
	// CPacket 포인터 데이타 넣음.
	//
	// Parameters: (DATA)데이타.
	// Return: (bool) true, false
	/////////////////////////////////////////////////////////////////////////
	bool	Push(DATA Data)
	{
		st_NODE *pNode = new st_NODE;
		st_TOP_NODE pPreTopNode;

		do {
			pPreTopNode.pTopNode = _pTop->pTopNode;
			pPreTopNode.iUniqueNum = _pTop->iUniqueNum;

			pNode->Data = Data;
			pNode->pNext = _pTop->pTopNode;
		} while (!InterlockedCompareExchange128((volatile LONG64 *)_pTop, _pTop->iUniqueNum, (LONG64)pNode, (LONG64 *)&pPreTopNode));
		InterlockedIncrement64((LONG64 *)&_pTop->iUniqueNum);
		_lUseSize += sizeof(pNode);

		return true;
	}

	/////////////////////////////////////////////////////////////////////////
	// 데이타 빼서 가져옴.
	//
	// Parameters: (DATA *) 뽑은 데이터 넣어줄 포인터
	// Return: (bool) true, false
	/////////////////////////////////////////////////////////////////////////
	bool	Pop(DATA *pOutData)
	{
		st_TOP_NODE pPreTopNode;
		st_NODE *pNode, *pNewTopNode;

		do
		{
			pPreTopNode.pTopNode = _pTop->pTopNode;
			pPreTopNode.iUniqueNum = _pTop->iUniqueNum;

			pNode = _pTop->pTopNode;

			pNewTopNode = _pTop->pTopNode->pNext;

		} while (!InterlockedCompareExchange128((volatile LONG64 *)_pTop, _pTop->iUniqueNum, (LONG64)pNewTopNode, (LONG64 *)&pPreTopNode));
		InterlockedIncrement64((LONG64 *)&_pTop->iUniqueNum);
		*pOutData = pNode->Data;
		free(pNode);

		return true;
	}

private:

	long			_lUseSize;

	st_TOP_NODE	*_pTop;
	__int64			_iUniqueNum;
};

#endif