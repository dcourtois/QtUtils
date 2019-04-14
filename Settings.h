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

		//!
		//! Default constructor. This will initialize the settings to be stored in the QStandardPaths::AppDataLocation in
		//! a Settings.ini file.
		//!
		Settings(void)
			: Settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Settings.bin", CustomFormat)
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

		// C++ API
		template< typename T > static inline T		Get(const QString & name, T defaultValue = T());
		template< typename T > static inline void	Set(const QString & name, T value);
		template< typename T > static inline bool	Init(const QString & name, T value);

		//!
		//! Get a setting value
		//!
		Q_INVOKABLE QVariant get(const QString & name, QVariant defaultValue = QVariant()) const
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

		//!
		//! Register our custom format
		//!
		static QSettings::Format RegisterFormat(void)
		{
			auto format = QSettings::registerFormat("ini", Settings::Read, Settings::Write);
			Q_ASSERT(format != QSettings::InvalidFormat);
			return format;
		}

		//!
		//! Read settings file.
		//!
		static bool Read(QIODevice & device, QSettings::SettingsMap & map)
		{
			for (int i = 0, iend = Read(device, 0); i < iend; ++i)
			{
				QVariant key = Read(device);
				if (key.isNull() == true || key.isValid() == false)
				{
					qDebug() << "couldn't read key";
					return false;
				}

				QVariant value = Read(device);
				if (value.isNull() == true || value.isValid() == false)
				{
					qDebug() << "couldn't read value";
					return false;
				}

				map.insert(key.toString(), value);
			}

			return true;
		}

		//!
		//! Reads an element
		//!
		static QVariant Read(QIODevice & device)
		{
			static_assert(sizeof(Type) == sizeof(int));

			// get the type
			Type type = Read(device, Type::Invalid);
			if (type == Type::Invalid)
			{
				qDebug() << "couldn't read setting type";
				return QVariant();
			}

			// get the value
			switch (type)
			{
				case Type::Bool:		return Read(device, static_cast< char >(0)) != 0;
				case Type::Int32:		return Read(device, static_cast< int >(0));
				case Type::Int64:		return Read(device, static_cast< int64_t >(0));
				case Type::Float32:		return Read(device, 0.0f);
				case Type::Float64:		return Read(device, 0.0);
				case Type::String:		return Read(device, QString(""));
				default:				return QVariant();
			}
		}

		//!
		//! Safely read a type
		//!
		template< typename T >
		static T Read(QIODevice & device, T defaultValue)
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
		template<>
		static QString Read(QIODevice & device, QString defaultValue)
		{
			int length = Read(device, static_cast< int >(-1));
			if (length > 0)
			{
				std::string string;
				string.resize(length, '\0');
				if (device.read(string.data(), length) != length)
				{
					qDebug() << "error reading string content";
					return defaultValue;
				}
				return string.c_str();
			}
			else if (length == 0)
			{
				return "";
			}
			qDebug() << "error reading string length";
			return defaultValue;
		}

		//!
		//! Write settings file
		//!
		static bool Write(QIODevice & device, const QSettings::SettingsMap & map)
		{
			Write(device, map.size());
			for (auto setting = map.constBegin(), end = map.constEnd(); setting != end; ++setting)
			{
				if (Write(device, Type::String) == false || Write(device, setting.key()) == false)
				{
					qDebug() << "couldn't write key";
					return false;
				}

				if (Write(device, setting.value()) == false)
				{
					qDebug() << "couldn't write value";
					return false;
				}
			}
			return true;
		}

		//!
		//! Write a value
		//!
		template< typename T >
		static bool Write(QIODevice & device, T value)
		{
			return device.write(reinterpret_cast< char * >(&value), sizeof(T)) == sizeof(T);
		}

		//!
		//! Write a QString
		//!
		template<>
		static bool Write(QIODevice & device, QString value)
		{
			return Write(device, value.size()) && (value.size() == 0 || device.write(value.toLocal8Bit()) == value.size());
		}

		//!
		//! Write a QVariant
		//!
		template<>
		static bool Write(QIODevice & device, QVariant value)
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

QT_UTILS_NAMESPACE_END

#endif

#if defined(QT_UTILS_IMPLEMENTATION)

QT_UTILS_NAMESPACE_BEGIN

// statics
Settings *					Settings::Instance		= nullptr;
const QSettings::Format		Settings::CustomFormat	= Settings::RegisterFormat();

QT_UTILS_NAMESPACE_END

#endif
