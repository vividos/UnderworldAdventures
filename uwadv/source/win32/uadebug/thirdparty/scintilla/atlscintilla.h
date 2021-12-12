/*
Filename: AtlScintilla.h
Version: 2.3
Description: Defines an easy wrapper for the Scintilla control, to be used with ATL/WTL projects.
Date: 12/12/2021

Copyright (c) 2005/2006 by Gilad Novik.
Copyright (c) 2006 by Reece Dunn.
Copyright (c) 2019-2021 by Michael Fink.

License Agreement (zlib license)
-------------------------
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

* If you use this code in a product, I would appreciate a small email from.

Gilad Novik (Web: http://gilad.gsetup.com, Email: gilad@gsetup.com)

History (Date/Author/Description):
----------------------------------

2019/11/06: vividos
- Formatted with Visual Studio's "Format Documents" using .editorconfig file
- Fixed signatures of SetSel() and HideSelection(), provided CString variants
  of ReplaceSel() and GetSelText() and added PosFromChar(), to match the CEdit
  class
- Added CScintillaEditCommands and CScintillaFindReplaceImpl classes
2019/11/15: vividos
- Merged changes from other atlscintilla.h files found on the internet that
  are marked "Version: 2.0" and "Modded by T.P Wang this is a trimed version".
  Mostly variable name changes and formatting, and some added methods.
  I marked this file "Version 2.1" to be able to tell the files apart.
2020/01/13: vividos
- Updated CScintillaEditCommands methods (CanCut, CanClear, etc.) to also
  check read-only status using IsReadOnly().
2020/01/13: vividos
- Updated code to work with Scintilla/Lexilla 5.x.
*/

///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CScintillaWindowT<TBase> - CScintillaWindow
// CScintillaEditCommands<TBase>
// CScintillaFindReplaceImpl<TBase>

#pragma once
#include <vector>
#include <string_view>
#include <Scintilla.h>
#include <ILexer.h>
#include <Lexilla.h>
#include <atlfile.h>
#include <atlfind.h>

#pragma comment(lib, "Imm32.lib")

template<class TBase>
class CScintillaWindowT : public TBase
{
public:
	static LPCTSTR GetWndClassName()
	{
		return _T("Scintilla");
	}

	/** @name Text retrieval and modification */
	//@{

	int GetText(char* text, int length) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTEXT, length, (LPARAM)text);
	}

	void SetText(const char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)text);
	}

	void SetSavePoint()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSAVEPOINT, 0, 0L);
	}

	int GetLine(int line, char* text) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINE, line, (LPARAM)text);
	}

	int GetLineLength(int line) const
	{
		return GetLine(line, NULL);
	}

#if defined(__ATLSTR_H__)
	void ReplaceSel(const CString& text)
	{
		ReplaceSel(CStringA(text));
	}
#endif

	void ReplaceSel(const char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_REPLACESEL, 0, (LPARAM)text);
	}

	bool GetReadOnly() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETREADONLY, 0, 0L) != 0;
	}

	void SetReadOnly(bool readOnly)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETREADONLY, readOnly, 0L);
	}

	int GetTextRange(Sci_TextRange& tr) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);
	}

	void Allocate(int bytes)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ALLOCATE, bytes, 0L);
	}

	void AddText(const char* text, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ADDTEXT, length, (LPARAM)text);
	}

	void AddStyledText(const char* text, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ADDSTYLEDTEXT, length, (LPARAM)text);
	}

	void AppendText(const char* text, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_APPENDTEXT, length, (LPARAM)text);
	}

	void InsertText(unsigned int pos, const char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_INSERTTEXT, pos, (LPARAM)text);
	}

	void ClearAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CLEARALL, 0, 0L);
	}

	void ClearDocumentStyle()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CLEARDOCUMENTSTYLE, 0, 0L);
	}

	char GetCharAt(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCHARAT, pos, 0L) & 0xFF;
	}

	int GetStyleAt(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSTYLEAT, pos, 0L);
	}

	int GetStyledText(Sci_TextRange& tr) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSTYLEDTEXT, 0, (LPARAM)&tr);
	}

	int GetStyledText(char* text, long first, long last) const
	{
		TextRange tr = { { first, last }, text };
		return GetStyledText(tr);
	}

#if defined(__ATLSTR_H__)
	int GetStyledText(long min, long max, CStringA& text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int length = max - min;
		int result = GetStyledText(min, max, text.GetBuffer(length));
		textReleaseBuffer(length);
		return result;
	}
