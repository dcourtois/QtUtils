Introduction
============

A little story in 5 acts:

1. Mandatory connection to a Qt account to be able to install Qt from the Qt maintenance tool, "for our own good".
2. First reaction: "... really ?" (at least don't give us that bullshit argument and assume being an asshole company)
3. Anyway, created an account just for that
4. Connect > don't have a license > click on the privded link to get one > lands on the Qt website where I can choose Open Source or Commercial ...
5. And no matter what I do from here, I can't seem to get a license, I don't know what the hell I should do to just install Qt.

SO !

After this dick-move from Qt, I decided to use my own build. After all, Qt is "still" open source (seing the way things go, and since the
main repository is hosted by Qt, I wouldn't be surprised if at some point they decide to make the sources only available from
an account ...)

And since I already have this small repo that I use in some of my pet projects, I decided to make my build scripts available.


Building Qt
===========

This is still a WIP page. All my computers are already setup, so those scripts might not work out of the box for you. Also, I do
not care about stuff I'm not using, so you might miss some stuff (for instance I think OpenSSL is not supported out of the box, etc.)

Basically to build Qt from sources, open a Git bash terminal on Windows and:

```
$> mkdir QtLibs
$> cd QtLibs
$> cp <QtUtils/BuildScripts>/*.bat .
$> ./setup.bat
$> ./update.bat 5.15
$> ./build.bat
```

`<QtUtils/BuildScripts>` is the path to the folder where this file is located. The built Qt libraries will be located in the `install`
folder, and once built, you can safely remove the `build` one.


Content
=======

#### setup.bat

Do once. This will clone and initiliaze the Qt sources repository.

#### update.bat <branch>

This will update the repository to the latest version of the given branch.

#### build.bat

Main build script. It will configure Qt, build the libraries and install them.
