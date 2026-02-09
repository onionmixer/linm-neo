#include <QLayout>
#include <QFrame>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>
#include <QSplitter>

#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

#include "qt_dialog.h"
#include "qt_paneltooltip.h"
#include "qt_panel.h"
#include "qt_mcd.h"
#include "qt_statusbar.h"
#include "qt_tabpanel.h"

class	QMcdPanel: public QSplitter
{
public:
	QMcdPanel( Qt::Orientation o, QWidget* parent )
		: QSplitter( o, parent ) {}

public:
	Qt_Panel*	_pPanel;
	Qt_Mcd*		_pMcd;
};

Qt_TabPanel::Qt_TabPanel( 	PanelToolTip* 	pToolTip,
							PanelStatusBar* pStatusBar,
							PanelCmd*		pPanelCmd,
							QWidget* parent ):
	QTabWidget ( parent ), _pToolTip( pToolTip ), _pStatusBar( pStatusBar ), _pPanelCmd( pPanelCmd )
{
	setFocusPolicy( Qt::NoFocus );

	qDebug() << "Qt_TabPanel::Qt_TabPanel";
	PanelInsert();

	QToolButton*	addButton = new QToolButton( this );
	addButton->setFocusPolicy( Qt::NoFocus );

	const QIcon& iconset = LinMGlobal::GetIconSet( "list-add" );
	addButton->setIcon( iconset );
	setCornerWidget( addButton, Qt::TopLeftCorner );

	setTabPosition( QTabWidget::South  );
	setTabsClosable( true );

	connect( addButton, SIGNAL( clicked() ), this, SLOT( PanelInsert() ) );
	connect( this, SIGNAL( tabCloseRequested(int) ), this, SLOT( PanelCloseTab(int) ) );

	qDebug() << "Qt_TabPanel::Qt_TabPanel End";
}

Qt_TabPanel::~Qt_TabPanel()
{
}

MLS::File*	Qt_TabPanel::getCurFile()
{
	QMcdPanel*	pPanel = (QMcdPanel*)currentWidget();
	return pPanel->_pPanel->GetCurFile();
}

Qt_Panel*	Qt_TabPanel::getViewPanel()
{
	QMcdPanel*	pPanel = (QMcdPanel*)currentWidget();
	return pPanel->_pPanel;
}

Qt_Mcd*		Qt_TabPanel::getViewMcd()
{
	QMcdPanel*	pPanel = (QMcdPanel*)currentWidget();
	return pPanel->_pMcd;
}

void		Qt_TabPanel::Refresh()
{
	return getViewPanel()->Refresh();
}

void		Qt_TabPanel::setTabLabelChg( Qt_Panel* pPanel, const QString& strName )
{
	for( int n = 0; n < count(); n++ )
	{
		QMcdPanel* p = (QMcdPanel*)widget( n );
		if ( p->_pPanel == pPanel )
		{
			setTabText( indexOf(p), strName );
			break;
		}
	}
}

void		Qt_TabPanel::PanelInsert()
{
	QMcdPanel* pPanelMcd = new QMcdPanel( Qt::Horizontal, this );

	Qt_Panel*	pPanel = new Qt_Panel( 	_pToolTip, _pStatusBar,
										_pPanelCmd, this, pPanelMcd );
	Qt_Mcd*		pMcd = new Qt_Mcd( pPanel, pPanelMcd );

	pPanelMcd->setFocusPolicy( Qt::NoFocus );
	pPanelMcd->setCollapsible( pPanelMcd->indexOf(pMcd), false );
	pPanelMcd->setCollapsible( pPanelMcd->indexOf(pPanel), false );
	pPanelMcd->insertWidget( 0, pMcd );

	qDebug() << "PanelInsert() :: 1";
	pPanel->Read("~");
	{
		QPalette pal = pPanel->palette();
		pal.setColor( QPalette::Window, Qt::white );
		pPanel->setPalette( pal );
		pPanel->setAutoFillBackground( true );
	}
	pPanel->setMinimumSize( 400, 200 );

	qDebug() << "PanelInsert() :: 2";

	pMcd->setHeaderLabel( "Name" );
	pMcd->setIndentation( 20 );
	pMcd->InitMcd( pPanel->GetReader(), "/" );
	{
		QPalette pal = pMcd->palette();
		pal.setColor( QPalette::Window, Qt::white );
		pMcd->setPalette( pal );
		pMcd->setAutoFillBackground( true );
	}
	pMcd->setColumnWidth( 0, 250 );
	pMcd->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
	pMcd->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	pMcd->setMinimumSize( 100, 100 );
	pMcd->setDir( pPanel->GetPath() );

	qDebug() << "PanelInsert() :: 3";

	pPanelMcd->_pMcd = pMcd;
	pPanelMcd->_pPanel = pPanel;

	const QIcon& iconset = LinMGlobal::GetIconSet( "folder" );
	addTab( pPanelMcd, iconset, "~" );

	setCurrentIndex( count() - 1 );
	qDebug() << "PanelInsert() :: 4";
}

