#include "./QuickView.h"
#include "./Settings.h"
#include "./Utils.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QRect>
#include <QScreen>
#include <QThread>
#include <QWindowStateChangeEvent>


QT_UTILS_NAMESPACE_BEGIN

	//!
	//! Constructor
	//!
	QuickView::QuickView(void)
		: m_RestorePosition(true)
		, m_RestoreSize(true)
		, m_RestoreMaximized(true)
		, m_RestoreFullScreen(true)
		, m_Maximized(false)
		, m_FullScreen(false)
		, m_Flags(0)
	{
		// init update state stack
		m_UpdateState.push(false);

		// make the view control the root object size
		this->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);

		// on status change, restore states
		QObject::connect(this, &QQuickView::statusChanged, [&] (QQuickView::Status status) {
			if (status == QQuickView::Status::Ready)
			{
				this->RestoreSettings();
				m_UpdateState.push(true);
			}
		});

		// keep track of the states of the window
		QObject::connect(this, &QWindow::xChanged, [&] (int value) {
			if (m_UpdateState.top() == true && m_FullScreen == false)
			{
				m_Previous.moveTo(m_Current.x(), m_Previous.y());
				m_Current.moveTo(value, m_Current.y());
				this->SaveSettings();
			}
		});
		QObject::connect(this, &QWindow::yChanged, [&] (int value) {
			if (m_UpdateState.top() == true && m_FullScreen == false)
			{
				m_Previous.moveTo(m_Previous.x(), m_Current.y());
				m_Current.moveTo(m_Current.x(), value);
				this->SaveSettings();
			}
		});
		QObject::connect(this, &QWindow::widthChanged, [&] (int value) {
			if (m_UpdateState.top() == true && m_FullScreen == false)
			{
				m_Previous.setWidth(m_Current.width());
				m_Current.setWidth(value);
				this->SaveSettings();
			}
		});
		QObject::connect(this, &QWindow::heightChanged, [&] (int value) {
			if (m_UpdateState.top() == true && m_FullScreen == false)
			{
				m_Previous.setHeight(m_Current.height());
				m_Current.setHeight(value);
				this->SaveSettings();
			}
		});
		QObject::connect(this, &QWindow::visibilityChanged, [&] (QWindow::Visibility value) {
			if (m_UpdateState.top() == true && m_FullScreen == false && value != QWindow::Hidden)
			{
				m_Maximized = value == QWindow::Maximized;
				m_Current = m_Maximized == true || m_FullScreen == true ? m_Previous : m_Current;
				this->SaveSettings();
			}
		});

		// configure the settings timer
		m_SettingsTimer.setSingleShot(true);
		m_SettingsTimer.callOnTimeout(Settings::Sync);
	}

	//!
	//! Destructor
	//!
	QuickView::~QuickView(void)
	{
		// kill the timer and manually save settings
		m_SettingsTimer.stop();
		Settings::Sync();
	}

	//!
	//! Set the fullscreen state
	//!
	void QuickView::SetFullScreen(bool value, bool force)
	{
		if (m_UpdateState.top() == false && force == false)
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
					m_UpdateState.push(false);
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
						this->setPosition(m_Current.topLeft());
						this->resize(m_Current.size());
						this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);
					}
					m_UpdateState.pop();
				}

				if (m_UpdateState.top() == true)
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
	void QuickView::SetRestorePosition(bool value)
	{
		if (m_RestorePosition != value)
		{
			m_RestorePosition = value;
			Settings::Set("RootView.RestorePosition", m_RestorePosition);
			emit restorePositionChanged(m_RestorePosition);
		}
	}

	//!
	//! Set the restore state for size
	//!
	void QuickView::SetRestoreSize(bool value)
	{
		if (m_RestoreSize != value)
		{
			m_RestoreSize = value;
			Settings::Set("RootView.RestoreSize", m_RestoreSize);
			emit restoreSizeChanged(m_RestoreSize);
		}
	}

	//!
	//! Set the restore state for maximized
	//!
	void QuickView::SetRestoreMaximized(bool value)
	{
		if (m_RestoreMaximized != value)
		{
			m_RestoreMaximized = value;
			Settings::Set("RootView.RestoreMaximized", m_RestoreMaximized);
			emit restoreMaximizedChanged(m_RestoreMaximized);
		}
	}

	//!
	//! Set the restore state for fullscreen
	//!
	void QuickView::SetRestoreFullScreen(bool value)
	{
		if (m_RestoreFullScreen != value)
		{
			m_RestoreFullScreen = value;
			Settings::Set("RootView.RestoreFullScreen", m_RestoreFullScreen);
			emit restoreFullscreenChanged(m_RestoreFullScreen);
		}
	}

	//!
	//! Save settings
	//!
	void QuickView::SaveSettings(void)
	{
		Settings::Set("RootView.Position", m_Current.topLeft(), false);
		Settings::Set("RootView.Size", m_Current.size(), false);
		Settings::Set("RootView.Maximized", m_Maximized, false);
		Settings::Set("RootView.FullScreen", m_FullScreen, false);
		Settings::Set("RootView.Init", true, false);
		m_SettingsTimer.start(1000);
	}

	//!
	//! Restore settings
	//!
	void QuickView::RestoreSettings(void)
	{
		// make sure state updates are disabled
		Q_ASSERT(m_UpdateState.top() == false);

		// read the settings
		const QPoint pos = Settings::Get("RootView.Position", this->position());
		const QSize size = Settings::Get("RootView.Size", this->size());

		// initialize internal states
		// note : clamp position to the current screen
		const QRect geom	= this->screen()->availableGeometry();
		m_Current			= { qBound(geom.topLeft(), pos, toPoint(geom.size() - size)), size };
		m_Maximized			= Settings::Get("RootView.Maximized", this->visibility() == QWindow::Maximized);
		m_FullScreen		= Settings::Get("RootView.FullScreen", m_FullScreen);
		m_RestorePosition	= Settings::Get("RootView.RestorePosition", m_RestorePosition);
		m_RestoreSize		= Settings::Get("RootView.RestoreSize", m_RestoreSize);
		m_RestoreMaximized	= Settings::Get("RootView.RestoreMaximized", m_RestoreMaximized);
		m_RestoreFullScreen	= Settings::Get("RootView.RestoreFullScreen", m_RestoreFullScreen);
		m_Previous			= m_Current;

		// check if the full screen state has been overriden from QML
		int forceFullScreen	= Settings::Get("RootView.ForceFullScreen", 0);
		Settings::Set("RootView.ForceFullScreen", 0);

		// restore states as needed
		if (Settings::Contains("RootView.Init") == true)
		{
			if (m_RestorePosition == true && m_FullScreen == false)
			{
				this->setPosition(m_Current.topLeft());
			}
			if (m_RestoreSize == true && m_FullScreen == false)
			{
				this->resize(m_Current.size());
			}
			if (m_RestoreMaximized == true && m_FullScreen == false && forceFullScreen != 2)
			{
				this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);
			}
			if (forceFullScreen != 0)
			{
				m_FullScreen = forceFullScreen == 2 ? true : false;
			}
			if (m_RestoreFullScreen == true || forceFullScreen != 0)
			{
				this->SetFullScreen(m_FullScreen, true);
			}
		}
		else
		{
			this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);

			m_Current = m_Previous = this->geometry();

			if (forceFullScreen == 2)
			{
				this->SetFullScreen(true, true);
			}
		}
	}

QT_UTILS_NAMESPACE_END
