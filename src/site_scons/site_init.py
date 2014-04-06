import subprocess

def checkForGCCOption(conf, option):
    """Check if compiler supports specified option"""
    conf.Message("Checking whether CC supports " + option + "... ")
    lastCPPFLAGS = []
    if 'CPPFLAGS' in conf.env:
        lastCPPFLAGS = conf.env['CPPFLAGS'][:]
    conf.env.Append(CPPFLAGS = [option])
    rc = conf.TryCompile("void dummy() {}",'.cpp')
    if not rc:
        conf.env.Replace(CPPFLAGS = lastCPPFLAGS)

    conf.Result(rc)
    return rc

class PkgConfig:
    def __init__(self, pkgs):
        self._pkgs = pkgs

    def libs(self):
        libs = []
        for pkg in self._pkgs:
            p = subprocess.Popen(["pkg-config", "--libs", pkg],
                                 stdout=subprocess.PIPE)
            if p.wait() != 0:
                print "Error from pkg-config --libs %s" % pkg
            libs.extend(Split(p.stdout.read()))

        return libs

    def cflags(self):
        cflags = []
        for pkg in self._pkgs:
            p = subprocess.Popen(["pkg-config", "--cflags", pkg],
                                 stdout=subprocess.PIPE)
            if p.wait() != 0:
                print "Error from pkg-config --cflags %s" % pkg
            cflags.extend(Split(p.stdout.read()))

        return cflags


class MyEnvironment:
    def __init__(self):
        self.env = Environment()
        self.initiated = False

    def initiate(self, debug):
        if self.initiated:
            return

        conf = Configure(self.env, {"CheckForGccOption": checkForGCCOption})

        WantedCPPFLAGS=[
            "-Wall",
            "-Wctor-dtor-privacy",
            "-Wstrict-null-sentinel",
            "-Woverloaded-virtual",
            "-Wextra",
            "-Wunused",
            "-Wswitch",
            "-Wreorder",
            "-Wformat-y2k",
            "-Wformat-security",
            "-Wuninitialized",
            "-Winit-self",
            "-Wmissing-include-dirs",
            "-Wswitch-enum",
            "-Wfloat-equal",
            "-pedantic",
            "-std=c++0x"
            ]
        for flag in WantedCPPFLAGS:
            conf.CheckForGccOption(flag)

        if debug:
            conf.CheckForGccOption("-O0")
            conf.CheckForGccOption("-ggdb")
        else:
            conf.CheckForGccOption("-O2")

        self.env = conf.Finish()
        self.env['CPPPATH'] = '#'
        self.env['CONFIGURED_LIBS'] = {}
        self.initiated = True

_g_basic_env = MyEnvironment()

def CoreEnvironment(**kwargs):
    debug = False
    if "DEBUG" in kwargs:
        if kwargs["DEBUG"]:
            debug = True

    _g_basic_env.initiate(debug)
    env = _g_basic_env.env.Clone()

    conf = Configure(env, {"CheckForGccOption": checkForGCCOption })

    libs = []
    if "LIBS" in kwargs:
        libs.extend(kwargs["LIBS"])

    env['LIBS'] = []

    for lib in libs:
        if lib in env['CONFIGURED_LIBS']:
            env.Append(LIBS=env['CONFIGURED_LIBS'][lib])
        elif conf.CheckLib(lib + "-mt", language="C++"):
            _g_basic_env.env['CONFIGURED_LIBS'][lib] = lib + "-mt"
        elif conf.CheckLib(lib, language="C++"):
            _g_basic_env.env['CONFIGURED_LIBS'][lib] = lib
        else:
            print "We need C/C++ library %s." % lib
            Exit(1)

    env = conf.Finish()

    if "PKGS" in kwargs:
        pkgs = PkgConfig(kwargs["PKGS"])
        env["LIBS"].extend(pkgs.libs())
        env["CPPFLAGS"].extend(pkgs.cflags())

    return env