void		Qt_TabPanel::PanelRemove()
{
	if ( count() > 1 )
	{
		QMcdPanel*	pPanelMcd = (QMcdPanel*)currentWidget();

		if ( pPanelMcd )
		{
			removeTab( indexOf(pPanelMcd) );

			// Update shared status bar/tooltip to surviving panel before deletion
			_pStatusBar->setPanel( ((QMcdPanel*)currentWidget())->_pPanel );
			_pToolTip->HideToolTip();

			pPanelMcd->close();
			delete pPanelMcd;
			pPanelMcd = 0;
		}
	}
}

void		Qt_TabPanel::PanelCloseTab( int index )
{
	if ( count() > 1 )
	{
		QMcdPanel*	pPanelMcd = (QMcdPanel*)widget( index );

		if ( pPanelMcd )
		{
			removeTab( index );

			// Update shared status bar/tooltip to surviving panel before deletion
			_pStatusBar->setPanel( ((QMcdPanel*)currentWidget())->_pPanel );
			_pToolTip->HideToolTip();

			pPanelMcd->close();
			delete pPanelMcd;
			pPanelMcd = 0;
		}
	}
}

void	Qt_TabPanel::keyPressEvent( QKeyEvent* event )
{
	qDebug( "Qt_TabPanel [%d]", event->key() );

	if ( QApplication::focusWidget() == this )
	{
		qDebug() << "current sam....";
	}

	qDebug() << QApplication::focusWidget();
	qDebug() << this;

	switch( event->key() )
	{
		case Qt::Key_Left:
			qDebug("QKeyEvent :: KEYLEFT" );
			break;
		case Qt::Key_Right:
			qDebug("QKeyEvent :: KEYRIGHT" );
			break;
	}
}

void	Qt_TabPanel::focusInEvent( QFocusEvent* )
{
	qDebug("Qt_TabPanel :: focusInEvent");
	QMcdPanel*	pMcdPanel = (QMcdPanel*)currentWidget();
	if ( pMcdPanel && pMcdPanel->_pPanel )
	{
		pMcdPanel->_pPanel->activateWindow();
		pMcdPanel->_pPanel->setFocus();
	}
}

CentralMain::CentralMain( 	PanelToolTip* 	pToolTip,
							PanelCmd*		pPanelCmd,
							QWidget* 		parent,
							const char* 	name )
	: QFrame( parent )
{
	setFrameStyle( QFrame::Panel | QFrame::Sunken );
	setContentsMargins( 0, 0, 0, 0 );

	_pToolTip = pToolTip;
	_pPanelCmd = pPanelCmd;

	_pStatusBar = new PanelStatusBar( this );
	_pStatusBar->setFixedHeight( 20 );

	_pLeftTabWidget = new Qt_TabPanel( pToolTip, _pStatusBar, _pPanelCmd, this );
	_pRightTabWidget = new Qt_TabPanel( pToolTip, _pStatusBar, _pPanelCmd, this );

	_QHbox = 0;
	_QVbox = 0;

	_bSplit = false;
	_bViewType = false;

	DrawPanel();

	GetFocusPanel()->setFocus();
}

