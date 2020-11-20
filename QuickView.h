#ifndef QT_UTILS_QUICK_VIEW_H
#define QT_UTILS_QUICK_VIEW_H

#include "./Setup.h"

#if (_MSC_VER)
#	pragma warning ( push, 0 )
#endif

#include <QQuickView>
#include <QStack>
#include <QTimer>
#include <QFlag>

#if (_MSC_VER)
#	pragma warning ( pop )
#endif


QT_UTILS_NAMESPACE_BEGIN

	//!
	//! Implement a QQuickView and improve provides support for the following:
	//!
	//! - Save and restore position/size/maximized states. This works only if a
	//!		Settings instance has been created.
	//! - Support dynamic switch to fullscreen using the `fullscreen` property.
	//! - Makes itself available to QML through the global `rootView` property.
	//!
	//! Here is a quick example of how to use it:
	//!
	//! ```
	//! int main(int argc, char ** argv)
	//! {
	//! 	QApplication app(argc, argv);
	//! 	QuickView view;
	//! 	view.setSource(QUrl("qrc:/Main.qml"));
	//! 	view.Restore(800, 600, QWindow::Visibility::Windowed);
	//! 	return app.exec();
	//! }
	//! ```
	//!
	//! That's all. On the first run, the view will have a 800 by 600 size,
	//! and on the next runs, it will automatically restore the previous position,
	//! size and maximized state.
	//!
	class QuickView
		: public QQuickView
	{

		Q_OBJECT

	public:

		enum class PersistenceFlags
		{
			None = 0,
			FullScreen = 1,
			Maximized = 1 << 1,
			Position = 1 << 2,
			Size = 1 << 3,
			All = FullScreen | Maximized | Position | Size
		};

		typedef QFlags< PersistenceFlags > Persistence;

	private:

		Q_PROPERTY(bool fullscreen			READ IsFullScreen		WRITE SetFullScreen		NOTIFY fullscreenChanged)
		Q_PROPERTY(bool maximized			READ IsMaximized		WRITE SetMaximized		NOTIFY maximizedChanged)
		Q_PROPERTY(bool minimized			READ IsMinimized		WRITE SetMinimized		NOTIFY minimizedChanged)
		Q_PROPERTY(Persistence persistence	READ GetPersistence		WRITE SetPersistence	NOTIFY persistenceChanged)

	signals:

		void fullscreenChanged(bool fullscreen);
		void maximizedChanged(bool maximized);
		void minimizedChanged(bool minimized);
		void persistenceChanged(Persistence persistence);

	public:

		// constructor
		QuickView(void);

		// C++ API
		inline bool			IsReady(void) const;
		inline bool			IsFullScreen(void) const;
		void				SetFullScreen(bool value);
		inline bool			IsMaximized(void) const;
		void				SetMaximized(bool value);
		inline bool			IsMinimized(void) const;
		void				SetMinimized(bool value);
		inline Persistence	GetPersistence(void) const;
		void				SetPersistence(Persistence value);
		void				Restore(int width, int height, QWindow::Visibility visibility);

	protected:

		// reimplemented from QObject
		bool	eventFilter(QObject * watched, QEvent * event) override;

	private:

		// helpers
		QRect	GetRestoreRect(void) const;

		//! Persitence flags
		Persistence m_Persistence;

		//! minimized state
		bool m_Minimized;

		//! maximized state
		bool m_Maximized;

		//! true if we're in fullscreen
		bool m_FullScreen;

		//! the windowed geometry
		QRect m_WindowedGeometry;

		//! window flags
		Qt::WindowFlags m_Flags;

	};

	//!
	//! Returns true if the status of the view is ready
	//!
	inline bool QuickView::IsReady(void) const
	{
		return this->status() == QQuickView::Status::Ready;
	}

	//!
	//! Get the current fullscreen state.
	//!
	inline bool QuickView::IsFullScreen(void) const
	{
		return m_FullScreen;
	}

	//!
	//! Get the current maximized state.
	//!
	inline bool QuickView::IsMaximized(void) const
	{
		return m_Maximized;
	}

	//!
	//! Get the current minimized state.
	//!
	inline bool QuickView::IsMinimized(void) const
	{
		return m_Minimized;
	}

	//!
	//! Get the persistence flags.
	//!
	inline QuickView::Persistence QuickView::GetPersistence(void) const
	{
		return m_Persistence;
	}

	Q_DECLARE_OPERATORS_FOR_FLAGS(QuickView::Persistence)

QT_UTILS_NAMESPACE_END

#endif
