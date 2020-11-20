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
			// note: don't remove the maxmized state: when we exit fullscreen, we want to go back to maximized.
			m_FullScreen = value;
			if (m_FullScreen == true)
			{
				// backup the state of the window
				m_Flags = this->flags();
				m_WindowedGeometry = GetRestoreRect();

				// make it a frameless window occupying the whole screen
				this->setFlags(Qt::Window | Qt::FramelessWindowHint);
				this->setPosition({ 0, 0 });
				this->resize(this->screen()->geometry().size());
			}
			else
			{
				// restore the window as it was previously
				this->setFlags(m_Flags);
				this->setPosition(m_WindowedGeometry.topLeft());
				this->resize(m_WindowedGeometry.size());
				this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);
			}
		}

		emit fullscreenChanged(m_FullScreen);
	}

	//!
	//! Set the window to maximized.
	//!
	void QuickView::SetMaximized(bool value)
	{
		if (m_Maximized != value)
		{
			m_Maximized = value;
			if (m_Maximized == true)
			{
				this->showMaximized();
			}
			else
			{
				this->showNormal();
			}
			emit maximizedChanged(m_Maximized);
		}
	}

	//!
	//! Set the minimized state of the window
	//!
	void QuickView::SetMinimized(bool value)
	{
		if (m_Minimized != value)
		{
			m_Minimized = value;
			if (m_Minimized == true)
			{
				this->showMinimized();
			}
			else
			{
				if (m_Maximized == true)
				{
					this->showMaximized();
				}
				else
				{
					this->showNormal();
				}
			}
			emit minimizedChanged(m_Minimized);
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

		// notify if maximized is true
		if (m_Maximized == true)
		{
			emit maximizedChanged(m_Maximized);
		}

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
	//! Local helper to change a variable and notify if the value actually changed
	//!
#define SET_PROPERTY(prop, value, notify)	\
	if (prop != value)						\
	{										\
		prop = value;						\
		notify(prop);						\
	}

	//!
	//! At some point in time, I had to implement this method to be able to catch the "about to be
	//! closed" event in order to save the view's settings, all because there no fucking way to just
	//! have a virtual "onClose" method, like most everything else (try Google'ing this, a ton of
	//! people have the same problem, also there's a closing signals, but
	//! https://bugreports.qt.io/browse/QTBUG-55722... 3 years later, still have to install a filter
	//! that will be called for EACH f...ing event, just to do some stuff when I'm about to be closed)
	//!
	//! But then I started noticing some bugs in how the view is restored depending on its state, and
	//! (more annoyingly) depending on the platform.
	//!
	//! And now, I'm wondering what's the fucking point of a GUI abstraction library if it fucking
	//! behaves differently on each fucking platform ????? Why the hell on Windows, frameless window
	//! are set to fullscreen when calling showMaximized whereas on all other platforms it correctly
	//! set the state to Maximized ?
	//!
	//! Why the fuck do I receive move and resize events before the state is set to maximized on Windows
	//! whereas on Linux I receive the maximized event before the move and resize ones ??????
	//!
	//! Anyways, the whole mess of this method is to try and fix the fucking inconsistencies of Qt -_-
	//!
	bool QuickView::eventFilter(QObject * watched, QEvent * event)
	{
		switch (event->type())
		{
#ifndef WINDOWS
			case QEvent::Move:
			case QEvent::Resize:
				if (m_Maximized == false && m_FullScreen == false)
				{
					m_WindowedGeometry = this->geometry();
				}
#endif
			case QEvent::WindowStateChange:
				qDebug() << this->windowState();
				switch (this->windowState())
				{
					case  Qt::WindowState::WindowMinimized:
						SET_PROPERTY(m_Minimized,	true,	minimizedChanged)
						break;

					case Qt::WindowState::WindowMaximized:
						SET_PROPERTY(m_Minimized,	false,	minimizedChanged)
						SET_PROPERTY(m_Maximized,	true,	maximizedChanged)
						SET_PROPERTY(m_FullScreen,	false,	fullscreenChanged)
						break;

					case Qt::WindowState::WindowFullScreen:
						// ... for fuck's sake ...
						if (this->windowStates().testFlag(Qt::WindowState::WindowMaximized))
						{
							SET_PROPERTY(m_Minimized,	false,	minimizedChanged)
							SET_PROPERTY(m_Maximized,	true,	maximizedChanged)
							SET_PROPERTY(m_FullScreen,	false,	fullscreenChanged)
						}
						else
						{
							// ok, real fullscreen state, buggy stuff, avoid at all cost
							Q_ASSERT(false && "Do not use the fullscreen visibility. Use QuickView's custom fullscreen property instead");
						}
						break;

					case Qt::WindowState::WindowNoState:
						// ok, if we come from a minimized state, we need to restore the "maximized"
						if (m_Minimized == true)
						{
							SET_PROPERTY(m_Minimized, false, minimizedChanged);
							if (m_Maximized)
							{
								this->showMaximized();
							}

						}
						else if (m_FullScreen == false)
						{
							SET_PROPERTY(m_Maximized, false, maximizedChanged)
						}
						break;

					default:
						break;
				}
				break;

			case QEvent::Close:
				m_WindowedGeometry = GetRestoreRect();
				Settings::Set("RootView.Position",		m_WindowedGeometry.topLeft(),	false);
				Settings::Set("RootView.Size",			m_WindowedGeometry.size(),		false);
				Settings::Set("RootView.Maximized",		m_Maximized,					false);
				Settings::Set("RootView.Version", 		s_version,						false);
				Settings::Sync();
				break;

			default:
				break;
		}

		// let the base class handle the event
		return QQuickView::eventFilter(watched, event);
	}

	//!
	//! Helper used to get the correct restore rect.
	//!
	QRect QuickView::GetRestoreRect(void) const
	{
#ifdef WINDOWS
		WINDOWPLACEMENT placement{};
		GetWindowPlacement(reinterpret_cast< HWND >(this->winId()), &placement);
		RECT & rect = placement.rcNormalPosition;
		return {
			QPoint{ rect.left, rect.top },
			QSize{ rect.right - rect.left, rect.bottom - rect.top },
		};
#else
		return m_WindowedGeometry;
#endif
	}


QT_UTILS_NAMESPACE_END
