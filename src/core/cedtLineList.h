#pragma once

#include <vector>

// CLineList - a drop-in replacement for MFC's CList that can reach line n without
// walking to it.
//
// Why:  the document's lines used to live in a CList, a doubly-linked list. Reaching
// line n meant walking n nodes. On a 900,000-line file, reaching the last line took
// 7 ms -- and worse than linearly, because the nodes are scattered and a deep walk
// misses every level of cache. CCedtView::GetLineFromPosY is the gateway every caret
// move, edit, search and repaint goes through, and it paid that on every call.
//
// The linked list did not even win at its own game. Its supposed advantage is O(1)
// insertion, but to insert you must first FIND the place, and that walk dominates:
// inserting a line in the middle of 900,000 measured 3,378 us on the CList against
// 187 us here. See docs/refactoring-line-container.md.
//
//
// HOW IT WORKS, AND THE ONE THING TO KNOW
//
// Elements are held by pointer in a vector. The pointers move; the objects never do.
// That is not a preference, it is required: the editor holds C++ references into the
// container across mutations of it (CCedtDoc::InsertBlock does), and a vector of
// values would relocate its elements and leave those references dangling. Holding
// pointers also makes an insert move 8 bytes per line rather than a whole line object.
//
// POSITION is the element's index, plus one so that index 0 is not NULL. It is
// therefore NOT the stable thing a CList POSITION was:
//
//     *** A POSITION IS INVALIDATED BY ANY STRUCTURAL CHANGE TO THE LIST. ***
//
// A CList POSITION was a node pointer, so removing some OTHER element left it alone.
// An index does not survive that -- remove element i and everything after it shifts
// down, so a POSITION past i now silently names the wrong line. That is the exact
// failure mode this class must not be allowed to have: no crash, just the wrong line
// edited.
//
// So it is not left to whoever writes the next loop to remember. In debug builds the
// list keeps a modification counter, every POSITION is stamped with the counter it was
// minted under, and every use of a POSITION asserts the stamp still matches. A stale
// POSITION stops the debugger on the line that used it. In release builds the stamp
// and the check compile away and a POSITION is just an index.
//
// Legitimate code is unaffected -- reassigning from the return value, as in
// `pos = InsertAfter(pos, x)`, yields a freshly minted POSITION.
//
// Bulk edits: use InsertGap / RemoveRange. Removing n lines one at a time is n
// memmoves, which turns a large block delete from an edit into a hang.

template<class TYPE, class ARG_TYPE>
class CLineList
{
public:
	// nBlockSize is accepted and ignored. CList used it to batch its node allocations;
	// there are no nodes here. Kept so the CList-derived classes can keep their ctors.
	explicit CLineList(INT_PTR nBlockSize = 10)
	{
		(void)nBlockSize;
#ifdef _DEBUG
		m_nModCount = 1;
#endif
	}

	virtual ~CLineList() { RemoveAll(); }

	// -- size ------------------------------------------------------------------

	INT_PTR GetCount() const  { return (INT_PTR)m_vec.size(); }
	INT_PTR GetSize() const   { return (INT_PTR)m_vec.size(); }
	BOOL    IsEmpty() const   { return m_vec.empty(); }

	// -- positions -------------------------------------------------------------

	POSITION FindIndex(INT_PTR nIndex) const
	{
		if( nIndex < 0 || nIndex >= (INT_PTR)m_vec.size() ) return NULL;
		return _Pos(nIndex);
	}

	POSITION GetHeadPosition() const
	{
		return m_vec.empty() ? NULL : _Pos(0);
	}

	POSITION GetTailPosition() const
	{
		return m_vec.empty() ? NULL : _Pos((INT_PTR)m_vec.size() - 1);
	}

	// -- access ----------------------------------------------------------------

	TYPE & GetAt(POSITION pos)              { return * m_vec[ _CheckedIdx(pos) ]; }
	const TYPE & GetAt(POSITION pos) const  { return * m_vec[ _CheckedIdx(pos) ]; }

	TYPE & GetHead()                        { ASSERT( ! m_vec.empty() ); return * m_vec.front(); }
	const TYPE & GetHead() const            { ASSERT( ! m_vec.empty() ); return * m_vec.front(); }

	TYPE & GetTail()                        { ASSERT( ! m_vec.empty() ); return * m_vec.back(); }
	const TYPE & GetTail() const            { ASSERT( ! m_vec.empty() ); return * m_vec.back(); }

	// Return the element at pos and advance pos to the next (NULL past the end),
	// exactly as CList does.
	TYPE & GetNext(POSITION & pos)
	{
		INT_PTR i = _CheckedIdx(pos);
		TYPE & rElement = * m_vec[i];
		pos = ( i + 1 < (INT_PTR)m_vec.size() ) ? _Pos(i + 1) : NULL;
		return rElement;
	}

	TYPE & GetPrev(POSITION & pos)
	{
		INT_PTR i = _CheckedIdx(pos);
		TYPE & rElement = * m_vec[i];
		pos = ( i > 0 ) ? _Pos(i - 1) : NULL;
		return rElement;
	}

	// Direct indexed access - the whole reason this class exists. O(1).
	TYPE & ElementAt(INT_PTR nIndex)
	{
		ASSERT( nIndex >= 0 && nIndex < (INT_PTR)m_vec.size() );
		return * m_vec[nIndex];
	}

