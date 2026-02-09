#ifndef __QT_COMMAND_H__
#define __QT_COMMAND_H__

#include <QObject>

#include "selection.h"

class Qt_Panel;
class Qt_MainWindow;

class PanelCmd:public QObject
{
Q_OBJECT

public:
	PanelCmd(QObject * parent = 0 );

	void	SetMain( Qt_MainWindow* pMain );

	int		Run( const std::string &cmd, bool bPause, bool bBackground);
	int		ParseAndRun(const std::string &p, bool Pause);

public slots:
	// Existing slots
	void	Refresh();
	void	Left();
	void	Right();
	void	Up();
	void	Down();
	void	Enter();
	void	Home();
	void	End();
	void	PgDn();
	void	PgUp();
	void	Select();
	void	Archive();
	void	RemoteConnect();
	void	RemoteClose();
	void	Split();
	bool	Quit();
	void	Empty();
	void	NextWindow();

	// Group A: Navigation
	void	GoParent();
	void	GoRoot();
	void	GoHome();
	void	Back();
	void	Forward();

	// Group A: Selection
	void	SelectAll();
	void	SelectInvert();

	// Group A: Sort
	void	SortChange();
	void	SortAscDescend();

	// Group A: View settings
	void	HiddenFileView();
	void	ColumnAuto();
	void	Column1();
	void	Column2();
	void	Column3();
	void	Column4();

	// Group B: InputDialog + Reader
	void	Mkdir();
	void	Rename();
	void	NewFile();
	void	TouchFile();
	void	Remove();
	void	Execute();

	// Group C: Clipboard
	void	ClipCopy();
	void	ClipCut();
	void	ClipPaste();
	void	Copy();
	void	Move();

	// Group D: Archive
	void	Extract();
	void	TargzComp();
	void	Tarbz2Comp();
	void	ZipComp();

	// Group E: Info/Dialogs
	void	About();
	void	Help();
	void	FileInfo();
	void	Chmod();
	void	Chown();

	// Group F: Shell/Console
	void	Shell();
	void	ConsoleMode();

	// Group G: Settings
	void	FontSelect();

protected:
	void	GetMainPanel();
	void	syncMcdAndRefresh();

private:
	Qt_Panel*		_pPanel;
	Qt_MainWindow*	_pMain;

	// Clipboard state
	MLS::Selection	_clipboard;
	bool			_bClipCut;
};

#endif // __QT_COMMAND_H__