void	CentralMain::DrawPanel()
{
	qDebug() << "CentralMain::DrawPanel() start";
	if ( _QHbox ) delete _QHbox;
	if ( _QVbox ) delete _QVbox;

	_QHbox = 0; _QVbox = 0;

	if ( !_bSplit )
	{
		_QHbox = new QHBoxLayout;
		_QVbox = new QVBoxLayout( this );
		_QHbox->setContentsMargins( 0, 0, 0, 0 );
		_QHbox->setSpacing( 0 );
		_QVbox->setContentsMargins( 0, 0, 0, 0 );
		_QVbox->setSpacing( 0 );

		if ( _pLeftTabWidget->getViewPanel()->_bFocus )
		{
			_QHbox->addWidget( _pLeftTabWidget );
			_pRightTabWidget->hide();
		}
		else
		{
			_QHbox->addWidget( _pRightTabWidget );
			_pLeftTabWidget->hide();
		}

		_QVbox->addLayout( _QHbox );
		_QVbox->addWidget( _pStatusBar );
	}
	else if (_bSplit && !_bViewType)
	{
		_QHbox = new QHBoxLayout;
		_QVbox = new QVBoxLayout( this );
		_QHbox->setContentsMargins( 0, 0, 0, 0 );
		_QHbox->setSpacing( 0 );
		_QVbox->setContentsMargins( 0, 0, 0, 0 );
		_QVbox->setSpacing( 0 );

		_QHbox->addWidget( _pLeftTabWidget );
		_QHbox->addWidget( _pRightTabWidget );

		_pLeftTabWidget->show();
		_pRightTabWidget->show();

		_QVbox->addLayout( _QHbox );
		_QVbox->setContentsMargins( 0, 0, 0, 0 );
		_QVbox->addWidget( _pStatusBar );
	}
	else if (_bSplit && _bViewType)
	{
		_QHbox = new QVBoxLayout;
		_QVbox = new QVBoxLayout( this );
		_QHbox->setContentsMargins( 0, 0, 0, 0 );
		_QHbox->setSpacing( 0 );
		_QVbox->setContentsMargins( 0, 0, 0, 0 );
		_QVbox->setSpacing( 0 );

		_QHbox->addWidget( _pLeftTabWidget );
		_QHbox->addWidget( _pRightTabWidget );

		_pLeftTabWidget->show();
		_pRightTabWidget->show();

		_QVbox->addLayout( _QHbox );
		_QVbox->setContentsMargins( 0, 0, 0, 0 );
		_QVbox->addWidget( _pStatusBar );
	}

	_QVbox->activate();

	_pStatusBar->show();
	qDebug() << "CentralMain::DrawPanel() end..";
}

void	CentralMain::Split()
{
	if (_bSplit && !_bViewType)
	{
		_bSplit = true;
		_bViewType = true;
	}
	else if (_bSplit && _bViewType)
	{
		_bSplit = false;
		_bViewType = false;
	}
	else
	{
		_bSplit = !_bSplit;
		_bViewType = false;
	}

	DrawPanel();
}

Qt_Panel* 	CentralMain::GetFocusPanel()
{
	if ( _pRightTabWidget->getViewPanel()->_bFocus )
		return _pRightTabWidget->getViewPanel();

	if ( !_pLeftTabWidget->getViewPanel()->_bFocus )
		_pLeftTabWidget->getViewPanel()->setFocus();

	return _pLeftTabWidget->getViewPanel();
}

Qt_Mcd*		CentralMain::GetFocusMcd()
{
	if ( _pRightTabWidget->getViewPanel()->_bFocus )
		return _pRightTabWidget->getViewMcd();

	if ( !_pLeftTabWidget->getViewPanel()->_bFocus )
		_pLeftTabWidget->getViewPanel()->setFocus();

	return _pLeftTabWidget->getViewMcd();
}

void		CentralMain::Refresh()
{
	_pLeftTabWidget->Refresh();
	_pRightTabWidget->Refresh();
}

void CentralMain::NextFocus()
{
	if ( _pRightTabWidget->getViewPanel()->_bFocus )
	{
		_pRightTabWidget->getViewPanel()->_bFocus = false;
		_pLeftTabWidget->getViewPanel()->_bFocus = true;

		_pRightTabWidget->getViewPanel()->clearFocus();
		_pLeftTabWidget->getViewPanel()->setFocus();
	}
	else
	{
		_pRightTabWidget->getViewPanel()->_bFocus = true;
		_pLeftTabWidget->getViewPanel()->_bFocus = false;

		_pRightTabWidget->getViewPanel()->setFocus();
		_pLeftTabWidget->getViewPanel()->clearFocus();
	}
	Refresh();
}

bool CentralMain::isSplit()
{
	return _bSplit;
}
