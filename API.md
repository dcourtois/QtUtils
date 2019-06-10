# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[`File`](#class_file) | Simple utility to read and write file from / to the disk.
`class `[`QuickView`](#class_quick_view) | Implement a QQuickView and improve provides support for the following:
`class `[`Settings`](#class_settings) | Wrapper around QSettings to make its functionalities known to QML

# class `File` 

```
class File
  : public QObject
```  

Simple utility to read and write file from / to the disk.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public Q_INVOKABLE QString `[`read`](#class_file_1aca8119ac3c184f01ddd0b8e4ec838ded)`(const QString & filename)` | Read an entier file and return its content as a string.
`public Q_INVOKABLE bool `[`write`](#class_file_1a5c0ec723afef3be9077408d5414f4873)`(const QString & filename,const QString & content)` | Writes a string into a file. This will overwrites any previous content, if `filename` already exists. It returns true if the operation succeeded.

## Members

#### `public Q_INVOKABLE QString `[`read`](#class_file_1aca8119ac3c184f01ddd0b8e4ec838ded)`(const QString & filename)` 

Read an entier file and return its content as a string.

#### `public Q_INVOKABLE bool `[`write`](#class_file_1a5c0ec723afef3be9077408d5414f4873)`(const QString & filename,const QString & content)` 

Writes a string into a file. This will overwrites any previous content, if `filename` already exists. It returns true if the operation succeeded.

# class `QuickView` 

```
class QuickView
  : public QQuickView
```  

Implement a QQuickView and improve provides support for the following:

* Save and restore position/size/maximized states. This works only if a [Settings](#class_settings) instance has been created, and can also be disabled using the `restoreState` property.

* Support dynamic switch to fullscreen using the `fullscreen` property.

* Makes itself available to QML through the global `rootView` property.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`{property} bool `[`fullscreen`](#class_quick_view_1a9c32465019cbe56bc780cb3662c75825) | 
`{property} bool `[`restorePosition`](#class_quick_view_1aa4efc1019a39799dcc5e835fa046d679) | 
`{property} bool `[`restoreSize`](#class_quick_view_1ade998e62f60f6465186a146c993e7811) | 
`{property} bool `[`restoreMaximized`](#class_quick_view_1a48e4108848adcc94d57e66d89e74a409) | 
`{property} bool `[`restoreFullscreen`](#class_quick_view_1adf40b89e46b089fff9783e7476be80d9) | 
`public  `[`QuickView`](#class_quick_view_1a79216279c1c697705a21a1e688028f25)`(void)` | Constructor
`public  `[`~QuickView`](#class_quick_view_1a338794eb458e5dedaf998b02167ab195)`(void)` | Destructor
`public inline bool `[`IsFullScreen`](#class_quick_view_1af11cd8fe3275f28fd4c5e97592b4ac29)`(void) const` | Get the current fullscreen state.
`public void `[`SetFullScreen`](#class_quick_view_1a6cbdcb7b0cb4b994bfe574ffef9f63cf)`(bool value,bool force)` | Set the fullscreen state
`public inline bool `[`GetRestorePosition`](#class_quick_view_1a7bd875653b27d04ce1af6a952d98a7b8)`(void) const` | Get the current restore state for position.
`public void `[`SetRestorePosition`](#class_quick_view_1a0de8373a52e2e7299eaccccd42f90bfd)`(bool value)` | Set the restore state for position
`public inline bool `[`GetRestoreSize`](#class_quick_view_1a797f1460ea2edc5a2693dd4db978b256)`(void) const` | Get the current restore state for size.
`public void `[`SetRestoreSize`](#class_quick_view_1a2f3258f40f0c08834a61712148fffe10)`(bool value)` | Set the restore state for size
`public inline bool `[`GetRestoreMaximized`](#class_quick_view_1acec600186c6191d208b9bedaa761e14d)`(void) const` | Get the current restore state for maximized.
`public void `[`SetRestoreMaximized`](#class_quick_view_1a6a0376f6bf2db6a5a9752cc3f6d46ffb)`(bool value)` | Set the restore state for maximized
`public inline bool `[`GetRestoreFullScreen`](#class_quick_view_1a720bb68cbf52c1db13fa5ea1d1dd61c0)`(void) const` | Get the current restore state for fullscreen.
`public void `[`SetRestoreFullScreen`](#class_quick_view_1a6c97dbfb6ba88aefc67d5dce0dbc7c3f)`(bool value)` | Set the restore state for fullscreen
`{signal} public void `[`fullscreenChanged`](#class_quick_view_1a0d7e7cbf04b47f0d12c4fda061f025ee)`(bool fullscreen)` | 
`{signal} public void `[`restorePositionChanged`](#class_quick_view_1ae32dffc3485d97d2268dcf5f7a130b13)`(bool restorePosition)` | 
`{signal} public void `[`restoreSizeChanged`](#class_quick_view_1afeb01016287dbfd29b3d32d8ee048b18)`(bool restoreSize)` | 
`{signal} public void `[`restoreMaximizedChanged`](#class_quick_view_1abb7da8e672646ece7189eb0e09aaf593)`(bool restoreMaximized)` | 
`{signal} public void `[`restoreFullscreenChanged`](#class_quick_view_1ab848711a2a7af2084393316581c51916)`(bool restoreFullscreen)` | 

## Members

#### `{property} bool `[`fullscreen`](#class_quick_view_1a9c32465019cbe56bc780cb3662c75825) 

#### `{property} bool `[`restorePosition`](#class_quick_view_1aa4efc1019a39799dcc5e835fa046d679) 

#### `{property} bool `[`restoreSize`](#class_quick_view_1ade998e62f60f6465186a146c993e7811) 

#### `{property} bool `[`restoreMaximized`](#class_quick_view_1a48e4108848adcc94d57e66d89e74a409) 

#### `{property} bool `[`restoreFullscreen`](#class_quick_view_1adf40b89e46b089fff9783e7476be80d9) 

#### `public  `[`QuickView`](#class_quick_view_1a79216279c1c697705a21a1e688028f25)`(void)` 

Constructor

#### `public  `[`~QuickView`](#class_quick_view_1a338794eb458e5dedaf998b02167ab195)`(void)` 

Destructor

#### `public inline bool `[`IsFullScreen`](#class_quick_view_1af11cd8fe3275f28fd4c5e97592b4ac29)`(void) const` 

Get the current fullscreen state.

#### `public void `[`SetFullScreen`](#class_quick_view_1a6cbdcb7b0cb4b994bfe574ffef9f63cf)`(bool value,bool force)` 

Set the fullscreen state

#### `public inline bool `[`GetRestorePosition`](#class_quick_view_1a7bd875653b27d04ce1af6a952d98a7b8)`(void) const` 

Get the current restore state for position.

#### `public void `[`SetRestorePosition`](#class_quick_view_1a0de8373a52e2e7299eaccccd42f90bfd)`(bool value)` 

Set the restore state for position

#### `public inline bool `[`GetRestoreSize`](#class_quick_view_1a797f1460ea2edc5a2693dd4db978b256)`(void) const` 

Get the current restore state for size.

#### `public void `[`SetRestoreSize`](#class_quick_view_1a2f3258f40f0c08834a61712148fffe10)`(bool value)` 

Set the restore state for size

#### `public inline bool `[`GetRestoreMaximized`](#class_quick_view_1acec600186c6191d208b9bedaa761e14d)`(void) const` 

Get the current restore state for maximized.

#### `public void `[`SetRestoreMaximized`](#class_quick_view_1a6a0376f6bf2db6a5a9752cc3f6d46ffb)`(bool value)` 

Set the restore state for maximized

#### `public inline bool `[`GetRestoreFullScreen`](#class_quick_view_1a720bb68cbf52c1db13fa5ea1d1dd61c0)`(void) const` 

Get the current restore state for fullscreen.

#### `public void `[`SetRestoreFullScreen`](#class_quick_view_1a6c97dbfb6ba88aefc67d5dce0dbc7c3f)`(bool value)` 

Set the restore state for fullscreen

#### `{signal} public void `[`fullscreenChanged`](#class_quick_view_1a0d7e7cbf04b47f0d12c4fda061f025ee)`(bool fullscreen)` 

#### `{signal} public void `[`restorePositionChanged`](#class_quick_view_1ae32dffc3485d97d2268dcf5f7a130b13)`(bool restorePosition)` 

#### `{signal} public void `[`restoreSizeChanged`](#class_quick_view_1afeb01016287dbfd29b3d32d8ee048b18)`(bool restoreSize)` 

#### `{signal} public void `[`restoreMaximizedChanged`](#class_quick_view_1abb7da8e672646ece7189eb0e09aaf593)`(bool restoreMaximized)` 

#### `{signal} public void `[`restoreFullscreenChanged`](#class_quick_view_1ab848711a2a7af2084393316581c51916)`(bool restoreFullscreen)` 

# class `Settings` 

```
class Settings
  : public QSettings
```  

Wrapper around QSettings to make its functionalities known to QML

I'm not using the [Settings](#class_settings) QML type because it doesn't let you configure it like QSettings. And having all settings handled by a single mechanism/instance also has its benefits such as having a single place to connect to settings changes, etc.

Also, I'm using a custom format by default because QSettings is quite dumb and doesn't store the type of the settings in its IniFormat, so that if you save a bool, then you'll restore a string. And "false" is true in Javascript, which is kind of a big problem if we want to use this in QML ...

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`Settings`](#class_settings_1a3714d5ef1ba032f74183aacb77fd711a)`(void)` | Default constructor. This will initialize the instance to store settings in a `Settings.bin` file located in the directory returned by `QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)`
`public  `[`Settings`](#class_settings_1a7b188732ede0e75f3b37a8f0d5c2ec22)`(const QString & path)` | Initialize the settings so that the settings file will be stored in `path` file
`public Q_INVOKABLE QVariant `[`get`](#class_settings_1a16a7495e77691af12cec8811abca3d93)`(const QString & key,QVariant defaultValue) const` | Get a setting value
`public Q_INVOKABLE void `[`set`](#class_settings_1a65a08d46389d76fc149e7b645f228cef)`(const QString & key,QVariant value,bool sync)` | Set a setting value
`public Q_INVOKABLE bool `[`init`](#class_settings_1af99f05a86264cd9aee00f7287364e5b6)`(const QString & key,QVariant value,bool sync)` | Init a setting. This will set it if it doesn't already exist only. 
`public Q_INVOKABLE bool `[`contains`](#class_settings_1a9c7ea60575cfe180c19d083710e185d9)`(const QString & key) const` | Returns whether the settings contain the given `key`
`public Q_INVOKABLE void `[`sync`](#class_settings_1a61522a22d1ca2ecb1fcb7d1eb8e7caf6)`(void)` | Sync the settings.
`public template<>`  <br/>`inline QString `[`Read`](#class_settings_1a400e3c1842e8fee2de7821b3df4e7497)`(QIODevice & device,QString defaultValue)` | Specialization for strings
`public template<>`  <br/>`inline bool `[`Write`](#class_settings_1ad7c3f7054efa2604968a4b39677c0bea)`(QIODevice & device,QString value)` | Write a QString
`public template<>`  <br/>`inline bool `[`Write`](#class_settings_1af86f207c1f3a31e4806879b150c8f1a7)`(QIODevice & device,QVariant value)` | Write a QVariant
`{signal} public void `[`settingChanged`](#class_settings_1a437659bed7a85c33674fecd8aa307924)`(QString key,QVariant oldValue,QVariant value)` | 

## Members

#### `public  `[`Settings`](#class_settings_1a3714d5ef1ba032f74183aacb77fd711a)`(void)` 

Default constructor. This will initialize the instance to store settings in a `Settings.bin` file located in the directory returned by `QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)`

#### `public  `[`Settings`](#class_settings_1a7b188732ede0e75f3b37a8f0d5c2ec22)`(const QString & path)` 

Initialize the settings so that the settings file will be stored in `path` file

#### `public Q_INVOKABLE QVariant `[`get`](#class_settings_1a16a7495e77691af12cec8811abca3d93)`(const QString & key,QVariant defaultValue) const` 

Get a setting value

#### `public Q_INVOKABLE void `[`set`](#class_settings_1a65a08d46389d76fc149e7b645f228cef)`(const QString & key,QVariant value,bool sync)` 

Set a setting value

#### `public Q_INVOKABLE bool `[`init`](#class_settings_1af99f05a86264cd9aee00f7287364e5b6)`(const QString & key,QVariant value,bool sync)` 

Init a setting. This will set it if it doesn't already exist only. 
#### Returns
true if the setting was set, false if it already existed.

#### `public Q_INVOKABLE bool `[`contains`](#class_settings_1a9c7ea60575cfe180c19d083710e185d9)`(const QString & key) const` 

Returns whether the settings contain the given `key`

#### `public Q_INVOKABLE void `[`sync`](#class_settings_1a61522a22d1ca2ecb1fcb7d1eb8e7caf6)`(void)` 

Sync the settings.

#### `public template<>`  <br/>`inline QString `[`Read`](#class_settings_1a400e3c1842e8fee2de7821b3df4e7497)`(QIODevice & device,QString defaultValue)` 

Specialization for strings

#### `public template<>`  <br/>`inline bool `[`Write`](#class_settings_1ad7c3f7054efa2604968a4b39677c0bea)`(QIODevice & device,QString value)` 

Write a QString

#### `public template<>`  <br/>`inline bool `[`Write`](#class_settings_1af86f207c1f3a31e4806879b150c8f1a7)`(QIODevice & device,QVariant value)` 

Write a QVariant

#### `{signal} public void `[`settingChanged`](#class_settings_1a437659bed7a85c33674fecd8aa307924)`(QString key,QVariant oldValue,QVariant value)` 

Generated by [Moxygen](https://sourcey.com/moxygen)