#endif

	void SetStyleBits(int bits)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSTYLEBITS, bits, 0L);
	}

	int GetStyleBits() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSTYLEBITS, 0, 0L);
	}

	int TargetAsUTF8(char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_TARGETASUTF8, 0, (LPARAM)text);
	}

	int EncodeFromUTF8(const char* utf8, char* encoded)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_ENCODEFROMUTF8, (WPARAM)utf8, (
			LPARAM)encoded);
	}

	void SetLengthForEncode(int bytes)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETLENGTHFORENCODE, bytes, 0L);
	}

	//@}
	/** @name Searching */
	//@{

	int FindText(int flags, Sci_TextToFind& ttf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_FINDTEXT, flags, (LPARAM)&ttf);
	}

	void SearchAnchor()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SEARCHANCHOR, 0, 0L);
	}

	int SearchNext(int flags, const char* text) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_SEARCHNEXT, flags, (LPARAM)text);
	}

	int SearchPrev(int flags, const char* text) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_SEARCHPREV, flags, (LPARAM)text);
	}

	//@}
	/** @name Search And Replace Using The Target */
	//@{

	unsigned int GetTargetStart() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTARGETSTART, 0, 0L);
	}

	void SetTargetStart(unsigned int pos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETTARGETSTART, pos, 0L);
	}

	unsigned int GetTargetEnd() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTARGETEND, 0, 0L);
	}

	void SetTargetEnd(unsigned int pos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETTARGETEND, pos, 0L);
	}

	void TargetFromSelection()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_TARGETFROMSELECTION, 0, 0L);
	}

	int GetSearchFlags() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSEARCHFLAGS, 0, 0L);
	}

	void SetSearchFlags(int flags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSEARCHFLAGS, flags, 0L);
	}

	int SearchInTarget(const char* text, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_SEARCHINTARGET, length, (LPARAM)text);
	}

	int ReplaceTarget(const char* text, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_REPLACETARGET, length, (LPARAM)text);
	}

	int ReplaceTargetRE(const char* text, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_REPLACETARGETRE, length, (LPARAM)text);
	}

	//@}
	/** @name Overtype */
	//@{

	bool GetOvertype() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETOVERTYPE, 0, 0L) != 0;
	}

	void SetOvertype(bool overtype)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETOVERTYPE, overtype, 0L);
	}

	//@}
	/** @name Cut, Copy And Paste */
	//@{

	void Cut()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CUT, 0, 0L);
	}

	void Copy()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_COPY, 0, 0L);
	}

	void Paste()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PASTE, 0, 0L);
	}

	bool CanPaste() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_CANPASTE, 0, 0L) != 0;
	}

	void Clear()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CLEAR, 0, 0L);
	}

	void CopyRange(unsigned int first, unsigned int last)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_COPYRANGE, first, last);
	}

	void CopyText(const char* text, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_COPYTEXT, length, (LPARAM)text);
	}

	void SetPasteConvertEndings(bool convert)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETPASTECONVERTENDINGS, convert, 0L);
	}

	bool GetPasteConvertEndings() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETPASTECONVERTENDINGS, 0, 0L) != 0;
	}

	//@}
	/** @name Error Handling */
	//@{

	int GetStatus() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSTATUS, 0, 0L);
	}

	void SetStatus(int statusCode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSTATUS, statusCode, 0L);
	}

	//@}
	/** @name Undo/Redo */
	//@{

	void Undo()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_UNDO, 0, 0L);
	}

	bool CanUndo() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_CANUNDO, 0, 0L) != 0;
	}

	void Redo()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_REDO, 0, 0L);
	}

	bool CanRedo() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_CANREDO, 0, 0L) != 0;
	}

	void EmptyUndoBuffer()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_EMPTYUNDOBUFFER, 0, 0L);
	}

	void SetUndoCollection(bool collectUndo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETUNDOCOLLECTION, collectUndo, 0L);
	}

	bool GetUndoCollection() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETUNDOCOLLECTION, 0, 0L) != 0;
	}

	void BeginUndoAction()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_BEGINUNDOACTION, 0, 0L);
	}

	void EndUndoAction()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ENDUNDOACTION, 0, 0L);
	}

	//@}
	/** @name Selection And Information */
	//@{

	int GetTextLength() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0L);
	}

	int GetLength() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLENGTH, 0, 0L);
	}

	int GetLineCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINECOUNT, 0, 0L);
	}

	int GetFirstVisibleLine() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETFIRSTVISIBLELINE, 0, 0L);
	}

	int LinesOnScreen() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_LINESONSCREEN, 0, 0L);
	}

	bool GetModify() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMODIFY, 0, 0L) != 0;
	}

	void SetSel(int start, int end)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSEL, start, end);
	}

	void GotoPos(unsigned int pos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_GOTOPOS, pos, 0L);
	}

	void GotoLine(int line)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_GOTOLINE, line, 0L);
	}

	unsigned int GetCurrentPos() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCURRENTPOS, 0, 0L);
	}

	void SetCurrentPos(unsigned int position)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCURRENTPOS, position, 0L);
	}

	unsigned int GetAnchor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETANCHOR, 0, 0L);
	}

	void SetAnchor(unsigned int anchor)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETANCHOR, anchor, 0L);
	}

	void SetSelectionStart(unsigned int pos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSELECTIONSTART, pos, 0L);
	}

	unsigned int GetSelectionStart() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSELECTIONSTART, 0, 0L);
	}

	void SetSelectionEnd(unsigned int pos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSELECTIONEND, pos, 0L);
	}

	unsigned int GetSelectionEnd() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSELECTIONEND, 0, 0L);
	}

	void SelectAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SELECTALL, 0, 0L);
	}

	int LineFromPosition(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_LINEFROMPOSITION, pos, 0L);
	}

	unsigned int PositionFromLine(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_POSITIONFROMLINE, line, 0L);
	}

	int GetLineEndPosition(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINEENDPOSITION, line, 0L);
	}

	int LineLength(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_LINELENGTH, line, 0L);
	}

	int GetColumn(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCOLUMN, pos, 0L);
	}

	int FindColumn(int line, int column) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_FINDCOLUMN, line, column);
	}

	unsigned int PositionFromPoint(int x, int y) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_POSITIONFROMPOINT, x, y);
	}

	unsigned int PositionFromPointClose(int x, int y) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_POSITIONFROMPOINTCLOSE, x, y);
	}

	POINT PosFromChar(UINT nChar) const
	{
		POINT point =
		{
			PointXFromPosition(nChar),
			PointYFromPosition(nChar)
		};

		return point;
	}

	int PointXFromPosition(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_POINTXFROMPOSITION, 0, pos);
	}

	int PointYFromPosition(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_POINTYFROMPOSITION, 0, pos);
	}

	void HideSelection(BOOL bHide = TRUE, BOOL bChangeStyle = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bChangeStyle == FALSE); // changing style is not supported by Scintilla
		::SendMessage(m_hWnd, SCI_HIDESELECTION, bHide, 0L);
	}

#if defined(__ATLSTR_H__)
	int GetSelText(CString& text) const
	{
		unsigned int bufferLength = GetSelText(nullptr);

		CStringA ansiText;
		int ret = GetSelText(ansiText.GetBuffer(bufferLength + 1));
		ansiText.ReleaseBuffer();

		text = ansiText;

		return ret;
	}
