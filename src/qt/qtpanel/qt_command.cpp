#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>

#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QApplication>
#include <QFontDialog>

#include "selection.h"
#include "strutil.h"

#include "qt_dialog.h"
#include "qt_panel.h"
#include "qt_mcd.h"
#include "qt_command.h"
#include "qt_mainwindow.h"

using namespace MLS;
using namespace MLSUTIL;

PanelCmd::PanelCmd(QObject * parent )
	: QObject ( parent )
{
	_pPanel = 0;
	_pMain = 0;
	_bClipCut = false;
}

void	PanelCmd::SetMain( Qt_MainWindow* pMain )
{
	_pMain = pMain;
}

void	PanelCmd::Empty()
{
	qDebug("PanelCmd::Cmd_Empty()");
}

void	PanelCmd::GetMainPanel()
{
	if ( _pMain )
		_pPanel = _pMain->GetFocusPanel();
}

void	PanelCmd::syncMcdAndRefresh()
{
	Qt_Mcd* pMcd = _pMain->GetFocusMcd();
	pMcd->setDir( _pPanel->GetPath() );
	_pPanel->TabLabelChg();
	Refresh();
}

void	PanelCmd::Refresh()
{
	_pPanel = _pMain->GetFocusPanel();
	_pMain->Refresh();
	_pPanel->setFocus();
}

void	PanelCmd::Left()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Left();
}

void	PanelCmd::Right()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Right();
}

void	PanelCmd::Up()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Up();
}

void	PanelCmd::Down()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Down();
}

void	PanelCmd::PgDn()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_PageDown();
}

void	PanelCmd::PgUp()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_PageUp();
}

void	PanelCmd::End()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_End();
}

void	PanelCmd::Home()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Home();
}

void	PanelCmd::Enter()
{
	_pPanel = _pMain->GetFocusPanel();

	const string& strBefInitType = _pPanel->GetReader()->GetInitType();

	_pPanel->MLS::Panel::Key_Enter();

	Qt_Mcd* pMcd = _pMain->GetFocusMcd();

	if ( strBefInitType != _pPanel->GetReader()->GetInitType() )
		pMcd->InitMcd( _pPanel->GetReader(), "/" );

	pMcd->setDir( _pPanel->GetPath() );
	_pPanel->TabLabelChg();
}

void	PanelCmd::Select()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Select();
}

bool	PanelCmd::Quit()
{
	_pPanel = _pMain->GetFocusPanel();

	if ( _pPanel->GetReader()->GetReaderName() == "sftp" ||
		 _pPanel->GetReader()->GetReaderName() == "ftp")
	{
		RemoteClose();
		return false;
	}

	_pMain->close();
	return true;
}

void	PanelCmd::Split()
{
	_pMain->Split();
}

void PanelCmd::RemoteConnect()
{
	_pPanel = _pMain->GetFocusPanel();

	if (_pPanel->GetReader()->GetReaderName() != "file" )
	{
		MsgBox(_("Error"), _("current remote connected."));
		return;
	}

	string sConnectionInfo;

	sConnectionInfo = MLS::g_tCfg.GetValue("Default", "LastRemoteConnected");

	if (InputBox(	_("Input sftp(ftp) connect url (sftp://user:pswd@hostname)"),
							sConnectionInfo) == ERROR) return;

	if (sConnectionInfo.size() > 6)
	{
		if (sConnectionInfo.substr(0, 6) == "ftp://")
		{
			sConnectionInfo = sConnectionInfo.substr(6);
			_pPanel->PluginOpen(sConnectionInfo, "ftp");
		}
		else if (sConnectionInfo.substr(0, 7) == "sftp://")
		{
			sConnectionInfo = sConnectionInfo.substr(7);
			_pPanel->PluginOpen(sConnectionInfo, "sftp");
		}
		else
		{
			_pPanel->PluginOpen(sConnectionInfo, "sftp");
		}
	}
	else
		MsgBox(_("Error"), _("input url invalid format."));

	if (_pPanel->GetReader()->GetReaderName() == "ftp" ||
		_pPanel->GetReader()->GetReaderName() == "sftp")
	{
		MLS::g_tCfg.SetValue(	"Default",
							"LastRemoteConnected",
							_pPanel->GetReader()->GetInitType());
	}

	Refresh();
}

