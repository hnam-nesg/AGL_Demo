This project contains:

HomeScreen: AGL Home Screen reference implementation

**AGL repo for source code**

```
$ mkdir WORK
$ cd WORK
$ repo init -b master -u https://gerrit.automotivelinux.org/gerrit/AGL/AGL-repo
$ repo sync

```

Then you can get the following recipe.

* `meta-agl-demo/recipes-demo-hmi/homescreen`


**Bitbake**

```
$ source meta-agl/scripts/aglsetup.sh -m m3ulcb agl-demo agl-devel
$ bitbake homescreen
```

Instructions for building Home Screen app
----------------------------------------

The Home Screen app is part of the packagegroup-agl-demo-platform
packagegroup.

To build all the above, follow the instrucions on the AGL
documentation website:
http://docs.automotivelinux.org/docs/getting_started/en/dev/reference/source-code.html#features-supported-by-aglsetup

Please activate the "agl-demo" feature when running the aglsetup script:
http://docs.automotivelinux.org/docs/getting_started/en/dev/reference/source-code.html#features-supported-by-aglsetup


Launch Home Screen App:

Usage:

```
systemctl start homescreen
```
