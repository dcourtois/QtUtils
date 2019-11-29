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

		Q_PROPERTY(bool fullscreen			READ IsFullScreen			WRITE SetFullScreen			NOTIFY fullscreenChanged)
		Q_PROPERTY(Persistence persistence	READ GetPersistence			WRITE SetPersistence		NOTIFY persistenceChanged)

	signals:

		void fullscreenChanged(bool fullscreen);
		void persistenceChanged(Persistence persistence);

	public:

		// constructor
		QuickView(void);

		// C++ API
		inline bool			IsFullScreen(void) const;
		inline bool			IsReady(void) const;
		void				SetFullScreen(bool value, bool force = false);
		inline Persistence	GetPersistence(void) const;
		void				SetPersistence(Persistence value);
		inline bool			GetRestoreSize(void) const;

	protected:

		// reimplemented from QObject
		bool	eventFilter(QObject * watched, QEvent * event) override;

	private:

		// C++ API
		void	RestoreSettings(void);

		//! Persitence flags
		Persistence m_Persistence;

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
	//! Get the current fullscreen state.
	//!
	inline bool QuickView::IsFullScreen(void) const
	{
		return m_FullScreen;
	}

	//!
	//! Returns true if the status of the view is ready
	//!
	inline bool QuickView::IsReady(void) const
	{
		return this->status() == QQuickView::Status::Ready;
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