void PanelCmd::RemoteClose()
{
	_pPanel = _pMain->GetFocusPanel();

	if (MLS::g_tCfg.GetBool("Default", "AskRemoteExit") == true)
	{
		bool bYN = YNBox(_("Remote Close"), _("Do you really want to remote connection close ?"), true);
		if (bYN == false) return;
	}

	_pPanel->PluginClose("file");
	Refresh();
}

void PanelCmd::Archive()
{
	_pPanel = _pMain->GetFocusPanel();

	MLS::File* pFile = _pPanel->GetCurFile();

	qDebug("ArchiveFileView [%s]", pFile->sFullName.c_str());

	if (pFile->sType.substr(0, 7) == "archive")
	{
		MsgBox(_("Error"), "Archive file view failure !!!");
	}
	else
	{
		if (_pPanel->GetReader()->GetReaderName() != "file")
			_pPanel->PluginOpen(MLS::g_tCfg.GetValue("Static", "TmpDir") + pFile->sName, "archive");
		else
			_pPanel->PluginOpen(pFile->sFullName, "archive");
	}
}

///	@brief	명령어를 실행한다.
///	@param	cmd		명령어및 인자
///	@param	bPause	명령어를 실행시키고 종료시 잠시 정지시킬지 여부
/// @param	bFork	Fork할 것인지?
///	@return	명령어 실행 결과
int 	PanelCmd::Run( const std::string &cmd, bool bPause, bool bBackground)
{
	_pPanel = _pMain->GetFocusPanel();

	// command 창에서 exit, quit 를 치면 종료하게 한다.
	if (cmd == "exit" || cmd == "quit")
	{
		Quit();
		return 0;
	}

	int status;

	if (_pPanel->GetReader()->GetReaderName() == "file")
	{
		chdir(_pPanel->GetReader()->GetPath().c_str());
	}

	if (bBackground)
	{
		string sCmd = cmd + " > /dev/null 2>&1 &";
		status = system(sCmd.c_str());
	}
	else
	{
		string sConsoleName = MLS::g_tCfg.GetValue("Qt_Setting", "UseConsole", "konsole");
		sConsoleName = sConsoleName + " " + cmd;
		status = system( sConsoleName.c_str() );
	}

	if (bPause)
	{

	}

	(void)status;
	Refresh();
	return 0;
}

