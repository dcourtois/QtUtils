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
to the namespace you want to use.

Also, most utilities are header only, but some of them might require some implementation (to initiliaze a
static member for instance) This is handled by defining `QT_UTILS_IMPLEMENTATION` in a cpp file and
including again those headers.

Finally, since we're dealing with Qt, don't forget to `moc` them ! This means that it's required to add
the files to your projects files (in your qmake project file, CMake one, etc.)

Here is a complete example:

```cpp
// 1. Declaration
// use a custom namespace. needs to be done in all places where you want
// to use the utility, so it's recommended to set this macro through your
// build system generator (CMake or whatever) Or use precompiled headers.
#define QT_UTILS_NAMESPACE QtUtils
#include "QtUtils/Settings.h"

// 2. Definition
// define the implementation. this must be done only in one cpp file
// it must also use the same QT_UTILS_NAMESPACE as all the declaractions
#define QT_UTILS_IMPLEMENTATION
#include "QtUtils/Settings.h"

// 3. Use
QtUtils::Settings settings;
int foo = settings.Get("Foo", 42);
```

Settings
--------

QSettings lacks the possibility to be notified when a setting is changed. It also lacks the ability to be
used in QML. Using Qt.labs Settings in QML can partially solve the problem, but:

1. You still lack communications between C++ and QML (e.g. how do I know QML has changed a setting ?)
2. You can't control the format of the setting like with QSettings

This class simply adds the missing features and can be shared with a QML context. It's a singleton-like
class (yes I know) You just declare 1 instance somewhere, you can share this instance to QML, and you
can then update settings from everywhere and keep track of what's going on.

This utility needs an implementation.
