Introduction
============

After the dick-move from Qt that forced users to have an account to install Qt. Worst part is the piece of shit
justification they posted on their blog... I don't mind the move in itself, but I really fucking hate it when
people are being hypocrits and justify those type of moves as being made "for the good of our users".

For my own good ? Really ? Before this I couldn't download and update the libraries in a few clicks. After this
change ? I had to create an account just for that and provide you with personal data. And even after I did, I
couldn't figure out own to fucking download the libs !!! The installer says I don't have a license, and the link
provided send me to the Qt website where I have no idea what to do...

How the FUCK IS THIS FOR MY OWN GOOD ?????????

Sorry for whoever reads this, I had to let this out of my system.

Now, that being said, I'm still grateful for the Qt libraries and all the work behind it. And since I'm not willing
switch to some other framework, here are the scripts I use to build my own Qt libraries.


Building Qt
===========

This is still a WIP page. All my computers are already setup, so those scripts might not work out of the box for you.
Also, I do not care about stuff I'm not using, so you might miss some stuff (for instance I think OpenSSL is not supported
out of the box, etc.)

Basically to build Qt from sources, open a Git bash terminal on Windows and:

```
$ mkdir QtLibs
$ cd QtLibs
$ cp <QtUtils/BuildScripts>/*.bat .
$ ./setup.bat
$ ./update.bat 5.15
$ ./build.bat
```

`<QtUtils/BuildScripts>` is the path to the folder where this file is located. The built Qt libraries will be located
in the `install` folder, and once built, you can safely remove the `build` one.

To build on Linux, it's just the same, just replace all the `.bat` extensions by `.sh`

Content
=======

#### setup.bat/sh

Do once. This will clone and initiliaze the Qt sources repository.

#### update.bat/sh branch

This will update the repository to the latest version of the given `branch`.

#### build.bat/sh

Main build script. It will configure Qt, build the libraries and install them.