#endif

	int GetSelText(char* text) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSELTEXT, 0, (LPARAM)text);
	}

	int GetCurLine(char* text, int length) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCURLINE, length, (LPARAM)text);
	}

	bool SelectionIsRectangle() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_SELECTIONISRECTANGLE, 0, 0L) != 0;
	}

	int GetSelectionMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSELECTIONMODE, 0, 0L);
	}

	void SetSelectionMode(int mode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSELECTIONMODE, mode, 0L);
	}

	unsigned int GetLineSelStartPosition(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINESELSTARTPOSITION, line, 0L);
	}

	unsigned int GetLineSelEndPosition(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINESELENDPOSITION, line, 0L);
	}

	void MoveCaretInsideView()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MOVECARETINSIDEVIEW, 0, 0L);
	}

	int WordStartPosition(unsigned int pos, bool onlyWordCharacters)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_WORDSTARTPOSITION, pos,
			onlyWordCharacters);
	}

	int WordEndPosition(unsigned int pos, bool onlyWordCharacters)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_WORDENDPOSITION, pos,
			onlyWordCharacters);
	}

	unsigned int PositionBefore(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_POSITIONBEFORE, pos, 0L);
	}

	unsigned int PositionAfter(unsigned int pos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_POSITIONAFTER, pos, 0L);
	}

	int TextWidth(int style, const char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_TEXTWIDTH, style, (LPARAM)text);
	}

	int TextHeight(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_TEXTHEIGHT, line, 0L);
	}

	void ChooseCaretX() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CHOOSECARETX, 0, 0L);
	}

	//@}
	/** @name Scrolling And Auto-Scrolling */
	//@{

	void LineScroll(int columns, int lines)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINESCROLL, columns, lines);
	}

	void ScrollToLine(int line)
	{
		LineScroll(0, line - LineFromPosition(GetCurrentPos()));
	}

	void ScrollCaret()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SCROLLCARET, 0, 0L);
	}

	void SetXCaretPolicy(int caretPolicy, int caretSlop)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
	}

	void SetYCaretPolicy(int caretPolicy, int caretSlop)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
	}

	void SetVisiblePolicy(int visiblePolicy, int visibleSlop)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETVISIBLEPOLICY, visiblePolicy,
			visibleSlop);
	}

	bool GetHScrollBar() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETHSCROLLBAR, 0, 0L) != 0;
	}

	void SetHScrollBar(bool show)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETHSCROLLBAR, show, 0L);
	}

	bool GetVScrollBar() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETVSCROLLBAR, 0, 0L) != 0;
	}

	void SetVScrollBar(bool show)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETVSCROLLBAR, show, 0L);
	}

	int GetXOffset() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETXOFFSET, 0, 0L);
	}

	void SetXOffset(int offset)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETXOFFSET, offset, 0L);
	}

	int GetScrollWidth() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSCROLLWIDTH, 0, 0L);
	}

	void SetScrollWidth(int pixelWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSCROLLWIDTH, pixelWidth, 0L);
	}

	void SetScrollWidthTracking(bool track)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSCROLLWIDTHTRACKING, track, 0L);
	}

	bool GetEndAtLastLine() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETENDATLASTLINE, 0, 0L) != 0;
	}

	void SetEndAtLastLine(bool endAtLastLine)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETENDATLASTLINE, endAtLastLine, 0L);
	}

	//@}
	/** @name WhiteSpace */
	//@{

	int GetViewWS() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETVIEWWS, 0, 0L);
	}

	void SetViewWS(int viewWS)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETVIEWWS, viewWS, 0L);
	}

	void SetWhitespaceFore(bool useSetting, COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWHITESPACEFORE, useSetting, fore);
	}

	void SetWhitespaceBack(bool useSetting, COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWHITESPACEBACK, useSetting, back);
	}

	//@}
	/** @name Cursor */
	//@{

	int GetCursor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCURSOR, 0, 0L);
	}

	void SetCursor(int cursorType)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCURSOR, cursorType, 0L);
	}

	//@}
	/** @name Mouse Capture */
	//@{

	bool GetMouseDownCaptures() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMOUSEDOWNCAPTURES, 0, 0L) != 0;
	}

	void SetMouseDownCaptures(bool captures)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMOUSEDOWNCAPTURES, captures, 0L);
	}

	//@}
	/** @name Line Endings */
	//@{

	int GetEOLMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETEOLMODE, 0, 0L);
	}

	void SetEOLMode(int eolMode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETEOLMODE, eolMode, 0L);
	}

	void ConvertEOLs(int eolMode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CONVERTEOLS, eolMode, 0L);
	}

	bool GetViewEOL() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETVIEWEOL, 0, 0L) != 0;
	}

	void SetViewEOL(bool visible)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETVIEWEOL, visible, 0L);
	}

	//@}
	/** @name Styling */
	//@{

	unsigned int GetEndStyled() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETENDSTYLED, 0, 0L);
	}

	void StartStyling(unsigned int pos, int mask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STARTSTYLING, pos, mask);
	}

	void SetStyling(int length, int style)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSTYLING, length, style);
	}

	void SetStylingEx(int length, const char* styles)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSTYLINGEX, length, (LPARAM)styles);
	}

	int GetLineState(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINESTATE, line, 0L);
	}

	void SetLineState(int line, int state)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETLINESTATE, line, state);
	}

	int GetMaxLineState() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMAXLINESTATE, 0, 0L);
	}

	//@}
	/** @name Style Definition */
	//@{

	void StyleResetDefault()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLERESETDEFAULT, 0, 0L);
	}

	void StyleClearAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0L);
	}

	void StyleSetFont(int style, const char* fontName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETFONT, style, (LPARAM)fontName);
	}

	void StyleSetSize(int style, int sizePoints)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETSIZE, style, sizePoints);
	}

	void StyleSetBold(int style, bool bold)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETBOLD, style, bold);
	}

	void StyleSetItalic(int style, bool italic)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETITALIC, style, italic);
	}

	void StyleSetUnderline(int style, bool underline)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETUNDERLINE, style, underline);
	}

	void StyleSetFore(int style, COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETFORE, style, fore);
	}

	void StyleSetBack(int style, COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETBACK, style, back);
	}

	void StyleSetEOLFilled(int style, bool filled)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETEOLFILLED, style, filled);
	}

	void StyleSetCharacterSet(int style, int characterSet)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETCHARACTERSET, style, characterSet);
	}

	void StyleSetCase(int style, int caseForce)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETCASE, style, caseForce);
	}

	void StyleSetVisible(int style, bool visible)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETVISIBLE, style, visible);
	}

	void StyleSetChangeable(int style, bool changeable)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETCHANGEABLE, style, changeable);
	}

	void StyleSetHotSpot(int style, bool hotspot)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STYLESETHOTSPOT, style, hotspot);
	}

	//@}
	/** @name Caret, Selection and Hotspot Styles */
	//@{

	int GetControlCharSymbol() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCONTROLCHARSYMBOL, 0, 0L);
	}

	void SetControlCharSymbol(int symbol)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCONTROLCHARSYMBOL, symbol, 0L);
	}

	/** @name Caret Styles */
	//@{

	void SetCaretPolicy(int caretPolicy, int caretSlop)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETPOLICY, caretPolicy, caretSlop);
	}

	COLORREF GetCaretFore() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCARETFORE, 0, 0L);
	}

	void SetCaretFore(COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETFORE, fore, 0L);
	}

	bool GetCaretLineVisible() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCARETLINEVISIBLE, 0, 0L) != 0;
	}

	void SetCaretLineVisible(bool show)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETLINEVISIBLE, show, 0L);
	}

	COLORREF GetCaretLineBack() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCARETLINEBACK, 0, 0L);
	}

	void SetCaretLineBack(COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETLINEBACK, back, 0L);
	}

	void SetCaretLineBackAlpha(int alpha)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETLINEBACKALPHA, alpha, 0);
	}

	int GetCaretPeriod() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCARETPERIOD, 0, 0L);
	}

	void SetCaretPeriod(int periodMilliseconds)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETPERIOD, periodMilliseconds, 0L);
	}

	int GetCaretWidth() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCARETWIDTH, 0, 0L);
	}

	void SetCaretWidth(int pixelWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETWIDTH, pixelWidth, 0L);
	}

	bool GetCaretSticky() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCARETSTICKY, 0, 0L) != 0;
	}

	void SetCaretSticky(bool useCaretStickyBehaviour)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETSTICKY, useCaretStickyBehaviour, 0L);
	}

	void ToggleCaretSticky()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_TOGGLECARETSTICKY, 0, 0L);
	}

	//@}
	/** @name Selection Styles */
	//@{

	void SetSelFore(bool useSetting, COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSELFORE, useSetting, fore);
	}

	void SetSelBack(bool useSetting, COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSELBACK, useSetting, back);
	}

	void SetSelAlpha(int alpha)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETSELALPHA, alpha, 0L);
	}

	//@}
	/** @name Hotspot Styles */
	//@{

	void SetHotspotActiveFore(bool useSetting, COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETHOTSPOTACTIVEFORE, useSetting, fore);
	}

	void SetHotspotActiveBack(bool useSetting, COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETHOTSPOTACTIVEBACK, useSetting, back);
	}

	void SetHotspotActiveUnderline(bool underline)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETHOTSPOTACTIVEUNDERLINE, underline, 0L);
	}

	void SetHotspotSingleLine(bool singleLine)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETHOTSPOTSINGLELINE, singleLine, 0L);
	}

	//@}
	//@}
	/** @name Margins */
	//@{

	int GetMarginTypeN(int margin) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMARGINTYPEN, margin, 0L);
	}

	void SetMarginTypeN(int margin, int marginType)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMARGINTYPEN, margin, marginType);
	}

	int GetMarginWidthN(int margin) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMARGINWIDTHN, margin, 0L);
	}

	void SetMarginWidthN(int margin, int pixelWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, margin, pixelWidth);
	}

	int GetMarginMaskN(int margin) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMARGINMASKN, margin, 0L);
	}

	void SetMarginMaskN(int margin, int mask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMARGINMASKN, margin, mask);
	}

	bool GetMarginSensitiveN(int margin) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMARGINSENSITIVEN, margin, 0L) !=
			0;
	}

	void SetMarginSensitiveN(int margin, bool sensitive)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMARGINSENSITIVEN, margin, sensitive);
	}

	int GetMarginLeft() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMARGINLEFT, 0, 0L);
	}

	void SetMarginLeft(int pixelWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMARGINLEFT, 0, pixelWidth);
	}

	int GetMarginRight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMARGINRIGHT, 0, 0L);
	}

	void SetMarginRight(int pixelWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMARGINRIGHT, 0, pixelWidth);
	}

	void SetFoldMarginColour(bool useSetting, COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETFOLDMARGINCOLOUR, useSetting, back);
	}

	void SetFoldMarginHiColour(bool useSetting, COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETFOLDMARGINHICOLOUR, useSetting, fore);
	}

	//@}
	/** @name Other Settings */
	//@{

	bool GetUsePalette() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETUSEPALETTE, 0, 0L) != 0;
	}

	void SetUsePalette(bool usePalette)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETUSEPALETTE, usePalette, 0L);
	}

	bool GetBufferedDraw() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETBUFFEREDDRAW) != 0;
	}

	void SetBufferedDraw(bool buffered)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETBUFFEREDDRAW, buffered, 0L);
	}

	bool GetTwoPhaseDraw() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTWOPHASEDRAW, 0, 0L) != 0;
	}

	void SetTwoPhaseDraw(bool twoPhase)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETTWOPHASEDRAW, twoPhase, 0L);
	}

	int GetCodePage() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETCODEPAGE, 0, 0L);
	}

	void SetCodePage(int codePage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCODEPAGE, codePage, 0L);
	}

	void SetWordChars(const char* characters)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWORDCHARS, 0, (LPARAM)characters);
	}

	void SetWhitespaceChars(const char* characters)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWHITESPACECHARS, 0, (LPARAM)characters);
	}

	void SetCharsDefault()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCHARSDEFAULT, 0, 0L);
	}

	void GrabFocus()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_GRABFOCUS, 0, 0L);
	}

	bool GetFocus() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETFOCUS, 0, 0L) != 0;
	}

	void SetFocus(bool focus)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETFOCUS, focus, 0L);
	}

	//@}
	/** @name Brace Highlighting */
	//@{

	void BraceHighlight(unsigned int pos1, unsigned int pos2)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_BRACEHIGHLIGHT, pos1, pos2);
	}

	void BraceBadLight(unsigned int pos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_BRACEBADLIGHT, pos, 0L);
	}

	unsigned int BraceMatch(unsigned int pos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_BRACEMATCH, pos, 0L);
	}

	//@}
	/** @name Tabs and Indentation Guides */
	//@{

	int GetTabWidth() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTABWIDTH);
	}

	void SetTabWidth(int tabWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETTABWIDTH, tabWidth, 0L);
	}

	bool GetUseTabs() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETUSETABS, 0, 0L) != 0;
	}

	void SetUseTabs(bool useTabs)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETUSETABS, useTabs, 0L);
	}

	int GetIndent() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETINDENT, 0, 0L);
	}

	void SetIndent(int indentSize)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETINDENT, indentSize, 0L);
	}

	bool GetTabIndents() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETTABINDENTS, 0, 0L) != 0;
	}

	void SetTabIndents(bool tabIndents)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETTABINDENTS, tabIndents, 0L);
	}

	bool GetBackSpaceUnIndents() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETBACKSPACEUNINDENTS, 0, 0L) != 0;
	}

	void SetBackSpaceUnIndents(bool bsUnIndents)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETBACKSPACEUNINDENTS, bsUnIndents, 0L);
	}

	int GetLineIndentation(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINEINDENTATION, line, 0L);
	}

	void SetLineIndentation(int line, int indentSize)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETLINEINDENTATION, line, indentSize);
	}

	unsigned int GetLineIndentPosition(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINEINDENTPOSITION, line, 0L);
	}

	bool GetIndentationGuides() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETINDENTATIONGUIDES, 0, 0L) != 0;
	}

	void SetIndentationGuides(bool show)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETINDENTATIONGUIDES, show, 0L);
	}

	int GetHighlightGuide() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETHIGHLIGHTGUIDE, 0, 0L);
	}

	void SetHighlightGuide(int column)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETHIGHLIGHTGUIDE, column, 0L);
	}

	//@}
	/** @name Markers */
	//@{

	void MarkerDefine(int markerNumber, int markerSymbol)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MARKERDEFINE, markerNumber, markerSymbol);
	}

	void MarkerDefinePixmap(int markerNumber, const char* pixmap)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MARKERDEFINEPIXMAP, markerNumber, (LPARAM)pixmap);
	}

	void MarkerSetFore(int markerNumber, COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MARKERSETFORE, markerNumber, fore);
	}

	void MarkerSetBack(int markerNumber, COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MARKERSETBACK, markerNumber, back);
	}

	int MarkerAdd(int line, int markerNumber)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_MARKERADD, line, markerNumber);
	}

	int MarkerAddSet(int line, int markerNumber)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_MARKERADDSET, line, markerNumber);
	}

	void MarkerDelete(int line, int markerNumber)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MARKERDELETE, line, markerNumber);
	}

	void MarkerDeleteAll(int markerNumber)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MARKERDELETEALL, markerNumber, 0L);
	}

	int MarkerGet(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_MARKERGET, line, 0L);
	}

	int MarkerNext(int lineStart, int markerMask) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_MARKERNEXT, lineStart, markerMask);
	}

	int MarkerPrevious(int lineStart, int markerMask) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_MARKERPREVIOUS, lineStart,
			markerMask);
	}

	int MarkerLineFromHandle(int handle) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_MARKERLINEFROMHANDLE, handle, 0L);
	}

	void MarkerDeleteHandle(int handle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_MARKERDELETEHANDLE, handle, 0L);
	}

	//@}
	/** @name Indicators */
	//@{

	int IndicGetStyle(int indic) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_INDICGETSTYLE, indic, 0L);
	}

	void IndicSetStyle(int indic, int style)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_INDICSETSTYLE, indic, style);
	}

	COLORREF IndicGetFore(int indic) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_INDICGETFORE, indic, 0L);
	}

	void IndicSetFore(int indic, COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_INDICSETFORE, indic, fore);
	}

	void SetIndicatorCurrent(int indic)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETINDICATORCURRENT, indic, 0L);
	}

	void SetIndicatorValue(int value)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETINDICATORVALUE, value, 0L);
	}

	void IndicatorFillRange(int start, int length)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_INDICATORFILLRANGE, start, length);
	}

	//@}
	/** @name Annotations */
	//@{

	void AnnotationSetText(int line, const char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ANNOTATIONSETTEXT, line, (LPARAM)text);
	}

	void AnnotationSetStyle(int line, int style)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ANNOTATIONSETSTYLE, line, style);
	}

	void AnnotationSetVisible(int visible)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ANNOTATIONSETVISIBLE, visible, 0L);
	}

	void AnnotationClearAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ANNOTATIONCLEARALL, 0L, 0L);
	}

	//@}
	/** @name Autocomplete */
	//@{

	void AutoCShow(int lenEntered, const char* itemList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSHOW, lenEntered, (LPARAM)itemList);
	}

	void AutoCCancel()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCCANCEL, 0, 0L);
	}

	bool AutoCActive() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCACTIVE, 0, 0L) != 0;
	}

	unsigned int AutoCPosStart() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCPOSSTART, 0, 0L);
	}

	void AutoCComplete()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCCOMPLETE, 0, 0L);
	}

	void AutoCStops(const char* characterSet)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSTOPS, 0, (LPARAM)characterSet);
	}

	int AutoCGetSeparator() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETSEPARATOR, 0, 0L);
	}

	void AutoCSetSeparator(int separatorCharacter)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETSEPARATOR, separatorCharacter, 0L);
	}

	void AutoCSelect(const char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSELECT, 0, (LPARAM)text);
	}

	int AutoCGetCurrent() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETCURRENT, 0, 0L);
	}

	bool AutoCGetCancelAtStart() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETCANCELATSTART, 0, 0L) != 0;
	}

	void AutoCSetCancelAtStart(bool cancel)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETCANCELATSTART, cancel, 0L);
	}

	void AutoCSetFillUps(const char* characterSet)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETFILLUPS, 0, (LPARAM)characterSet);
	}

	bool AutoCGetChooseSingle() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETCHOOSESINGLE, 0, 0L) != 0;
	}

	void AutoCSetChooseSingle(bool chooseSingle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETCHOOSESINGLE, chooseSingle, 0L);
	}

	bool AutoCGetIgnoreCase() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETIGNORECASE, 0, 0L) != 0;
	}

	void AutoCSetIgnoreCase(bool ignoreCase)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETIGNORECASE, ignoreCase, 0L);
	}

	bool AutoCGetAutoHide() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETAUTOHIDE, 0, 0L) != 0;
	}

	void AutoCSetAutoHide(bool autoHide)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETAUTOHIDE, autoHide, 0L);
	}

	bool AutoCGetDropRestOfWord() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETDROPRESTOFWORD, 0, 0L) != 0;
	}

	void AutoCSetDropRestOfWord(bool dropRestOfWord)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETDROPRESTOFWORD, dropRestOfWord, 0L);
	}

	void RegisterImage(int type, const char* xpmData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_REGISTERIMAGE, type, (LPARAM)xpmData);
	}

	void ClearRegisteredImages()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CLEARREGISTEREDIMAGES, 0, 0L);
	}

	int AutoCGetTypeSeparator() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETTYPESEPARATOR, 0, 0L);
	}

	void AutoCSetTypeSeparator(int separatorCharacter)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETTYPESEPARATOR, separatorCharacter,
			0L);
	}

	int AutoCGetMaxWidth() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETMAXWIDTH, 0, 0L);
	}

	void AutoCSetMaxWidth(int characterCount)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETMAXWIDTH, characterCount, 0L);
	}

	int AutoCGetMaxHeight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_AUTOCGETMAXHEIGHT, 0, 0L);
	}

	void AutoCSetMaxHeight(int rowCount)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_AUTOCSETMAXHEIGHT, rowCount, 0L);
	}

	//@}
	/** @name User Lists */
	//@{

	void UserListShow(int listType, const char* itemList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_USERLISTSHOW, listType, (LPARAM)itemList);
	}

	//@}
	/** @name Call tips */
	//@{

	void CallTipShow(unsigned int pos, const char* definition)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CALLTIPSHOW, pos, (LPARAM)definition);
	}

	void CallTipCancel()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CALLTIPCANCEL, 0, 0L);
	}

	bool CallTipActive()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_CALLTIPACTIVE, 0, 0L) != 0;
	}

	unsigned int CallTipPosStart() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_CALLTIPPOSSTART, 0, 0L);
	}

	void CallTipSetHlt(int start, int end)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CALLTIPSETHLT, start, end);
	}

	void CallTipSetBack(COLORREF back)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CALLTIPSETBACK, back, 0L);
	}

	void CallTipSetFore(COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CALLTIPSETFORE, fore, 0L);
	}

	void CallTipSetForeHlt(COLORREF fore)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CALLTIPSETFOREHLT, fore, 0L);
	}

	//@}
	/** @name Keyboard Commands */
	//@{

	void LineDown()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEDOWN, 0, 0L);
	}

	void LineDownExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEDOWNEXTEND, 0, 0L);
	}

	void LineUp()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEUP, 0, 0L);
	}

	void LineUpExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEUPEXTEND, 0, 0L);
	}

	void LineDownRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEDOWNRECTEXTEND, 0, 0L);
	}

	void LineUpRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEUPRECTEXTEND, 0, 0L);
	}

	void LineScrollDown()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINESCROLLDOWN, 0, 0L);
	}

	void LineScrollUp()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINESCROLLUP, 0, 0L);
	}

	void ParaDown()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PARADOWN, 0, 0L);
	}

	void ParaDownExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PARADOWNEXTEND, 0, 0L);
	}

	void ParaUp()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PARAUP, 0, 0L);
	}

	void ParaUpExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PARAUPEXTEND, 0, 0L);
	}

	void CharLeft()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CHARLEFT, 0, 0L);
	}

	void CharLeftExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CHARLEFTEXTEND, 0, 0L);
	}

	void CharRight()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CHARRIGHT, 0, 0L);
	}

	void CharRightExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CHARRIGHTEXTEND, 0, 0L);
	}

	void CharLeftRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CHARLEFTRECTEXTEND, 0, 0L);
	}

	void CharRightRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CHARRIGHTRECTEXTEND, 0, 0L);
	}

	void WordLeft()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDLEFT, 0, 0L);
	}

	void WordLeftExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDLEFTEXTEND, 0, 0L);
	}

	void WordRight()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDRIGHT, 0, 0L);
	}

	void WordRightExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDRIGHTEXTEND, 0, 0L);
	}

	void WordLeftEnd()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDLEFTEND, 0, 0L);
	}

	void WordLeftEndExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDLEFTENDEXTEND, 0, 0L);
	}

	void WordRightEnd()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDRIGHTEND, 0, 0L);
	}

	void WordRightEndExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDRIGHTENDEXTEND, 0, 0L);
	}

	void WordPartLeft()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDPARTLEFT, 0, 0L);
	}

	void WordPartLeftExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDPARTLEFTEXTEND, 0, 0L);
	}

	void WordPartRight()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDPARTRIGHT, 0, 0L);
	}

	void WordPartRightExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_WORDPARTRIGHTEXTEND, 0, 0L);
	}

	void Home()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HOME, 0, 0L);
	}

	void HomeExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HOMEEXTEND, 0, 0L);
	}

	void HomeRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HOMERECTEXTEND, 0, 0L);
	}

	void HomeDisplay()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HOMEDISPLAY, 0, 0L);
	}

	void HomeDisplayExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HOMEDISPLAYEXTEND, 0, 0L);
	}

	void HomeWrap()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HOMEWRAP, 0, 0L);
	}

	void HomeWrapExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HOMEWRAPEXTEND, 0, 0L);
	}

	void VCHome()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_VCHOME, 0, 0L);
	}

	void VCHomeExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_VCHOMEEXTEND, 0, 0L);
	}

	void VCHomeRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_VCHOMERECTEXTEND, 0, 0L);
	}

	void VCHomeWrap()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_VCHOMEWRAP, 0, 0L);
	}

	void VCHomeWrapExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_VCHOMEWRAPEXTEND, 0, 0L);
	}

	void LineEnd()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEEND, 0, 0L);
	}

	void LineEndExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEENDEXTEND, 0, 0L);
	}

	void LineEndRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEENDRECTEXTEND, 0, 0L);
	}

	void LineEndDisplay()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEENDDISPLAY, 0, 0L);
	}

	void LineEndDisplayExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEENDDISPLAYEXTEND, 0, 0L);
	}

	void LineEndWrap()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEENDWRAP, 0, 0L);
	}

	void LineEndWrapExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEENDWRAPEXTEND, 0, 0L);
	}

	void DocumentStart()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DOCUMENTSTART, 0, 0L);
	}

	void DocumentStartExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DOCUMENTSTARTEXTEND, 0, 0L);
	}

	void DocumentEnd()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DOCUMENTEND, 0, 0L);
	}

	void DocumentEndExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DOCUMENTENDEXTEND, 0, 0L);
	}

	void PageUp()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PAGEUP, 0, 0L);
	}

	void PageUpExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PAGEUPEXTEND, 0, 0L);
	}

	void PageUpRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PAGEUPRECTEXTEND, 0, 0L);
	}

	void PageDown()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PAGEDOWN, 0, 0L);
	}

	void PageDownExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PAGEDOWNEXTEND, 0, 0L);
	}

	void PageDownRectExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_PAGEDOWNRECTEXTEND, 0, 0L);
	}

	void StutteredPageUp()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STUTTEREDPAGEUP, 0, 0L);
	}

	void StutteredPageUpExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STUTTEREDPAGEUPEXTEND, 0, 0L);
	}

	void StutteredPageDown()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STUTTEREDPAGEDOWN, 0, 0L);
	}

	void StutteredPageDownExtend()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STUTTEREDPAGEDOWNEXTEND, 0, 0L);
	}

	void DeleteBack()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DELETEBACK, 0, 0L);
	}

	void DeleteBackNotLine()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DELETEBACKNOTLINE, 0, 0L);
	}

	void DelWordLeft()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DELWORDLEFT, 0, 0L);
	}

	void DelWordRight()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DELWORDRIGHT, 0, 0L);
	}

	void DelLineLeft()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DELLINELEFT, 0, 0L);
	}

	void DelLineRight()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_DELLINERIGHT, 0, 0L);
	}

	void LineDelete()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEDELETE, 0, 0L);
	}

	void LineCut()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINECUT, 0, 0L);
	}

	void LineCopy()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINECOPY, 0, 0L);
	}

	void LineTranspose()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINETRANSPOSE, 0, 0L);
	}

	void LineDuplicate()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINEDUPLICATE, 0, 0L);
	}

	void LowerCase()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LOWERCASE, 0, 0L);
	}

	void UpperCase()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_UPPERCASE, 0, 0L);
	}

	void Cancel()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CANCEL, 0, 0L);
	}

	void EditToggleOvertype()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_EDITTOGGLEOVERTYPE, 0, 0L);
	}

	void NewLine()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_NEWLINE, 0, 0L);
	}

	void FormFeed()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_FORMFEED, 0, 0L);
	}

	void Tab()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_TAB, 0, 0L);
	}

	void BackTab()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_BACKTAB, 0, 0L);
	}

	void SelectionDuplicate()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SELECTIONDUPLICATE, 0, 0L);
	}

	//@}
	/** @name Key Bindings */
	//@{

	void AssignCmdKey(DWORD key, int command)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ASSIGNCMDKEY, key, command);
	}

	void AssignCmdKey(WORD vk, WORD mod, int command)
	{
		AssignCmdKey(vk + (mod << 16), command);
	}

	void ClearCmdKey(DWORD key)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CLEARCMDKEY, key, 0L);
	}

	void ClearAllCmdKeys()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_CLEARALLCMDKEYS, 0, 0L);
	}

	void Null()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_NULL, 0, 0L);
	}

	//@}
	/** @name Popup Edit Menu */
	//@{

	void UsePopUp(bool allowPopUp)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_USEPOPUP, allowPopUp, 0L);
	}

	//@}
	/** @name Macro Recording */
	//@{

	void StartRecord()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STARTRECORD, 0, 0L);
	}

	void StopRecord()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_STOPRECORD, 0, 0L);
	}

	//@}
	/** @name Printing */
	//@{

	unsigned int FormatRange(bool draw, Sci_RangeToFormat& range)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_FORMATRANGE, draw, (LPARAM)&range);
	}

	int GetPrintMagnification() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETPRINTMAGNIFICATION, 0, 0L);
	}

	void SetPrintMagnification(int magnification)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETPRINTMAGNIFICATION, magnification, 0L);
	}

	int GetPrintColourMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETPRINTCOLOURMODE, 0, 0L);
	}

	void SetPrintColourMode(int mode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETPRINTCOLOURMODE, mode, 0L);
	}

	int GetPrintWrapMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETPRINTWRAPMODE, 0, 0L);
	}

	void SetPrintWrapMode(int mode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETPRINTWRAPMODE, mode, 0L);
	}

	//@}
	/** @name Direct Access */
	//@{

	SciFnDirect GetDirectFunction() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (SciFnDirect)::SendMessage(m_hWnd, SCI_GETDIRECTFUNCTION, 0,
			0L);
	}

	void* GetDirectPointer() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (void*)::SendMessage(m_hWnd, SCI_GETDIRECTPOINTER, 0, 0L);
	}

	//@}
	/** @name Multiple Views */
	//@{

	void* GetDocPointer() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETDOCPOINTER, 0, 0L);
	}

	void SetDocPointer(void* pointer)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETDOCPOINTER, 0, (LPARAM)pointer);
	}

	void* CreateDocument()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (void*)::SendMessage(m_hWnd, SCI_CREATEDOCUMENT, 0, 0L);
	}

	void AddRefDocument(void* doc)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ADDREFDOCUMENT, 0, (LPARAM)doc);
	}

	void ReleaseDocument(void* doc)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_RELEASEDOCUMENT, 0, (LPARAM)doc);
	}

	//@}
	/** @name Folding */
	//@{

	int VisibleFromDocLine(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_VISIBLEFROMDOCLINE, line, 0L);
	}

	int DocLineFromVisible(int lineDisplay) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_DOCLINEFROMVISIBLE, lineDisplay, 0L);
	}

	void ShowLines(int lineStart, int lineEnd)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SHOWLINES, lineStart, lineEnd);
	}

	void HideLines(int lineStart, int lineEnd)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_HIDELINES, lineStart, lineEnd);
	}

	bool GetLineVisible(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLINEVISIBLE, line, 0L) != 0;
	}

	int GetFoldLevel(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETFOLDLEVEL, line, 0L);
	}

	void SetFoldLevel(int line, int level)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETFOLDLEVEL, line, level);
	}

	void SetFoldFlags(int flags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETFOLDFLAGS, flags, 0L);
	}

	int GetLastChild(int line, int level) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLASTCHILD, line, level);
	}

	int GetFoldParent(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETFOLDPARENT, line, 0L);
	}

	bool GetFoldExpanded(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETFOLDEXPANDED, line, 0L) != 0;
	}

	void SetFoldExpanded(int line, bool expanded)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETFOLDEXPANDED, line, expanded);
	}

	void ToggleFold(int line)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_TOGGLEFOLD, line, 0L);
	}

	void EnsureVisible(int line)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ENSUREVISIBLE, line, 0L);
	}

	void EnsureVisibleEnforcePolicy(int line)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ENSUREVISIBLEENFORCEPOLICY, line, 0L);
	}

	//@}
	/** @name Line Wrapping */
	//@{

	int GetWrapMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETWRAPMODE, 0, 0L);
	}

	void SetWrapMode(int mode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWRAPMODE, mode, 0L);
	}

	int GetWrapVisualFlags() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETWRAPVISUALFLAGS, 0, 0L);
	}

	void SetWrapVisualFlags(int wrapVisualFlags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWRAPVISUALFLAGS, wrapVisualFlags, 0L);
	}

	void SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWRAPVISUALFLAGSLOCATION,
			wrapVisualFlagsLocation, 0L);
	}

	int GetWrapVisualFlagsLocation() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETWRAPVISUALFLAGSLOCATION, 0, 0L);
	}

	int GetWrapStartIndent() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETWRAPSTARTINDENT, 0, 0L);
	}

	void SetWrapStartIndent(int indent)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETWRAPSTARTINDENT, indent, 0L);
	}

	int GetLayoutCache() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLAYOUTCACHE, 0, 0L);
	}

	void SetLayoutCache(int mode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETLAYOUTCACHE, mode, 0L);
	}

	void LinesSplit(int pixelWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINESSPLIT, pixelWidth, 0L);
	}

	void LinesJoin()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LINESJOIN, 0, 0L);
	}

	int WrapCount(int line) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_WRAPCOUNT, line, 0L);
	}

	//@}
	/** @name Zooming */
	//@{

	void ZoomIn()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ZOOMIN, 0, 0L);
	}

	void ZoomOut()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_ZOOMOUT, 0, 0L);
	}

	int GetZoom() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETZOOM, 0, 0L);
	}

	void SetZoom(int zoom)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETZOOM, zoom, 0L);
	}

	//@}
	/** @name Long Lines */
	//@{

	int GetEdgeMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETEDGEMODE, 0, 0L);
	}

	void SetEdgeMode(int mode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETEDGEMODE, mode, 0L);
	}

	int GetEdgeColumn() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETEDGECOLUMN, 0, 0L);
	}

	void SetEdgeColumn(int column)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETEDGECOLUMN, column, 0L);
	}

	COLORREF GetEdgeColour() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETEDGECOLOUR, 0, 0L);
	}

	void SetEdgeColour(COLORREF edgeColour)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETEDGECOLOUR, edgeColour, 0L);
	}

	//@}
	/** @name Lexer */
	//@{

	int GetLexer() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETLEXER, 0, 0L);
	}

	DEPRECATE_DEFINITION
	void SetLexer(int lexerID)
	{
		const char* lexerName = LexerNameFromID(lexerID);
		ILexer5* lexer = CreateLexer(lexerName);

		SetILexer(lexer);
	}

	void SetLexerByName(const char* lexerName)
	{
		ILexer5* lexer = CreateLexer(lexerName);
		SetILexer(lexer);
	}

	void SetILexer(ILexer5* lexer)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETILEXER, (LPARAM)lexer, 0L);
	}

	void SetLexerLanguage(const char* language)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETLEXERLANGUAGE, 0, (LPARAM)language);
	}

	void LoadLexerLibrary(const char* path)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_LOADLEXERLIBRARY, 0, (LPARAM)path);
	}

	void Colourise(unsigned int start, unsigned int end)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_COLOURISE, start, end);
	}

	int GetProperty(const char* key, char* buf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETPROPERTY, (WPARAM)key, (LPARAM)buf);
	}

	void SetProperty(const char* key, const char* value)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETPROPERTY, (WPARAM)key, (LPARAM)value);
	}

	int GetPropertyExpanded(const char* key, char* buf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETPROPERTYEXPANDED, (WPARAM)key,
			(LPARAM)buf);
	}

	int GetPropertyInt(const char* key, int defaultValue = 0) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETPROPERTYINT, (WPARAM)key, (LPARAM)defaultValue);
	}

	void SetKeyWords(int keywordSet, const char* keyWords)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETKEYWORDS, keywordSet, (LPARAM)keyWords);
	}

	int GetStyleBitsNeeded() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETSTYLEBITSNEEDED, 0, 0L);
	}

	//@}
	/** @name Notifications */
	//@{

	int GetModEventMask() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMODEVENTMASK, 0, 0L);
	}

	void SetModEventMask(int mask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMODEVENTMASK, mask, 0L);
	}

	int GetMouseDwellTime() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_GETMOUSEDWELLTIME, 0, 0L);
	}

	void SetMouseDwellTime(int periodMilliseconds)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETMOUSEDWELLTIME, periodMilliseconds, 0L);
	}

