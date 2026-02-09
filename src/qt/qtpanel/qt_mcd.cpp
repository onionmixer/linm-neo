#include <QResizeEvent>
#include <QDebug>

#include "qpixmap.h"
#include "exception.h"
#include "qt_dialog.h"
#include "qt_panel.h"
#include "qt_mcd.h"

Qt_McdItem::Qt_McdItem( Qt_McdItem* pParent, Qt_Mcd* pMcd, MLS::File& tFile ):
	QTreeWidgetItem( pParent ), _pMcd( pMcd ), _tFile( tFile )
{
	setText( 0, QString::fromStdString( tFile.sName ) );
	if ( tFile.bDir && !tFile.isExecute() )
		setIcon( 0, QIcon(LinMGlobal::GetSmallIcon( "folder-locked" )) );
	else
		setIcon( 0, QIcon(LinMGlobal::GetSmallIcon( "folder" )) );
}

Qt_McdItem::Qt_McdItem( Qt_Mcd* pMcd, MLS::File& tFile ):
	QTreeWidgetItem( (QTreeWidget*)pMcd ), _pMcd( pMcd ), _tFile( tFile )
{
	setText( 0, QString::fromStdString( tFile.sName ) );
}

MLS::File*	Qt_McdItem::GetFile()
{
	return &_tFile;
}


bool		Qt_McdItem::OpenChk( const string& sPath, bool bChkSubDir, bool bSubDirAll )
{
	Reader* pReader = _pMcd->GetReader();

	if (pReader == NULL)
		throw Exception( "Qt_McdItem pReader is NULL." );

	string 	sBefPath = pReader->GetPath();

	if ( !_tFile.bDir ) return -1;

	if ( pReader->Read( sPath ) == false)
		return -1;

	vector<Qt_McdItem*>	vDirList;
	bool	bChkDir = false;

	while( pReader->Next() )
	{
		File	tFile;
		if (!pReader->GetInfo(tFile)) continue;

		if (tFile.sName == "." || tFile.sName == "..")
			continue;

		if ( tFile.bDir && !tFile.bLink )
		{
			if (_pMcd->GetHidden() == false)
				if (tFile.sName.substr(0, 1) == ".")
					continue;

			if ( !bChkSubDir )
			{
				Qt_McdItem* pItem = new Qt_McdItem( this, _pMcd, tFile );
				vDirList.push_back( pItem );
			}

			bChkDir = true;
		}
	}

	while( !vDirList.empty() )
	{
		Qt_McdItem*	pDirNode = vDirList.back();
		vDirList.pop_back();

		if ( bSubDirAll )
			pDirNode->setOpen( true );
		else
		{
			if ( pDirNode->OpenChk( pDirNode->GetFile()->sFullName, true, false ) )
				pDirNode->setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
			else
				pDirNode->setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicator );
		}
	}

	pReader->Read( sBefPath );
	return bChkDir;
}

void Qt_McdItem::setOpen( bool bOpen )
{
	if ( bOpen )
		setIcon( 0, QIcon(LinMGlobal::GetSmallIcon( "folder-open" )) );
	else
		setIcon( 0, QIcon(LinMGlobal::GetSmallIcon( "folder" )) );

	if ( bOpen && !childCount() )
	{
		treeWidget()->setUpdatesEnabled( false );

		OpenChk( _tFile.sFullName, false, false );

		treeWidget()->setUpdatesEnabled( true );
	}
	setExpanded( bOpen );
}

Qt_Mcd::Qt_Mcd(Qt_Panel* pPanel, QWidget* parent, const char* name):
	QTreeWidget( parent ), _pPanel( pPanel )
{
	_bHidden = false;
	_pReader = 0;

	setFocusPolicy( Qt::NoFocus );

	connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int ) ),
			 this, SLOT( SetPanelDirChg( QTreeWidgetItem *, int ) ) );
}

Qt_Mcd::~Qt_Mcd()
{
}

bool	Qt_Mcd::GetHidden()
{
	return _bHidden;
}

MLS::Reader*	Qt_Mcd::GetReader()
{
	return _pReader;
}

bool	Qt_Mcd::InitMcd( MLS::Reader* pReader, const string& sRootPath )
{
	qDebug() << "Qt_Mcd::InitMcd";
	if ( !pReader )
		throw Exception( "InitMcd Reader is NULL !!!" );

	_pReader = pReader;

	string 	sBefPath = pReader->GetPath();

	if ( pReader->Read( sRootPath.c_str() ) == false)
		return false;

	File	tFile;
	tFile.sFullName = "/";
	tFile.sName = "/";
	tFile.bDir = true;

	clear();

	Qt_McdItem* pRoot = new Qt_McdItem( this, tFile );

	pRoot->setOpen( true );

	pReader->Read( sBefPath );

	setAllColumnsShowFocus( true );
	qDebug() << "Qt_Mcd::InitMcd End";
	return true;
}

void 	Qt_Mcd::setDir( const string &sFullName )
{
	QTreeWidgetItemIterator it( this );

	++it;

	for( ; *it; ++it )
		(*it)->setExpanded( false );

	QStringList lst( QString( tr( sFullName.c_str() ) ).split("/", Qt::SkipEmptyParts) );
	QStringList::Iterator it2 = lst.begin();

	Qt_McdItem *item = (Qt_McdItem*)topLevelItem(0);

	for ( ; it2 != lst.end(); ++it2 )
	{
		while ( item )
		{
			if ( item->text( 0 ) == *it2 )
			{
				item->setOpen( true );
				break;
			}

			item = (Qt_McdItem*)itemBelow(item);
		}
	}

	if ( item )
		setCurrentItem( item );
}

void	Qt_Mcd::setDir( const MLS::File& file )
{
	setDir( file.sFullName );
}

void	Qt_Mcd::SetPanelDirChg ( QTreeWidgetItem * item, int column )
{
	MLS::File*	pFile = ((Qt_McdItem*)item)->GetFile();

	_pPanel->Read( pFile->sFullName );

	if ( _pPanel->isVisible() )
		_pPanel->Refresh();

	item->setIcon( 0, QIcon(LinMGlobal::GetSmallIcon( "folder-open" )) );
}

void	Qt_Mcd::resizeEvent( QResizeEvent* e )
{
	setColumnWidth( 0, e->size().width() - 20 );
	QTreeWidget::resizeEvent( e );
}
