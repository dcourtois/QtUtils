#ifndef QT_UTILS_QUICK_VIEW_H
#define QT_UTILS_QUICK_VIEW_H

#include "./Setup.h"

#if (_MSC_VER)
#	pragma warning ( push, 0 )
#endif

#include <QQuickView>
#include <QStack>
#include <QTimer>

#if (_MSC_VER)
#	pragma warning ( pop )
#endif


QT_UTILS_NAMESPACE_BEGIN

	//!
	//! Implement a QQuickView and improve provides support for the following:
	//!
	//! - Save and restore position/size/maximized states. This works only if a
	//!   Settings instance has been created, and can also be disabled using the
	//!   `restoreState` property.
	//! - Support dynamic switch to fullscreen using the `fullscreen` property.
	//! - Makes itself available to QML through the global `rootView` property.
	//!
	class QuickView
		: public QQuickView
	{

		Q_OBJECT

		Q_PROPERTY(bool fullscreen			READ IsFullScreen			WRITE SetFullScreen			NOTIFY fullscreenChanged)
		Q_PROPERTY(bool restorePosition		READ GetRestorePosition		WRITE SetRestorePosition	NOTIFY restorePositionChanged)
		Q_PROPERTY(bool restoreSize			READ GetRestoreSize			WRITE SetRestoreSize		NOTIFY restoreSizeChanged)
		Q_PROPERTY(bool restoreMaximized	READ GetRestoreMaximized	WRITE SetRestoreMaximized	NOTIFY restoreMaximizedChanged)
		Q_PROPERTY(bool restoreFullscreen	READ GetRestoreFullScreen	WRITE SetRestoreFullScreen	NOTIFY restoreFullscreenChanged)

	signals:

		void fullscreenChanged(bool fullscreen);
		void restorePositionChanged(bool restorePosition);
		void restoreSizeChanged(bool restoreSize);
		void restoreMaximizedChanged(bool restoreMaximized);
		void restoreFullscreenChanged(bool restoreFullscreen);

	public:

		// constructor
		QuickView(void);
		~QuickView(void);

		// C++ API
		inline bool		IsFullScreen(void) const;
		void			SetFullScreen(bool value);
		inline bool		GetRestorePosition(void) const;
		void			SetRestorePosition(bool value);
		inline bool		GetRestoreSize(void) const;
		void			SetRestoreSize(bool value);
		inline bool		GetRestoreMaximized(void) const;
		void			SetRestoreMaximized(bool value);
		inline bool		GetRestoreFullScreen(void) const;
		void			SetRestoreFullScreen(bool value);

	private:

		// C++ API
		void	SaveSettings(void);
		void	RestoreSettings(void);

		//! true if the view should backup / restore position
		bool m_RestorePosition;

		//! true if the view should backup / restore size
		bool m_RestoreSize;

		//! true if the view should backup / restore maximized state
		bool m_RestoreMaximized;

		//! true if the view should backup / restore fullscreen state
		bool m_RestoreFullScreen;

		//! maximized state
		bool m_Maximized;

		//! true if we're in fullscreen
		bool m_FullScreen;

		//! true when the view is loading
		bool m_Loading;

		//! current position & size
		QRect m_Current;

		//! previous position & size
		QRect m_Previous;

		//! flags
		Qt::WindowFlags m_Flags;

		//! Timer used to avoid writing the settings too many times
		QTimer m_SettingsTimer;

		//! Used to block current states update
		QStack< bool > m_UpdateState;

	};

	//!
	//! Get the current fullscreen state.
	//!
	inline bool QuickView::IsFullScreen(void) const
	{
		return m_FullScreen;
	}

	//!
	//! Get the current restore state for position.
	//!
	inline bool QuickView::GetRestorePosition(void) const
	{
		return m_RestorePosition;
	}

	//!
	//! Get the current restore state for size.
	//!
	inline bool QuickView::GetRestoreSize(void) const
	{
		return m_RestoreSize;
	}

	//!
	//! Get the current restore state for maximized.
	//!
	inline bool QuickView::GetRestoreMaximized(void) const
	{
		return m_RestoreMaximized;
	}

	//!
	//! Get the current restore state for fullscreen.
	//!
	inline bool QuickView::GetRestoreFullScreen(void) const
	{
		return m_RestoreFullScreen;
	}

QT_UTILS_NAMESPACE_END

#endif
