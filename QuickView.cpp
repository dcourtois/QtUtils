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
	static constexpr int s_version = 1;

	//!
	//! Constructor
	//!
	QuickView::QuickView(void)
		: m_Persistence(PersistenceFlags::All)
		, m_Maximized(false)
		, m_FullScreen(false)
		, m_Flags(0)
	{
		// make the view control the root object size
		this->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);

		// install event filtering
		this->installEventFilter(this);
	}

	//!
	//! Set the fullscreen state
	//!
	void QuickView::SetFullScreen(bool value, bool force)
	{
		if (this->status() != QQuickView::Status::Ready && force == false)
		{
			// not yet ready, this is probably called from QML
			Settings::Set("RootView.ForceFullScreen", value == true ? 2 : 1);
			return;
		}
		else
		{
			if (m_FullScreen != value || force == true)
			{
				if (m_FullScreen != value)
				{
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

				if (this->IsReady() == true)
				{
					Settings::Set("RootView.FullScreen", m_FullScreen);
				}

				emit fullscreenChanged(m_FullScreen);
			}
		}
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
		for (; version < s_version; version = Settings::Get("RootView.Version", 0))
		{
			switch (version)
			{
				case 0:
					// remove settings that are no longer used
					Settings::Remove("RootView.RestorePosition");
					Settings::Remove("RootView.RestoreSize");
					Settings::Remove("RootView.RestoreMaximized");
					Settings::Remove("RootView.RestoreFullScreen");
					Settings::Remove("RootView.FirstTime");
					Settings::Remove("RootView.Init");

					// add settings of version 1
					Settings::Set("RootView.Persistence",	static_cast< unsigned int >(m_Persistence));
					Settings::Set("RootView.Version",		1);
					break;

				default:
					break;
			}
		}

		// read the settings
		const QPoint pos	= Settings::Get("RootView.Position", this->position());
		const QSize size	= Settings::Get("RootView.Size", this->size());
		m_Maximized			= Settings::Get("RootView.Maximized", m_Maximized);
		m_FullScreen		= Settings::Get("RootView.FullScreen", m_FullScreen);
		m_Persistence		= QFlag(Settings::Get("RootView.Persistence", static_cast< int >(static_cast< QFlag >(m_Persistence))));

		// clamp geometry to the current screen
		const QRect geom	= this->screen()->availableGeometry();
		m_WindowedGeometry	= { qBound(geom.topLeft(), pos, toPoint(geom.size() - size)), size };

		// check if the full screen state has been overriden from QML
		int forceFullScreen	= Settings::Get("RootView.ForceFullScreen", 0);
		Settings::Set("RootView.ForceFullScreen", 0);

		// restore states as needed
		if (firstTime == false)
		{
			if (m_Persistence.testFlag(PersistenceFlags::Position) == true && m_FullScreen == false)
			{
				this->setPosition(m_WindowedGeometry.topLeft());
			}
			if (m_Persistence.testFlag(PersistenceFlags::Size) == true && m_FullScreen == false)
			{
				this->resize(m_WindowedGeometry.size());
			}
			if (m_Persistence.testFlag(PersistenceFlags::Maximized) == true && m_FullScreen == false && forceFullScreen != 2)
			{
				this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);
			}
			if (forceFullScreen != 0)
			{
				m_FullScreen = forceFullScreen == 2 ? true : false;
			}
			if (m_Persistence.testFlag(PersistenceFlags::FullScreen) == true || forceFullScreen != 0)
			{
				this->SetFullScreen(m_FullScreen, true);
			}
		}

		// first time, show the window and set options that might have been set from QML
		else
		{
			this->setVisibility(visibility);
			this->resize(width, height);

			m_WindowedGeometry = this->geometry();

			if (forceFullScreen == 2)
			{
				this->SetFullScreen(true, true);
			}
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
				if (this->windowState() == Qt::WindowState::WindowMaximized)
				{
					m_Maximized = true;
					m_FullScreen = false;
				}
				else if (this->windowState() == Qt::WindowState::WindowFullScreen)
				{
					m_FullScreen = true;
					m_Maximized = false;
				}
				else if (this->windowState() != Qt::WindowState::WindowMinimized)
				{
					m_FullScreen = false;
					m_Maximized = false;
					m_WindowedGeometry = this->geometry();
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
				Settings::Set("RootView.FullScreen",	m_FullScreen);
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