#if defined(__ATLSTR_H__)
	int GetCurLine(CStringA& text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int length = GetCurLine(0, 0);
		int result = GetCurLine(text.GetBuffer(length), length);
		text.ReleaseBuffer(length - 1);
		return result;
	}

	int GetLine(int line, CStringA& text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int length = GetLine(line, 0);
		int result = GetLine(line, text.GetBuffer(length));
		text.ReleaseBuffer(length);
		return result;
	}

	int GetSimpleLine(int line, CStringA& text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TextRange range;
		range.chrg.cpMin = PositionFromLine(line);
		range.chrg.cpMax = GetLineEndPosition(line);
		int length = range.chrg.cpMax - range.chrg.cpMin;
		range.lpstrText = text.GetBuffer(length);
		int result = GetTextRange(range);
		text.ReleaseBuffer(length);
		return result;
	}

	int GetSelText(CStringA& text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int length = GetSelText(0) - 1;
		int result = GetSelText(text.GetBuffer(length));
		text.ReleaseBuffer(length);
		return result;
	}

	int GetText(CStringA& text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int length = GetLength();
		int result = GetText(text.GetBuffer(length), length + 1);
		text.ReleaseBuffer(length);
		return result;
	}
#endif

#if defined(GTK)
	int TargetAsUTF8(char* text)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_TARGETASUTF8, 0, (LPARAM)text);
	}

	void SetLengthForEncode(int bytes)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETLENGTHFORENCODE, bytes);
	}

	int EncodedFromUTF8(const char* utf8, char* encoded)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SCI_ENCODEDFROMUTF8, (WPARAM)utf8, (LPARAM)encoded);
	}
