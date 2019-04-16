Disclaimer
==========

This collection of Qt utilities is here so that I can conveniently add it as a sub-module for my
other projects. It's probably full of bugs, unoptimized and maybe contains horrifying coding abdominations.

So: Use at your own risk :)

Content
=======

Each of the utility is commented, but following is a small overview of each one. For more information,
check the comments directly in the sources.

All utility support an optional namespace to avoid name clashes. To use this, just define `QT_UTILS_NAMESPACE`
to the namespace you want to use when compiling.

These utilities are meant to be dropped in your projects. Just add the .cpp and .h files that you need
to your project files. Note that some utilities depends on one another, in which case the dependency
is listed in this readme.

Settings
--------

QSettings lacks the possibility to be notified when a setting is changed. It also lacks the ability to be
used in QML. Using Qt.labs Settings in QML can partially solve the problem, but:

1. You still lack communications between C++ and QML (e.g. how do I know QML has changed a setting ?)
2. You can't control the format of the setting like with QSettings

This class simply adds the missing features and can be shared with a QML context. It's a singleton-like
class (yes I know) You just declare 1 instance somewhere, you can share this instance to QML, and you
can then update settings from everywhere and keep track of what's going on.
