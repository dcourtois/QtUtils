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

		Q_PROPERTY(bool fullscreen READ IsFullScreen WRITE SetFullScreen NOTIFY fullscreenChanged)
		Q_PROPERTY(bool restoreState READ GetRestoreState WRITE SetRestoreState NOTIFY restoreStateChanged)

	signals:

		void fullscreenChanged(bool fullscreen);
		void restoreStateChanged(bool restoreState);

	public:

		// constructor
		QuickView(void);
		~QuickView(void);

		// C++ API
		inline bool		IsFullScreen(void) const;
		void			SetFullScreen(bool value);
		inline bool		GetRestoreState(void) const;
		void			SetRestoreState(bool value);

	private:

		// C++ API
		void	SaveSettings(void);
		void	RestoreSettings(void);

		//! true if we're in fullscreen
		bool m_FullScreen;

		//! true if the view is backuping / restoring its states
		bool m_RestoreState;

		//! current position & size
		QRect m_Current;

		//! previous position & size
		QRect m_Previous;

		//! maximized state
		bool m_Maximized;

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
	//! Get the current restore state.
	//!
	inline bool QuickView::GetRestoreState(void) const
	{
		return m_RestoreState;
	}

QT_UTILS_NAMESPACE_END

#endif
