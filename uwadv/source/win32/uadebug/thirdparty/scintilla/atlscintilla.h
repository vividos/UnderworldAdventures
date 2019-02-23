/**
 * @file ScintillaIF.h
 * @brief Define scintilla wrapper class CScintilla
 * @author Simon Steele
 * @note Copyright (c) 2002 Simon Steele <s.steele@pnotepad.org>
 *
 * Programmers Notepad 2 : The license file (license.[txt|html]) describes 
 * the conditions under which this source may be modified / distributed.
 */

#ifndef __SCINTILLAIF_H__
#define __SCINTILLAIF_H__

//#define STATIC_SCILEXER
//#define _WTL_NOSCINTILLAMETHODS
//#define SCINTILLA_PIXMAPS

#ifdef SCINTILLA_PIXMAPS
	#include "ScintillaPixmaps.h"
#endif

#define WTL_SCINTILLA 1
#define PLAT_WIN 1

#define SC_BOOKMARK 0
#define SC_NUMBERED_BOOKMARK 1

#include "scintilla.h"

//! Block size for disk reading and writing.
const int blockSize = 131072;

class ScintillaIFBase
{
	protected:

		//! Handle of the loaded scilexer.dll
		static HMODULE scidll;
		//! Reference counter.
		static int refs;
};

/**
 * CScintilla is a no-framework Scintilla wrapper for C++, it still requires
 * assistance from a window loop in the containing window. The code for
 * many methods is derived from / taken from code found in Scite.
 */
template <class T>
class CScintillaIF : public ScintillaIFBase
{
	public:
		//! Scintilla direct message function.
		typedef long(__cdecl* scmsgfn)(void *ptr, long Msg, WPARAM wParam, LPARAM lParam);
		typedef enum {efsVSNet, efsVSNetR, efsPlus, efsArrow} EFoldStyle;

	public:
		/// Default constructor
		CScintillaIF()
		{
		#ifndef STATIC_SCILEXER
			if(!scidll)
			{
				scidll = LoadLibrary(_T("SciLexer.dll"));
			}
		#else
			if(!refs)
				Scintilla_RegisterClasses(GetModuleHandle(NULL));
		#endif
			refs++;
			m_Modified = false;
			Perform = NULL;
			m_TabWidth = 4;
			m_SelLength = 0;			
		}

		/// Destructor
		~CScintillaIF()
		{
			refs--;
		#ifndef STATIC_SCILEXER
			if(!refs)
			{
				FreeLibrary(scidll);
				scidll = NULL;
			}
		#else
			if(!refs)
				Scintilla_ReleaseResources();
		#endif
		}

		/**
		 * Is the control modified - gained from HandleNotify function.
		 * A using class should call IsScintillaNotify on a Notify message
		 * (WM_NOTIFY) and should then call HandleNotify if the result is
		 * true.
		 */
		bool IsScintillaNotify(LPARAM lParam)
		{
			SCNotification *scn = (SCNotification*)lParam;
			T* pT = static_cast<T*>(this);
			if (scn->nmhdr.hwndFrom == pT->m_hWnd)
				return true;
			return false;
		}
		
		/**
		 * HandleNotify currently only updates the Modified flag
		 * of CScintilla.
		 */
		/*virtual*/ int HandleNotify(LPARAM lParam)
		{
			SCNotification *scn = (SCNotification*)lParam;
			switch (scn->nmhdr.code)
			{
				case SCN_SAVEPOINTREACHED :
					m_Modified = false;
					break;
		
				case SCN_SAVEPOINTLEFT :
					m_Modified = true;
					break;
				
				// Folding Notifications:
				case SCN_MARGINCLICK :
					if (scn->margin == 2) 
					{
						MarginClick(scn->position, scn->modifiers);
					}
					break;
				case SCN_MODIFIED:
					if (0 != (scn->modificationType & SC_MOD_CHANGEFOLD)) 
					{
						FoldChanged(scn->line, scn->foldLevelNow, scn->foldLevelPrev);
					}
					break;
				case SCN_UPDATEUI:
					{
						m_SelLength = SPerform(SCI_GETSELECTIONEND) - SPerform(SCI_GETSELECTIONSTART);
					}
					break;
			}
			return scn->nmhdr.code;
		}
		
		//! Returns whether the text in the control has been modified since the last save.
		bool GetModified()
		{
			return m_Modified;
		}
	
		/**
		 * SPerform uses either SendMessage or the function pointer (Perform)
		 * to run scintilla commands on the relevant scintilla control.
		 */
		/*virtual*/ inline long SPerform(long Msg, WPARAM wParam=0, LPARAM lParam=0)
		{
			T* pT = static_cast<T*>(this);
			if (Perform)
				return pT->Perform(m_Pointer, Msg, wParam, lParam);
			else
				return ::SendMessage(pT->m_hWnd, Msg, wParam, lParam);
		}

		/// Load a file from "filename".
		virtual bool OpenFile(LPCTSTR filename)
		{
			FILE *fp = fopen(filename, "rb");
			if (fp) 
			{
				//fileModTime = GetModTime(fullPath);
		
				SPerform(SCI_CLEARALL);
				// Disable UNDO
				SPerform(SCI_SETUNDOCOLLECTION, 0);
				char data[blockSize];
				int lenFile = fread(data, 1, sizeof(data), fp);
				while (lenFile > 0) 
				{
					SPerform(SCI_ADDTEXT, lenFile, (long)data);
					lenFile = fread(data, 1, sizeof(data), fp);
				}
				fclose(fp);
				SPerform(SCI_SETSEL, 0, 0);
				// Re-Enable UNDO
				SPerform(SCI_SETUNDOCOLLECTION, 1);
				SPerform(SCI_SETSAVEPOINT);
				return true;
			}
			return false;
		}

		/// Save the contents of the control to "filename".
		virtual bool SaveFile(LPCTSTR filename)
		{
			FILE *fp = fopen(filename, "wb");
			if (fp) {
				char data[blockSize + 1];
				int lengthDoc = SPerform(SCI_GETLENGTH);
				for (int i = 0; i < lengthDoc; i += blockSize) {
					int grabSize = lengthDoc - i;
					if (grabSize > blockSize)
						grabSize = blockSize;
					GetRange(i, i + grabSize, data);
					/*if (props.GetInt("strip.trailing.spaces"))
						grabSize = StripTrailingSpaces(
									   data, grabSize, grabSize != blockSize);*/
					fwrite(data, grabSize, 1, fp);
				}
				fclose(fp);
				SPerform(SCI_SETSAVEPOINT);
				return true;
			}
			return false;
		}


		void SetInitialTabWidth(int ts){m_TabWidth = ts;}

		int GetSelLength(){return m_SelLength;}

		/**
		 * @name Public Helper Functions
		 * The following are helper functions - they allow
		 * the code using certain parts of Scintilla to look
		 * a little cleaner.
		 */
		//@{
		void GetSel(CharacterRange& cr)
		{
			cr.cpMin = GetSelectionStart();
			cr.cpMax = GetSelectionEnd();
		}
		
		#define min(a, b)  (((a) < (b)) ? (a) : (b))
		#define max(a, b)  (((a) > (b)) ? (a) : (b))
		
		void EnsureRangeVisible(int begin, int end)
		{
			int lineStart = LineFromPosition(min(begin, end));
			int lineEnd = LineFromPosition(max(begin, end));
		
			for(int line = lineStart; line <= lineEnd; line++)
			{
				EnsureVisible(line);
			}		
		}
		
		void SetTarget(int begin, int end)
		{
			SPerform(SCI_SETTARGETSTART, begin);
			SPerform(SCI_SETTARGETEND, end);
		}
		
		void SetTarget(CharacterRange* cr)
		{
			SPerform(SCI_SETTARGETSTART, cr->cpMin);
			SPerform(SCI_SETTARGETEND, cr->cpMax);
		}
		
		void GotoLineEnsureVisible(int line)
		{
			SPerform(SCI_ENSUREVISIBLEENFORCEPOLICY, line);
			SPerform(SCI_GOTOLINE, line);
		}

