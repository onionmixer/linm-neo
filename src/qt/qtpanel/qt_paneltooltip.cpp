#include <QBrush>
#include <QPen>
#include <QRect>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QGuiApplication>

#include "define.h"
#include "file.h"
#include "strutil.h"

#include "qt_paneltooltip.h"

PanelToolTip::PanelToolTip( QWidget* parent )
	: QWidget( parent, Qt::BypassWindowManagerHint )
{
	_pFile = NULL;

	setAutoFillBackground( true );
	setAttribute( Qt::WA_OpaquePaintEvent );

	_lineColor.setRgb( 0, 0, 255 );
 	_backColor.setRgb( 250, 100, 255, 200 );
	_font1Color.setRgb( 255, 255, 255 );
	_font2Color.setRgb( 0, 0, 0 );

	_nDuration = 2000;
	_bTranslucency = false;

	_pShowTimer = new QTimer( this );
	_pHideTimer = new QTimer( this );

	connect( _pShowTimer, SIGNAL(timeout()), this, SLOT(ShowToolTip()) );
	connect( _pHideTimer, SIGNAL(timeout()), this, SLOT(HideToolTip()) );

	if( _bTranslucency )
	{
		QScreen *screen = QGuiApplication::primaryScreen();
		if (screen)
			_pixScreenshot = screen->grabWindow( 0 );
	}
}

PanelToolTip::~PanelToolTip()
{
}

void	PanelToolTip::setFile( MLS::File* pFile )
{
	if ( pFile && _pFile != pFile )
	{
		qDebug("PanelToolTip::setFile");
		_pFile = pFile;

		if ( &_pFile->sName == 0x00 ) return;

		QString		sInfo;
		QString		strName = _pFile->sName.c_str();
		QFontMetrics 	fm( font() );

		int 		nNameWidth = fm.horizontalAdvance( strName );

		if (_pFile->uSize >= 1000000000)
		{
			sInfo = QString::asprintf( "%s Byte (%.2fG) / %s %s / %s", 	MLSUTIL::toregular( _pFile->uSize ).c_str(),
										(float)_pFile->uSize/1073741824,
										_pFile->sDate.c_str(),
										_pFile->sTime.c_str(),
										_pFile->sAttr.c_str() );
		}
		else if (_pFile->uSize >= 10000000)
		{
			sInfo = QString::asprintf( "%s Byte (%.2fM) / %s %s / %s", MLSUTIL::toregular( _pFile->uSize ).c_str(),
													(float)_pFile->uSize/1048576,
													_pFile->sDate.c_str(),
													_pFile->sTime.c_str(),
													_pFile->sAttr.c_str() );
		}
		else
		{
			sInfo = QString::asprintf( "%s / %s %s / %s", MLSUTIL::toregular( _pFile->uSize ).c_str(),
													_pFile->sDate.c_str(),
													_pFile->sTime.c_str(),
													_pFile->sAttr.c_str() );
		}


		int nInfoWidth = fm.horizontalAdvance( sInfo );
		int	nWidth = 0;

		if ( nInfoWidth > nNameWidth )
			nWidth = nInfoWidth + 10;
		else
			nWidth = nNameWidth + 10;

		_sInfo1 = strName;
		_sInfo2 = sInfo;

		_rectToolTip.setWidth( nWidth );
		_rectToolTip.setHeight( (fm.height() * 2) + 4 );

		resize( _rectToolTip.width(), _rectToolTip.height() );

		qDebug( "setFile !!!!!!! resize signal _pShowTimer !!!");
		_pShowTimer->setSingleShot( true );
		_pShowTimer->start( 1500 );
	}
}

void	PanelToolTip::ShowToolTip()
{
	qDebug("ShowToolTip !!!!!!!!!!");
	if ( !isVisible() )
		show();
	else
		update();

	_pHideTimer->stop();
}

void	PanelToolTip::HideToolTip()
{
	hide();
	_pShowTimer->stop();

	if( _bTranslucency )
	{
		QScreen *screen = QGuiApplication::primaryScreen();
		if (screen)
			_pixScreenshot = screen->grabWindow( 0 );
		qDebug("_bTranslucency  ~~~~~~~ HideToolTip !!!!!!!");
	}
}

void	PanelToolTip::showEvent( QShowEvent* /*event*/ )
{
	if( _nDuration )
	{
		_pHideTimer->setSingleShot( true );
		_pHideTimer->start( _nDuration );
	}
}

void 	PanelToolTip::moveEvent( QMoveEvent * /*event*/ )
{
	if( _nDuration )
	{
		if ( _pHideTimer->isActive() )
			_pHideTimer->stop();
	}
}

void	PanelToolTip::paintEvent( QPaintEvent* event )
{
	if ( !_pFile ) return;
	qDebug( "PanelToolTip :: paintEvent Event !!! - event ");

	QFontMetrics 	fm( font() );
	QPoint 			point;
    QRect 			rect( point, size() );

	QPainter 		painter( this );
	painter.setRenderHint(QPainter::Antialiasing);

	if( _bTranslucency )
	{
		QPixmap		backPixmap( size() );
		QPainter	bltPainter( &backPixmap );
		bltPainter.drawPixmap( 0, 0, _pixScreenshot, x(), y(), width(), height() );
		bltPainter.end();

		painter.drawPixmap( 0, 0, backPixmap );
	}
	else
	{
		painter.fillRect( rect, _backColor );
	}

	painter.setPen( _lineColor );

	QRect	rectLine = QRect( rect.x()+1, rect.y()+1, rect.width() - 2, rect.height() - 2 );
	painter.drawRoundedRect( rectLine, 3, 3 );

	painter.setPen( QPen( _font1Color ) ); // Color
	painter.drawText(5, fm.height(), _sInfo1 );

	painter.setPen( QPen( _font2Color ) ); // Color
	painter.drawText(5, fm.height()*2, _sInfo2 );
	painter.end();

	qDebug("PanelToolTip :: Painter [%d] [%d] [%d] [%d]", x(), y(), width(), height() );
}

void	PanelToolTip::keyPressEvent( QKeyEvent* event )
{
	qDebug("PanelToolTip keyPressEvent [%d]", event->key() );
}