///	@brief	모든 커맨드 메시지를 관리
///	@param	p	실행 시킬 명령어
///	@return		명령어 실행 결과
int 	PanelCmd::ParseAndRun(const string &p, bool Pause)
{
	bool	bFlag  = false,
		 	bPause = Pause,
		 	bParam = false,
		 	bMcd   = false ,
		 	bConfirm = false,
			bBackground = false,
			bRootExe = false;

	_pPanel = _pMain->GetFocusPanel();

	// cd 구현
	if (p.substr(0, 2) == "cd")
	{
		string sStr;
		if (p == "cd" || p == "cd ")
			sStr = _pPanel->GetReader()->GetRealPath("~");
		else
			sStr = _pPanel->GetReader()->GetRealPath(p.substr(3));
		sStr = Replace(sStr, "\\", "");
		if (sStr != "")
		{
			if (_pPanel->Read(sStr) == true)
			{
				return 1;
			}
		}
		return 0;
	}

	string 	arg, tmp_name;

	MLS::File	tFile;

	// 파일을 복사 해서 보여주기 위한.
	{
		MLS::File* 	pFile = _pPanel->GetCurFile();
		if (_pPanel->GetReader()->View(pFile, &tFile) == false)
		{
			return 0;
		}
	}

	// 파싱
	for (string::const_iterator i = p.begin(); i!= p.end(); i++)
	{
		if (*i == '%')
		{
			bFlag = true;
			continue;
		}

		if (bFlag)
		{
			switch(*i)
			{
			// 파일을 리턴해줌
			case '1':
			case 'F':
				arg += addslash(tFile.sFullName);
				break;

			case 'N':
			{
				if (tFile.sName[0]=='.') return 0;

				string::size_type p = tFile.sName.rfind('.');

				if (p != string::npos)
					arg += addslash(tFile.sFullName.substr(0, p-1));
				else
					arg += addslash(tFile.sFullName);
				break;
			}

			case 'E':
				if (tFile.Ext().empty()) return 0;
				arg += addslash(tFile.Ext());
				break;

			case 'W':
				bPause = true;
				break;

			case '%':
				arg += '%';
				break;

			case 'S':
			{
				MLS::Selection tSelection;
				_pPanel->GetSelection(tSelection);

				if (tSelection.GetSize() == 0) return 0;
				bool first = true;

				for (int t = 0; t< (int)tSelection.GetSize(); t++)
				{
					if (first) first= false;
					else arg += ' ';
					arg += addslash(tSelection[t]->sFullName);
				}
				break;
			}

			case 's':
			{
				MLS::Selection tSelection;
				_pPanel->GetSelection(tSelection);

				if (!tSelection.GetSize() == 0)
				{
					bool first = true;

					for (int t = 0; t< (int)tSelection.GetSize(); t++)
					{
						if (first) first= false;
						else arg += ' ';

						arg += addslash(tSelection[t]->sFullName);
					}
				}
				else
				{
					if (tFile.sName == "..") return 0;
					arg += addslash(tFile.sFullName);
				}
				break;
			}

			case 'd':
				arg += addslash(_pPanel->GetPath());
				break;

			case 'D':
				bMcd = true;
				break;

			case 'P':
				bParam= true;
				break;

			case 'R': // root 계정 실행시
				bRootExe = true;
				break;

			case 'H':
				arg += addslash(_pPanel->GetReader()->GetRealPath("~"));
				break;

			case 'Q':
				bConfirm = true;
				break;

			case 'B':
				bBackground = true;
				break;

			default:
				break;
			}

			bFlag = false;
			continue;
		}
		else arg += *i;
	}

	if (bMcd)
	{
		/*
		Mcd mcd(_tPanel);
		mcd.setCur(_tPanel->GetCurrentPath());
		if (!mcd.proc())
            return 0;
		arg += addslash(mcd.getCur());
		*/
	}

	if (bParam)	if (InputBox(_("Edit Parameter"), arg)<0) return 0;

	if (bConfirm)
		if (YNBox(_("Question"), _("Do you want to run this operation?"), false) != true) return 0;

	if (bRootExe)
		arg = "su - --command=\"" + arg + "\"";

	// 실행
	int nRt = Run(arg, bPause, bBackground);
	(void)nRt;
	return 0;
}

void PanelCmd::NextWindow()
{
	if ( _pMain->isSplit() )
		_pMain->NextFocus();
}

// ============================================================
// Group A: Navigation
// ============================================================

void PanelCmd::GoParent()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->Read("..");
	syncMcdAndRefresh();
}

void PanelCmd::GoRoot()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->Read("/");
	syncMcdAndRefresh();
}

void PanelCmd::GoHome()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->Read("~");
	syncMcdAndRefresh();
}

void PanelCmd::Back()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->MLS::Panel::Back();
	syncMcdAndRefresh();
}

void PanelCmd::Forward()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->MLS::Panel::Forward();
	syncMcdAndRefresh();
}

// ============================================================
// Group A: Selection
// ============================================================

void PanelCmd::SelectAll()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->MLS::Panel::SelectAll();
	Refresh();
}

void PanelCmd::SelectInvert()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->MLS::Panel::SelectInvert();
	Refresh();
}

// ============================================================
// Group A: Sort
// ============================================================