#endif

#if defined(__ATLSTR_H__)
	int GetProperty(const char* key, CStringA& value)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int length = GetProperty(key, 0);
		int result = GetProperty(key, value.GetBuffer(length));
		value.ReleaseBuffer(length);
		return result;
	}

	int GetPropertyExpanded(const char* key, CStringA& value)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int length = GetPropertyExpanded(key, 0);
		int result = GetPropertyExpanded(key, value.GetBuffer(length));
		value.ReleaseBuffer(length);
		return result;
	}
#endif

#ifdef INCLUDE_DEPRECATED_FEATURES
	void SetCaretPolicy(int caretPolicy, int caretSlop)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SCI_SETCARETPOLICY, caretPolicy, caretSlop);
	}
#endif

	BOOL Save(LPCTSTR szFilename)
	{
		CAtlFile File;
		if (FAILED(File.Create(szFilename, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, 0)))
			return FALSE;
		char buffer[8192 + 1];
		Sci_TextRange range;
		range.lpstrText = buffer;
		DWORD dwWritten;
		if (GetCodePage() == SC_CP_UTF8)
		{
			static BYTE UTF8[] = { 0xEF,0xBB,0xBF };
			if (FAILED(File.Write(UTF8, sizeof(UTF8), &dwWritten)) || dwWritten != sizeof(UTF8))
				return FALSE;
		}
		for (int nIndex = 0, length = GetLength(); nIndex < length; nIndex += (int)dwWritten)
		{
			dwWritten = (DWORD)(length - nIndex) > (sizeof(buffer) - 1) ? (sizeof(buffer) - 1) : (DWORD)(length - nIndex);
			range.chrg.cpMin = nIndex;
			range.chrg.cpMax = nIndex + (long)dwWritten;
			GetTextRange(range);
			if (FAILED(File.Write(buffer, dwWritten, &dwWritten)))
				return FALSE;
		}
		SetSavePoint();
		return TRUE;
	}

	BOOL Load(LPCTSTR szFilename)
	{
		CAtlFile File;
		if (FAILED(File.Create(szFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0)))
			return FALSE;
		ClearAll();
		SetUndoCollection(false);
		static BYTE UTF8[] = { 0xEF,0xBB,0xBF };
		char buffer[8192];
		DWORD dwRead;
		if (SUCCEEDED(File.Read(buffer, sizeof(UTF8), dwRead)) && dwRead > 0)
		{
			if (dwRead != sizeof(UTF8) || memcmp(buffer, UTF8, sizeof(UTF8)))
			{
				SetCodePage(0);
				AddText(buffer, (int)dwRead);
			}
			else
				SetCodePage(SC_CP_UTF8);
		}
		for (; SUCCEEDED(File.Read(buffer, sizeof(buffer), dwRead)) && dwRead > 0; AddText(buffer, (int)dwRead));
		SetSel(0, 0);
		SetUndoCollection(true);
		SetSavePoint();
		SetReadOnly((GetFileAttributes(szFilename) & FILE_ATTRIBUTE_READONLY) ? true : false);
		return TRUE;
	}
};

