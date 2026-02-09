#include <QObject>
#include <QMessageBox>
#include <QTimer>
#include <QProgressDialog>
#include <QInputDialog>
#include <QPixmapCache>
#include <QFileIconProvider>
#include <QApplication>
#include <QDebug>

#include "mlsdialog.h"
#include "qt_dialog.h"

void	Qt_Dialog::ProgressBreak()
{
	_bProgBreak = true;
}

void	Qt_Dialog::ProgressTimerPerform()
{
	if ( _pProgDig )
	{
		_pProgDig->setValue( _nSteps );
		_nSteps++;
		if ( _nSteps > _pProgDig->maximum() )
			_nSteps = 0;
	}
}

void	Qt_Dialog::MsgBox(const string& sTitle, const string& sMsg)
{
	QWidget* pWidget = QApplication::activeWindow();
	QMessageBox::critical( pWidget, QObject::tr(sTitle.c_str()), QObject::tr(sMsg.c_str()) );
}

bool	Qt_Dialog::YNBox(const string& sTitle, const string& sMsg, bool bYes)
{
	QWidget* pWidget = QApplication::activeWindow();
	QMessageBox::StandardButton result = QMessageBox::question( pWidget,
								QObject::tr(sTitle.c_str()),
								QObject::tr(sMsg.c_str()),
								QMessageBox::Yes | QMessageBox::No );
	if ( result == QMessageBox::Yes )
		return true;
	return false;
}

void*	Qt_Dialog::MsgWaitBox(const string& sTitle, const string& sMsg)
{
	MsgWaitEnd( 0 );

	_pProgDig = new QProgressDialog( QObject::tr(sTitle.c_str()), QObject::tr("Cancel"), 0, 100 );
	_pWidget->connect( _pProgDig, SIGNAL(canceled()), this, SLOT(ProgressBreak()) );
	_nSteps = 0;

	_pTimer = new QTimer( _pWidget );
	_pWidget->connect( _pTimer, SIGNAL(timeout()), this, SLOT(ProgressTimerPerform()) );
	_pTimer->start( 100 );

	_bProgBreak = false;
	return NULL;
}

void	Qt_Dialog::MsgWaitEnd(void* p)
{
	if ( _pProgDig )
	{
		if ( _pTimer )
			delete _pTimer;
		_pTimer = 0;

		_pProgDig->setValue( _nSteps );

		_pProgDig->close();
		delete _pProgDig;
		_pProgDig = 0;
	}
}

int		Qt_Dialog::GetChar(bool bNoDelay)
{
	if ( _pProgDig && _bProgBreak ) return 27; // ESC
	return 0;
}

int		Qt_Dialog::InputBox(const string& sTitle, string& sInputStr, bool bPasswd)
{
	QWidget* pWidget = QApplication::activeWindow();

	bool ok = false;
	QString		strRt;

	if ( !bPasswd )
		strRt = QInputDialog::getText( pWidget, QObject::tr(sTitle.c_str()), QObject::tr(sTitle.c_str()),
										QLineEdit::Normal, sInputStr.c_str(), &ok );
	else
		strRt = QInputDialog::getText( pWidget, QObject::tr(sTitle.c_str()), QObject::tr(sTitle.c_str()),
										QLineEdit::Password, sInputStr.c_str(), &ok );

	sInputStr = (const char*)strRt.toLocal8Bit();
	if ( ok && !sInputStr.empty() ) return SUCCESS;
	return ERROR;
}

int		Qt_Dialog::SelectBox(const string& sTitle, vector<string>& vMsgStr, int n)
{
	return 0;
}

int		Qt_Dialog::TextBox( const string& sTitle, vector<string>& vTextStr, bool bCurShow, int width )
{
	return 0;
}

QPixmap LinMGlobal::GetSmallIcon( MLS::File* pFile )
{
	QString		strIconName;

	if ( !pFile )
	{
		QPixmap pm;
		if ( !QPixmapCache::find("unknown", &pm) )
		{
			pm.load( strIconName + ".png" );
			QPixmapCache::insert("unknown", pm);
		}
		return pm;
	}

	if ( pFile->bDir )
		strIconName = "folder";
	else
	{
		strIconName = pFile->Ext().c_str();
		if ( strIconName.isEmpty() )
			strIconName = pFile->sName.c_str();
	}

	QPixmap pm;
	if ( !QPixmapCache::find(strIconName, &pm) )
	{
		pm.load( strIconName + ".png" );
		QPixmapCache::insert(strIconName, pm);
	}
	return pm;
}

QPixmap LinMGlobal::GetSmallIcon( const QString& strIconName )
{
	QPixmap 	pm;
	QString		strTmp = strIconName;

	if ( strTmp.isEmpty() )
		strTmp = "unknown";

	if ( !QPixmapCache::find(strTmp, &pm) )
	{
		pm.load( strIconName + ".png" );
		QPixmapCache::insert(strTmp, pm);
	}
	return pm;
}

QIcon	LinMGlobal::GetIconSet( const QString& strIconName )
{
	return QIcon( strIconName );
}

QColor	LinMGlobal::GetColor( int nNum )
{
	// 0 black  1 red  2 green  3 brown  4 blue  5 purple  6 cyan  7 white
	// 8 gray  9 orange 10 lime 11 yellow 12 sky 13 pink 14 teal 15 bright white
	switch( nNum )
	{
		case 0:	return Qt::black;
		case 1: return QColor(100,0,0);
		case 2: return QColor(30, 80, 0);
		case 3: return QColor( 152, 0, 0);
		case 4: return Qt::darkBlue;
		case 5: return QColor( 110, 2, 120);
		case 6: return QColor( 200,0,0);
		case 7: return QColor( 30, 30, 30);
		case 8: return Qt::gray;
		case 9: return QColor( 150, 0, 0 );
		case 10: return QColor( 0, 139, 0 );
		case 11: return Qt::yellow;
		case 12: return QColor( 0, 55, 165 );
		case 13: return QColor(119, 7, 129);
		case 14: return Qt::blue;
		case 15: return Qt::white;
	}
	return QColor( 0, 0, 0 );
}