void PanelCmd::SortChange()
{
	_pPanel = _pMain->GetFocusPanel();
	int n = _pPanel->GetSortMethod() + 1;
	if (n >= SORT_END_) n = 0;
	_pPanel->SetSortMethod(n);
	_pPanel->MLS::Panel::Sort();
	Refresh();
}

void PanelCmd::SortAscDescend()
{
	_pPanel = _pMain->GetFocusPanel();
	if (_pPanel->GetSortMethod() != SORT_NONE)
	{
		_pPanel->_bFileSortAscend = !_pPanel->_bFileSortAscend;
		_pPanel->MLS::Panel::Sort();
		Refresh();
	}
}

// ============================================================
// Group A: View settings
// ============================================================

void PanelCmd::HiddenFileView()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->SetHiddenView();
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::ColumnAuto()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->SetViewColumn(0);
	Refresh();
}

void PanelCmd::Column1()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->SetViewColumn(1);
	Refresh();
}

void PanelCmd::Column2()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->SetViewColumn(2);
	Refresh();
}

void PanelCmd::Column3()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->SetViewColumn(3);
	Refresh();
}

void PanelCmd::Column4()
{
	_pPanel = _pMain->GetFocusPanel();
	_pPanel->SetViewColumn(4);
	Refresh();
}

// ============================================================
// Group B: InputDialog + Reader
// ============================================================