typedef CScintillaWindowT<ATL::CWindow> CScintillaWindow;

/// \brief edit commands mixin for scintilla windows
/// \details CEditCommands provides most of the function; this class adds the
/// handler for "Edit | Redo" command and provides a correct CanCut().
template <typename T>
class CScintillaEditCommands : public CEditCommands<T>
{
	/// this class type
	typedef CScintillaEditCommands<T> ThisClass;

	/// base class type
	typedef CEditCommands<T> BaseClass;

public:
	BEGIN_MSG_MAP(ThisClass)
		ALT_MSG_MAP(1)
	{
		// Only perform edit commands when our window has the focus.
		// This is done to allow multiple windows in a main frame that
		// can copy/paste/etc. to/from.
		HWND hWndFocus = ::GetFocus();
		T* pT = static_cast<T*>(this);

		if (pT->m_hWnd == hWndFocus)
		{
			COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
				CHAIN_MSG_MAP_ALT(BaseClass, 1)
		}
	}
	END_MSG_MAP()

	/// called for command "Edit | Redo"
	LRESULT OnEditRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

		pT->Redo();
		return 0;
	}

	// State (update UI) helpers
	BOOL CanCut() const
	{
		return !IsReadOnly() && HasSelection();
	}

	BOOL CanClear() const
	{
		return !IsReadOnly() && HasSelection();
	}

	BOOL CanClearAll() const
	{
		return !IsReadOnly() && HasText();
	}

	// Implementation
	BOOL HasSelection() const
	{
		const T* pT = static_cast<const T*>(this);
		ATLASSERT(pT->m_hWnd);
		return (::SendMessage(pT->m_hWnd, SCI_GETSELECTIONSTART, 0, 0) != ::SendMessage(pT->m_hWnd, SCI_GETSELECTIONEND, 0, 0));
	}

	BOOL HasText() const
	{
		const T* pT = static_cast<const T*>(this);
		ATLASSERT(pT->m_hWnd);
		return ::SendMessage(pT->m_hWnd, SCI_GETLENGTH, 0, 0) != 0;
	}

	BOOL IsReadOnly() const
	{
		const T* pT = static_cast<const T*>(this);
		ATLASSERT(pT->m_hWnd);
		return ::SendMessage(pT->m_hWnd, SCI_GETREADONLY, 0, 0);
	}
};

