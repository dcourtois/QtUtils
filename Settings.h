#ifndef QT_UTILS_SETTINGS_H
#define QT_UTILS_SETTINGS_H

#include "./Setup.h"

#include <QIODevice>
#include <QSettings>
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
	//!		Also, I'm using a custom format by default because QSettings is
	//!		quite dumb and doesn't store the type of the settings in its
	//!		IniFormat, so that if you save a bool, then you'll restore a
	//!		string. And "false" is true in Javascript, which is kind of a big
	//!		problem if we want to use this in QML ...
	//!
	class Settings
		: public QSettings
	{

		Q_OBJECT

	signals:

		void settingChanged(QString key, QVariant oldValue, QVariant value);

	public:

		// constructors
		Settings(void);
		Settings(const QString & path);

		// C++ API
		template< typename T > static inline T		Get(const QString & name, T defaultValue = T());
		template< typename T > static inline void	Set(const QString & name, T value);
		template< typename T > static inline bool	Init(const QString & name, T value);

		// QML API
		Q_INVOKABLE QVariant	get(const QString & name, QVariant defaultValue = QVariant()) const;
		Q_INVOKABLE void		set(const QString & name, QVariant value);
		Q_INVOKABLE bool		init(const QString & name, QVariant value);

	private:

		//! The types of settings we're supporting
		enum class Type
			: int
		{
			Bool = 0,
			Int32,
			Int64,
			Float32,
			Float64,
			String,
			Invalid,
		};

		// full constructor
		Settings(const QString & path, QSettings::Format format);

		// private API
		static QSettings::Format				RegisterFormat(void);
		static bool								Read(QIODevice & device, QSettings::SettingsMap & map);
		static QVariant							Read(QIODevice & device);
		template< typename T >	static T		Read(QIODevice & device, T defaultValue);
		template< >				static QString	Read(QIODevice & device, QString defaultValue);
		static bool								Write(QIODevice & device, const QSettings::SettingsMap & map);
		template< typename T > static bool		Write(QIODevice & device, T value);
		template< > static bool					Write(QIODevice & device, QString value);
		template< > static bool					Write(QIODevice & device, QVariant value);

		//! The single instance of the settings
		static Settings * Instance;

		//! Our custom format
		static const QSettings::Format CustomFormat;

	};

	//!
	//! Get a setting
	//!
	template< typename T >
	inline T Settings::Get(const QString & name, T defaultValue)
	{
		return Instance != nullptr ?
			Instance->get(name, defaultValue).value< T >() :
			T();
	}

	//!
	//! Update or set a setting
	//!
	template< typename T >
	inline void Settings::Set(const QString & name, T value)
	{
		if (Instance != nullptr)
		{
			Instance->set(name, value);
		}
	}

	//!
	//! Init a setting. This will set it if it doesn't already exist only.
	//! @returns true if the setting was set, false if it already existed.
	//!
	template< typename T >
	inline bool Settings::Init(const QString & name, T value)
	{
		return Instance != nullptr ? Instance->init(name, value) : false;
	}

	//!
	//! Safely read a type
	//!
	template< typename T >
	T Settings::Read(QIODevice & device, T defaultValue)
	{
		T value = T();
		if (device.read(reinterpret_cast< char * >(&value), sizeof(T)) != sizeof(T))
		{
			return defaultValue;
		}
		return value;
	}

	//!
	//! Specialization for strings
	//!
	template< >
	QString Settings::Read(QIODevice & device, QString defaultValue)
	{
		int length = Read(device, static_cast< int >(-1));
		if (length > 0)
		{
			std::string string;
			string.resize(length, '\0');
			if (device.read(string.data(), length) != length)
			{
				return defaultValue;
			}
			return string.c_str();
		}
		else if (length == 0)
		{
			return "";
		}
		return defaultValue;
	}

	//!
	//! Write a value
	//!
	template< typename T >
	static bool Settings::Write(QIODevice & device, T value)
	{
		return device.write(reinterpret_cast< char * >(&value), sizeof(T)) == sizeof(T);
	}

	//!
	//! Write a QString
	//!
	template< >
	static bool Settings::Write(QIODevice & device, QString value)
	{
		return Write(device, value.size()) && (value.size() == 0 || device.write(value.toLocal8Bit()) == value.size());
	}

	//!
	//! Write a QVariant
	//!
	template< >
	static bool Settings::Write(QIODevice & device, QVariant value)
	{
		static_assert(sizeof(long) == sizeof(int));
		static_assert(sizeof(long long) == sizeof(int64_t));

		switch ((QMetaType::Type)value.type())
		{
			case QMetaType::Bool:
				return Write(device, Type::Bool) && Write(device, static_cast< char >(value.toBool() ? 1 : 0));

			case QMetaType::Int:
			case QMetaType::Long:
			case QMetaType::UInt:
			case QMetaType::ULong:
				return Write(device, Type::Int32) && Write(device, value.toInt());

			case QMetaType::LongLong:
			case QMetaType::ULongLong:
				return Write(device, Type::Int64) && Write(device, value.toLongLong());

			case QMetaType::Float:
				return Write(device, Type::Float32) && Write(device, value.toFloat());

			case QMetaType::Double:
				return Write(device, Type::Float64) && Write(device, value.toDouble());

			case QMetaType::QString:
				return Write(device, Type::String) && Write(device, value.toString());

			default:
				return false;
		}
	}

QT_UTILS_NAMESPACE_END

#endif