void PanelCmd::Mkdir()
{
	_pPanel = _pMain->GetFocusPanel();
	string sDir;
	if (InputBox(_("Create Directory"), sDir) == SUCCESS && !sDir.empty())
	{
		_pPanel->GetReader()->Mkdir(sDir);
	}
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::Rename()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::File* pFile = _pPanel->GetCurFile();
	if (!pFile) return;
	string sNewName = pFile->sName;
	string sTitle = string(_("Rename")) + " - " + pFile->sName;
	if (InputBox(sTitle, sNewName) == SUCCESS && !sNewName.empty())
	{
		_pPanel->GetReader()->Rename(pFile, sNewName);
	}
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::NewFile()
{
	_pPanel = _pMain->GetFocusPanel();
	string sName;
	if (InputBox(_("New File Name"), sName) == SUCCESS && !sName.empty())
	{
		string sPath = _pPanel->GetPath() + "/" + sName;
		FILE* fp = fopen(sPath.c_str(), "w");
		if (fp) fclose(fp);
	}
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::TouchFile()
{
	_pPanel = _pMain->GetFocusPanel();
	string sName;
	if (InputBox(_("Touch File Name"), sName) == SUCCESS && !sName.empty())
	{
		string sPath = _pPanel->GetPath() + "/" + sName;
		FILE* fp = fopen(sPath.c_str(), "a");
		if (fp) fclose(fp);
	}
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::Remove()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::Selection tSel;
	_pPanel->GetSelection(tSel);
	if (tSel.GetSize() == 0) return;

	string sMsg;
	vector<MLS::File*>& vFiles = tSel.GetData();
	if (vFiles.size() == 1)
		sMsg = string(_("Delete")) + " [" + vFiles[0]->sName + "] ?";
	else
		sMsg = string(_("Delete")) + " [" + vFiles[0]->sName + "] "
			+ _("and") + " " + to_string(vFiles.size() - 1) + " "
			+ _("other file(s)") + " ?";

	if (!YNBox(_("Delete"), sMsg, false)) return;

	tSel.ExpandDir(_pPanel->GetReader(), false);
	_pPanel->GetReader()->Remove(tSel);
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::Execute()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::File* pFile = _pPanel->GetCurFile();
	if (pFile)
		_pPanel->SelectExecute();
	Refresh();
}

// ============================================================
// Group C: Clipboard
// ============================================================

void PanelCmd::ClipCopy()
{
	_pPanel = _pMain->GetFocusPanel();
	_clipboard.Clear();
	_pPanel->GetSelection(_clipboard);
	_clipboard.SetSelectPath(_pPanel->GetPath());
	_clipboard.SetReader(_pPanel->GetReader());
	_bClipCut = false;
}

void PanelCmd::ClipCut()
{
	_pPanel = _pMain->GetFocusPanel();
	_clipboard.Clear();
	_pPanel->GetSelection(_clipboard);
	_clipboard.SetSelectPath(_pPanel->GetPath());
	_clipboard.SetReader(_pPanel->GetReader());
	_bClipCut = true;
}

void PanelCmd::ClipPaste()
{
	_pPanel = _pMain->GetFocusPanel();
	if (_clipboard.GetSize() == 0) return;

	string sTargetPath = _pPanel->GetPath();
	if (_bClipCut)
	{
		_clipboard.GetReader()->Move(_clipboard, sTargetPath);
		_clipboard.Clear();
		_bClipCut = false;
	}
	else
	{
		_clipboard.GetReader()->Copy(_clipboard, sTargetPath);
	}
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::Copy()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::Selection tSel;
	_pPanel->GetSelection(tSel);
	if (tSel.GetSize() == 0) return;

	vector<MLS::File*>& vFiles = tSel.GetData();
	string sTitle;
	if (vFiles.size() == 1)
		sTitle = string(_("Copy")) + " [" + vFiles[0]->sName + "] " + _("to");
	else
		sTitle = string(_("Copy")) + " [" + vFiles[0]->sName + "] "
			+ _("and") + " " + to_string(vFiles.size() - 1) + " "
			+ _("other file(s)") + " " + _("to");

	tSel.ExpandDir(_pPanel->GetReader(), false);
	string sTarget = _pPanel->GetPath();
	if (InputBox(sTitle, sTarget) == ERROR) return;
	_pPanel->GetReader()->Copy(tSel, sTarget);
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::Move()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::Selection tSel;
	_pPanel->GetSelection(tSel);
	if (tSel.GetSize() == 0) return;

	vector<MLS::File*>& vFiles = tSel.GetData();
	string sTitle;
	if (vFiles.size() == 1)
		sTitle = string(_("Move")) + " [" + vFiles[0]->sName + "] " + _("to");
	else
		sTitle = string(_("Move")) + " [" + vFiles[0]->sName + "] "
			+ _("and") + " " + to_string(vFiles.size() - 1) + " "
			+ _("other file(s)") + " " + _("to");

	tSel.ExpandDir(_pPanel->GetReader(), false);
	string sTarget = _pPanel->GetPath();
	if (InputBox(sTitle, sTarget) == ERROR) return;
	_pPanel->GetReader()->Move(tSel, sTarget);
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

// ============================================================
// Group D: Archive (Extract / Compress)
// ============================================================

void PanelCmd::Extract()
{
	_pPanel = _pMain->GetFocusPanel();

	if (_pPanel->GetReader()->GetReaderName() == "archive")
	{
		MsgBox(_("Error"), _("Don't touch file in archive."));
		return;
	}

	MLS::File* pFile = _pPanel->GetCurFile();
	if (!pFile) return;

	string sExt = pFile->Ext();
	string sFullName = addslash(pFile->sFullName);
	string sCmd;

	if (sExt == "gz" || sExt == "tgz")
		sCmd = "tar xzf " + sFullName;
	else if (sExt == "bz2")
		sCmd = "tar xjf " + sFullName;
	else if (sExt == "xz" || sExt == "txz")
		sCmd = "tar xJf " + sFullName;
	else if (sExt == "zip")
		sCmd = "unzip -o " + sFullName;
	else if (sExt == "tar")
		sCmd = "tar xf " + sFullName;
	else
	{
		// fallback: try tar xf
		sCmd = "tar xf " + sFullName;
	}

	if (_pPanel->GetReader()->GetReaderName() == "file")
		chdir(_pPanel->GetReader()->GetPath().c_str());

	system(sCmd.c_str());
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::TargzComp()
{
	_pPanel = _pMain->GetFocusPanel();

	if (_pPanel->GetReader()->GetReaderName() != "file")
	{
		MsgBox(_("Error"), _("local file system use only."));
		return;
	}

	MLS::Selection tSel;
	_pPanel->GetSelection(tSel);
	if (tSel.GetSize() == 0) return;

	string sFileName;
	if (InputBox(_("Archive name (.tar.gz)"), sFileName) == ERROR || sFileName.empty()) return;
	sFileName += ".tar.gz";

	string sCmd = "tar czf " + addslash(sFileName);
	for (uint i = 0; i < tSel.GetSize(); i++)
		sCmd += " " + addslash(tSel[i]->sName);

	chdir(_pPanel->GetReader()->GetPath().c_str());
	system(sCmd.c_str());
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::Tarbz2Comp()
{
	_pPanel = _pMain->GetFocusPanel();

	if (_pPanel->GetReader()->GetReaderName() != "file")
	{
		MsgBox(_("Error"), _("local file system use only."));
		return;
	}

	MLS::Selection tSel;
	_pPanel->GetSelection(tSel);
	if (tSel.GetSize() == 0) return;

	string sFileName;
	if (InputBox(_("Archive name (.tar.bz2)"), sFileName) == ERROR || sFileName.empty()) return;
	sFileName += ".tar.bz2";

	string sCmd = "tar cjf " + addslash(sFileName);
	for (uint i = 0; i < tSel.GetSize(); i++)
		sCmd += " " + addslash(tSel[i]->sName);

	chdir(_pPanel->GetReader()->GetPath().c_str());
	system(sCmd.c_str());
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::ZipComp()
{
	_pPanel = _pMain->GetFocusPanel();

	if (_pPanel->GetReader()->GetReaderName() != "file")
	{
		MsgBox(_("Error"), _("local file system use only."));
		return;
	}

	MLS::Selection tSel;
	_pPanel->GetSelection(tSel);
	if (tSel.GetSize() == 0) return;

	string sFileName;
	if (InputBox(_("Archive name (.zip)"), sFileName) == ERROR || sFileName.empty()) return;
	sFileName += ".zip";

	string sCmd = "zip -r " + addslash(sFileName);
	for (uint i = 0; i < tSel.GetSize(); i++)
		sCmd += " " + addslash(tSel[i]->sName);

	chdir(_pPanel->GetReader()->GetPath().c_str());
	system(sCmd.c_str());
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

// ============================================================
// Group E: Info / Dialogs
// ============================================================

void PanelCmd::About()
{
	MsgBox("LinM", string("LinM File Manager\nVersion ") + VERSION + "\n(C) la9527");
}

void PanelCmd::Help()
{
	MsgBox(_("Help"), _("F1:Help  F3:View  F4:Edit  F5:Copy  F6:Move  F7:Mkdir  F8:Delete"));
}

void PanelCmd::FileInfo()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::File* pFile = _pPanel->GetCurFile();
	if (!pFile) return;

	string sInfo = pFile->sFullName
		+ "\nSize: " + toregular(pFile->uSize)
		+ "\nDate: " + pFile->sDate + " " + pFile->sTime
		+ "\nAttr: " + pFile->sAttr;
	MsgBox(_("File Info"), sInfo);
}

// ── ChmodDialog ──────────────────────────────────────────────
class ChmodDialog : public QDialog
{
public:
	ChmodDialog(mode_t currentMode, const QString& fileName, QWidget* parent)
		: QDialog(parent)
	{
		setWindowTitle(tr("Chmod - %1").arg(fileName));

		// Owner group
		QGroupBox* grpOwner = new QGroupBox(tr("Owner"));
		_chkOwnerRead  = new QCheckBox(tr("Read"));
		_chkOwnerWrite = new QCheckBox(tr("Write"));
		_chkOwnerExec  = new QCheckBox(tr("Execute"));
		QVBoxLayout* layOwner = new QVBoxLayout;
		layOwner->addWidget(_chkOwnerRead);
		layOwner->addWidget(_chkOwnerWrite);
		layOwner->addWidget(_chkOwnerExec);
		grpOwner->setLayout(layOwner);

		// Group group
		QGroupBox* grpGroup = new QGroupBox(tr("Group"));
		_chkGroupRead  = new QCheckBox(tr("Read"));
		_chkGroupWrite = new QCheckBox(tr("Write"));
		_chkGroupExec  = new QCheckBox(tr("Execute"));
		QVBoxLayout* layGroup = new QVBoxLayout;
		layGroup->addWidget(_chkGroupRead);
		layGroup->addWidget(_chkGroupWrite);
		layGroup->addWidget(_chkGroupExec);
		grpGroup->setLayout(layGroup);

		// Other group
		QGroupBox* grpOther = new QGroupBox(tr("Other"));
		_chkOtherRead  = new QCheckBox(tr("Read"));
		_chkOtherWrite = new QCheckBox(tr("Write"));
		_chkOtherExec  = new QCheckBox(tr("Execute"));
		QVBoxLayout* layOther = new QVBoxLayout;
		layOther->addWidget(_chkOtherRead);
		layOther->addWidget(_chkOtherWrite);
		layOther->addWidget(_chkOtherExec);
		grpOther->setLayout(layOther);

		// Groups row
		QHBoxLayout* layGroups = new QHBoxLayout;
		layGroups->addWidget(grpOwner);
		layGroups->addWidget(grpGroup);
		layGroups->addWidget(grpOther);

		// Permission label
		_lblPermission = new QLabel;

		// Button box
		QDialogButtonBox* btnBox = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

		// Main layout
		QVBoxLayout* layMain = new QVBoxLayout;
		layMain->addLayout(layGroups);
		layMain->addWidget(_lblPermission);
		layMain->addWidget(btnBox);
		setLayout(layMain);

		// Set initial checkbox state from currentMode
		_chkOwnerRead->setChecked(currentMode & S_IRUSR);
		_chkOwnerWrite->setChecked(currentMode & S_IWUSR);
		_chkOwnerExec->setChecked(currentMode & S_IXUSR);
		_chkGroupRead->setChecked(currentMode & S_IRGRP);
		_chkGroupWrite->setChecked(currentMode & S_IWGRP);
		_chkGroupExec->setChecked(currentMode & S_IXGRP);
		_chkOtherRead->setChecked(currentMode & S_IROTH);
		_chkOtherWrite->setChecked(currentMode & S_IWOTH);
		_chkOtherExec->setChecked(currentMode & S_IXOTH);

		// Connect all checkboxes to update label via lambda
		QCheckBox* allChk[] = {
			_chkOwnerRead, _chkOwnerWrite, _chkOwnerExec,
			_chkGroupRead, _chkGroupWrite, _chkGroupExec,
			_chkOtherRead, _chkOtherWrite, _chkOtherExec
		};
		for (QCheckBox* chk : allChk)
			connect(chk, &QCheckBox::toggled, [this](bool) { updatePermissionLabel(); });

		updatePermissionLabel();
	}

	mode_t getMode() const
	{
		mode_t m = 0;
		if (_chkOwnerRead->isChecked())  m |= S_IRUSR;
		if (_chkOwnerWrite->isChecked()) m |= S_IWUSR;
		if (_chkOwnerExec->isChecked())  m |= S_IXUSR;
		if (_chkGroupRead->isChecked())  m |= S_IRGRP;
		if (_chkGroupWrite->isChecked()) m |= S_IWGRP;
		if (_chkGroupExec->isChecked())  m |= S_IXGRP;
		if (_chkOtherRead->isChecked())  m |= S_IROTH;
		if (_chkOtherWrite->isChecked()) m |= S_IWOTH;
		if (_chkOtherExec->isChecked())  m |= S_IXOTH;
		return m;
	}

private:
	void updatePermissionLabel()
	{
		mode_t m = getMode();

		// Build rwx string
		char perm[10];
		perm[0] = (m & S_IRUSR) ? 'r' : '-';
		perm[1] = (m & S_IWUSR) ? 'w' : '-';
		perm[2] = (m & S_IXUSR) ? 'x' : '-';
		perm[3] = (m & S_IRGRP) ? 'r' : '-';
		perm[4] = (m & S_IWGRP) ? 'w' : '-';
		perm[5] = (m & S_IXGRP) ? 'x' : '-';
		perm[6] = (m & S_IROTH) ? 'r' : '-';
		perm[7] = (m & S_IWOTH) ? 'w' : '-';
		perm[8] = (m & S_IXOTH) ? 'x' : '-';
		perm[9] = '\0';

		// Octal value
		int octal = ((m & S_IRUSR) ? 4 : 0) + ((m & S_IWUSR) ? 2 : 0) + ((m & S_IXUSR) ? 1 : 0);
		octal = octal * 10 + ((m & S_IRGRP) ? 4 : 0) + ((m & S_IWGRP) ? 2 : 0) + ((m & S_IXGRP) ? 1 : 0);
		octal = octal * 10 + ((m & S_IROTH) ? 4 : 0) + ((m & S_IWOTH) ? 2 : 0) + ((m & S_IXOTH) ? 1 : 0);

		_lblPermission->setText(
			QString("Permission: %1 (%2)").arg(perm).arg(octal, 3, 10, QChar('0')));
	}

	QCheckBox* _chkOwnerRead;
	QCheckBox* _chkOwnerWrite;
	QCheckBox* _chkOwnerExec;
	QCheckBox* _chkGroupRead;
	QCheckBox* _chkGroupWrite;
	QCheckBox* _chkGroupExec;
	QCheckBox* _chkOtherRead;
	QCheckBox* _chkOtherWrite;
	QCheckBox* _chkOtherExec;
	QLabel*    _lblPermission;
};

// ─────────────────────────────────────────────────────────────

void PanelCmd::Chmod()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::File* pFile = _pPanel->GetCurFile();
	if (!pFile) return;

	if (_pPanel->GetReader()->GetReaderName() != "file")
	{
		MsgBox(_("Error"), _("chmod local file system only."));
		return;
	}

	struct stat st;
	if (stat(pFile->sFullName.c_str(), &st) != 0)
	{
		MsgBox(_("Error"), _("Cannot read file permissions."));
		return;
	}

	ChmodDialog dlg(st.st_mode & 0777, QString::fromStdString(pFile->sName),
					dynamic_cast<QWidget*>(_pMain));

	if (dlg.exec() == QDialog::Accepted)
		chmod(pFile->sFullName.c_str(), dlg.getMode());

	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

void PanelCmd::Chown()
{
	_pPanel = _pMain->GetFocusPanel();
	MLS::File* pFile = _pPanel->GetCurFile();
	if (!pFile) return;

	string sTitle = string(_("Chown")) + " - " + pFile->sName + " (user:group)";
	string sOwner = pFile->sOwner + ":" + pFile->sGroup;
	if (InputBox(sTitle, sOwner) == SUCCESS && !sOwner.empty())
	{
		string sCmd = "chown " + sOwner + " " + addslash(pFile->sFullName);
		system(sCmd.c_str());
	}
	_pPanel->Read( _pPanel->GetPath() );
	Refresh();
}

// ============================================================
// Group F: Shell / Console
// ============================================================

void PanelCmd::Shell()
{
	string sCmd;
	if (InputBox(_("Shell command"), sCmd) == SUCCESS && !sCmd.empty())
		Run(sCmd, true, false);
}

void PanelCmd::ConsoleMode()
{
	string sTerm = MLS::g_tCfg.GetValue("Qt_Setting", "UseConsole", "x-terminal-emulator");
	Run(sTerm, false, true);
}

// ============================================================
// Group G: Settings
// ============================================================

void PanelCmd::FontSelect()
{
	bool ok;
	QFont selectedFont = QFontDialog::getFont(&ok, QApplication::font(),
											   dynamic_cast<QWidget*>(_pMain));
	if (ok)
	{
		QApplication::setFont(selectedFont);
		g_tCfg.SetValue("Qt_Setting", "FontFamily", selectedFont.family().toStdString());
		g_tCfg.SetValue("Qt_Setting", "FontSize", to_string(selectedFont.pointSize()));
		_pMain->Refresh();
	}
}