/// \brief find/replace impl for scintilla windows
/// \details implement FindTextSimple() called by CFindReplaceDialog class.
/// Also adds message filter, so that tab, escape and other keys work in the dialog.
template <typename T>
class CScintillaFindReplaceImpl :
	public CEditFindReplaceImplBase<T, CFindReplaceDialog>
{
	/// this class type
	typedef CScintillaFindReplaceImpl<T> ThisClass;

	/// base class type
	typedef CEditFindReplaceImplBase<T, CFindReplaceDialog> BaseClass;

public:
	/// pre-translates message
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (m_pFindReplaceDialog == nullptr)
			return FALSE;

		T* pT = static_cast<T*>(this);

		HWND hWndFocus = ::GetFocus();
		if (hWndFocus != nullptr && (
			(pT->m_hWnd == hWndFocus) || m_pFindReplaceDialog->IsChild(hWndFocus)))
			return m_pFindReplaceDialog->IsDialogMessage(pMsg);

		return FALSE;
	}

	/// finds text; used by CEditFindReplaceImplBase
	BOOL FindTextSimple(LPCTSTR lpszFind, BOOL bMatchCase, BOOL bWholeWord, BOOL bFindDown = TRUE)
	{
		T* pT = static_cast<T*>(this);

		Sci_TextToFind tf = { 0 };

		CStringA cszaTextToFind = lpszFind;
		tf.lpstrText = cszaTextToFind;

		tf.chrg.cpMin = pT->GetSelectionStart();
		tf.chrg.cpMax = pT->GetSelectionEnd();

		if (m_bFirstSearch)
		{
			if (bFindDown)
				m_nInitialSearchPos = tf.chrg.cpMin;
			else
				m_nInitialSearchPos = tf.chrg.cpMax;

			m_bFirstSearch = FALSE;
		}

		if (tf.chrg.cpMin != tf.chrg.cpMax) // when there's a selection
		{
			if (bFindDown)
				tf.chrg.cpMin++;
			else
				// won't wraparound backwards
				tf.chrg.cpMin = std::max<long>(tf.chrg.cpMin, 0);
		}

		tf.chrg.cpMax = pT->GetTextLength() + m_nInitialSearchPos;

		if (bFindDown)
		{
			if (m_nInitialSearchPos >= 0)
				tf.chrg.cpMax = pT->GetTextLength();
		}
		else
		{
			if (m_nInitialSearchPos >= 0)
				tf.chrg.cpMax = 0;
		}

		int flags = bMatchCase ? SCFIND_MATCHCASE : 0;
		flags |= bWholeWord ? SCFIND_WHOLEWORD : 0;

		bool bRet = FindAndSelect(flags, tf);

		if (!bRet && m_nInitialSearchPos > 0)
		{
			// if the original starting point was not the beginning
			// of the buffer and we haven't already been here
			if (bFindDown)
			{
				tf.chrg.cpMin = 0;
				tf.chrg.cpMax = m_nInitialSearchPos;
			}
			else
			{
				tf.chrg.cpMin = pT->GetTextLength();
				tf.chrg.cpMax = m_nInitialSearchPos;
			}
			m_nInitialSearchPos = m_nInitialSearchPos - pT->GetTextLength();

			bRet = FindAndSelect(flags, tf);
		}

		return bRet ? TRUE : FALSE;
	}

	/// finds and selects next text
	bool FindAndSelect(int flags, Sci_TextToFind& tf)
	{
		T* pT = static_cast<T*>(this);

		int pos = pT->FindText(flags, tf);
		bool bRet = pos != -1;

		if (bRet)
			pT->SetSel(tf.chrgText.cpMin, tf.chrgText.cpMax);

		return bRet;
	}

