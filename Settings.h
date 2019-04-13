#ifndef QT_UTILS_SETTINGS_H
#define QT_UTILS_SETTINGS_H

#include "./Setup.h"

#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QVariant>


QT_UTILS_NAMESPACE_BEGIN

	//!
	//! Wrapper around QSettings to make its functionalities known to QML
	//!
	//! @note
	//!		I'm not using the Settings QML type because it doesn't let you
	//!		configure it like QSettings. And having all settings handled by
	//!		a single mechanism/instance also has its benefits such as having
	//!		a single place to connect to settings changes, etc.
	//!
	class Settings
		: public QSettings
	{

		Q_OBJECT

	signals:

		void settingChanged(QString key, QVariant oldValue, QVariant value);

	public:

		//!
		//! Default constructor. This will initialize the settings to be stored in the QStandardPaths::AppDataLocation in
		//! a Settings.ini file.
		//!
		Settings(void)
			: Settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Settings.ini", QSettings::IniFormat)
		{
		}

		//!
		//! Construct the settings with a given file @p path and @p format. This uses
		//! QSettings::QSettings(const QString &, QSettings::Format)
		//!
		Settings(const QString & path, QSettings::Format format)
			: QSettings(path, format)
		{
			Q_ASSERT(Instance == nullptr);
			Q_ASSERT(this->isWritable() == true);
			Instance = this;
		}

		// singleton-like
		static Settings * Instance;

		// C++ API
		template< typename T > static inline T		Get(const QString & name, T defaultValue);
		template< typename T > static inline void	Set(const QString & name, T value);
		template< typename T > static inline bool	Init(const QString & name, T value);

		//!
		//! Get a setting value
		//!
		Q_INVOKABLE QVariant get(const QString & name, QVariant defaultValue) const
		{
			return QSettings::value(name, defaultValue);
		}

		//!
		//! Set a setting value
		//!
		Q_INVOKABLE void set(const QString & name, QVariant value)
		{
			QVariant oldValue = this->value(name);
			if (oldValue != value)
			{
				QSettings::setValue(name, value);
				this->sync();
				emit settingChanged(name, oldValue, value);
			}
		}

		//!
		//! Init a setting. This will set it if it doesn't already exist only.
		//! @returns true if the setting was set, false if it already existed.
		//!
		Q_INVOKABLE bool init(const QString & name, QVariant value)
		{
			if (this->contains(name) == false)
			{
				QSettings::setValue(name, value);
				this->sync();
				emit settingChanged(name, QVariant(), value);
				return true;
			}
			return false;
		}

	};


	//!
	//! Get a setting
	//!
	template< typename T >
	inline T Settings::Get(const QString & name, T defaultValue)
	{
		return Instance->get(name, defaultValue).value< T >();
	}

	//!
	//! Update or set a setting
	//!
	template< typename T >
	inline void Settings::Set(const QString & name, T value)
	{
		Instance->set(name, value);
	}

	//!
	//! Init a setting. This will set it if it doesn't already exist only.
	//! @returns true if the setting was set, false if it already existed.
	//!
	template< typename T >
	inline bool Settings::Init(const QString & name, T value)
	{
		return Instance->init(name, value);
	}

QT_UTILS_NAMESPACE_END

#endif

#if defined(QT_UTILS_IMPLEMENTATION)

QT_UTILS_NAMESPACE_BEGIN

// statics
Settings * Settings::Instance = nullptr;

QT_UTILS_NAMESPACE_END

#endif
