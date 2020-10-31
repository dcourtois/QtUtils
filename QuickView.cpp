#include "./QuickView.h"
#include "./Settings.h"
#include "./Utils.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QRect>
#include <QScreen>
#include <QThread>


QT_UTILS_NAMESPACE_BEGIN

	//! Settings version
	static constexpr int s_version = 2;

	//!
	//! Constructor
	//!
	QuickView::QuickView(void)
		: m_Persistence(PersistenceFlags::All)
		, m_Maximized(false)
		, m_FullScreen(false)
		, m_Flags{}
	{
		// make the view control the root object size
		this->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);

		// install event filtering
		this->installEventFilter(this);
	}

	//!
	//! Set the fullscreen state
	//!
	void QuickView::SetFullScreen(bool value)
	{
		if (m_FullScreen != value)
		{
			qDebug() << "fullscreen changed: " << value;
			m_FullScreen = value;
			if (m_FullScreen == true)
			{
				m_Flags = this->flags();
				this->setFlags(Qt::Window | Qt::FramelessWindowHint);
				this->setPosition({ 0, 0 });
				this->resize(this->screen()->geometry().size());
			}
			else
			{
				this->setFlags(m_Flags);
				this->setPosition(m_WindowedGeometry.topLeft());
				this->resize(m_WindowedGeometry.size());
				this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);
			}
		}

		emit fullscreenChanged(m_FullScreen);
	}

	//!
	//! Set the restore state for position
	//!
	void QuickView::SetPersistence(Persistence value)
	{
		if (m_Persistence != value)
		{
			m_Persistence = value;
			Settings::Set("RootView.Persistence", static_cast< int >(static_cast< QFlag >(m_Persistence)));
			emit persistenceChanged(m_Persistence);
		}
	}

	//!
	//! Restore the settings. This must be called before the view is ready.
	//!
	//! @param width
	//!		Default width (used on the first run)
	//!
	//! @param height
	//!		Default height (used on the first run)
	//!
	//! @param visibility
	//!		Default visibility (used on the first run)
	//!
	void QuickView::Restore(int width, int height, QWindow::Visibility visibility)
	{
		// get the settings version
		int version = Settings::Get("RootView.Version", 0);
		bool firstTime = version == 0;

		// update the settings
		switch (version)
		{
			case 0:
				Settings::Remove("RootView.RestorePosition");
				Settings::Remove("RootView.RestoreSize");
				Settings::Remove("RootView.RestoreMaximized");
				Settings::Remove("RootView.RestoreFullScreen");
				Settings::Remove("RootView.FirstTime");
				Settings::Remove("RootView.Init");
				Settings::Set("RootView.Persistence", static_cast< unsigned int >(m_Persistence));

			case 1:
				Settings::Remove("RootView.ForceFullScreen");
				Settings::Remove("RootView.FullScreen");

			default:
				break;
		}

		// read the settings
		m_WindowedGeometry = {
			Settings::Get("RootView.Position",	this->position()),
			Settings::Get("RootView.Size",		this->size())
		};
		m_Maximized		= Settings::Get("RootView.Maximized", m_Maximized);
		m_Persistence	= QFlag(Settings::Get("RootView.Persistence", static_cast< int >(static_cast< QFlag >(m_Persistence))));

		// sometimes when we disconnect a screen or change resolution (or some bug happens) the restored geometry
		// can be entirely outside the availabe screen space. Make sure that the window will be partly visible.
		// note that this is not perfect, but it should handle the most common setups and avoid the application
		// window being entirely out of the available screen areas.
		const QRect geom = this->screen()->availableVirtualGeometry();
		if (m_WindowedGeometry.right() < 0)
		{
			m_WindowedGeometry.moveLeft(0);
			if (m_WindowedGeometry.right() > geom.width())
			{
				m_WindowedGeometry.setWidth(geom.width());
			}
		}
		if (m_WindowedGeometry.x() > geom.width())
		{
			if (m_WindowedGeometry.width() > geom.width())
			{
				m_WindowedGeometry.moveLeft(0);
				m_WindowedGeometry.setWidth(geom.width());
			}
			else
			{
				m_WindowedGeometry.moveLeft(geom.width() - m_WindowedGeometry.width());
			}
		}
		if (m_WindowedGeometry.bottom() < 0)
		{
			m_WindowedGeometry.moveTop(0);
			if (m_WindowedGeometry.bottom() > geom.height())
			{
				m_WindowedGeometry.setHeight(geom.height());
			}
		}
		if (m_WindowedGeometry.y() > geom.height())
		{
			if (m_WindowedGeometry.height() > geom.height())
			{
				m_WindowedGeometry.moveTop(0);
				m_WindowedGeometry.setHeight(geom.height());
			}
			else
			{
				m_WindowedGeometry.moveTop(geom.height() - m_WindowedGeometry.height());
			}
		}

		// restore states as needed
		if (firstTime == false)
		{
			if (m_Persistence.testFlag(PersistenceFlags::Position) == true)
			{
				this->setPosition(m_WindowedGeometry.topLeft());
			}
			if (m_Persistence.testFlag(PersistenceFlags::Size) == true)
			{
				this->resize(m_WindowedGeometry.size());
			}
			if (m_Persistence.testFlag(PersistenceFlags::Maximized) == true)
			{
				this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);
			}
		}

		// first time, set the options that were passed to the method
		else
		{
			this->setVisibility(visibility);
			this->resize(width, height);
			m_WindowedGeometry = this->geometry();
		}
	}

	//!
	//! It's completely unfathomable to me that there is no "easy" way to catch the "I'm about to be
	//! closed" event in Qt... Try Google'ing this, a ton of people have the same problem. There's
	//! a closing signals, but https://bugreports.qt.io/browse/QTBUG-55722... 3 years later, still
	//! have to install a filter that will be called for EACH f...ing event, just to do some stuff when
	//! I'm about to be closed...
	//!
	//! Oh, and reimplementing this method in itself does nothing, we have to call installEventFilter
	//! on ourself. Why can't I just override a closeEvent virtual and be done with it ? No idea...
	//!
	//! @note
	//!		Since I'm kind of forced to use this mechanism, might as well move all the state tracking
	//!		logic in here. This is why the code does not only check for the close state.
	//!
	bool QuickView::eventFilter(QObject * watched, QEvent * event)
	{
		switch (event->type())
		{
			case QEvent::WindowStateChange:
			{
				switch (this->windowState())
				{
					case Qt::WindowState::WindowMaximized:
						m_Maximized = true;
						m_FullScreen = false;
						break;

					case Qt::WindowState::WindowFullScreen:
						Q_ASSERT(false && "Use the `fullscreen` property of QuickView instead.");
						break;

					case Qt::WindowState::WindowNoState:
						if (m_FullScreen == false) {
							m_Maximized = false;
						}
						break;

					default:
						break;
				}
				break;
			}

			case QEvent::Close:
			{
				if (m_Maximized == false && m_FullScreen == false)
				{
					m_WindowedGeometry = this->geometry();
				}

				Settings::Set("RootView.Position",		m_WindowedGeometry.topLeft());
				Settings::Set("RootView.Size",			m_WindowedGeometry.size());
				Settings::Set("RootView.Maximized",		m_Maximized);
				Settings::Set("RootView.Version", 		s_version);

				break;
			}

			default:
				break;
		}

		// let the base class handle the event
		return QQuickView::eventFilter(watched, event);
	}

QT_UTILS_NAMESPACE_END
