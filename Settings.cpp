#include "./Settings.h"

#include <QStandardPaths>


QT_UTILS_NAMESPACE_BEGIN

	// statics
	Settings *					Settings::Instance		= nullptr;
	const QSettings::Format		Settings::CustomFormat	= Settings::RegisterFormat();

	//!
	//! Default constructor. This will initialize the settings to be stored in the QStandardPaths::AppDataLocation in
	//! a Settings.bin file.
	//!
	Settings::Settings(void)
		: Settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
	{
	}

	//!
	//! Initialize the settings so that the settings file will be stored in @p directory (using
	//! 'Settings.bin' as the file name)
	//!
	Settings::Settings(const QString & directory)
		: Settings(directory + "/Settings.bin", CustomFormat)
	{
	}

	//!
	//! Construct the settings with a given file @p path and @p format. This uses
	//! QSettings::QSettings(const QString &, QSettings::Format)
	//!
	Settings::Settings(const QString & path, QSettings::Format format)
		: QSettings(path, format)
	{
		Q_ASSERT(Instance == nullptr);
		Q_ASSERT(this->isWritable() == true);
		Instance = this;
	}

	//!
	//! Get a setting value
	//!
	QVariant Settings::get(const QString & name, QVariant defaultValue) const
	{
		return QSettings::value(name, defaultValue);
	}

	//!
	//! Set a setting value
	//!
	void Settings::set(const QString & name, QVariant value)
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
	bool Settings::init(const QString & name, QVariant value)
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

	//!
	//! Register our custom format
	//!
	QSettings::Format Settings::RegisterFormat(void)
	{
		auto format = QSettings::registerFormat("ini", Settings::Read, Settings::Write);
		Q_ASSERT(format != QSettings::InvalidFormat);
		return format;
	}

	//!
	//! Read settings file.
	//!
	bool Settings::Read(QIODevice & device, QSettings::SettingsMap & map)
	{
		for (int i = 0, iend = Read(device, 0); i < iend; ++i)
		{
			QVariant key = Read(device);
			if (key.isNull() == true || key.isValid() == false)
			{
				return false;
			}

			QVariant value = Read(device);
			if (value.isNull() == true || value.isValid() == false)
			{
				return false;
			}

			map.insert(key.toString(), value);
		}

		return true;
	}

	//!
	//! Reads an element
	//!
	QVariant Settings::Read(QIODevice & device)
	{
		static_assert(sizeof(Type) == sizeof(int));

		// get the type
		Type type = Read(device, Type::Invalid);
		if (type == Type::Invalid)
		{
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
	//! Write settings file
	//!
	bool Settings::Write(QIODevice & device, const QSettings::SettingsMap & map)
	{
		Write(device, map.size());
		for (auto setting = map.constBegin(), end = map.constEnd(); setting != end; ++setting)
		{
			if (Write(device, Type::String) == false || Write(device, setting.key()) == false)
			{
				return false;
			}

			if (Write(device, setting.value()) == false)
			{
				return false;
			}
		}
		return true;
	}

QT_UTILS_NAMESPACE_END