private:
	BEGIN_MSG_MAP(ThisClass)
		ALT_MSG_MAP(1)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCommand)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCommand)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditCommand)
		COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditCommand)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditCommand)
		COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditCommand)
		CHAIN_MSG_MAP_ALT(BaseClass, 1)
	END_MSG_MAP()

	/// called for edit commands
	LRESULT OnEditCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		if (m_pFindReplaceDialog == nullptr)
		{
			bHandled = false;
			return 0;
		}

		HWND hWndFocus = ::GetFocus();
		if (!m_pFindReplaceDialog->IsChild(hWndFocus))
		{
			bHandled = false;
			return 0;
		}

		CWindow wnd(hWndFocus);
		switch (wID)
		{
		case ID_EDIT_CUT:    wnd.SendMessage(WM_CUT); break;
		case ID_EDIT_COPY:   wnd.SendMessage(WM_COPY); break;
		case ID_EDIT_PASTE:  wnd.SendMessage(WM_PASTE); break;
		case ID_EDIT_SELECT_ALL:   wnd.SendMessage(EM_SETSEL, 0, -1); break;
		case ID_EDIT_UNDO:   wnd.SendMessage(EM_UNDO); break;
		case ID_EDIT_REDO:   wnd.SendMessage(EM_REDO); break;
		default:
			ATLASSERT(false);
			break;
		}

		return 0;
	}

	BOOL CanReplace() const
	{
		return !IsReadOnly() && HasText();
	}
};

class CScintillaAutoRegister
{
	static long    m_refCount;
#ifndef STATIC_SCILEXER
	static HMODULE m_scintilla;
#endif
public:
#ifdef STATIC_SCILEXER
	CScintillaAutoRegister(HINSTANCE instance = ATL::_AtlBaseModule.GetModuleInstance())
#else
	CScintillaAutoRegister()
#endif
	{
		if (!m_refCount)
		{
#ifndef STATIC_SCILEXER
			m_scintilla = ::LoadLibrary(_T("SciLexer.dll"));
#else
			Scintilla_RegisterClasses(instance);
#endif
		}
		++m_refCount;
	}

	~CScintillaAutoRegister()
	{
		--m_refCount;
		if (m_refCount == 0)
		{
#ifndef STATIC_SCILEXER
			::FreeLibrary(m_scintilla);
			m_scintilla = NULL;
#else
			Scintilla_ReleaseResources();
#endif
		}
	}
};

__declspec(selectany) long    CScintillaAutoRegister::m_refCount = 0;
__declspec(selectany) HMODULE CScintillaAutoRegister::m_scintilla = NULL;

class XPMImages
{
public:
	enum image
	{
		xpmMethod = 0,
		xpmMember = 23
	};

	static const char* Image(image nImage)
	{
		static const char* Images[] =
		{
			"16 16 6 1"," 	c #FFFFFF",".	c #000000","+	c #FF00FF","@	c #800080","#	c #808080","$	c #C0C0C0","                ","                ","                ","           .    ","          .+.   ","         .+@+.  ","        .+@+@+. "," ## ##  ..+@+@+.","        .+.+@+..","  $#$## .@+.+.@.","         .@+.@@.","   #$###  .@.@. ","           ...  ","            .   ","                ","                ",
			"16 16 5 1"," 	c #FFFFFF",".	c #808080","+	c #00FFFF","@	c #008080","#	c #000000","                ","                ","                ","           .    ","          .+.   ","         .+@+.  ","        .+@+@+# ","       .+@+@+## ","       ..+@+#.# ","       .+.+#..# ","       .++#..#  ","        .+#.#   ","         .##    ","          #     ","                ","                "
		};
		return (const char*)(Images + nImage);
	}
};