		void DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back)
		{
			SPerform(SCI_MARKERDEFINE, marker, markerType);
			SPerform(SCI_MARKERSETFORE, marker, fore);
			SPerform(SCI_MARKERSETBACK, marker, back);
		}
		//@}

		/**
		 * @name NumberedBookmarks
		 */
		//@{
		void DefineNumberedBookmarks(int base = SC_NUMBERED_BOOKMARK, bool SetDefaultColours = true)
		{
		#ifdef SCINTILLA_PIXMAPS
			for(int i = 0; i < 10; i++)
			{
				SPerform(SCI_MARKERDEFINEPIXMAP, base + i, (LPARAM)scpixmap_bookmarks[i]);
			}
		#else
			COLORREF fore;
			COLORREF back;
		
			if(SetDefaultColours)
			{
				fore = ::GetSysColor(COLOR_HIGHLIGHTTEXT /*COLOR_INFOTEXT*/);
				back = ::GetSysColor(COLOR_HIGHLIGHT /*COLOR_INFOBK*/);
			}
		
			for(int i = 0; i < 10; i++)
			{
				SPerform(SCI_MARKERDEFINE, base + i, SC_MARK_CHARACTER + '0' + i);
				m_numberedBookmarks[i] = -1;
				if(SetDefaultColours)
				{
					SPerform(SCI_MARKERSETFORE, base+i, fore);
					SPerform(SCI_MARKERSETBACK, base+i, back);
				}
			}
		#endif	
		}
		
		void ToggleNumberedBookmark(int number, int base = SC_NUMBERED_BOOKMARK)
		{
			if(number >= 0 && number <= 9)
			{
				int line = LineFromPosition(GetCurrentPos());
				int oldline = -1;
		
				if(m_numberedBookmarks[number] != -1)
				{
					oldline = MarkerLineFromHandle(m_numberedBookmarks[number]);
					MarkerDeleteHandle(m_numberedBookmarks[number]);
				}
		
				int markers = MarkerGet(line);
				for(int i = 0; i < 10; i++)
				{
					if(markers & (1 << (base + i)))
					{
						MarkerDelete(line, base + i);
						m_numberedBookmarks[base+i] = -1;
					}
				}
		
				if(oldline != line)
				{
					m_numberedBookmarks[number] = MarkerAdd(line, base + number);
				}
			}
		}
		
		void JumpToNumberedBookmark(int number, int base = SC_NUMBERED_BOOKMARK)
		{
			if(number >= 0 && number <= 9 && m_numberedBookmarks[number] != -1)
			{
				int line = MarkerLineFromHandle(m_numberedBookmarks[number]);
				GotoLineEnsureVisible(line);
			}
		}
		//@}

		/**
		 * @name SimpleBookmarks
		 */
		//@{
		void DefineBookmarks()
		{
		#ifndef SCINTILLA_PIXMAPS
			MarkerSetFore(SC_BOOKMARK, ::GetSysColor(COLOR_HIGHLIGHT));
			MarkerSetBack(SC_BOOKMARK, ::GetSysColor(COLOR_HOTLIGHT));
			MarkerDefine(SC_BOOKMARK, SC_MARK_CIRCLE);
		#else
			SPerform(SCI_MARKERDEFINEPIXMAP, SC_BOOKMARK, (LPARAM)scpixmap_bookmark);
		#endif
		}
		
		void ToggleBookmark(int marker = SC_BOOKMARK)
		{
			int line = LineFromPosition(GetCurrentPos());
			if(MarkerGet(line) & (1 << SC_BOOKMARK))
			{
				MarkerDelete(line, SC_BOOKMARK);
			}
			else
			{
				MarkerAdd(line, SC_BOOKMARK);
			}
		}

		void NextBookmark()
		{
			int line = LineFromPosition(GetCurrentPos());
			int nextLine = MarkerNext(line+1, 1 << SC_BOOKMARK);
			if (nextLine < 0)
				nextLine = MarkerNext(0, 1 << SC_BOOKMARK);
			if(! (nextLine < 0 || nextLine == line))
			{
				GotoLineEnsureVisible(nextLine);
			}
		}
		//@}
        
		// Folding (mostly Scite implementation)
		/**
		 * Call SetFoldingMargins to have CScintilla automatically set up 
		 * the folding margin indicators in one of several given styles. The
		 * function also enables folding by setting the fold and fold.compact
		 * properties in Scintilla. Finally, it sets the fold flags to 16.
		 *
		 * By default, CScintilla handles all notification messages related to
		 * folding and implements Scite-Style folding control.
		 *
		 * @param style Style is one value from the EFoldStyle enumeration
		 */
		void SetFoldingMargins(EFoldStyle style)
		{
			/*SetProperty("fold", "1");
			SetProperty("fold.compact", "1");
			SetFoldFlags(16);*/
		
			switch (style)
			{
			case efsVSNet:
				{
					DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
					DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
					DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
					DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
					DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
					DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
					DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
				}
				break;
			case efsVSNetR:
				{
					DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_CIRCLEMINUS, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
					DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
					DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
					DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
					DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_CIRCLEPLUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
					DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_CIRCLEMINUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
					DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
				}
				break;
			case efsPlus:
				{
					DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_PLUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
				}
				break;
			default:
			case efsArrow:
				{
					DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_ARROWDOWN, RGB(0, 0, 0), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_ARROW, RGB(0, 0, 0), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0, 0, 0), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0, 0, 0), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
					DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
				}
				break;
			}
		}
		
		/// Called when Fold Changed
		void FoldChanged(int line, int levelNow, int levelPrev)
		{
			//Platform::DebugPrintf("Fold %d %x->%x\n", line, levelPrev, levelNow);
			if (levelNow & SC_FOLDLEVELHEADERFLAG) 
			{
				if (!(levelPrev & SC_FOLDLEVELHEADERFLAG)) 
				{
					SetFoldExpanded(line, 1);
				}
			} 
			else if (levelPrev & SC_FOLDLEVELHEADERFLAG) 
			{
				//Platform::DebugPrintf("Fold removed %d-%d\n", line, SendEditor(SCI_GETLASTCHILD, line));
				if (!GetFoldExpanded(line)) 
				{
					// Removing the fold from one that has been contracted so should expand
					// otherwise lines are left invisible with no way to make them visible
					Expand(line, true, false, 0, levelPrev);
				}
			}
		}
		
		/// Called to Expand and Collapse folded sections
		void Expand(int &line, bool doExpand, bool force=false, int visLevels=0, int level=-1)
		{
			int lineMaxSubord = GetLastChild(line, level & SC_FOLDLEVELNUMBERMASK);
			line++;
			while (line <= lineMaxSubord) 
			{
				if (force) 
				{
					if (visLevels > 0)
						ShowLines(line, line);
					else
						HideLines(line, line);
				} 
				else 
				{
					if (doExpand)
						ShowLines(line, line);
				}
				
				int levelLine = level;
		
				if (levelLine == -1)
					levelLine = GetFoldLevel(line);
		
				if (levelLine & SC_FOLDLEVELHEADERFLAG) 
				{
					if (force) 
					{
						if (visLevels > 1)
							SetFoldExpanded(line, 1);
						else
							SetFoldExpanded(line, 0);
						
						Expand(line, doExpand, force, visLevels - 1);
					} 
					else 
					{
						if (doExpand) 
						{
							if (!GetFoldExpanded(line))
								SetFoldExpanded(line, 1);
						
							Expand(line, true, force, visLevels - 1);
						} 
						else 
						{
							Expand(line, false, force, visLevels - 1);
						}
					}
				} 
				else 
				{
					line++;
				}
			}
		}
		
		/// Called when a margin is clicked on.
		bool MarginClick(int position, int modifiers)
		{
			int lineClick = LineFromPosition(position);//SendEditor(SCI_LINEFROMPOSITION, position);

			//Platform::DebugPrintf("Margin click %d %d %x\n", position, lineClick,
			//	SendEditor(SCI_GETFOLDLEVEL, lineClick) & SC_FOLDLEVELHEADERFLAG);
		
			if ((modifiers & SCMOD_SHIFT) && (modifiers & SCMOD_CTRL)) 
			{
				FoldAll();
			}
			else 
			{
				int levelClick = GetFoldLevel(lineClick);
				if (levelClick & SC_FOLDLEVELHEADERFLAG) 
				{
					if (modifiers & SCMOD_SHIFT) 
					{
						// Ensure all children visible
						SetFoldExpanded(lineClick, 1);
						Expand(lineClick, true, true, 100, levelClick);
					} 
					else if (modifiers & SCMOD_CTRL) 
					{
						if (GetFoldExpanded(lineClick)) 
						{
							// Contract this line and all children
							SetFoldExpanded(lineClick, 0);
							Expand(lineClick, false, true, 0, levelClick);
						}
						else 
						{
							// Expand this line and all children
							SetFoldExpanded(lineClick, 1);
							Expand(lineClick, true, true, 100, levelClick);
						}
					} 
					else 
					{
						// Toggle this line
						ToggleFold(lineClick);
					}
				}
			}
			return true;
		}
		
		/// Call FoldAll() to collapse the entire document.
		void FoldAll()
		{
			Colourise(0, -1);
			int maxLine = GetLineCount();
			bool expanding = true;
			for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) 
			{
				if (GetFoldLevel(lineSeek) & SC_FOLDLEVELHEADERFLAG) 
				{
					expanding = !GetFoldExpanded(lineSeek);
					break;
				}
			}
			for (int line = 0; line < maxLine; line++) 
			{
				int level = GetFoldLevel(line);
		
				if ((level & SC_FOLDLEVELHEADERFLAG) &&
						(SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK))) 
				{
					if (expanding) 
					{
						SetFoldExpanded(line, 1);
						Expand(line, true, false, 0, level);
						line--;
					} 
					else 
					{
						int lineMaxSubord = GetLastChild(line, -1);
						SetFoldExpanded(line, 0);
						if (lineMaxSubord > line)
							HideLines(line+1, lineMaxSubord);
					}
				}
			}
		}

	protected:
	
		// Locally Written CScintilla members.
		//! Handle of the relevant scintilla window - inherit one if we're a WTL window.
		//HWND	m_scihWnd;
		//! Used for Scintilla's GetDirectPointer
		void	*m_Pointer;
		//! Function pointer to Scintilla window message pump.
		scmsgfn	Perform;
		
		//! Is text modified?
		bool m_Modified;
		
		//! Locally used method to get a range of text.
		void GetRange(int start, int end, char *text)
		{
			TextRange tr;
			tr.chrg.cpMin = start;
			tr.chrg.cpMax = end;
			tr.lpstrText = text;
			SPerform(SCI_GETTEXTRANGE, 0, reinterpret_cast<long>(&tr));
		}

		int	m_TabWidth;

		int m_SelLength;

		int m_numberedBookmarks[10];
	
	// Python Wrapper-Generator Generated header...
	public:
		/**
		 * @name Public Wrapper Functions
		 * The functions below are defined and implemented automagically
		 * using the Python wrapper generator code contained in SHFace.py and
		 * SHFacer.py.
		 */