	// -- single-element mutation -----------------------------------------------
	//
	// Each of these invalidates every POSITION previously handed out (see the header
	// comment); the one they return is freshly minted and safe to use.

	POSITION AddTail(ARG_TYPE newElement)
	{
		m_vec.push_back( _New(newElement) );
		_Bump();
		return _Pos((INT_PTR)m_vec.size() - 1);
	}

	POSITION AddHead(ARG_TYPE newElement)
	{
		m_vec.insert( m_vec.begin(), _New(newElement) );
		_Bump();
		return _Pos(0);
	}

	POSITION InsertAfter(POSITION pos, ARG_TYPE newElement)
	{
		INT_PTR i = _CheckedIdx(pos);
		m_vec.insert( m_vec.begin() + i + 1, _New(newElement) );
		_Bump();
		return _Pos(i + 1);
	}

	POSITION InsertBefore(POSITION pos, ARG_TYPE newElement)
	{
		INT_PTR i = _CheckedIdx(pos);
		m_vec.insert( m_vec.begin() + i, _New(newElement) );
		_Bump();
		return _Pos(i);
	}

	void RemoveAt(POSITION pos)
	{
		INT_PTR i = _CheckedIdx(pos);
		delete m_vec[i];
		m_vec.erase( m_vec.begin() + i );
		_Bump();
	}

	void RemoveAll()
	{
		for(TYPE * p : m_vec) delete p;
		m_vec.clear();
		_Bump();
	}

	// -- bulk mutation ---------------------------------------------------------
	//
	// One structural change, whatever the count. Looping the single-element versions
	// would be one memmove per line.

	// Insert nCount default-constructed elements so the first of them lands at nIndex.
	// The caller fills them in through ElementAt. nIndex == GetCount() appends.
	void InsertGap(INT_PTR nIndex, INT_PTR nCount)
	{
		if( nCount <= 0 ) return;
		ASSERT( nIndex >= 0 && nIndex <= (INT_PTR)m_vec.size() );

		std::vector<TYPE *> vecNew;
		vecNew.reserve(nCount);
		for(INT_PTR i = 0; i < nCount; i++) vecNew.push_back( new TYPE );

		m_vec.insert( m_vec.begin() + nIndex, vecNew.begin(), vecNew.end() );
		_Bump();
	}

	void RemoveRange(INT_PTR nIndex, INT_PTR nCount)
	{
		if( nCount <= 0 ) return;
		ASSERT( nIndex >= 0 && nIndex + nCount <= (INT_PTR)m_vec.size() );

		for(INT_PTR i = nIndex; i < nIndex + nCount; i++) delete m_vec[i];
		m_vec.erase( m_vec.begin() + nIndex, m_vec.begin() + nIndex + nCount );
		_Bump();
	}

private:
	std::vector<TYPE *> m_vec;

	// CList default-constructs a node and then ASSIGNS the argument into it, rather
	// than constructing from it. Mimic that: the element types here have assignment
	// operators that do real work (CFormatedString::operator= deep-copies its word
	// array) and may not have a matching converting constructor at all.
	static TYPE * _New(ARG_TYPE newElement)
	{
		TYPE * p = new TYPE;
		* p = newElement;
		return p;
	}

	// --- POSITION encoding ----------------------------------------------------
	//
	// Release: index + 1, so that index 0 is a non-NULL POSITION.
	// Debug:   the modification counter in the high 32 bits as well, so that using a
	//          POSITION minted before a structural change trips an assert instead of
	//          quietly naming the wrong line.

#ifdef _DEBUG
public:
	// Test hook. The stale-POSITION assert below is the whole safety net of this class,
	// so it needs a test of its own -- an assert that never fires because it is broken
	// looks exactly like an assert that never fires because the code is correct. This
	// reports the same condition without tripping it.
	BOOL IsStalePosition(POSITION pos) const
	{
		return (UINT)( (ULONGLONG)pos >> 32 ) != m_nModCount;
	}

private:
	UINT m_nModCount;

	void _Bump() { m_nModCount++; }

	POSITION _Pos(INT_PTR nIndex) const
	{
		return (POSITION)( ( (ULONGLONG)m_nModCount << 32 ) | (ULONGLONG)(nIndex + 1) );
	}

	INT_PTR _CheckedIdx(POSITION pos) const
	{
		ASSERT( pos != NULL );

		ULONGLONG raw = (ULONGLONG)pos;
		UINT nGen = (UINT)( raw >> 32 );
		INT_PTR nIndex = (INT_PTR)( raw & 0xFFFFFFFFULL ) - 1;

		// "This POSITION was handed out before the list was structurally changed."
		// Re-fetch it (FindIndex / GetHeadPosition / the return value of the call that
		// changed the list) rather than carrying it across the change.
		ASSERT( nGen == m_nModCount );

		ASSERT( nIndex >= 0 && nIndex < (INT_PTR)m_vec.size() );
		return nIndex;
	}
#else
	void _Bump() {}

	POSITION _Pos(INT_PTR nIndex) const { return (POSITION)(ULONGLONG)(nIndex + 1); }

	INT_PTR _CheckedIdx(POSITION pos) const { return (INT_PTR)(ULONGLONG)pos - 1; }
#endif

	// Not copyable - the elements are owned by pointer, and CList was not copyable
	// either.
	CLineList(const CLineList &);
	CLineList & operator=(const CLineList &);
};
