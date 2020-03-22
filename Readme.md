Disclaimer
==========

This collection of Qt utilities is here so that I can conveniently add it as a sub-module for my
other projects. It's probably full of bugs, unoptimized and maybe contains horrifying coding abdominations.

So: Use at your own risk :)

Content
=======

Each of the utility is commented, but following is a small overview of each one. For more information,
check the comments directly in the sources.

Qt5.natvis
----------

This is usefull if you're using Visual Studio 201X or Visual Studio Code: it adds support for better
debugger visualization of lots of Qt types.

- Visual Studio Code - in your launch configurations, just add the following line:
`"visualizerFile": "${workspaceRoot}\\QtUtils\\Qt5.natvis"`
- Visual Studio 201X - copy the file to `%UserProfile%\Documents\Visual Studio 201X\Visualizers`.
Note that `Visualizers` might not exist; just create it.

Generated from https://code.qt.io/cgit/qt-labs/vstools.git/tree/src/qtvstools/qt5.natvis.xml @
6c19f966d47362ec9f51909bae740ff60a7bde99

Settings
--------

QSettings lacks the possibility to be notified when a setting is changed. It also lacks the ability to be
used in QML. Using Qt.labs Settings in QML can partially solve the problem, but:

1. You still lack communications between C++ and QML (e.g. how do I know QML has changed a setting ?)
2. You can't control the format of the setting like with QSettings

This class simply adds the missing features and can be shared with a QML context. It's a singleton-like
class (yes I know) You just declare 1 instance somewhere, you can share this instance to QML, and you
can then update settings from everywhere and keep track of what's going on.

Example creation:

```.cpp
// Create an instance somewhere. I usually create one in my main function so that it has the
// same lifetime as my application
int main(int argc, char ** argv)
{
	Settings settings;

	// then I create my app in its own scope so that settings outlive it, in case I need to store
	// settings in the destruction of the app.
	{
		QApplication application(argc, argv);
		// ...

		// if your application is a QML application, you can make the settings instance available to QML
		// like this (assuming rootContext is your QQmlContext root context):
		qmlContext->setContextProperties({
			{ "settings", QVariant::fromValue(&settings) }
		});
	}

	return 0;
}
```

Example usage:

```.cpp
// now you can add setting by simply setting them
Settings::Set("Hello", "World");

// get some settings (with or without a default value)
QString hello = Settings::Get< QString >("Hello");
QString foo = Settings::Get("Foo", QString("Bar"));

// and connect a slot to detect setting changes
QObject::connect(Settings::Instance(), &Settings::settingChanged, [] (QString key, QVariant oldValue, QVariant value) {
	// do stuff
});
```

And in QML:

```.qml
Component.onCompleted: {
	// set
	settings.set("Hello", "from QML");

	// get
	const stuff = settings.get("Stuff", -1);
}

// bind
Connections {
	target: settings
	onSettingChanged: {
		// do something with it. key, oldValue and value are accessible here.
	}
}
```

There are additional functionalities, check `Settings.h/cpp` for documentation.


QuickView
---------

Depends on Settings.

This is meant as a drop-in replacement of QQuickView. It support the following things:

* It will store and restore its internal states upon restarting (position, size, etc.)
* It will correctly maintain the size when the view is maximized so that when you reload
your app and un-maximize, the window doesn't stay in a "kind-of-maximized-but-not-really"
state like some well-known apps (Spotify, Deezer, just to name a few ... see it's not so hard ...)
* It supports fullscreen toggling. It's done by just resizing the view and remove the borders,
so that there's no context switch.
* It exposes itself to the internal engine's root context as the global QML property "rootView"
so that you can toggle fullscreen and be notified from your QML application.

Job
---

A minimal class that can be used to push a new job on the global thread pool. This one is really
simple, and is used like this:

```.cpp
new Job([] (void) {
	// this is running on a secondary thread, and the Job instance is automatically
	// deleted by the thread pool once the job is done running (e.g. you exist this lambda)
});
```

HttpRequest
-----------

A few functions used to retrieve stuff from http(s) requests.

Synchronously:

```.cpp
// this call blocks the current thread
QByteArray reply = RequestUrl("https://some_url");
```

Asynchronously:

```.cpp
// this call returns immediately
RequestUrl(
	// the URL
	"https://some_url",

	// the success callback
	[] (QByteArray reply) {
		// process the reply. this is executed in a secondary thread
	},

	// the failure callback
	[] (QNetworkReply::NetworkError error, QString errorString) {
		// log errors, do stuff. this is executed in a secondary thread
	},
);
```

Utils
-----

This header-only file contains small generic helpers:

* arithmetics operators for `QPoint(F)`
* component-wise `qMin`, `qMax` and `qBound` for `QPoint(F)`
* conversion between `QPoint(F)` and `QSize(F)`