#ifndef _WTL_NOSCINTILLAMETHODS
		//@{
		//++InlineFuncImp
		/**
		 * Add text to the document.
		 */
		void AddText(int length, const char* text)
		{
			SPerform(SCI_ADDTEXT, (long)length, (long)text);
		}

		/**
		 * Add array of cells to document.
		 */
		void AddStyledText(int length, char* c)
		{
			SPerform(SCI_ADDSTYLEDTEXT, (long)length, (long)c);
		}

		/**
		 * Insert string at a position.
		 */
		void InsertText(long pos, const char* text)
		{
			SPerform(SCI_INSERTTEXT, pos, (long)text);
		}

		/**
		 * Delete all text in the document.
		 */
		void ClearAll()
		{
			SPerform(SCI_CLEARALL, 0, 0);
		}

		/**
		 * Set all style bytes to 0, remove all folding information.
		 */
		void ClearDocumentStyle()
		{
			SPerform(SCI_CLEARDOCUMENTSTYLE, 0, 0);
		}

		/**
		 * The number of characters in the document.
		 */
		int GetLength()
		{
			return (int)SPerform(SCI_GETLENGTH, 0, 0);
		}

		/**
		 * Returns the character byte at the position.
		 */
		int GetCharAt(long pos)
		{
			return (int)SPerform(SCI_GETCHARAT, pos, 0);
		}

		/**
		 * Returns the position of the caret.
		 */
		long GetCurrentPos()
		{
			return SPerform(SCI_GETCURRENTPOS, 0, 0);
		}

		/**
		 * Returns the position of the opposite end of the selection to the caret.
		 */
		long GetAnchor()
		{
			return SPerform(SCI_GETANCHOR, 0, 0);
		}

		/**
		 * Returns the style byte at the position.
		 */
		int GetStyleAt(long pos)
		{
			return (int)SPerform(SCI_GETSTYLEAT, pos, 0);
		}

		/**
		 * Redoes the next action on the undo history.
		 */
		void Redo()
		{
			SPerform(SCI_REDO, 0, 0);
		}

		/**
		 * Choose between collecting actions into the undo
		 * history and discarding them.
		 */
		void SetUndoCollection(bool collectUndo)
		{
			SPerform(SCI_SETUNDOCOLLECTION, (long)collectUndo, 0);
		}

		/**
		 * Select all the text in the document.
		 */
		void SelectAll()
		{
			SPerform(SCI_SELECTALL, 0, 0);
		}

		/**
		 * Remember the current position in the undo history as the position
		 * at which the document was saved.
		 */
		void SetSavePoint()
		{
			SPerform(SCI_SETSAVEPOINT, 0, 0);
		}

		/**
		 * Retrieve a buffer of cells.
		 * Returns the number of bytes in the buffer not including terminating nulls.
		 */
		int GetStyledText(TextRange* tr)
		{
			return (int)SPerform(SCI_GETSTYLEDTEXT, 0, (long)tr);
		}

		/**
		 * Are there any redoable actions in the undo history?
		 */
		bool CanRedo()
		{
			return SPerform(SCI_CANREDO, 0, 0) != 0;
		}

		/**
		 * Retrieve the line number at which a particular marker is located.
		 */
		int MarkerLineFromHandle(int handle)
		{
			return (int)SPerform(SCI_MARKERLINEFROMHANDLE, (long)handle, 0);
		}

		/**
		 * Delete a marker.
		 */
		void MarkerDeleteHandle(int handle)
		{
			SPerform(SCI_MARKERDELETEHANDLE, (long)handle, 0);
		}

		/**
		 * Is undo history being collected?
		 */
		bool GetUndoCollection()
		{
			return SPerform(SCI_GETUNDOCOLLECTION, 0, 0) != 0;
		}

		/**
		 * Are white space characters currently visible?
		 * Returns one of SCWS_* constants.
		 */
		int GetViewWS()
		{
			return (int)SPerform(SCI_GETVIEWWS, 0, 0);
		}

		/**
		 * Make white space characters invisible, always visible or visible outside indentation.
		 */
		void SetViewWS(int viewWS)
		{
			SPerform(SCI_SETVIEWWS, (long)viewWS, 0);
		}

		/**
		 * Find the position from a point within the window.
		 */
		int PositionFromPoint(int x, int y)
		{
			return (int)SPerform(SCI_POSITIONFROMPOINT, (long)x, (long)y);
		}

		/**
		 * Find the position from a point within the window but return
		 * INVALID_POSITION if not close to text.
		 */
		int PositionFromPointClose(int x, int y)
		{
			return (int)SPerform(SCI_POSITIONFROMPOINTCLOSE, (long)x, (long)y);
		}

		/**
		 * Set caret to start of a line and ensure it is visible.
		 */
		void GotoLine(int line)
		{
			SPerform(SCI_GOTOLINE, (long)line, 0);
		}

		/**
		 * Set caret to a position and ensure it is visible.
		 */
		void GotoPos(long pos)
		{
			SPerform(SCI_GOTOPOS, pos, 0);
		}

		/**
		 * Set the selection anchor to a position. The anchor is the opposite
		 * end of the selection from the caret.
		 */
		void SetAnchor(long posAnchor)
		{
			SPerform(SCI_SETANCHOR, posAnchor, 0);
		}

		/**
		 * Retrieve the text of the line containing the caret.
		 * Returns the index of the caret on the line.
		 */
		int GetCurLine(int length, char* text)
		{
			return (int)SPerform(SCI_GETCURLINE, (long)length, (long)text);
		}

		/**
		 * Retrieve the position of the last correctly styled character.
		 */
		long GetEndStyled()
		{
			return SPerform(SCI_GETENDSTYLED, 0, 0);
		}

		/**
		 * Convert all line endings in the document to one mode.
		 */
		void ConvertEOLs(int eolMode)
		{
			SPerform(SCI_CONVERTEOLS, (long)eolMode, 0);
		}

		/**
		 * Retrieve the current end of line mode - one of CRLF, CR, or LF.
		 */
		int GetEOLMode()
		{
			return (int)SPerform(SCI_GETEOLMODE, 0, 0);
		}

		/**
		 * Set the current end of line mode.
		 */
		void SetEOLMode(int eolMode)
		{
			SPerform(SCI_SETEOLMODE, (long)eolMode, 0);
		}

		/**
		 * Set the current styling position to pos and the styling mask to mask.
		 * The styling mask can be used to protect some bits in each styling byte from modification.
		 */
		void StartStyling(long pos, int mask)
		{
			SPerform(SCI_STARTSTYLING, pos, (long)mask);
		}

		/**
		 * Change style from current styling position for length characters to a style
		 * and move the current styling position to after this newly styled segment.
		 */
		void SetStyling(int length, int style)
		{
			SPerform(SCI_SETSTYLING, (long)length, (long)style);
		}

		/**
		 * Is drawing done first into a buffer or direct to the screen?
		 */
		bool GetBufferedDraw()
		{
			return SPerform(SCI_GETBUFFEREDDRAW, 0, 0) != 0;
		}

		/**
		 * If drawing is buffered then each line of text is drawn into a bitmap buffer
		 * before drawing it to the screen to avoid flicker.
		 */
		void SetBufferedDraw(bool buffered)
		{
			SPerform(SCI_SETBUFFEREDDRAW, (long)buffered, 0);
		}

		/**
		 * Change the visible size of a tab to be a multiple of the width of a space character.
		 */
		void SetTabWidth(int tabWidth)
		{
			SPerform(SCI_SETTABWIDTH, (long)tabWidth, 0);
		}

		/**
		 * Retrieve the visible size of a tab.
		 */
		int GetTabWidth()
		{
			return (int)SPerform(SCI_GETTABWIDTH, 0, 0);
		}

		/**
		 * Set the code page used to interpret the bytes of the document as characters.
		 * The SC_CP_UTF8 value can be used to enter Unicode mode.
		 */
		void SetCodePage(int codePage)
		{
			SPerform(SCI_SETCODEPAGE, (long)codePage, 0);
		}

		/**
		 * In palette mode, Scintilla uses the environment's palette calls to display
		 * more colours. This may lead to ugly displays.
		 */
		void SetUsePalette(bool usePalette)
		{
			SPerform(SCI_SETUSEPALETTE, (long)usePalette, 0);
		}

		/**
		 * Set the symbol used for a particular marker number.
		 */
		void MarkerDefine(int markerNumber, int markerSymbol)
		{
			SPerform(SCI_MARKERDEFINE, (long)markerNumber, (long)markerSymbol);
		}

		/**
		 * Set the foreground colour used for a particular marker number.
		 */
		void MarkerSetFore(int markerNumber, COLORREF fore)
		{
			SPerform(SCI_MARKERSETFORE, (long)markerNumber, (long)fore);
		}

		/**
		 * Set the background colour used for a particular marker number.
		 */
		void MarkerSetBack(int markerNumber, COLORREF back)
		{
			SPerform(SCI_MARKERSETBACK, (long)markerNumber, (long)back);
		}

		/**
		 * Add a marker to a line, returning an ID which can be used to find or delete the marker.
		 */
		int MarkerAdd(int line, int markerNumber)
		{
			return (int)SPerform(SCI_MARKERADD, (long)line, (long)markerNumber);
		}

		/**
		 * Delete a marker from a line.
		 */
		void MarkerDelete(int line, int markerNumber)
		{
			SPerform(SCI_MARKERDELETE, (long)line, (long)markerNumber);
		}

		/**
		 * Delete all markers with a particular number from all lines.
		 */
		void MarkerDeleteAll(int markerNumber)
		{
			SPerform(SCI_MARKERDELETEALL, (long)markerNumber, 0);
		}

		/**
		 * Get a bit mask of all the markers set on a line.
		 */
		int MarkerGet(int line)
		{
			return (int)SPerform(SCI_MARKERGET, (long)line, 0);
		}

		/**
		 * Find the next line after lineStart that includes a marker in mask.
		 */
		int MarkerNext(int lineStart, int markerMask)
		{
			return (int)SPerform(SCI_MARKERNEXT, (long)lineStart, (long)markerMask);
		}

		/**
		 * Find the previous line before lineStart that includes a marker in mask.
		 */
		int MarkerPrevious(int lineStart, int markerMask)
		{
			return (int)SPerform(SCI_MARKERPREVIOUS, (long)lineStart, (long)markerMask);
		}

		/**
		 * Define a marker from a pixmap.
		 */
		void MarkerDefinePixmap(int markerNumber, const char* pixmap)
		{
			SPerform(SCI_MARKERDEFINEPIXMAP, (long)markerNumber, (long)pixmap);
		}

		/**
		 * Set a margin to be either numeric or symbolic.
		 */
		void SetMarginTypeN(int margin, int marginType)
		{
			SPerform(SCI_SETMARGINTYPEN, (long)margin, (long)marginType);
		}

		/**
		 * Retrieve the type of a margin.
		 */
		int GetMarginTypeN(int margin)
		{
			return (int)SPerform(SCI_GETMARGINTYPEN, (long)margin, 0);
		}

		/**
		 * Set the width of a margin to a width expressed in pixels.
		 */
		void SetMarginWidthN(int margin, int pixelWidth)
		{
			SPerform(SCI_SETMARGINWIDTHN, (long)margin, (long)pixelWidth);
		}

		/**
		 * Retrieve the width of a margin in pixels.
		 */
		int GetMarginWidthN(int margin)
		{
			return (int)SPerform(SCI_GETMARGINWIDTHN, (long)margin, 0);
		}

		/**
		 * Set a mask that determines which markers are displayed in a margin.
		 */
		void SetMarginMaskN(int margin, int mask)
		{
			SPerform(SCI_SETMARGINMASKN, (long)margin, (long)mask);
		}

		/**
		 * Retrieve the marker mask of a margin.
		 */
		int GetMarginMaskN(int margin)
		{
			return (int)SPerform(SCI_GETMARGINMASKN, (long)margin, 0);
		}

		/**
		 * Make a margin sensitive or insensitive to mouse clicks.
		 */
		void SetMarginSensitiveN(int margin, bool sensitive)
		{
			SPerform(SCI_SETMARGINSENSITIVEN, (long)margin, (long)sensitive);
		}

		/**
		 * Retrieve the mouse click sensitivity of a margin.
		 */
		bool GetMarginSensitiveN(int margin)
		{
			return SPerform(SCI_GETMARGINSENSITIVEN, (long)margin, 0) != 0;
		}

		/**
		 * Clear all the styles and make equivalent to the global default style.
		 */
		void StyleClearAll()
		{
			SPerform(SCI_STYLECLEARALL, 0, 0);
		}

		/**
		 * Set the foreground colour of a style.
		 */
		void StyleSetFore(int style, COLORREF fore)
		{
			SPerform(SCI_STYLESETFORE, (long)style, (long)fore);
		}

		/**
		 * Set the background colour of a style.
		 */
		void StyleSetBack(int style, COLORREF back)
		{
			SPerform(SCI_STYLESETBACK, (long)style, (long)back);
		}

		/**
		 * Set a style to be bold or not.
		 */
		void StyleSetBold(int style, bool bold)
		{
			SPerform(SCI_STYLESETBOLD, (long)style, (long)bold);
		}

		/**
		 * Set a style to be italic or not.
		 */
		void StyleSetItalic(int style, bool italic)
		{
			SPerform(SCI_STYLESETITALIC, (long)style, (long)italic);
		}

		/**
		 * Set the size of characters of a style.
		 */
		void StyleSetSize(int style, int sizePoints)
		{
			SPerform(SCI_STYLESETSIZE, (long)style, (long)sizePoints);
		}

		/**
		 * Set the font of a style.
		 */
		void StyleSetFont(int style, const char* fontName)
		{
			SPerform(SCI_STYLESETFONT, (long)style, (long)fontName);
		}

		/**
		 * Set a style to have its end of line filled or not.
		 */
		void StyleSetEOLFilled(int style, bool filled)
		{
			SPerform(SCI_STYLESETEOLFILLED, (long)style, (long)filled);
		}

		/**
		 * Reset the default style to its state at startup
		 */
		void StyleResetDefault()
		{
			SPerform(SCI_STYLERESETDEFAULT, 0, 0);
		}

		/**
		 * Set a style to be underlined or not.
		 */
		void StyleSetUnderline(int style, bool underline)
		{
			SPerform(SCI_STYLESETUNDERLINE, (long)style, (long)underline);
		}

		/**
		 * Set a style to be mixed case, or to force upper or lower case.
		 */
		void StyleSetCase(int style, int caseForce)
		{
			SPerform(SCI_STYLESETCASE, (long)style, (long)caseForce);
		}

		/**
		 * Set the character set of the font in a style.
		 */
		void StyleSetCharacterSet(int style, int characterSet)
		{
			SPerform(SCI_STYLESETCHARACTERSET, (long)style, (long)characterSet);
		}

		/**
		 * Set the foreground colour of the selection and whether to use this setting.
		 */
		void SetSelFore(bool useSetting, COLORREF fore)
		{
			SPerform(SCI_SETSELFORE, (long)useSetting, (long)fore);
		}

		/**
		 * Set the background colour of the selection and whether to use this setting.
		 */
		void SetSelBack(bool useSetting, COLORREF back)
		{
			SPerform(SCI_SETSELBACK, (long)useSetting, (long)back);
		}

		/**
		 * Set the foreground colour of the caret.
		 */
		void SetCaretFore(COLORREF fore)
		{
			SPerform(SCI_SETCARETFORE, (long)fore, 0);
		}

		/**
		 * When key+modifier combination km is pressed perform msg.
		 */
		void AssignCmdKey(DWORD km, int msg)
		{
			SPerform(SCI_ASSIGNCMDKEY, (long)km, (long)msg);
		}

		/**
		 * When key+modifier combination km do nothing.
		 */
		void ClearCmdKey(DWORD km)
		{
			SPerform(SCI_CLEARCMDKEY, (long)km, 0);
		}

		/**
		 * Drop all key mappings.
		 */
		void ClearAllCmdKeys()
		{
			SPerform(SCI_CLEARALLCMDKEYS, 0, 0);
		}

		/**
		 * Set the styles for a segment of the document.
		 */
		void SetStylingEx(int length, const char* styles)
		{
			SPerform(SCI_SETSTYLINGEX, (long)length, (long)styles);
		}

		/**
		 * Set a style to be visible or not.
		 */
		void StyleSetVisible(int style, bool visible)
		{
			SPerform(SCI_STYLESETVISIBLE, (long)style, (long)visible);
		}

		/**
		 * Get the time in milliseconds that the caret is on and off.
		 */
		int GetCaretPeriod()
		{
			return (int)SPerform(SCI_GETCARETPERIOD, 0, 0);
		}

		/**
		 * Get the time in milliseconds that the caret is on and off. 0 = steady on.
		 */
		void SetCaretPeriod(int periodMilliseconds)
		{
			SPerform(SCI_SETCARETPERIOD, (long)periodMilliseconds, 0);
		}

		/**
		 * Set the set of characters making up words for when moving or selecting by word.
		 */
		void SetWordChars(const char* characters)
		{
			SPerform(SCI_SETWORDCHARS, 0, (long)characters);
		}

		/**
		 * Start a sequence of actions that is undone and redone as a unit.
		 * May be nested.
		 */
		void BeginUndoAction()
		{
			SPerform(SCI_BEGINUNDOACTION, 0, 0);
		}

		/**
		 * End a sequence of actions that is undone and redone as a unit.
		 */
		void EndUndoAction()
		{
			SPerform(SCI_ENDUNDOACTION, 0, 0);
		}

		/**
		 * Set an indicator to plain, squiggle or TT.
		 */
		void IndicSetStyle(int indic, int style)
		{
			SPerform(SCI_INDICSETSTYLE, (long)indic, (long)style);
		}

		/**
		 * Retrieve the style of an indicator.
		 */
		int IndicGetStyle(int indic)
		{
			return (int)SPerform(SCI_INDICGETSTYLE, (long)indic, 0);
		}

		/**
		 * Set the foreground colour of an indicator.
		 */
		void IndicSetFore(int indic, COLORREF fore)
		{
			SPerform(SCI_INDICSETFORE, (long)indic, (long)fore);
		}

		/**
		 * Retrieve the foreground colour of an indicator.
		 */
		COLORREF IndicGetFore(int indic)
		{
			return (COLORREF)SPerform(SCI_INDICGETFORE, (long)indic, 0);
		}

		/**
		 * Set the foreground colour of all whitespace and whether to use this setting.
		 */
		void SetWhitespaceFore(bool useSetting, COLORREF fore)
		{
			SPerform(SCI_SETWHITESPACEFORE, (long)useSetting, (long)fore);
		}

		/**
		 * Set the background colour of all whitespace and whether to use this setting.
		 */
		void SetWhitespaceBack(bool useSetting, COLORREF back)
		{
			SPerform(SCI_SETWHITESPACEBACK, (long)useSetting, (long)back);
		}

		/**
		 * Divide each styling byte into lexical class bits (default: 5) and indicator
		 * bits (default: 3). If a lexer requires more than 32 lexical states, then this
		 * is used to expand the possible states.
		 */
		void SetStyleBits(int bits)
		{
			SPerform(SCI_SETSTYLEBITS, (long)bits, 0);
		}

		/**
		 * Retrieve number of bits in style bytes used to hold the lexical state.
		 */
		int GetStyleBits()
		{
			return (int)SPerform(SCI_GETSTYLEBITS, 0, 0);
		}

		/**
		 * Used to hold extra styling information for each line.
		 */
		void SetLineState(int line, int state)
		{
			SPerform(SCI_SETLINESTATE, (long)line, (long)state);
		}

		/**
		 * Retrieve the extra styling information for a line.
		 */
		int GetLineState(int line)
		{
			return (int)SPerform(SCI_GETLINESTATE, (long)line, 0);
		}

		/**
		 * Retrieve the last line number that has line state.
		 */
		int GetMaxLineState()
		{
			return (int)SPerform(SCI_GETMAXLINESTATE, 0, 0);
		}

		/**
		 * Is the background of the line containing the caret in a different colour?
		 */
		bool GetCaretLineVisible()
		{
			return SPerform(SCI_GETCARETLINEVISIBLE, 0, 0) != 0;
		}

		/**
		 * Display the background of the line containing the caret in a different colour.
		 */
		void SetCaretLineVisible(bool show)
		{
			SPerform(SCI_SETCARETLINEVISIBLE, (long)show, 0);
		}

		/**
		 * Get the colour of the background of the line containing the caret.
		 */
		COLORREF GetCaretLineBack()
		{
			return (COLORREF)SPerform(SCI_GETCARETLINEBACK, 0, 0);
		}

		/**
		 * Set the colour of the background of the line containing the caret.
		 */
		void SetCaretLineBack(COLORREF back)
		{
			SPerform(SCI_SETCARETLINEBACK, (long)back, 0);
		}

		/**
		 * Set a style to be changeable or not (read only).
		 * Experimental feature, currently buggy.
		 */
		void StyleSetChangeable(int style, bool changeable)
		{
			SPerform(SCI_STYLESETCHANGEABLE, (long)style, (long)changeable);
		}

		/**
		 * Display a auto-completion list.
		 * The lenEntered parameter indicates how many characters before
		 * the caret should be used to provide context.
		 */
		void AutoCShow(int lenEntered, const char* itemList)
		{
			SPerform(SCI_AUTOCSHOW, (long)lenEntered, (long)itemList);
		}

		/**
		 * Remove the auto-completion list from the screen.
		 */
		void AutoCCancel()
		{
			SPerform(SCI_AUTOCCANCEL, 0, 0);
		}

		/**
		 * Is there an auto-completion list visible?
		 */
		bool AutoCActive()
		{
			return SPerform(SCI_AUTOCACTIVE, 0, 0) != 0;
		}

		/**
		 * Retrieve the position of the caret when the auto-completion list was displayed.
		 */
		long AutoCPosStart()
		{
			return SPerform(SCI_AUTOCPOSSTART, 0, 0);
		}

		/**
		 * User has selected an item so remove the list and insert the selection.
		 */
		void AutoCComplete()
		{
			SPerform(SCI_AUTOCCOMPLETE, 0, 0);
		}

		/**
		 * Define a set of character that when typed cancel the auto-completion list.
		 */
		void AutoCStops(const char* characterSet)
		{
			SPerform(SCI_AUTOCSTOPS, 0, (long)characterSet);
		}

		/**
		 * Change the separator character in the string setting up an auto-completion list.
		 * Default is space but can be changed if items contain space.
		 */
		void AutoCSetSeparator(int separatorCharacter)
		{
			SPerform(SCI_AUTOCSETSEPARATOR, (long)separatorCharacter, 0);
		}

		/**
		 * Retrieve the auto-completion list separator character.
		 */
		int AutoCGetSeparator()
		{
			return (int)SPerform(SCI_AUTOCGETSEPARATOR, 0, 0);
		}

		/**
		 * Select the item in the auto-completion list that starts with a string.
		 */
		void AutoCSelect(const char* text)
		{
			SPerform(SCI_AUTOCSELECT, 0, (long)text);
		}

		/**
		 * Should the auto-completion list be cancelled if the user backspaces to a
		 * position before where the box was created.
		 */
		void AutoCSetCancelAtStart(bool cancel)
		{
			SPerform(SCI_AUTOCSETCANCELATSTART, (long)cancel, 0);
		}

		/**
		 * Retrieve whether auto-completion cancelled by backspacing before start.
		 */
		bool AutoCGetCancelAtStart()
		{
			return SPerform(SCI_AUTOCGETCANCELATSTART, 0, 0) != 0;
		}

		/**
		 * Define a set of characters that when typed will cause the autocompletion to
		 * choose the selected item.
		 */
		void AutoCSetFillUps(const char* characterSet)
		{
			SPerform(SCI_AUTOCSETFILLUPS, 0, (long)characterSet);
		}

		/**
		 * Should a single item auto-completion list automatically choose the item.
		 */
		void AutoCSetChooseSingle(bool chooseSingle)
		{
			SPerform(SCI_AUTOCSETCHOOSESINGLE, (long)chooseSingle, 0);
		}

		/**
		 * Retrieve whether a single item auto-completion list automatically choose the item.
		 */
		bool AutoCGetChooseSingle()
		{
			return SPerform(SCI_AUTOCGETCHOOSESINGLE, 0, 0) != 0;
		}

		/**
		 * Set whether case is significant when performing auto-completion searches.
		 */
		void AutoCSetIgnoreCase(bool ignoreCase)
		{
			SPerform(SCI_AUTOCSETIGNORECASE, (long)ignoreCase, 0);
		}

		/**
		 * Retrieve state of ignore case flag.
		 */
		bool AutoCGetIgnoreCase()
		{
			return SPerform(SCI_AUTOCGETIGNORECASE, 0, 0) != 0;
		}

		/**
		 * Display a list of strings and send notification when user chooses one.
		 */
		void UserListShow(int listType, const char* itemList)
		{
			SPerform(SCI_USERLISTSHOW, (long)listType, (long)itemList);
		}

		/**
		 * Set whether or not autocompletion is hidden automatically when nothing matches.
		 */
		void AutoCSetAutoHide(bool autoHide)
		{
			SPerform(SCI_AUTOCSETAUTOHIDE, (long)autoHide, 0);
		}

		/**
		 * Retrieve whether or not autocompletion is hidden automatically when nothing matches.
		 */
		bool AutoCGetAutoHide()
		{
			return SPerform(SCI_AUTOCGETAUTOHIDE, 0, 0) != 0;
		}

		/**
		 * Set whether or not autocompletion deletes any word characters
		 * after the inserted text upon completion.
		 */
		void AutoCSetDropRestOfWord(bool dropRestOfWord)
		{
			SPerform(SCI_AUTOCSETDROPRESTOFWORD, (long)dropRestOfWord, 0);
		}

		/**
		 * Retrieve whether or not autocompletion deletes any word characters
		 * after the inserted text upon completion.
		 */
		bool AutoCGetDropRestOfWord()
		{
			return SPerform(SCI_AUTOCGETDROPRESTOFWORD, 0, 0) != 0;
		}

		/**
		 * Register an XPM image for use in autocompletion lists.
		 */
		void RegisterImage(int type, int xpmData)
		{
			SPerform(SCI_REGISTERIMAGE, (long)type, (long)xpmData);
		}

		/**
		 * Set the number of spaces used for one level of indentation.
		 */
		void SetIndent(int indentSize)
		{
			SPerform(SCI_SETINDENT, (long)indentSize, 0);
		}

		/**
		 * Retrieve indentation size.
		 */
		int GetIndent()
		{
			return (int)SPerform(SCI_GETINDENT, 0, 0);
		}

		/**
		 * Indentation will only use space characters if useTabs is false, otherwise
		 * it will use a combination of tabs and spaces.
		 */
		void SetUseTabs(bool useTabs)
		{
			SPerform(SCI_SETUSETABS, (long)useTabs, 0);
		}

		/**
		 * Retrieve whether tabs will be used in indentation.
		 */
		bool GetUseTabs()
		{
			return SPerform(SCI_GETUSETABS, 0, 0) != 0;
		}

		/**
		 * Change the indentation of a line to a number of columns.
		 */
		void SetLineIndentation(int line, int indentSize)
		{
			SPerform(SCI_SETLINEINDENTATION, (long)line, (long)indentSize);
		}

		/**
		 * Retrieve the number of columns that a line is indented.
		 */
		int GetLineIndentation(int line)
		{
			return (int)SPerform(SCI_GETLINEINDENTATION, (long)line, 0);
		}

		/**
		 * Retrieve the position before the first non indentation character on a line.
		 */
		long GetLineIndentPosition(int line)
		{
			return SPerform(SCI_GETLINEINDENTPOSITION, (long)line, 0);
		}

		/**
		 * Retrieve the column number of a position, taking tab width into account.
		 */
		int GetColumn(long pos)
		{
			return (int)SPerform(SCI_GETCOLUMN, pos, 0);
		}

		/**
		 * Show or hide the horizontal scroll bar.
		 */
		void SetHScrollBar(bool show)
		{
			SPerform(SCI_SETHSCROLLBAR, (long)show, 0);
		}

		/**
		 * Is the horizontal scroll bar visible?
		 */
		bool GetHScrollBar()
		{
			return SPerform(SCI_GETHSCROLLBAR, 0, 0) != 0;
		}

		/**
		 * Show or hide indentation guides.
		 */
		void SetIndentationGuides(bool show)
		{
			SPerform(SCI_SETINDENTATIONGUIDES, (long)show, 0);
		}

		/**
		 * Are the indentation guides visible?
		 */
		bool GetIndentationGuides()
		{
			return SPerform(SCI_GETINDENTATIONGUIDES, 0, 0) != 0;
		}

		/**
		 * Set the highlighted indentation guide column.
		 * 0 = no highlighted guide.
		 */
		void SetHighlightGuide(int column)
		{
			SPerform(SCI_SETHIGHLIGHTGUIDE, (long)column, 0);
		}

		/**
		 * Get the highlighted indentation guide column.
		 */
		int GetHighlightGuide()
		{
			return (int)SPerform(SCI_GETHIGHLIGHTGUIDE, 0, 0);
		}

		/**
		 * Get the position after the last visible characters on a line.
		 */
		int GetLineEndPosition(int line)
		{
			return (int)SPerform(SCI_GETLINEENDPOSITION, (long)line, 0);
		}

		/**
		 * Get the code page used to interpret the bytes of the document as characters.
		 */
		int GetCodePage()
		{
			return (int)SPerform(SCI_GETCODEPAGE, 0, 0);
		}

		/**
		 * Get the foreground colour of the caret.
		 */
		COLORREF GetCaretFore()
		{
			return (COLORREF)SPerform(SCI_GETCARETFORE, 0, 0);
		}

		/**
		 * In palette mode?
		 */
		bool GetUsePalette()
		{
			return SPerform(SCI_GETUSEPALETTE, 0, 0) != 0;
		}

		/**
		 * In read-only mode?
		 */
		bool GetReadOnly()
		{
			return SPerform(SCI_GETREADONLY, 0, 0) != 0;
		}

		/**
		 * Sets the position of the caret.
		 */
		void SetCurrentPos(long pos)
		{
			SPerform(SCI_SETCURRENTPOS, pos, 0);
		}

		/**
		 * Sets the position that starts the selection - this becomes the anchor.
		 */
		void SetSelectionStart(long pos)
		{
			SPerform(SCI_SETSELECTIONSTART, pos, 0);
		}

		/**
		 * Returns the position at the start of the selection.
		 */
		long GetSelectionStart()
		{
			return SPerform(SCI_GETSELECTIONSTART, 0, 0);
		}

		/**
		 * Sets the position that ends the selection - this becomes the currentPosition.
		 */
		void SetSelectionEnd(long pos)
		{
			SPerform(SCI_SETSELECTIONEND, pos, 0);
		}

		/**
		 * Returns the position at the end of the selection.
		 */
		long GetSelectionEnd()
		{
			return SPerform(SCI_GETSELECTIONEND, 0, 0);
		}

		/**
		 * Sets the print magnification added to the point size of each style for printing.
		 */
		void SetPrintMagnification(int magnification)
		{
			SPerform(SCI_SETPRINTMAGNIFICATION, (long)magnification, 0);
		}

		/**
		 * Returns the print magnification.
		 */
		int GetPrintMagnification()
		{
			return (int)SPerform(SCI_GETPRINTMAGNIFICATION, 0, 0);
		}

		/**
		 * Modify colours when printing for clearer printed text.
		 */
		void SetPrintColourMode(int mode)
		{
			SPerform(SCI_SETPRINTCOLOURMODE, (long)mode, 0);
		}

		/**
		 * Returns the print colour mode.
		 */
		int GetPrintColourMode()
		{
			return (int)SPerform(SCI_GETPRINTCOLOURMODE, 0, 0);
		}

		/**
		 * Find some text in the document.
		 */
		long FindText(int flags, TextToFind* ft)
		{
			return SPerform(SCI_FINDTEXT, (long)flags, (long)ft);
		}

		/**
		 * On Windows, will draw the document into a display context such as a printer.
		 */
		void FormatRange(bool draw, long fr)
		{
			SPerform(SCI_FORMATRANGE, (long)draw, fr);
		}

		/**
		 * Retrieve the display line at the top of the display.
		 */
		int GetFirstVisibleLine()
		{
			return (int)SPerform(SCI_GETFIRSTVISIBLELINE, 0, 0);
		}

		/**
		 * Retrieve the contents of a line.
		 * Returns the length of the line.
		 */
		int GetLine(int line, char* text)
		{
			return (int)SPerform(SCI_GETLINE, (long)line, (long)text);
		}

		/**
		 * Returns the number of lines in the document. There is always at least one.
		 */
		int GetLineCount()
		{
			return (int)SPerform(SCI_GETLINECOUNT, 0, 0);
		}

		/**
		 * Sets the size in pixels of the left margin.
		 */
		void SetMarginLeft(int pixelWidth)
		{
			SPerform(SCI_SETMARGINLEFT, 0, (long)pixelWidth);
		}

		/**
		 * Returns the size in pixels of the left margin.
		 */
		int GetMarginLeft()
		{
			return (int)SPerform(SCI_GETMARGINLEFT, 0, 0);
		}

		/**
		 * Sets the size in pixels of the right margin.
		 */
		void SetMarginRight(int pixelWidth)
		{
			SPerform(SCI_SETMARGINRIGHT, 0, (long)pixelWidth);
		}

		/**
		 * Returns the size in pixels of the right margin.
		 */
		int GetMarginRight()
		{
			return (int)SPerform(SCI_GETMARGINRIGHT, 0, 0);
		}

		/**
		 * Is the document different from when it was last saved?
		 */
		bool GetModify()
		{
			return SPerform(SCI_GETMODIFY, 0, 0) != 0;
		}

		/**
		 * Select a range of text.
		 */
		void SetSel(long start, long end)
		{
			SPerform(SCI_SETSEL, start, end);
		}

		/**
		 * Retrieve the selected text.
		 * Return the length of the text.
		 */
		int GetSelText(char* text)
		{
			return (int)SPerform(SCI_GETSELTEXT, 0, (long)text);
		}

		/**
		 * Retrieve a range of text.
		 * Return the length of the text.
		 */
		int GetTextRange(TextRange* tr)
		{
			return (int)SPerform(SCI_GETTEXTRANGE, 0, (long)tr);
		}

		/**
		 * Draw the selection in normal style or with selection highlighted.
		 */
		void HideSelection(bool normal)
		{
			SPerform(SCI_HIDESELECTION, (long)normal, 0);
		}

		/**
		 * Retrieve the x value of the point in the window where a position is displayed.
		 */
		int PointXFromPosition(long pos)
		{
			return (int)SPerform(SCI_POINTXFROMPOSITION, 0, pos);
		}

		/**
		 * Retrieve the y value of the point in the window where a position is displayed.
		 */
		int PointYFromPosition(long pos)
		{
			return (int)SPerform(SCI_POINTYFROMPOSITION, 0, pos);
		}

		/**
		 * Retrieve the line containing a position.
		 */
		int LineFromPosition(long pos)
		{
			return (int)SPerform(SCI_LINEFROMPOSITION, pos, 0);
		}

		/**
		 * Retrieve the position at the start of a line.
		 */
		int PositionFromLine(int line)
		{
			return (int)SPerform(SCI_POSITIONFROMLINE, (long)line, 0);
		}

		/**
		 * Scroll horizontally and vertically.
		 */
		void LineScroll(int columns, int lines)
		{
			SPerform(SCI_LINESCROLL, (long)columns, (long)lines);
		}

		/**
		 * Ensure the caret is visible.
		 */
		void ScrollCaret()
		{
			SPerform(SCI_SCROLLCARET, 0, 0);
		}

		/**
		 * Replace the selected text with the argument text.
		 */
		void ReplaceSel(const char* text)
		{
			SPerform(SCI_REPLACESEL, 0, (long)text);
		}

		/**
		 * Set to read only or read write.
		 */
		void SetReadOnly(bool readOnly)
		{
			SPerform(SCI_SETREADONLY, (long)readOnly, 0);
		}

		/**
		 * Null operation.
		 */
		void Null()
		{
			SPerform(SCI_NULL, 0, 0);
		}

		/**
		 * Will a paste succeed?
		 */
		bool CanPaste()
		{
			return SPerform(SCI_CANPASTE, 0, 0) != 0;
		}

		/**
		 * Are there any undoable actions in the undo history?
		 */
		bool CanUndo()
		{
			return SPerform(SCI_CANUNDO, 0, 0) != 0;
		}

		/**
		 * Delete the undo history.
		 */
		void EmptyUndoBuffer()
		{
			SPerform(SCI_EMPTYUNDOBUFFER, 0, 0);
		}

		/**
		 * Undo one action in the undo history.
		 */
		void Undo()
		{
			SPerform(SCI_UNDO, 0, 0);
		}

		/**
		 * Cut the selection to the clipboard.
		 */
		void Cut()
		{
			SPerform(SCI_CUT, 0, 0);
		}

		/**
		 * Copy the selection to the clipboard.
		 */
		void Copy()
		{
			SPerform(SCI_COPY, 0, 0);
		}

		/**
		 * Paste the contents of the clipboard into the document replacing the selection.
		 */
		void Paste()
		{
			SPerform(SCI_PASTE, 0, 0);
		}

		/**
		 * Clear the selection.
		 */
		void Clear()
		{
			SPerform(SCI_CLEAR, 0, 0);
		}

		/**
		 * Replace the contents of the document with the argument text.
		 */
		void SetText(const char* text)
		{
			SPerform(SCI_SETTEXT, 0, (long)text);
		}

		/**
		 * Retrieve all the text in the document.
		 * Returns number of characters retrieved.
		 */
		int GetText(int length, char* text)
		{
			return (int)SPerform(SCI_GETTEXT, (long)length, (long)text);
		}

		/**
		 * Retrieve the number of characters in the document.
		 */
		int GetTextLength()
		{
			return (int)SPerform(SCI_GETTEXTLENGTH, 0, 0);
		}

		/**
		 * Retrieve a pointer to a function that processes messages for this Scintilla.
		 */
		int GetDirectFunction()
		{
			return (int)SPerform(SCI_GETDIRECTFUNCTION, 0, 0);
		}

		/**
		 * Retrieve a pointer value to use as the first argument when calling
		 * the function returned by GetDirectFunction.
		 */
		int GetDirectPointer()
		{
			return (int)SPerform(SCI_GETDIRECTPOINTER, 0, 0);
		}

		/**
		 * Set to overtype (true) or insert mode.
		 */
		void SetOvertype(bool overtype)
		{
			SPerform(SCI_SETOVERTYPE, (long)overtype, 0);
		}

		/**
		 * Returns true if overtype mode is active otherwise false is returned.
		 */
		bool GetOvertype()
		{
			return SPerform(SCI_GETOVERTYPE, 0, 0) != 0;
		}

		/**
		 * Set the width of the insert mode caret.
		 */
		void SetCaretWidth(int pixelWidth)
		{
			SPerform(SCI_SETCARETWIDTH, (long)pixelWidth, 0);
		}

		/**
		 * Returns the width of the insert mode caret.
		 */
		int GetCaretWidth()
		{
			return (int)SPerform(SCI_GETCARETWIDTH, 0, 0);
		}

		/**
		 * Sets the position that starts the target which is used for updating the
		 * document without affecting the scroll position.
		 */
		void SetTargetStart(long pos)
		{
			SPerform(SCI_SETTARGETSTART, pos, 0);
		}

		/**
		 * Get the position that starts the target.
		 */
		long GetTargetStart()
		{
			return SPerform(SCI_GETTARGETSTART, 0, 0);
		}

		/**
		 * Sets the position that ends the target which is used for updating the
		 * document without affecting the scroll position.
		 */
		void SetTargetEnd(long pos)
		{
			SPerform(SCI_SETTARGETEND, pos, 0);
		}

		/**
		 * Get the position that ends the target.
		 */
		long GetTargetEnd()
		{
			return SPerform(SCI_GETTARGETEND, 0, 0);
		}

		/**
		 * Replace the target text with the argument text.
		 * Text is counted so it can contain nulls.
		 * Returns the length of the replacement text.
		 */
		int ReplaceTarget(int length, const char* text)
		{
			return (int)SPerform(SCI_REPLACETARGET, (long)length, (long)text);
		}

		/**
		 * Replace the target text with the argument text after \d processing.
		 * Text is counted so it can contain nulls.
		 * Looks for \d where d is between 1 and 9 and replaces these with the strings
		 * matched in the last search operation which were surrounded by \( and \).
		 * Returns the length of the replacement text including any change
		 * caused by processing the \d patterns.
		 */
		int ReplaceTargetRE(int length, const char* text)
		{
			return (int)SPerform(SCI_REPLACETARGETRE, (long)length, (long)text);
		}

		/**
		 * Search for a counted string in the target and set the target to the found
		 * range. Text is counted so it can contain nulls.
		 * Returns length of range or -1 for failure in which case target is not moved.
		 */
		int SearchInTarget(int length, const char* text)
		{
			return (int)SPerform(SCI_SEARCHINTARGET, (long)length, (long)text);
		}

		/**
		 * Set the search flags used by SearchInTarget.
		 */
		void SetSearchFlags(int flags)
		{
			SPerform(SCI_SETSEARCHFLAGS, (long)flags, 0);
		}

		/**
		 * Get the search flags used by SearchInTarget.
		 */
		int GetSearchFlags()
		{
			return (int)SPerform(SCI_GETSEARCHFLAGS, 0, 0);
		}

		/**
		 * Show a call tip containing a definition near position pos.
		 */
		void CallTipShow(long pos, const char* definition)
		{
			SPerform(SCI_CALLTIPSHOW, pos, (long)definition);
		}

		/**
		 * Remove the call tip from the screen.
		 */
		void CallTipCancel()
		{
			SPerform(SCI_CALLTIPCANCEL, 0, 0);
		}

		/**
		 * Is there an active call tip?
		 */
		bool CallTipActive()
		{
			return SPerform(SCI_CALLTIPACTIVE, 0, 0) != 0;
		}

		/**
		 * Retrieve the position where the caret was before displaying the call tip.
		 */
		long CallTipPosStart()
		{
			return SPerform(SCI_CALLTIPPOSSTART, 0, 0);
		}

		/**
		 * Highlight a segment of the definition.
		 */
		void CallTipSetHlt(int start, int end)
		{
			SPerform(SCI_CALLTIPSETHLT, (long)start, (long)end);
		}

		/**
		 * Set the background colour for the call tip.
		 */
		void CallTipSetBack(COLORREF back)
		{
			SPerform(SCI_CALLTIPSETBACK, (long)back, 0);
		}

		/**
		 * Find the display line of a document line taking hidden lines into account.
		 */
		int VisibleFromDocLine(int line)
		{
			return (int)SPerform(SCI_VISIBLEFROMDOCLINE, (long)line, 0);
		}

		/**
		 * Find the document line of a display line taking hidden lines into account.
		 */
		int DocLineFromVisible(int lineDisplay)
		{
			return (int)SPerform(SCI_DOCLINEFROMVISIBLE, (long)lineDisplay, 0);
		}

		/**
		 * Set the fold level of a line.
		 * This encodes an integer level along with flags indicating whether the
		 * line is a header and whether it is effectively white space.
		 */
		void SetFoldLevel(int line, int level)
		{
			SPerform(SCI_SETFOLDLEVEL, (long)line, (long)level);
		}

		/**
		 * Retrieve the fold level of a line.
		 */
		int GetFoldLevel(int line)
		{
			return (int)SPerform(SCI_GETFOLDLEVEL, (long)line, 0);
		}

		/**
		 * Find the last child line of a header line.
		 */
		int GetLastChild(int line, int level)
		{
			return (int)SPerform(SCI_GETLASTCHILD, (long)line, (long)level);
		}

		/**
		 * Find the parent line of a child line.
		 */
		int GetFoldParent(int line)
		{
			return (int)SPerform(SCI_GETFOLDPARENT, (long)line, 0);
		}

		/**
		 * Make a range of lines visible.
		 */
		void ShowLines(int lineStart, int lineEnd)
		{
			SPerform(SCI_SHOWLINES, (long)lineStart, (long)lineEnd);
		}

		/**
		 * Make a range of lines invisible.
		 */
		void HideLines(int lineStart, int lineEnd)
		{
			SPerform(SCI_HIDELINES, (long)lineStart, (long)lineEnd);
		}

		/**
		 * Is a line visible?
		 */
		bool GetLineVisible(int line)
		{
			return SPerform(SCI_GETLINEVISIBLE, (long)line, 0) != 0;
		}

		/**
		 * Show the children of a header line.
		 */
		void SetFoldExpanded(int line, bool expanded)
		{
			SPerform(SCI_SETFOLDEXPANDED, (long)line, (long)expanded);
		}

		/**
		 * Is a header line expanded?
		 */
		bool GetFoldExpanded(int line)
		{
			return SPerform(SCI_GETFOLDEXPANDED, (long)line, 0) != 0;
		}

		/**
		 * Switch a header line between expanded and contracted.
		 */
		void ToggleFold(int line)
		{
			SPerform(SCI_TOGGLEFOLD, (long)line, 0);
		}

		/**
		 * Ensure a particular line is visible by expanding any header line hiding it.
		 */
		void EnsureVisible(int line)
		{
			SPerform(SCI_ENSUREVISIBLE, (long)line, 0);
		}

		/**
		 * Set some style options for folding.
		 */
		void SetFoldFlags(int flags)
		{
			SPerform(SCI_SETFOLDFLAGS, (long)flags, 0);
		}

		/**
		 * Ensure a particular line is visible by expanding any header line hiding it.
		 * Use the currently set visibility policy to determine which range to display.
		 */
		void EnsureVisibleEnforcePolicy(int line)
		{
			SPerform(SCI_ENSUREVISIBLEENFORCEPOLICY, (long)line, 0);
		}

		/**
		 * Sets whether a tab pressed when caret is within indentation indents.
		 */
		void SetTabIndents(bool tabIndents)
		{
			SPerform(SCI_SETTABINDENTS, (long)tabIndents, 0);
		}

		/**
		 * Does a tab pressed when caret is within indentation indent?
		 */
		bool GetTabIndents()
		{
			return SPerform(SCI_GETTABINDENTS, 0, 0) != 0;
		}

		/**
		 * Sets whether a backspace pressed when caret is within indentation unindents.
		 */
		void SetBackSpaceUnIndents(bool bsUnIndents)
		{
			SPerform(SCI_SETBACKSPACEUNINDENTS, (long)bsUnIndents, 0);
		}

		/**
		 * Does a backspace pressed when caret is within indentation unindent?
		 */
		bool GetBackSpaceUnIndents()
		{
			return SPerform(SCI_GETBACKSPACEUNINDENTS, 0, 0) != 0;
		}

		/**
		 * Sets the time the mouse must sit still to generate a mouse dwell event.
		 */
		void SetMouseDwellTime(int periodMilliseconds)
		{
			SPerform(SCI_SETMOUSEDWELLTIME, (long)periodMilliseconds, 0);
		}

		/**
		 * Retrieve the time the mouse must sit still to generate a mouse dwell event.
		 */
		int GetMouseDwellTime()
		{
			return (int)SPerform(SCI_GETMOUSEDWELLTIME, 0, 0);
		}

		/**
		 * Get position of start of word.
		 */
		int WordStartPosition(long pos, bool onlyWordCharacters)
		{
			return (int)SPerform(SCI_WORDSTARTPOSITION, pos, (long)onlyWordCharacters);
		}

		/**
		 * Get position of end of word.
		 */
		int WordEndPosition(long pos, bool onlyWordCharacters)
		{
			return (int)SPerform(SCI_WORDENDPOSITION, pos, (long)onlyWordCharacters);
		}

		/**
		 * Sets whether text is word wrapped.
		 */
		void SetWrapMode(int mode)
		{
			SPerform(SCI_SETWRAPMODE, (long)mode, 0);
		}

		/**
		 * Retrieve whether text is word wrapped.
		 */
		int GetWrapMode()
		{
			return (int)SPerform(SCI_GETWRAPMODE, 0, 0);
		}

		/**
		 * Sets the degree of caching of layout information.
		 */
		void SetLayoutCache(int mode)
		{
			SPerform(SCI_SETLAYOUTCACHE, (long)mode, 0);
		}

		/**
		 * Retrieve the degree of caching of layout information.
		 */
		int GetLayoutCache()
		{
			return (int)SPerform(SCI_GETLAYOUTCACHE, 0, 0);
		}

		/**
		 * Sets the document width assumed for scrolling.
		 */
		void SetScrollWidth(int pixelWidth)
		{
			SPerform(SCI_SETSCROLLWIDTH, (long)pixelWidth, 0);
		}

		/**
		 * Retrieve the document width assumed for scrolling.
		 */
		int GetScrollWidth()
		{
			return (int)SPerform(SCI_GETSCROLLWIDTH, 0, 0);
		}

		/**
		 * Measure the pixel width of some text in a particular style.
		 * Nul terminated text argument.
		 * Does not handle tab or control characters.
		 */
		int TextWidth(int style, const char* text)
		{
			return (int)SPerform(SCI_TEXTWIDTH, (long)style, (long)text);
		}

		/**
		 * Sets the scroll range so that maximum scroll position has
		 * the last line at the bottom of the view (default).
		 * Setting this to false allows scrolling one page below the last line.
		 */
		void SetEndAtLastLine(bool endAtLastLine)
		{
			SPerform(SCI_SETENDATLASTLINE, (long)endAtLastLine, 0);
		}

		/**
		 * Retrieve whether the maximum scroll position has the last
		 * line at the bottom of the view.
		 */
		int GetEndAtLastLine()
		{
			return (int)SPerform(SCI_GETENDATLASTLINE, 0, 0);
		}

		/**
		 * Retrieve the height of a particular line of text in pixels.
		 */
		int TextHeight(int line)
		{
			return (int)SPerform(SCI_TEXTHEIGHT, (long)line, 0);
		}

		/**
		 * Show or hide the vertical scroll bar.
		 */
		void SetVScrollBar(bool show)
		{
			SPerform(SCI_SETVSCROLLBAR, (long)show, 0);
		}

		/**
		 * Is the vertical scroll bar visible?
		 */
		bool GetVScrollBar()
		{
			return SPerform(SCI_GETVSCROLLBAR, 0, 0) != 0;
		}

		/**
		 * Append a string to the end of the document without changing the selection.
		 */
		void AppendText(int length, const char* text)
		{
			SPerform(SCI_APPENDTEXT, (long)length, (long)text);
		}

		/**
		 * Move caret down one line.
		 */
		void LineDown()
		{
			SPerform(SCI_LINEDOWN, 0, 0);
		}

		/**
		 * Move caret down one line extending selection to new caret position.
		 */
		void LineDownExtend()
		{
			SPerform(SCI_LINEDOWNEXTEND, 0, 0);
		}

		/**
		 * Move caret up one line.
		 */
		void LineUp()
		{
			SPerform(SCI_LINEUP, 0, 0);
		}

		/**
		 * Move caret up one line extending selection to new caret position.
		 */
		void LineUpExtend()
		{
			SPerform(SCI_LINEUPEXTEND, 0, 0);
		}

		/**
		 * Move caret left one character.
		 */
		void CharLeft()
		{
			SPerform(SCI_CHARLEFT, 0, 0);
		}

		/**
		 * Move caret left one character extending selection to new caret position.
		 */
		void CharLeftExtend()
		{
			SPerform(SCI_CHARLEFTEXTEND, 0, 0);
		}

		/**
		 * Move caret right one character.
		 */
		void CharRight()
		{
			SPerform(SCI_CHARRIGHT, 0, 0);
		}

		/**
		 * Move caret right one character extending selection to new caret position.
		 */
		void CharRightExtend()
		{
			SPerform(SCI_CHARRIGHTEXTEND, 0, 0);
		}

		/**
		 * Move caret left one word.
		 */
		void WordLeft()
		{
			SPerform(SCI_WORDLEFT, 0, 0);
		}

		/**
		 * Move caret left one word extending selection to new caret position.
		 */
		void WordLeftExtend()
		{
			SPerform(SCI_WORDLEFTEXTEND, 0, 0);
		}

		/**
		 * Move caret right one word.
		 */
		void WordRight()
		{
			SPerform(SCI_WORDRIGHT, 0, 0);
		}

		/**
		 * Move caret right one word extending selection to new caret position.
		 */
		void WordRightExtend()
		{
			SPerform(SCI_WORDRIGHTEXTEND, 0, 0);
		}

		/**
		 * Move caret to first position on line.
		 */
		void Home()
		{
			SPerform(SCI_HOME, 0, 0);
		}

		/**
		 * Move caret to first position on line extending selection to new caret position.
		 */
		void HomeExtend()
		{
			SPerform(SCI_HOMEEXTEND, 0, 0);
		}

		/**
		 * Move caret to last position on line.
		 */
		void LineEnd()
		{
			SPerform(SCI_LINEEND, 0, 0);
		}

		/**
		 * Move caret to last position on line extending selection to new caret position.
		 */
		void LineEndExtend()
		{
			SPerform(SCI_LINEENDEXTEND, 0, 0);
		}

		/**
		 * Move caret to first position in document.
		 */
		void DocumentStart()
		{
			SPerform(SCI_DOCUMENTSTART, 0, 0);
		}

		/**
		 * Move caret to first position in document extending selection to new caret position.
		 */
		void DocumentStartExtend()
		{
			SPerform(SCI_DOCUMENTSTARTEXTEND, 0, 0);
		}

		/**
		 * Move caret to last position in document.
		 */
		void DocumentEnd()
		{
			SPerform(SCI_DOCUMENTEND, 0, 0);
		}

		/**
		 * Move caret to last position in document extending selection to new caret position.
		 */
		void DocumentEndExtend()
		{
			SPerform(SCI_DOCUMENTENDEXTEND, 0, 0);
		}

		/**
		 * Move caret one page up.
		 */
		void PageUp()
		{
			SPerform(SCI_PAGEUP, 0, 0);
		}

		/**
		 * Move caret one page up extending selection to new caret position.
		 */
		void PageUpExtend()
		{
			SPerform(SCI_PAGEUPEXTEND, 0, 0);
		}

		/**
		 * Move caret one page down.
		 */
		void PageDown()
		{
			SPerform(SCI_PAGEDOWN, 0, 0);
		}

		/**
		 * Move caret one page down extending selection to new caret position.
		 */
		void PageDownExtend()
		{
			SPerform(SCI_PAGEDOWNEXTEND, 0, 0);
		}

		/**
		 * Switch from insert to overtype mode or the reverse.
		 */
		void EditToggleOvertype()
		{
			SPerform(SCI_EDITTOGGLEOVERTYPE, 0, 0);
		}

		/**
		 * Cancel any modes such as call tip or auto-completion list display.
		 */
		void Cancel()
		{
			SPerform(SCI_CANCEL, 0, 0);
		}

		/**
		 * Delete the selection or if no selection, the character before the caret.
		 */
		void DeleteBack()
		{
			SPerform(SCI_DELETEBACK, 0, 0);
		}

		/**
		 * If selection is empty or all on one line replace the selection with a tab character.
		 * If more than one line selected, indent the lines.
		 */
		void Tab()
		{
			SPerform(SCI_TAB, 0, 0);
		}

		/**
		 * Dedent the selected lines.
		 */
		void BackTab()
		{
			SPerform(SCI_BACKTAB, 0, 0);
		}

		/**
		 * Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
		 */
		void NewLine()
		{
			SPerform(SCI_NEWLINE, 0, 0);
		}

		/**
		 * Insert a Form Feed character.
		 */
		void FormFeed()
		{
			SPerform(SCI_FORMFEED, 0, 0);
		}

		/**
		 * Move caret to before first visible character on line.
		 * If already there move to first character on line.
		 */
		void VCHome()
		{
			SPerform(SCI_VCHOME, 0, 0);
		}

		/**
		 * Like VCHome but extending selection to new caret position.
		 */
		void VCHomeExtend()
		{
			SPerform(SCI_VCHOMEEXTEND, 0, 0);
		}

		/**
		 * Magnify the displayed text by increasing the sizes by 1 point.
		 */
		void ZoomIn()
		{
			SPerform(SCI_ZOOMIN, 0, 0);
		}

		/**
		 * Make the displayed text smaller by decreasing the sizes by 1 point.
		 */
		void ZoomOut()
		{
			SPerform(SCI_ZOOMOUT, 0, 0);
		}

		/**
		 * Delete the word to the left of the caret.
		 */
		void DelWordLeft()
		{
			SPerform(SCI_DELWORDLEFT, 0, 0);
		}

		/**
		 * Delete the word to the right of the caret.
		 */
		void DelWordRight()
		{
			SPerform(SCI_DELWORDRIGHT, 0, 0);
		}

		/**
		 * Cut the line containing the caret.
		 */
		void LineCut()
		{
			SPerform(SCI_LINECUT, 0, 0);
		}

		/**
		 * Delete the line containing the caret.
		 */
		void LineDelete()
		{
			SPerform(SCI_LINEDELETE, 0, 0);
		}

		/**
		 * Switch the current line with the previous.
		 */
		void LineTranspose()
		{
			SPerform(SCI_LINETRANSPOSE, 0, 0);
		}

		/**
		 * Duplicate the current line.
		 */
		void LineDuplicate()
		{
			SPerform(SCI_LINEDUPLICATE, 0, 0);
		}

		/**
		 * Transform the selection to lower case.
		 */
		void LowerCase()
		{
			SPerform(SCI_LOWERCASE, 0, 0);
		}

		/**
		 * Transform the selection to upper case.
		 */
		void UpperCase()
		{
			SPerform(SCI_UPPERCASE, 0, 0);
		}

		/**
		 * Scroll the document down, keeping the caret visible.
		 */
		void LineScrollDown()
		{
			SPerform(SCI_LINESCROLLDOWN, 0, 0);
		}

		/**
		 * Scroll the document up, keeping the caret visible.
		 */
		void LineScrollUp()
		{
			SPerform(SCI_LINESCROLLUP, 0, 0);
		}

		/**
		 * Delete the selection or if no selection, the character before the caret.
		 * Will not delete the character before at the start of a line.
		 */
		void DeleteBackNotLine()
		{
			SPerform(SCI_DELETEBACKNOTLINE, 0, 0);
		}

		/**
		 * Move caret to first position on display line.
		 */
		void HomeDisplay()
		{
			SPerform(SCI_HOMEDISPLAY, 0, 0);
		}

		/**
		 * Move caret to first position on display line extending selection to
		 * new caret position.
		 */
		void HomeDisplayExtend()
		{
			SPerform(SCI_HOMEDISPLAYEXTEND, 0, 0);
		}

		/**
		 * Move caret to last position on display line.
		 */
		void LineEndDisplay()
		{
			SPerform(SCI_LINEENDDISPLAY, 0, 0);
		}

		/**
		 * Move caret to last position on display line extending selection to new
		 * caret position.
		 */
		void LineEndDisplayExtend()
		{
			SPerform(SCI_LINEENDDISPLAYEXTEND, 0, 0);
		}

		/**
		 * Move the caret inside current view if it's not there already.
		 */
		void MoveCaretInsideView()
		{
			SPerform(SCI_MOVECARETINSIDEVIEW, 0, 0);
		}

		/**
		 * How many characters are on a line, not including end of line characters?
		 */
		int LineLength(int line)
		{
			return (int)SPerform(SCI_LINELENGTH, (long)line, 0);
		}

		/**
		 * Highlight the characters at two positions.
		 */
		void BraceHighlight(long pos1, long pos2)
		{
			SPerform(SCI_BRACEHIGHLIGHT, pos1, pos2);
		}

		/**
		 * Highlight the character at a position indicating there is no matching brace.
		 */
		void BraceBadLight(long pos)
		{
			SPerform(SCI_BRACEBADLIGHT, pos, 0);
		}

		/**
		 * Find the position of a matching brace or INVALID_POSITION if no match.
		 */
		long BraceMatch(long pos)
		{
			return SPerform(SCI_BRACEMATCH, pos, 0);
		}

		/**
		 * Are the end of line characters visible?
		 */
		bool GetViewEOL()
		{
			return SPerform(SCI_GETVIEWEOL, 0, 0) != 0;
		}

		/**
		 * Make the end of line characters visible or invisible.
		 */
		void SetViewEOL(bool visible)
		{
			SPerform(SCI_SETVIEWEOL, (long)visible, 0);
		}

		/**
		 * Retrieve a pointer to the document object.
		 */
		int GetDocPointer()
		{
			return (int)SPerform(SCI_GETDOCPOINTER, 0, 0);
		}

		/**
		 * Change the document object used.
		 */
		void SetDocPointer(int pointer)
		{
			SPerform(SCI_SETDOCPOINTER, 0, (long)pointer);
		}

		/**
		 * Set which document modification events are sent to the container.
		 */
		void SetModEventMask(int mask)
		{
			SPerform(SCI_SETMODEVENTMASK, (long)mask, 0);
		}

		/**
		 * Retrieve the column number which text should be kept within.
		 */
		int GetEdgeColumn()
		{
			return (int)SPerform(SCI_GETEDGECOLUMN, 0, 0);
		}

		/**
		 * Set the column number of the edge.
		 * If text goes past the edge then it is highlighted.
		 */
		void SetEdgeColumn(int column)
		{
			SPerform(SCI_SETEDGECOLUMN, (long)column, 0);
		}

		/**
		 * Retrieve the edge highlight mode.
		 */
		int GetEdgeMode()
		{
			return (int)SPerform(SCI_GETEDGEMODE, 0, 0);
		}

		/**
		 * The edge may be displayed by a line (EDGE_LINE) or by highlighting text that
		 * goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
		 */
		void SetEdgeMode(int mode)
		{
			SPerform(SCI_SETEDGEMODE, (long)mode, 0);
		}

		/**
		 * Retrieve the colour used in edge indication.
		 */
		COLORREF GetEdgeColour()
		{
			return (COLORREF)SPerform(SCI_GETEDGECOLOUR, 0, 0);
		}

		/**
		 * Change the colour used in edge indication.
		 */
		void SetEdgeColour(COLORREF edgeColour)
		{
			SPerform(SCI_SETEDGECOLOUR, (long)edgeColour, 0);
		}

		/**
		 * Sets the current caret position to be the search anchor.
		 */
		void SearchAnchor()
		{
			SPerform(SCI_SEARCHANCHOR, 0, 0);
		}

		/**
		 * Find some text starting at the search anchor.
		 * Does not ensure the selection is visible.
		 */
		int SearchNext(int flags, const char* text)
		{
			return (int)SPerform(SCI_SEARCHNEXT, (long)flags, (long)text);
		}

		/**
		 * Find some text starting at the search anchor and moving backwards.
		 * Does not ensure the selection is visible.
		 */
		int SearchPrev(int flags, const char* text)
		{
			return (int)SPerform(SCI_SEARCHPREV, (long)flags, (long)text);
		}

		/**
		 * Retrieves the number of lines completely visible.
		 */
		int LinesOnScreen()
		{
			return (int)SPerform(SCI_LINESONSCREEN, 0, 0);
		}

		/**
		 * Set whether a pop up menu is displayed automatically when the user presses
		 * the wrong mouse button.
		 */
		void UsePopUp(bool allowPopUp)
		{
			SPerform(SCI_USEPOPUP, (long)allowPopUp, 0);
		}

		/**
		 * Is the selection rectangular? The alternative is the more common stream selection.
		 */
		bool SelectionIsRectangle()
		{
			return SPerform(SCI_SELECTIONISRECTANGLE, 0, 0) != 0;
		}

		/**
		 * Set the zoom level. This number of points is added to the size of all fonts.
		 * It may be positive to magnify or negative to reduce.
		 */
		void SetZoom(int zoom)
		{
			SPerform(SCI_SETZOOM, (long)zoom, 0);
		}

		/**
		 * Retrieve the zoom level.
		 */
		int GetZoom()
		{
			return (int)SPerform(SCI_GETZOOM, 0, 0);
		}

		/**
		 * Create a new document object.
		 * Starts with reference count of 1 and not selected into editor.
		 */
		int CreateDocument()
		{
			return (int)SPerform(SCI_CREATEDOCUMENT, 0, 0);
		}

		/**
		 * Extend life of document.
		 */
		void AddRefDocument(int doc)
		{
			SPerform(SCI_ADDREFDOCUMENT, 0, (long)doc);
		}

		/**
		 * Release a reference to the document, deleting document if it fades to black.
		 */
		void ReleaseDocument(int doc)
		{
			SPerform(SCI_RELEASEDOCUMENT, 0, (long)doc);
		}

		/**
		 * Get which document modification events are sent to the container.
		 */
		int GetModEventMask()
		{
			return (int)SPerform(SCI_GETMODEVENTMASK, 0, 0);
		}

		/**
		 * Change internal focus flag.
		 */
		void SetFocus(bool focus)
		{
			SPerform(SCI_SETFOCUS, (long)focus, 0);
		}

		/**
		 * Get internal focus flag.
		 */
		bool GetFocus()
		{
			return SPerform(SCI_GETFOCUS, 0, 0) != 0;
		}

		/**
		 * Change error status - 0 = OK.
		 */
		void SetStatus(int statusCode)
		{
			SPerform(SCI_SETSTATUS, (long)statusCode, 0);
		}

		/**
		 * Get error status.
		 */
		int GetStatus()
		{
			return (int)SPerform(SCI_GETSTATUS, 0, 0);
		}

		/**
		 * Set whether the mouse is captured when its button is pressed.
		 */
		void SetMouseDownCaptures(bool captures)
		{
			SPerform(SCI_SETMOUSEDOWNCAPTURES, (long)captures, 0);
		}

		/**
		 * Get whether mouse gets captured.
		 */
		bool GetMouseDownCaptures()
		{
			return SPerform(SCI_GETMOUSEDOWNCAPTURES, 0, 0) != 0;
		}

		/**
		 * Sets the cursor to one of the SC_CURSOR* values.
		 */
		void SetCursor(int cursorType)
		{
			SPerform(SCI_SETCURSOR, (long)cursorType, 0);
		}

		/**
		 * Get cursor type.
		 */
		int GetCursor()
		{
			return (int)SPerform(SCI_GETCURSOR, 0, 0);
		}

		/**
		 * Change the way control characters are displayed:
		 * If symbol is < 32, keep the drawn way, else, use the given character.
		 */
		void SetControlCharSymbol(int symbol)
		{
			SPerform(SCI_SETCONTROLCHARSYMBOL, (long)symbol, 0);
		}

		/**
		 * Get the way control characters are displayed.
		 */
		int GetControlCharSymbol()
		{
			return (int)SPerform(SCI_GETCONTROLCHARSYMBOL, 0, 0);
		}

		/**
		 * Move to the previous change in capitalisation.
		 */
		void WordPartLeft()
		{
			SPerform(SCI_WORDPARTLEFT, 0, 0);
		}

		/**
		 * Move to the previous change in capitalisation extending selection
		 * to new caret position.
		 */
		void WordPartLeftExtend()
		{
			SPerform(SCI_WORDPARTLEFTEXTEND, 0, 0);
		}

		/**
		 * Move to the change next in capitalisation.
		 */
		void WordPartRight()
		{
			SPerform(SCI_WORDPARTRIGHT, 0, 0);
		}

		/**
		 * Move to the next change in capitalisation extending selection
		 * to new caret position.
		 */
		void WordPartRightExtend()
		{
			SPerform(SCI_WORDPARTRIGHTEXTEND, 0, 0);
		}

		/**
		 * Set the way the display area is determined when a particular line
		 * is to be moved to by Find, FindNext, GotoLine, etc.
		 */
		void SetVisiblePolicy(int visiblePolicy, int visibleSlop)
		{
			SPerform(SCI_SETVISIBLEPOLICY, (long)visiblePolicy, (long)visibleSlop);
		}

		/**
		 * Delete back from the current position to the start of the line.
		 */
		void DelLineLeft()
		{
			SPerform(SCI_DELLINELEFT, 0, 0);
		}

		/**
		 * Delete forwards from the current position to the end of the line.
		 */
		void DelLineRight()
		{
			SPerform(SCI_DELLINERIGHT, 0, 0);
		}

		/**
		 * Get and Set the xOffset (ie, horizonal scroll position).
		 */
		void SetXOffset(int newOffset)
		{
			SPerform(SCI_SETXOFFSET, (long)newOffset, 0);
		}

		/**
		 * Get and Set the xOffset (ie, horizonal scroll position).
		 */
		int GetXOffset()
		{
			return (int)SPerform(SCI_GETXOFFSET, 0, 0);
		}

		/**
		 * Set the last x chosen value to be the caret x position
		 */
		void ChooseCaretX()
		{
			SPerform(SCI_CHOOSECARETX, 0, 0);
		}

		/**
		 * Set the focus to this Scintilla widget.
		 * GTK+ Specific.
		 */
		void GrabFocus()
		{
			SPerform(SCI_GRABFOCUS, 0, 0);
		}

		/**
		 * Set the way the caret is kept visible when going sideway.
		 * The exclusion zone is given in pixels.
		 */
		void SetXCaretPolicy(int caretPolicy, int caretSlop)
		{
			SPerform(SCI_SETXCARETPOLICY, (long)caretPolicy, (long)caretSlop);
		}

		/**
		 * Set the way the line the caret is on is kept visible.
		 * The exclusion zone is given in lines.
		 */
		void SetYCaretPolicy(int caretPolicy, int caretSlop)
		{
			SPerform(SCI_SETYCARETPOLICY, (long)caretPolicy, (long)caretSlop);
		}

		/**
		 * Start notifying the container of all key presses and commands.
		 */
		void StartRecord()
		{
			SPerform(SCI_STARTRECORD, 0, 0);
		}

		/**
		 * Stop notifying the container of all key presses and commands.
		 */
		void StopRecord()
		{
			SPerform(SCI_STOPRECORD, 0, 0);
		}

		/**
		 * Set the lexing language of the document.
		 */
		void SetLexer(int lexer)
		{
			SPerform(SCI_SETLEXER, (long)lexer, 0);
		}

		/**
		 * Retrieve the lexing language of the document.
		 */
		int GetLexer()
		{
			return (int)SPerform(SCI_GETLEXER, 0, 0);
		}

		/**
		 * Colourise a segment of the document using the current lexing language.
		 */
		void Colourise(long start, long end)
		{
			SPerform(SCI_COLOURISE, start, end);
		}

		/**
		 * Set up a value that may be used by a lexer for some optional feature.
		 */
		void SetProperty(const char* key, const char* value)
		{
			SPerform(SCI_SETPROPERTY, (long)key, (long)value);
		}

		/**
		 * Set up the key words used by the lexer.
		 */
		void SetKeyWords(int keywordSet, const char* keyWords)
		{
			SPerform(SCI_SETKEYWORDS, (long)keywordSet, (long)keyWords);
		}

		/**
		 * Set the lexing language of the document based on string name.
		 */
		void SetLexerLanguage(const char* language)
		{
			SPerform(SCI_SETLEXERLANGUAGE, 0, (long)language);
		}

	//--
	//@}
