#ifndef QT_UTILS_SETTINGS_H
#define QT_UTILS_SETTINGS_H

#include "./Setup.h"

#include <QColor>
#include <QIODevice>
#include <QRect>
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
		template< typename T > static inline T		Get(const QString & key, T defaultValue = T());
		template< typename T > static inline void	Set(const QString & key, T value, bool sync = true);
		template< typename T > static inline bool	Init(const QString & key, T value, bool sync = true);
		static inline void							Sync(void);
		static inline bool							Contains(const QString & key);
		static inline void							Clear(void);
		static inline void							Remove(const QString & key);
		static inline bool							IsInitialized(void);
		static inline Settings *					Instance(void);

		// QML API
		Q_INVOKABLE QVariant	get(const QString & key, QVariant defaultValue = QVariant()) const;
		Q_INVOKABLE void		set(const QString & key, QVariant value, bool sync = true);
		Q_INVOKABLE bool		init(const QString & key, QVariant value, bool sync = true);
		Q_INVOKABLE bool		contains(const QString & key) const;
		Q_INVOKABLE void		remove(const QString & key);
		Q_INVOKABLE void		sync(void);

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
			Point,
			PointF,
			Size,
			SizeF,
			Rect,
			RectF,
			Color,
			Invalid,
		};

		// full constructor
		Settings(const QString & path, QSettings::Format format);

		// private API
		static QSettings::Format					RegisterFormat(void);
		static bool									Read(QIODevice & device, QSettings::SettingsMap & map);
		static QVariant								Read(QIODevice & device);
		template< typename T > static inline T		Read(QIODevice & device, T defaultValue);
		static bool									Write(QIODevice & device, const QSettings::SettingsMap & map);
		template< typename T > static inline bool	Write(QIODevice & device, T value);

		//! The single instance of the settings
		static Settings * s_Instance;

		//! Our custom format
		static const QSettings::Format CustomFormat;

	};

	//!
	//! Get a setting
	//!
	template< typename T >
	inline T Settings::Get(const QString & key, T defaultValue)
	{
		return s_Instance != nullptr ?
			s_Instance->get(key, defaultValue).template value< T >() :
			T();
	}

	//!
	//! Update or set a setting
	//!
	template< typename T >
	inline void Settings::Set(const QString & key, T value, bool sync)
	{
		if (s_Instance != nullptr)
		{
			s_Instance->set(key, value, sync);
		}
	}

	//!
	//! Init a setting. This will set it if it doesn't already exist only.
	//! @returns true if the setting was set, false if it already existed.
	//!
	template< typename T >
	inline bool Settings::Init(const QString & key, T value, bool sync)
	{
		return s_Instance != nullptr && s_Instance->init(key, value, sync);
	}

	//!
	//! Returns true if we have a value for the given key
	//
	inline bool Settings::Contains(const QString & key)
	{
		return s_Instance != nullptr && s_Instance->contains(key);
	}

	//!
	//! Clears the settings
	//
	inline void Settings::Clear(void)
	{
		if (s_Instance != nullptr)
		{
			s_Instance->clear();
		}
	}

	//
	//! Remove a setting.
	//
	inline void Settings::Remove(const QString & key)
	{
		if (s_Instance != nullptr)
		{
			s_Instance->remove(key);
		}
	}

	//!
	//! Returns true if Settings has been instanciated.
	//!
	inline bool Settings::IsInitialized(void)
	{
		return s_Instance != nullptr;
	}

	//!
	//! Get the current instance. Might be nullptr. Use only to bind, for all the other
	//! functionalities, use the C++ API static methods, they already check if the instance
	//! already exists.
	//!
	inline Settings * Settings::Instance(void)
	{
		return s_Instance;
	}

	//!
	//! Synchronize the settings
	//
	inline void Settings::Sync(void)
	{
		if (s_Instance != nullptr)
		{
			s_Instance->sync();
		}
	}

	//!
	//! Safely read a type
	//!
	template< typename T >
	inline T Settings::Read(QIODevice & device, T defaultValue)
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
	inline QString Settings::Read(QIODevice & device, QString defaultValue)
	{
		int length = Read(device, static_cast< int >(-1));
		if (length > 0)
		{
			std::string string;
			string.resize(length, '\0');
			if (device.read(&string[0], length) != length)
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
	inline bool Settings::Write(QIODevice & device, T value)
	{
		return device.write(reinterpret_cast< char * >(&value), sizeof(T)) == sizeof(T);
	}

	//!
	//! Write a QString
	//!
	template< >
	inline bool Settings::Write(QIODevice & device, QString value)
	{
		return Write(device, value.size()) && (value.size() == 0 || device.write(value.toLocal8Bit()) == value.size());
	}

	//!
	//! Write a QVariant
	//!
	template< >
	inline bool Settings::Write(QIODevice & device, QVariant value)
	{
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

			case QMetaType::QPoint: {
				const QPoint point(value.toPoint());
				return Write(device, Type::Point) && Write(device, point.x()) && Write(device, point.y());
			}

			case QMetaType::QPointF: {
				const QPointF point(value.toPointF());
				return Write(device, Type::PointF) && Write(device, point.x()) && Write(device, point.y());
			}

			case QMetaType::QSize: {
				const QSize size(value.toSize());
				return Write(device, Type::Size) && Write(device, size.width()) && Write(device, size.height());
			}

			case QMetaType::QSizeF: {
				const QSizeF size(value.toSizeF());
				return Write(device, Type::SizeF) && Write(device, size.width()) && Write(device, size.height());
			}

			case QMetaType::QRect: {
				const QRect rect(value.toRect());
				return Write(device, Type::Rect) && Write(device, rect.left()) && Write(device, rect.right()) && Write(device, rect.width()) && Write(device, rect.height());
			}

			case QMetaType::QRectF: {
				const QRectF rect(value.toRectF());
				return Write(device, Type::RectF) && Write(device, rect.left()) && Write(device, rect.right()) && Write(device, rect.width()) && Write(device, rect.height());
			}

			case QMetaType::QColor: {
				const QColor color(value.value< QColor >());
				return Write(device, Type::Color) && Write(device, color.redF()) && Write(device, color.greenF()) && Write(device, color.blueF()) && Write(device, color.alphaF());
			}

			default:
				return false;
		}
	}

QT_UTILS_NAMESPACE_END

#endif
