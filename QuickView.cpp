#include "./QuickView.h"
#include "./Settings.h"

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
		: m_FullScreen(false)
		, m_RestoreState(true)
		, m_Maximized(false)
		, m_Flags(0)
	{
		// init update state stack
		m_UpdateState.push(true);

		// make the view control the root object size
		this->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);

		// on status change, restore states
		QObject::connect(this, &QQuickView::statusChanged, [&] (QQuickView::Status status) {
			if (status == QQuickView::Status::Ready)
			{
				this->RestoreSettings();
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

		// expose ourselves to QML
		this->engine()->rootContext()->setContextProperty("rootView", this);
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
	void QuickView::SetFullScreen(bool value)
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
			this->SaveSettings();
			emit fullscreenChanged(m_FullScreen);
		}
	}

	//!
	//! Set the restore state
	//!
	void QuickView::SetRestoreState(bool value)
	{
		if (m_RestoreState != value)
		{
			m_RestoreState = value;
			emit restoreStateChanged(m_RestoreState);
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
		m_SettingsTimer.start(200);
	}

	//!
	//! Restore settings
	//!
	void QuickView::RestoreSettings(void)
	{
		// disable state update
		m_UpdateState.push(false);

		// read the settings and clamp to fit the window in the current screen
		QRect geom = this->screen()->availableGeometry();
		QPoint pos = Settings::Get("RootView.Position", this->position());
		QSize size = Settings::Get("RootView.Size", this->size());
		m_Current.setX(qBound(geom.left(), pos.x(), geom.right() - size.width()));
		m_Current.setY(qBound(geom.top(), pos.y(), geom.bottom() - size.height()));
		m_Current.setSize(size);
		m_Maximized = Settings::Get("RootView.Maximized", this->visibility() == QWindow::Maximized);
		m_FullScreen = Settings::Get("RootView.FullScreen", m_FullScreen);
		m_Previous = m_Current;

		// initialize the window, only if needed
		if (m_RestoreState == true && Settings::Contains("RootView.Init") == true)
		{
			this->setPosition(m_Current.topLeft());
			this->resize(m_Current.size());
			this->setVisibility(m_Maximized == true ? QWindow::Maximized : QWindow::Windowed);
			if (m_FullScreen == true)
			{
				m_FullScreen = false;
				this->SetFullScreen(true);
			}
		}
		else
		{
			// still need to show the view
			this->show();

			// and don't forget to override the internal states
			m_Current = m_Previous = this->geometry();
		}

		// restore state update
		m_UpdateState.pop();
	}

QT_UTILS_NAMESPACE_END