#endif //_WTL_NOSCINTILLAMETHODS
};

// Static variable initialisation...
__declspec( selectany ) HMODULE ScintillaIFBase::scidll = NULL;
__declspec( selectany ) int ScintillaIFBase::refs = 0;

template <class TBase = CWindow>
class CScintillaT : public TBase, 
	public CScintillaIF<CScintillaT>
{
	typedef CScintillaT<TBase> thisClass;
	
	public:
		static LPCTSTR GetWndClassName()
		{
			return _T("Scintilla");
		}
	
		thisClass& operator = (HWND hWnd)
		{
			m_hWnd = hWnd;
			return *this;
		}
		
		HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
		{
			HWND hWnd = CWindow::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
			
			if(hWnd)
			{
				m_Pointer = (void *)SPerform(SCI_GETDIRECTPOINTER);
				Perform = (scmsgfn)SPerform(SCI_GETDIRECTFUNCTION);
		
				SetTabWidth(m_TabWidth);
			}
			
			return hWnd;
		}
};

typedef CScintillaT<CWindow> CScintilla;

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CScintillaWindowImpl : public CWindowImplBaseT<CScintillaT<TBase>, TWinTraits>
{
	DECLARE_WND_SUPERCLASS(NULL, CScintilla::GetWndClassName())
	
	typedef CScintillaWindowImpl<T, TBase, TWinTraits> thisClass;

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(OCM_NOTIFY, OnNotify)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		
		if(IsScintillaNotify(lParam))
		{
			pT->HandleNotify(lParam);
			bHandled = TRUE;
		}
		else
			bHandled = FALSE;

		return 0;
	}
	
	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		if (T::GetWndClassInfo().m_lpszOrigName == NULL)
			T::GetWndClassInfo().m_lpszOrigName = GetWndClassName();
		ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

		dwStyle = T::GetWndStyle(dwStyle);
		dwExStyle = T::GetWndExStyle(dwExStyle);

		return CWindowImplBaseT<CScintillaT<TBase>, TWinTraits>::Create(hWndParent, 
			rcPos, szWindowName, dwStyle, dwExStyle, nID, atom, lpCreateParam);
	}
};

#endif