r"""Wrapper for dss.h

Generated with:
/home/mcm/src/dss-opt/dss-opt.git/python/build.ctypesgen-venv/bin/ctypesgen -o dssopt/pylibdssopt.py -l libdssopt.so ../dss.h ../helpers-for-main.h ../helpers-for-main-opt.h ../na.h ../nj_param.h ../nn_param.h ../opt.h ../ctools/random.h

Do not modify this file.
"""

__docformat__ = "restructuredtext"

# Begin preamble for Python

import ctypes
import sys
from ctypes import *  # noqa: F401, F403

_int_types = (ctypes.c_int16, ctypes.c_int32)
if hasattr(ctypes, "c_int64"):
    # Some builds of ctypes apparently do not have ctypes.c_int64
    # defined; it's a pretty good bet that these builds do not
    # have 64-bit pointers.
    _int_types += (ctypes.c_int64,)
for t in _int_types:
    if ctypes.sizeof(t) == ctypes.sizeof(ctypes.c_size_t):
        c_ptrdiff_t = t
del t
del _int_types



class UserString:
    def __init__(self, seq):
        if isinstance(seq, bytes):
            self.data = seq
        elif isinstance(seq, UserString):
            self.data = seq.data[:]
        else:
            self.data = str(seq).encode()

    def __bytes__(self):
        return self.data

    def __str__(self):
        return self.data.decode()

    def __repr__(self):
        return repr(self.data)

    def __int__(self):
        return int(self.data.decode())

    def __long__(self):
        return int(self.data.decode())

    def __float__(self):
        return float(self.data.decode())

    def __complex__(self):
        return complex(self.data.decode())

    def __hash__(self):
        return hash(self.data)

    def __le__(self, string):
        if isinstance(string, UserString):
            return self.data <= string.data
        else:
            return self.data <= string

    def __lt__(self, string):
        if isinstance(string, UserString):
            return self.data < string.data
        else:
            return self.data < string

    def __ge__(self, string):
        if isinstance(string, UserString):
            return self.data >= string.data
        else:
            return self.data >= string

    def __gt__(self, string):
        if isinstance(string, UserString):
            return self.data > string.data
        else:
            return self.data > string

    def __eq__(self, string):
        if isinstance(string, UserString):
            return self.data == string.data
        else:
            return self.data == string

    def __ne__(self, string):
        if isinstance(string, UserString):
            return self.data != string.data
        else:
            return self.data != string

    def __contains__(self, char):
        return char in self.data

    def __len__(self):
        return len(self.data)

    def __getitem__(self, index):
        return self.__class__(self.data[index])

    def __getslice__(self, start, end):
        start = max(start, 0)
        end = max(end, 0)
        return self.__class__(self.data[start:end])

    def __add__(self, other):
        if isinstance(other, UserString):
            return self.__class__(self.data + other.data)
        elif isinstance(other, bytes):
            return self.__class__(self.data + other)
        else:
            return self.__class__(self.data + str(other).encode())

    def __radd__(self, other):
        if isinstance(other, bytes):
            return self.__class__(other + self.data)
        else:
            return self.__class__(str(other).encode() + self.data)

    def __mul__(self, n):
        return self.__class__(self.data * n)

    __rmul__ = __mul__

    def __mod__(self, args):
        return self.__class__(self.data % args)

    # the following methods are defined in alphabetical order:
    def capitalize(self):
        return self.__class__(self.data.capitalize())

    def center(self, width, *args):
        return self.__class__(self.data.center(width, *args))

    def count(self, sub, start=0, end=sys.maxsize):
        return self.data.count(sub, start, end)

    def decode(self, encoding=None, errors=None):  # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.decode(encoding, errors))
            else:
                return self.__class__(self.data.decode(encoding))
        else:
            return self.__class__(self.data.decode())

    def encode(self, encoding=None, errors=None):  # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.encode(encoding, errors))
            else:
                return self.__class__(self.data.encode(encoding))
        else:
            return self.__class__(self.data.encode())

    def endswith(self, suffix, start=0, end=sys.maxsize):
        return self.data.endswith(suffix, start, end)

    def expandtabs(self, tabsize=8):
        return self.__class__(self.data.expandtabs(tabsize))

    def find(self, sub, start=0, end=sys.maxsize):
        return self.data.find(sub, start, end)

    def index(self, sub, start=0, end=sys.maxsize):
        return self.data.index(sub, start, end)

    def isalpha(self):
        return self.data.isalpha()

    def isalnum(self):
        return self.data.isalnum()

    def isdecimal(self):
        return self.data.isdecimal()

    def isdigit(self):
        return self.data.isdigit()

    def islower(self):
        return self.data.islower()

    def isnumeric(self):
        return self.data.isnumeric()

    def isspace(self):
        return self.data.isspace()

    def istitle(self):
        return self.data.istitle()

    def isupper(self):
        return self.data.isupper()

    def join(self, seq):
        return self.data.join(seq)

    def ljust(self, width, *args):
        return self.__class__(self.data.ljust(width, *args))

    def lower(self):
        return self.__class__(self.data.lower())

    def lstrip(self, chars=None):
        return self.__class__(self.data.lstrip(chars))

    def partition(self, sep):
        return self.data.partition(sep)

    def replace(self, old, new, maxsplit=-1):
        return self.__class__(self.data.replace(old, new, maxsplit))

    def rfind(self, sub, start=0, end=sys.maxsize):
        return self.data.rfind(sub, start, end)

    def rindex(self, sub, start=0, end=sys.maxsize):
        return self.data.rindex(sub, start, end)

    def rjust(self, width, *args):
        return self.__class__(self.data.rjust(width, *args))

    def rpartition(self, sep):
        return self.data.rpartition(sep)

    def rstrip(self, chars=None):
        return self.__class__(self.data.rstrip(chars))

    def split(self, sep=None, maxsplit=-1):
        return self.data.split(sep, maxsplit)

    def rsplit(self, sep=None, maxsplit=-1):
        return self.data.rsplit(sep, maxsplit)

    def splitlines(self, keepends=0):
        return self.data.splitlines(keepends)

    def startswith(self, prefix, start=0, end=sys.maxsize):
        return self.data.startswith(prefix, start, end)

    def strip(self, chars=None):
        return self.__class__(self.data.strip(chars))

    def swapcase(self):
        return self.__class__(self.data.swapcase())

    def title(self):
        return self.__class__(self.data.title())

    def translate(self, *args):
        return self.__class__(self.data.translate(*args))

    def upper(self):
        return self.__class__(self.data.upper())

    def zfill(self, width):
        return self.__class__(self.data.zfill(width))


class MutableString(UserString):
    """mutable string objects

    Python strings are immutable objects.  This has the advantage, that
    strings may be used as dictionary keys.  If this property isn't needed
    and you insist on changing string values in place instead, you may cheat
    and use MutableString.

    But the purpose of this class is an educational one: to prevent
    people from inventing their own mutable string class derived
    from UserString and than forget thereby to remove (override) the
    __hash__ method inherited from UserString.  This would lead to
    errors that would be very hard to track down.

    A faster and better solution is to rewrite your program using lists."""

    def __init__(self, string=""):
        self.data = string

    def __hash__(self):
        raise TypeError("unhashable type (it is mutable)")

    def __setitem__(self, index, sub):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data):
            raise IndexError
        self.data = self.data[:index] + sub + self.data[index + 1 :]

    def __delitem__(self, index):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data):
            raise IndexError
        self.data = self.data[:index] + self.data[index + 1 :]

    def __setslice__(self, start, end, sub):
        start = max(start, 0)
        end = max(end, 0)
        if isinstance(sub, UserString):
            self.data = self.data[:start] + sub.data + self.data[end:]
        elif isinstance(sub, bytes):
            self.data = self.data[:start] + sub + self.data[end:]
        else:
            self.data = self.data[:start] + str(sub).encode() + self.data[end:]

    def __delslice__(self, start, end):
        start = max(start, 0)
        end = max(end, 0)
        self.data = self.data[:start] + self.data[end:]

    def immutable(self):
        return UserString(self.data)

    def __iadd__(self, other):
        if isinstance(other, UserString):
            self.data += other.data
        elif isinstance(other, bytes):
            self.data += other
        else:
            self.data += str(other).encode()
        return self

    def __imul__(self, n):
        self.data *= n
        return self


class String(MutableString, ctypes.Union):

    _fields_ = [("raw", ctypes.POINTER(ctypes.c_char)), ("data", ctypes.c_char_p)]

    def __init__(self, obj=b""):
        if isinstance(obj, (bytes, UserString)):
            self.data = bytes(obj)
        else:
            self.raw = obj

    def __len__(self):
        return self.data and len(self.data) or 0

    def from_param(cls, obj):
        # Convert None or 0
        if obj is None or obj == 0:
            return cls(ctypes.POINTER(ctypes.c_char)())

        # Convert from String
        elif isinstance(obj, String):
            return obj

        # Convert from bytes
        elif isinstance(obj, bytes):
            return cls(obj)

        # Convert from str
        elif isinstance(obj, str):
            return cls(obj.encode())

        # Convert from c_char_p
        elif isinstance(obj, ctypes.c_char_p):
            return obj

        # Convert from POINTER(ctypes.c_char)
        elif isinstance(obj, ctypes.POINTER(ctypes.c_char)):
            return obj

        # Convert from raw pointer
        elif isinstance(obj, int):
            return cls(ctypes.cast(obj, ctypes.POINTER(ctypes.c_char)))

        # Convert from ctypes.c_char array
        elif isinstance(obj, ctypes.c_char * len(obj)):
            return obj

        # Convert from object
        else:
            return String.from_param(obj._as_parameter_)

    from_param = classmethod(from_param)


def ReturnString(obj, func=None, arguments=None):
    return String.from_param(obj)


# As of ctypes 1.0, ctypes does not support custom error-checking
# functions on callbacks, nor does it support custom datatypes on
# callbacks, so we must ensure that all callbacks return
# primitive datatypes.
#
# Non-primitive return values wrapped with UNCHECKED won't be
# typechecked, and will be converted to ctypes.c_void_p.
def UNCHECKED(type):
    if hasattr(type, "_type_") and isinstance(type._type_, str) and type._type_ != "P":
        return type
    else:
        return ctypes.c_void_p


# ctypes doesn't have direct support for variadic functions, so we have to write
# our own wrapper class
class _variadic_function(object):
    def __init__(self, func, restype, argtypes, errcheck):
        self.func = func
        self.func.restype = restype
        self.argtypes = argtypes
        if errcheck:
            self.func.errcheck = errcheck

    def _as_parameter_(self):
        # So we can pass this variadic function as a function pointer
        return self.func

    def __call__(self, *args):
        fixed_args = []
        i = 0
        for argtype in self.argtypes:
            # Typecheck what we can
            fixed_args.append(argtype.from_param(args[i]))
            i += 1
        return self.func(*fixed_args + list(args[i:]))


def ord_if_char(value):
    """
    Simple helper used for casts to simple builtin types:  if the argument is a
    string type, it will be converted to it's ordinal value.

    This function will raise an exception if the argument is string with more
    than one characters.
    """
    return ord(value) if (isinstance(value, bytes) or isinstance(value, str)) else value

# End preamble

_libs = {}
_libdirs = []

# Begin loader

"""
Load libraries - appropriately for all our supported platforms
"""
# ----------------------------------------------------------------------------
# Copyright (c) 2008 David James
# Copyright (c) 2006-2008 Alex Holkner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of pyglet nor the names of its
#    contributors may be used to endorse or promote products
#    derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

import ctypes
import ctypes.util
import glob
import os.path
import platform
import re
import sys


def _environ_path(name):
    """Split an environment variable into a path-like list elements"""
    if name in os.environ:
        return os.environ[name].split(":")
    return []


class LibraryLoader:
    """
    A base class For loading of libraries ;-)
    Subclasses load libraries for specific platforms.
    """

    # library names formatted specifically for platforms
    name_formats = ["%s"]

    class Lookup:
        """Looking up calling conventions for a platform"""

        mode = ctypes.DEFAULT_MODE

        def __init__(self, path):
            super(LibraryLoader.Lookup, self).__init__()
            self.access = dict(cdecl=ctypes.CDLL(path, self.mode))

        def get(self, name, calling_convention="cdecl"):
            """Return the given name according to the selected calling convention"""
            if calling_convention not in self.access:
                raise LookupError(
                    "Unknown calling convention '{}' for function '{}'".format(
                        calling_convention, name
                    )
                )
            return getattr(self.access[calling_convention], name)

        def has(self, name, calling_convention="cdecl"):
            """Return True if this given calling convention finds the given 'name'"""
            if calling_convention not in self.access:
                return False
            return hasattr(self.access[calling_convention], name)

        def __getattr__(self, name):
            return getattr(self.access["cdecl"], name)

    def __init__(self):
        self.other_dirs = []

    def __call__(self, libname):
        """Given the name of a library, load it."""
        paths = self.getpaths(libname)

        for path in paths:
            # noinspection PyBroadException
            try:
                return self.Lookup(path)
            except Exception:  # pylint: disable=broad-except
                pass

        raise ImportError("Could not load %s." % libname)

    def getpaths(self, libname):
        """Return a list of paths where the library might be found."""
        if os.path.isabs(libname):
            yield libname
        else:
            # search through a prioritized series of locations for the library

            # we first search any specific directories identified by user
            for dir_i in self.other_dirs:
                for fmt in self.name_formats:
                    # dir_i should be absolute already
                    yield os.path.join(dir_i, fmt % libname)

            # check if this code is even stored in a physical file
            try:
                this_file = __file__
            except NameError:
                this_file = None

            # then we search the directory where the generated python interface is stored
            if this_file is not None:
                for fmt in self.name_formats:
                    yield os.path.abspath(os.path.join(os.path.dirname(__file__), fmt % libname))

            # now, use the ctypes tools to try to find the library
            for fmt in self.name_formats:
                path = ctypes.util.find_library(fmt % libname)
                if path:
                    yield path

            # then we search all paths identified as platform-specific lib paths
            for path in self.getplatformpaths(libname):
                yield path

            # Finally, we'll try the users current working directory
            for fmt in self.name_formats:
                yield os.path.abspath(os.path.join(os.path.curdir, fmt % libname))

    def getplatformpaths(self, _libname):  # pylint: disable=no-self-use
        """Return all the library paths available in this platform"""
        return []


# Darwin (Mac OS X)


class DarwinLibraryLoader(LibraryLoader):
    """Library loader for MacOS"""

    name_formats = [
        "lib%s.dylib",
        "lib%s.so",
        "lib%s.bundle",
        "%s.dylib",
        "%s.so",
        "%s.bundle",
        "%s",
    ]

    class Lookup(LibraryLoader.Lookup):
        """
        Looking up library files for this platform (Darwin aka MacOS)
        """

        # Darwin requires dlopen to be called with mode RTLD_GLOBAL instead
        # of the default RTLD_LOCAL.  Without this, you end up with
        # libraries not being loadable, resulting in "Symbol not found"
        # errors
        mode = ctypes.RTLD_GLOBAL

    def getplatformpaths(self, libname):
        if os.path.pathsep in libname:
            names = [libname]
        else:
            names = [fmt % libname for fmt in self.name_formats]

        for directory in self.getdirs(libname):
            for name in names:
                yield os.path.join(directory, name)

    @staticmethod
    def getdirs(libname):
        """Implements the dylib search as specified in Apple documentation:

        http://developer.apple.com/documentation/DeveloperTools/Conceptual/
            DynamicLibraries/Articles/DynamicLibraryUsageGuidelines.html

        Before commencing the standard search, the method first checks
        the bundle's ``Frameworks`` directory if the application is running
        within a bundle (OS X .app).
        """

        dyld_fallback_library_path = _environ_path("DYLD_FALLBACK_LIBRARY_PATH")
        if not dyld_fallback_library_path:
            dyld_fallback_library_path = [
                os.path.expanduser("~/lib"),
                "/usr/local/lib",
                "/usr/lib",
            ]

        dirs = []

        if "/" in libname:
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
        else:
            dirs.extend(_environ_path("LD_LIBRARY_PATH"))
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
            dirs.extend(_environ_path("LD_RUN_PATH"))

        if hasattr(sys, "frozen") and getattr(sys, "frozen") == "macosx_app":
            dirs.append(os.path.join(os.environ["RESOURCEPATH"], "..", "Frameworks"))

        dirs.extend(dyld_fallback_library_path)

        return dirs


# Posix


class PosixLibraryLoader(LibraryLoader):
    """Library loader for POSIX-like systems (including Linux)"""

    _ld_so_cache = None

    _include = re.compile(r"^\s*include\s+(?P<pattern>.*)")

    name_formats = ["lib%s.so", "%s.so", "%s"]

    class _Directories(dict):
        """Deal with directories"""

        def __init__(self):
            dict.__init__(self)
            self.order = 0

        def add(self, directory):
            """Add a directory to our current set of directories"""
            if len(directory) > 1:
                directory = directory.rstrip(os.path.sep)
            # only adds and updates order if exists and not already in set
            if not os.path.exists(directory):
                return
            order = self.setdefault(directory, self.order)
            if order == self.order:
                self.order += 1

        def extend(self, directories):
            """Add a list of directories to our set"""
            for a_dir in directories:
                self.add(a_dir)

        def ordered(self):
            """Sort the list of directories"""
            return (i[0] for i in sorted(self.items(), key=lambda d: d[1]))

    def _get_ld_so_conf_dirs(self, conf, dirs):
        """
        Recursive function to help parse all ld.so.conf files, including proper
        handling of the `include` directive.
        """

        try:
            with open(conf) as fileobj:
                for dirname in fileobj:
                    dirname = dirname.strip()
                    if not dirname:
                        continue

                    match = self._include.match(dirname)
                    if not match:
                        dirs.add(dirname)
                    else:
                        for dir2 in glob.glob(match.group("pattern")):
                            self._get_ld_so_conf_dirs(dir2, dirs)
        except IOError:
            pass

    def _create_ld_so_cache(self):
        # Recreate search path followed by ld.so.  This is going to be
        # slow to build, and incorrect (ld.so uses ld.so.cache, which may
        # not be up-to-date).  Used only as fallback for distros without
        # /sbin/ldconfig.
        #
        # We assume the DT_RPATH and DT_RUNPATH binary sections are omitted.

        directories = self._Directories()
        for name in (
            "LD_LIBRARY_PATH",
            "SHLIB_PATH",  # HP-UX
            "LIBPATH",  # OS/2, AIX
            "LIBRARY_PATH",  # BE/OS
        ):
            if name in os.environ:
                directories.extend(os.environ[name].split(os.pathsep))

        self._get_ld_so_conf_dirs("/etc/ld.so.conf", directories)

        bitage = platform.architecture()[0]

        unix_lib_dirs_list = []
        if bitage.startswith("64"):
            # prefer 64 bit if that is our arch
            unix_lib_dirs_list += ["/lib64", "/usr/lib64"]

        # must include standard libs, since those paths are also used by 64 bit
        # installs
        unix_lib_dirs_list += ["/lib", "/usr/lib"]
        if sys.platform.startswith("linux"):
            # Try and support multiarch work in Ubuntu
            # https://wiki.ubuntu.com/MultiarchSpec
            if bitage.startswith("32"):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ["/lib/i386-linux-gnu", "/usr/lib/i386-linux-gnu"]
            elif bitage.startswith("64"):
                # Assume Intel/AMD x86 compatible
                unix_lib_dirs_list += [
                    "/lib/x86_64-linux-gnu",
                    "/usr/lib/x86_64-linux-gnu",
                ]
            else:
                # guess...
                unix_lib_dirs_list += glob.glob("/lib/*linux-gnu")
        directories.extend(unix_lib_dirs_list)

        cache = {}
        lib_re = re.compile(r"lib(.*)\.s[ol]")
        # ext_re = re.compile(r"\.s[ol]$")
        for our_dir in directories.ordered():
            try:
                for path in glob.glob("%s/*.s[ol]*" % our_dir):
                    file = os.path.basename(path)

                    # Index by filename
                    cache_i = cache.setdefault(file, set())
                    cache_i.add(path)

                    # Index by library name
                    match = lib_re.match(file)
                    if match:
                        library = match.group(1)
                        cache_i = cache.setdefault(library, set())
                        cache_i.add(path)
            except OSError:
                pass

        self._ld_so_cache = cache

    def getplatformpaths(self, libname):
        if self._ld_so_cache is None:
            self._create_ld_so_cache()

        result = self._ld_so_cache.get(libname, set())
        for i in result:
            # we iterate through all found paths for library, since we may have
            # actually found multiple architectures or other library types that
            # may not load
            yield i


# Windows


class WindowsLibraryLoader(LibraryLoader):
    """Library loader for Microsoft Windows"""

    name_formats = ["%s.dll", "lib%s.dll", "%slib.dll", "%s"]

    class Lookup(LibraryLoader.Lookup):
        """Lookup class for Windows libraries..."""

        def __init__(self, path):
            super(WindowsLibraryLoader.Lookup, self).__init__(path)
            self.access["stdcall"] = ctypes.windll.LoadLibrary(path)


# Platform switching

# If your value of sys.platform does not appear in this dict, please contact
# the Ctypesgen maintainers.

loaderclass = {
    "darwin": DarwinLibraryLoader,
    "cygwin": WindowsLibraryLoader,
    "win32": WindowsLibraryLoader,
    "msys": WindowsLibraryLoader,
}

load_library = loaderclass.get(sys.platform, PosixLibraryLoader)()


def add_library_search_dirs(other_dirs):
    """
    Add libraries to search paths.
    If library paths are relative, convert them to absolute with respect to this
    file's directory
    """
    for path in other_dirs:
        if not os.path.isabs(path):
            path = os.path.abspath(path)
        load_library.other_dirs.append(path)


del loaderclass

# End loader

add_library_search_dirs([])

# Begin libraries
_libs["libdssopt.so"] = load_library("libdssopt.so")

# 1 libraries
# End libraries

# No modules

uint = c_uint# /usr/include/x86_64-linux-gnu/sys/types.h: 150

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 7
for _lib in _libs.values():
    if not _lib.has("dss_calc_U_pa", "cdecl"):
        continue
    dss_calc_U_pa = _lib.get("dss_calc_U_pa", "cdecl")
    dss_calc_U_pa.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double]
    dss_calc_U_pa.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 8
for _lib in _libs.values():
    if not _lib.has("dss_calc_gradU_pa", "cdecl"):
        continue
    dss_calc_gradU_pa = _lib.get("dss_calc_gradU_pa", "cdecl")
    dss_calc_gradU_pa.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, POINTER(POINTER(c_double))]
    dss_calc_gradU_pa.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 11
for _lib in _libs.values():
    if not _lib.has("dss_calc_U_pi", "cdecl"):
        continue
    dss_calc_U_pi = _lib.get("dss_calc_U_pi", "cdecl")
    dss_calc_U_pi.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double]
    dss_calc_U_pi.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 12
for _lib in _libs.values():
    if not _lib.has("dss_calc_gradU_pi", "cdecl"):
        continue
    dss_calc_gradU_pi = _lib.get("dss_calc_gradU_pi", "cdecl")
    dss_calc_gradU_pi.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, POINTER(POINTER(c_double))]
    dss_calc_gradU_pi.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 15
for _lib in _libs.values():
    if not _lib.has("dss_calc_U_pur_cauchy", "cdecl"):
        continue
    dss_calc_U_pur_cauchy = _lib.get("dss_calc_U_pur_cauchy", "cdecl")
    dss_calc_U_pur_cauchy.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double]
    dss_calc_U_pur_cauchy.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 16
for _lib in _libs.values():
    if not _lib.has("dss_calc_gradU_pur_cauchy", "cdecl"):
        continue
    dss_calc_gradU_pur_cauchy = _lib.get("dss_calc_gradU_pur_cauchy", "cdecl")
    dss_calc_gradU_pur_cauchy.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, POINTER(POINTER(c_double))]
    dss_calc_gradU_pur_cauchy.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 19
for _lib in _libs.values():
    if not _lib.has("dss_calc_U_negdesign_het", "cdecl"):
        continue
    dss_calc_U_negdesign_het = _lib.get("dss_calc_U_negdesign_het", "cdecl")
    dss_calc_U_negdesign_het.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double]
    dss_calc_U_negdesign_het.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 20
for _lib in _libs.values():
    if not _lib.has("dss_calc_gradU_negdesign_het", "cdecl"):
        continue
    dss_calc_gradU_negdesign_het = _lib.get("dss_calc_gradU_negdesign_het", "cdecl")
    dss_calc_gradU_negdesign_het.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, POINTER(POINTER(c_double))]
    dss_calc_gradU_negdesign_het.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 23
for _lib in _libs.values():
    if not _lib.has("dss_calc_U_negdesign_nj", "cdecl"):
        continue
    dss_calc_U_negdesign_nj = _lib.get("dss_calc_U_negdesign_nj", "cdecl")
    dss_calc_U_negdesign_nj.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, POINTER(POINTER(c_double)), POINTER(uint)]
    dss_calc_U_negdesign_nj.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 25
for _lib in _libs.values():
    if not _lib.has("dss_calc_gradU_negdesign_nj", "cdecl"):
        continue
    dss_calc_gradU_negdesign_nj = _lib.get("dss_calc_gradU_negdesign_nj", "cdecl")
    dss_calc_gradU_negdesign_nj.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, POINTER(POINTER(c_double)), POINTER(uint), POINTER(POINTER(c_double))]
    dss_calc_gradU_negdesign_nj.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 28
for _lib in _libs.values():
    if not _lib.has("dss_calc_U_het", "cdecl"):
        continue
    dss_calc_U_het = _lib.get("dss_calc_U_het", "cdecl")
    dss_calc_U_het.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, uint, POINTER(uint)]
    dss_calc_U_het.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/dss.h: 30
for _lib in _libs.values():
    if not _lib.has("dss_calc_gradU_het", "cdecl"):
        continue
    dss_calc_gradU_het = _lib.get("dss_calc_gradU_het", "cdecl")
    dss_calc_gradU_het.argtypes = [POINTER(POINTER(c_double)), uint, uint, c_double, uint, POINTER(uint), POINTER(POINTER(c_double))]
    dss_calc_gradU_het.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 9
for _lib in _libs.values():
    if not _lib.has("helper_make_K_nj_alloc", "cdecl"):
        continue
    helper_make_K_nj_alloc = _lib.get("helper_make_K_nj_alloc", "cdecl")
    helper_make_K_nj_alloc.argtypes = [c_size_t]
    helper_make_K_nj_alloc.restype = POINTER(POINTER(c_double))
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 10
for _lib in _libs.values():
    if not _lib.has("system_is_exploded", "cdecl"):
        continue
    system_is_exploded = _lib.get("system_is_exploded", "cdecl")
    system_is_exploded.argtypes = [POINTER(POINTER(c_double)), c_size_t, c_size_t]
    system_is_exploded.restype = c_bool
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 11
for _lib in _libs.values():
    if not _lib.has("calc_entropy", "cdecl"):
        continue
    calc_entropy = _lib.get("calc_entropy", "cdecl")
    calc_entropy.argtypes = [POINTER(POINTER(c_double)), c_size_t, c_size_t]
    calc_entropy.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 12
for _lib in _libs.values():
    if not _lib.has("is_bad_bp", "cdecl"):
        continue
    is_bad_bp = _lib.get("is_bad_bp", "cdecl")
    is_bad_bp.argtypes = [String, uint, uint]
    is_bad_bp.restype = c_bool
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 13
for _lib in _libs.values():
    if not _lib.has("show_bad_bp", "cdecl"):
        continue
    show_bad_bp = _lib.get("show_bad_bp", "cdecl")
    show_bad_bp.argtypes = [String, POINTER(uint), c_size_t, c_bool]
    show_bad_bp.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 14
for _lib in _libs.values():
    if not _lib.has("fix_bad_bp", "cdecl"):
        continue
    fix_bad_bp = _lib.get("fix_bad_bp", "cdecl")
    fix_bad_bp.argtypes = [String, POINTER(uint), c_size_t]
    fix_bad_bp.restype = c_size_t
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 15
for _lib in _libs.values():
    if not _lib.has("show_bad_prob", "cdecl"):
        continue
    show_bad_prob = _lib.get("show_bad_prob", "cdecl")
    show_bad_prob.argtypes = [POINTER(POINTER(c_double)), c_size_t, c_size_t, c_bool]
    show_bad_prob.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main.h: 16
for _lib in _libs.values():
    if not _lib.has("print_for_movie", "cdecl"):
        continue
    print_for_movie = _lib.get("print_for_movie", "cdecl")
    print_for_movie.argtypes = [POINTER(POINTER(c_double)), c_size_t, c_size_t, String]
    print_for_movie.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 18
for _lib in _libs.values():
    try:
        nn_G_stack = ((c_int * int(6)) * int(6)).in_dll(_lib, "nn_G_stack")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 19
for _lib in _libs.values():
    try:
        nn_G_mismatch_hairpin = (((c_int * int(4)) * int(4)) * int(6)).in_dll(_lib, "nn_G_mismatch_hairpin")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 20
for _lib in _libs.values():
    try:
        nn_G_mismatch_interior = (((c_int * int(4)) * int(4)) * int(6)).in_dll(_lib, "nn_G_mismatch_interior")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 21
for _lib in _libs.values():
    try:
        nn_G_dangle5 = ((c_int * int(4)) * int(6)).in_dll(_lib, "nn_G_dangle5")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 22
for _lib in _libs.values():
    try:
        nn_G_dangle3 = ((c_int * int(4)) * int(6)).in_dll(_lib, "nn_G_dangle3")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 23
for _lib in _libs.values():
    try:
        nn_G_int11 = ((((c_int * int(4)) * int(4)) * int(6)) * int(6)).in_dll(_lib, "nn_G_int11")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 24
for _lib in _libs.values():
    try:
        nn_G_int21 = (((((c_int * int(4)) * int(4)) * int(4)) * int(6)) * int(6)).in_dll(_lib, "nn_G_int21")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 25
for _lib in _libs.values():
    try:
        nn_G_int22 = ((((((c_int * int(4)) * int(4)) * int(4)) * int(4)) * int(6)) * int(6)).in_dll(_lib, "nn_G_int22")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 26
for _lib in _libs.values():
    try:
        nn_G_hairpin_loop = (c_int * int(31)).in_dll(_lib, "nn_G_hairpin_loop")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 27
for _lib in _libs.values():
    try:
        nn_G_bulge_loop = (c_int * int(31)).in_dll(_lib, "nn_G_bulge_loop")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 28
for _lib in _libs.values():
    try:
        nn_G_internal_loop = (c_int * int(31)).in_dll(_lib, "nn_G_internal_loop")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 29
for _lib in _libs.values():
    try:
        nn_G_ml_param = (c_int * int(4)).in_dll(_lib, "nn_G_ml_param")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 30
for _lib in _libs.values():
    try:
        nn_G_ninio = (c_int * int(2)).in_dll(_lib, "nn_G_ninio")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 31
for _lib in _libs.values():
    try:
        nn_tetraloop_bp = (POINTER(c_uint)).in_dll(_lib, "nn_tetraloop_bp")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 32
for _lib in _libs.values():
    try:
        nn_tetraloop_loop = (POINTER(c_uint * int(4))).in_dll(_lib, "nn_tetraloop_loop")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 33
for _lib in _libs.values():
    try:
        nn_G_tetraloop = (POINTER(c_int)).in_dll(_lib, "nn_G_tetraloop")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 20
for _lib in _libs.values():
    try:
        NA_NBASES = (uint).in_dll(_lib, "NA_NBASES")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 21
for _lib in _libs.values():
    try:
        NA_BASE_NAMES = (POINTER(c_char)).in_dll(_lib, "NA_BASE_NAMES")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 22
for _lib in _libs.values():
    try:
        NA_NBASEPAIRS = (uint).in_dll(_lib, "NA_NBASEPAIRS")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 23
for _lib in _libs.values():
    try:
        NA_BASEPAIR_NAMES = (POINTER(c_char * int(3))).in_dll(_lib, "NA_BASEPAIR_NAMES")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 24
for _lib in _libs.values():
    try:
        NA_2BASES_TO_PAIRTYPE = ((uint * int(4)) * int(4)).in_dll(_lib, "NA_2BASES_TO_PAIRTYPE")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 26
class struct_nn_stack(Structure):
    pass

struct_nn_stack.__slots__ = [
    'i1',
    'j1',
    'i2',
    'j2',
]
struct_nn_stack._fields_ = [
    ('i1', uint),
    ('j1', uint),
    ('i2', uint),
    ('j2', uint),
]

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 31
class struct_nn_hairpin(Structure):
    pass

struct_nn_hairpin.__slots__ = [
    'i',
    'j',
    'size',
]
struct_nn_hairpin._fields_ = [
    ('i', uint),
    ('j', uint),
    ('size', uint),
]

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 35
class struct_nn_bulge(Structure):
    pass

struct_nn_bulge.__slots__ = [
    'i1',
    'j1',
    'i2',
    'j2',
    'size',
]
struct_nn_bulge._fields_ = [
    ('i1', uint),
    ('j1', uint),
    ('i2', uint),
    ('j2', uint),
    ('size', uint),
]

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 39
class struct_nn_intloop(Structure):
    pass

struct_nn_intloop.__slots__ = [
    'i1',
    'j1',
    'i2',
    'j2',
    'size1',
    'size2',
]
struct_nn_intloop._fields_ = [
    ('i1', uint),
    ('j1', uint),
    ('i2', uint),
    ('j2', uint),
    ('size1', uint),
    ('size2', uint),
]

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 43
class struct_nn_multiloop(Structure):
    pass

struct_nn_multiloop.__slots__ = [
    'unpaired',
    'nstems',
    'ndangle5',
    'ndangle3',
    'stems',
    'dangle5',
    'dangle3',
]
struct_nn_multiloop._fields_ = [
    ('unpaired', uint),
    ('nstems', uint),
    ('ndangle5', uint),
    ('ndangle3', uint),
    ('stems', POINTER(uint * int(2))),
    ('dangle5', POINTER(uint * int(3))),
    ('dangle3', POINTER(uint * int(3))),
]

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 51
class struct_nn_inter(Structure):
    pass

struct_nn_inter.__slots__ = [
    'n',
    'nstack',
    'nhairpin',
    'nbulge',
    'nintloop',
    'nmultiloop',
    'pairs',
    'stack',
    'hairpin',
    'bulge',
    'intloop',
    'extloop',
    'multiloop',
]
struct_nn_inter._fields_ = [
    ('n', uint),
    ('nstack', uint),
    ('nhairpin', uint),
    ('nbulge', uint),
    ('nintloop', uint),
    ('nmultiloop', uint),
    ('pairs', POINTER(uint)),
    ('stack', POINTER(struct_nn_stack)),
    ('hairpin', POINTER(struct_nn_hairpin)),
    ('bulge', POINTER(struct_nn_bulge)),
    ('intloop', POINTER(struct_nn_intloop)),
    ('extloop', struct_nn_multiloop),
    ('multiloop', POINTER(struct_nn_multiloop)),
]

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 64
for _lib in _libs.values():
    if not _lib.has("vienna_to_pairs", "cdecl"):
        continue
    vienna_to_pairs = _lib.get("vienna_to_pairs", "cdecl")
    vienna_to_pairs.argtypes = [uint, String, c_bool, POINTER(uint)]
    vienna_to_pairs.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 65
for _lib in _libs.values():
    if not _lib.has("xvienna_to_pairs", "cdecl"):
        continue
    xvienna_to_pairs = _lib.get("xvienna_to_pairs", "cdecl")
    xvienna_to_pairs.argtypes = [uint, String, POINTER(uint)]
    xvienna_to_pairs.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 66
for _lib in _libs.values():
    if not _lib.has("pairs_to_vienna", "cdecl"):
        continue
    pairs_to_vienna = _lib.get("pairs_to_vienna", "cdecl")
    pairs_to_vienna.argtypes = [uint, POINTER(uint), c_bool, String]
    pairs_to_vienna.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 67
for _lib in _libs.values():
    if not _lib.has("xpairs_to_vienna", "cdecl"):
        continue
    xpairs_to_vienna = _lib.get("xpairs_to_vienna", "cdecl")
    xpairs_to_vienna.argtypes = [uint, POINTER(uint), String]
    if sizeof(c_int) == sizeof(c_void_p):
        xpairs_to_vienna.restype = ReturnString
    else:
        xpairs_to_vienna.restype = String
        xpairs_to_vienna.errcheck = ReturnString
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 68
for _lib in _libs.values():
    if not _lib.has("xstr_to_useq", "cdecl"):
        continue
    xstr_to_useq = _lib.get("xstr_to_useq", "cdecl")
    xstr_to_useq.argtypes = [uint, String, POINTER(uint)]
    xstr_to_useq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 69
for _lib in _libs.values():
    if not _lib.has("xstr_to_pseq", "cdecl"):
        continue
    xstr_to_pseq = _lib.get("xstr_to_pseq", "cdecl")
    xstr_to_pseq.argtypes = [uint, uint, String, POINTER(POINTER(c_double))]
    xstr_to_pseq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 70
for _lib in _libs.values():
    if not _lib.has("pseq_to_str", "cdecl"):
        continue
    pseq_to_str = _lib.get("pseq_to_str", "cdecl")
    pseq_to_str.argtypes = [POINTER(POINTER(c_double)), uint, uint, String]
    pseq_to_str.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 71
for _lib in _libs.values():
    if not _lib.has("xuseq_to_str", "cdecl"):
        continue
    xuseq_to_str = _lib.get("xuseq_to_str", "cdecl")
    xuseq_to_str.argtypes = [uint, POINTER(uint), String]
    xuseq_to_str.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 72
for _lib in _libs.values():
    if not _lib.has("useq_to_str", "cdecl"):
        continue
    useq_to_str = _lib.get("useq_to_str", "cdecl")
    useq_to_str.argtypes = [uint, POINTER(uint), c_bool, String]
    useq_to_str.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 74
for _lib in _libs.values():
    if not _lib.has("nn_inter_xnew", "cdecl"):
        continue
    nn_inter_xnew = _lib.get("nn_inter_xnew", "cdecl")
    nn_inter_xnew.argtypes = [uint]
    nn_inter_xnew.restype = POINTER(struct_nn_inter)
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 75
for _lib in _libs.values():
    if not _lib.has("nn_inter_delete", "cdecl"):
        continue
    nn_inter_delete = _lib.get("nn_inter_delete", "cdecl")
    nn_inter_delete.argtypes = [POINTER(struct_nn_inter)]
    nn_inter_delete.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 76
for _lib in _libs.values():
    if not _lib.has("find_interactions", "cdecl"):
        continue
    find_interactions = _lib.get("find_interactions", "cdecl")
    find_interactions.argtypes = [POINTER(struct_nn_inter)]
    find_interactions.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 77
for _lib in _libs.values():
    if not _lib.has("print_interactions", "cdecl"):
        continue
    print_interactions = _lib.get("print_interactions", "cdecl")
    print_interactions.argtypes = [POINTER(struct_nn_inter)]
    print_interactions.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 79
for _lib in _libs.values():
    if not _lib.has("random_pairs", "cdecl"):
        continue
    random_pairs = _lib.get("random_pairs", "cdecl")
    random_pairs.argtypes = [uint, POINTER(uint), uint]
    random_pairs.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 80
for _lib in _libs.values():
    if not _lib.has("random_useq", "cdecl"):
        continue
    random_useq = _lib.get("random_useq", "cdecl")
    random_useq.argtypes = [uint, POINTER(uint), POINTER(uint)]
    random_useq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 84
for _lib in _libs.values():
    if not _lib.has("calc_interactions_useq", "cdecl"):
        continue
    calc_interactions_useq = _lib.get("calc_interactions_useq", "cdecl")
    calc_interactions_useq.argtypes = [POINTER(struct_nn_inter), POINTER(uint)]
    calc_interactions_useq.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 85
for _lib in _libs.values():
    if not _lib.has("calc_interactions_pseq", "cdecl"):
        continue
    calc_interactions_pseq = _lib.get("calc_interactions_pseq", "cdecl")
    calc_interactions_pseq.argtypes = [POINTER(struct_nn_inter), POINTER(POINTER(c_double))]
    calc_interactions_pseq.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 86
for _lib in _libs.values():
    if not _lib.has("calc_interactions_dGdp_pseq", "cdecl"):
        continue
    calc_interactions_dGdp_pseq = _lib.get("calc_interactions_dGdp_pseq", "cdecl")
    calc_interactions_dGdp_pseq.argtypes = [POINTER(struct_nn_inter), POINTER(POINTER(c_double)), POINTER(POINTER(c_double))]
    calc_interactions_dGdp_pseq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 89
for _lib in _libs.values():
    if not _lib.has("G_stack", "cdecl"):
        continue
    G_stack = _lib.get("G_stack", "cdecl")
    G_stack.argtypes = [POINTER(uint), uint, uint, uint, uint]
    G_stack.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 90
for _lib in _libs.values():
    if not _lib.has("G_stack_pseq", "cdecl"):
        continue
    G_stack_pseq = _lib.get("G_stack_pseq", "cdecl")
    G_stack_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint, uint]
    G_stack_pseq.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 91
for _lib in _libs.values():
    if not _lib.has("dGdp_stack_pseq", "cdecl"):
        continue
    dGdp_stack_pseq = _lib.get("dGdp_stack_pseq", "cdecl")
    dGdp_stack_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint, uint, POINTER(POINTER(c_double))]
    dGdp_stack_pseq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 94
for _lib in _libs.values():
    if not _lib.has("G_bulgeloop", "cdecl"):
        continue
    G_bulgeloop = _lib.get("G_bulgeloop", "cdecl")
    G_bulgeloop.argtypes = [POINTER(uint), uint, uint, uint, uint, uint]
    G_bulgeloop.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 96
for _lib in _libs.values():
    if not _lib.has("G_bulgeloop_pseq", "cdecl"):
        continue
    G_bulgeloop_pseq = _lib.get("G_bulgeloop_pseq", "cdecl")
    G_bulgeloop_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint, uint, uint]
    G_bulgeloop_pseq.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 98
for _lib in _libs.values():
    if not _lib.has("dGdp_bulgeloop_pseq", "cdecl"):
        continue
    dGdp_bulgeloop_pseq = _lib.get("dGdp_bulgeloop_pseq", "cdecl")
    dGdp_bulgeloop_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint, uint, uint, POINTER(POINTER(c_double))]
    dGdp_bulgeloop_pseq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 101
for _lib in _libs.values():
    if not _lib.has("G_intloop", "cdecl"):
        continue
    G_intloop = _lib.get("G_intloop", "cdecl")
    G_intloop.argtypes = [POINTER(uint), uint, uint, uint, uint, uint, uint]
    G_intloop.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 103
for _lib in _libs.values():
    if not _lib.has("G_intloop_pseq", "cdecl"):
        continue
    G_intloop_pseq = _lib.get("G_intloop_pseq", "cdecl")
    G_intloop_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint, uint, uint, uint]
    G_intloop_pseq.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 105
for _lib in _libs.values():
    if not _lib.has("dGdp_intloop_pseq", "cdecl"):
        continue
    dGdp_intloop_pseq = _lib.get("dGdp_intloop_pseq", "cdecl")
    dGdp_intloop_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint, uint, uint, uint, POINTER(POINTER(c_double))]
    dGdp_intloop_pseq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 108
for _lib in _libs.values():
    if not _lib.has("G_hairpin", "cdecl"):
        continue
    G_hairpin = _lib.get("G_hairpin", "cdecl")
    G_hairpin.argtypes = [POINTER(uint), uint, uint, uint]
    G_hairpin.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 109
for _lib in _libs.values():
    if not _lib.has("G_hairpin_pseq", "cdecl"):
        continue
    G_hairpin_pseq = _lib.get("G_hairpin_pseq", "cdecl")
    G_hairpin_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint]
    G_hairpin_pseq.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 110
for _lib in _libs.values():
    if not _lib.has("dGdp_hairpin_pseq", "cdecl"):
        continue
    dGdp_hairpin_pseq = _lib.get("dGdp_hairpin_pseq", "cdecl")
    dGdp_hairpin_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, uint, POINTER(POINTER(c_double))]
    dGdp_hairpin_pseq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 113
for _lib in _libs.values():
    if not _lib.has("G_extloop_multiloop", "cdecl"):
        continue
    G_extloop_multiloop = _lib.get("G_extloop_multiloop", "cdecl")
    G_extloop_multiloop.argtypes = [POINTER(uint), uint, uint, POINTER(uint * int(2)), uint, POINTER(uint * int(3)), uint, POINTER(uint * int(3)), c_bool]
    G_extloop_multiloop.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 118
for _lib in _libs.values():
    if not _lib.has("G_extloop_multiloop_pseq", "cdecl"):
        continue
    G_extloop_multiloop_pseq = _lib.get("G_extloop_multiloop_pseq", "cdecl")
    G_extloop_multiloop_pseq.argtypes = [POINTER(POINTER(c_double)), uint, uint, POINTER(uint * int(2)), uint, POINTER(uint * int(3)), uint, POINTER(uint * int(3)), c_bool]
    G_extloop_multiloop_pseq.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 123
for _lib in _libs.values():
    if not _lib.has("dGdp_extloop_multiloop_pseq", "cdecl"):
        continue
    dGdp_extloop_multiloop_pseq = _lib.get("dGdp_extloop_multiloop_pseq", "cdecl")
    dGdp_extloop_multiloop_pseq.argtypes = [POINTER(POINTER(c_double)), uint, POINTER(uint * int(2)), uint, POINTER(uint * int(3)), uint, POINTER(uint * int(3)), POINTER(POINTER(c_double))]
    dGdp_extloop_multiloop_pseq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main-opt.h: 7
for _lib in _libs.values():
    if not _lib.has("x_ensure_positive", "cdecl"):
        continue
    x_ensure_positive = _lib.get("x_ensure_positive", "cdecl")
    x_ensure_positive.argtypes = [String, String, c_double]
    x_ensure_positive.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main-opt.h: 9
for _lib in _libs.values():
    if not _lib.has("set_dss_force_constants_defaults", "cdecl"):
        continue
    set_dss_force_constants_defaults = _lib.get("set_dss_force_constants_defaults", "cdecl")
    set_dss_force_constants_defaults.argtypes = [POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(uint)]
    set_dss_force_constants_defaults.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/helpers-for-main-opt.h: 12
for _lib in _libs.values():
    if not _lib.has("print_design_score_info_for_seq", "cdecl"):
        continue
    print_design_score_info_for_seq = _lib.get("print_design_score_info_for_seq", "cdecl")
    print_design_score_info_for_seq.argtypes = [POINTER(struct_nn_inter), String, uint, uint, POINTER(POINTER(c_double)), c_double, c_double, c_double, c_double, c_double, uint]
    print_design_score_info_for_seq.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/nj_param.h: 4
for _lib in _libs.values():
    try:
        K_NJ = ((c_double * int(4)) * int(4)).in_dll(_lib, "K_NJ")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 7
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_ndim = (uint).in_dll(_lib, "DSSOPT_DEFAULT_ndim")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 8
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_kpi = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_kpi")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 9
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_kpa = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_kpa")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 10
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_kneg = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_kneg")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 11
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_kpur = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_kpur")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 12
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_khet = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_khet")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 13
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_het_window = (uint).in_dll(_lib, "DSSOPT_DEFAULT_het_window")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 15
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_OPT_MD_time_total = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_OPT_MD_time_total")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 16
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_OPT_MD_time_print = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_OPT_MD_time_print")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 17
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_OPT_MD_timestep = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_OPT_MD_timestep")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 18
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_OPT_MD_T_start = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_OPT_MD_T_start")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 19
for _lib in _libs.values():
    if not _lib.has("dssopt_default_opt_md_time_cool", "cdecl"):
        continue
    dssopt_default_opt_md_time_cool = _lib.get("dssopt_default_opt_md_time_cool", "cdecl")
    dssopt_default_opt_md_time_cool.argtypes = [c_double]
    dssopt_default_opt_md_time_cool.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 20
for _lib in _libs.values():
    if not _lib.has("dssopt_default_opt_md_time_pur", "cdecl"):
        continue
    dssopt_default_opt_md_time_pur = _lib.get("dssopt_default_opt_md_time_pur", "cdecl")
    dssopt_default_opt_md_time_pur.argtypes = [c_double]
    dssopt_default_opt_md_time_pur.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 22
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_OPT_SD_maxsteps = (uint).in_dll(_lib, "DSSOPT_DEFAULT_OPT_SD_maxsteps")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 23
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_OPT_SD_nprint = (uint).in_dll(_lib, "DSSOPT_DEFAULT_OPT_SD_nprint")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 24
for _lib in _libs.values():
    try:
        DSSOPT_DEFAULT_OPT_SD_wiggle = (c_double).in_dll(_lib, "DSSOPT_DEFAULT_OPT_SD_wiggle")
        break
    except:
        pass

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 41
for _lib in _libs.values():
    if not _lib.has("x_parse_seq_constraints_hard", "cdecl"):
        continue
    x_parse_seq_constraints_hard = _lib.get("x_parse_seq_constraints_hard", "cdecl")
    x_parse_seq_constraints_hard.argtypes = [uint, POINTER(uint), String, POINTER(uint)]
    x_parse_seq_constraints_hard.restype = uint
    break

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 61
for _lib in _libs.values():
    if not _lib.has("parse_seq_constraints_hard", "cdecl"):
        continue
    parse_seq_constraints_hard = _lib.get("parse_seq_constraints_hard", "cdecl")
    parse_seq_constraints_hard.argtypes = [uint, POINTER(uint), POINTER(uint), String, c_bool, POINTER(uint)]
    parse_seq_constraints_hard.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 94
for _lib in _libs.values():
    if not _lib.has("run_md", "cdecl"):
        continue
    run_md = _lib.get("run_md", "cdecl")
    run_md.argtypes = [String, String, uint, uint, uint, uint, c_double, c_double, c_double, c_double, c_double, c_double, uint, c_double, c_bool, c_bool, c_bool, String]
    run_md.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/opt.h: 124
for _lib in _libs.values():
    if not _lib.has("run_sd", "cdecl"):
        continue
    run_sd = _lib.get("run_sd", "cdecl")
    run_sd.argtypes = [String, uint, uint, c_double, c_double, c_double, c_double, c_double, c_double, uint, c_bool, c_bool, String]
    run_sd.restype = c_int
    break

# /home/mcm/src/dss-opt/dss-opt.git/ctools/random.h: 9
for _lib in _libs.values():
    if not _lib.has("random_seed", "cdecl"):
        continue
    random_seed = _lib.get("random_seed", "cdecl")
    random_seed.argtypes = [uint]
    random_seed.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/ctools/random.h: 12
for _lib in _libs.values():
    if not _lib.has("random_get_seedval_from_current_time", "cdecl"):
        continue
    random_get_seedval_from_current_time = _lib.get("random_get_seedval_from_current_time", "cdecl")
    random_get_seedval_from_current_time.argtypes = []
    random_get_seedval_from_current_time.restype = uint
    break

# /home/mcm/src/dss-opt/dss-opt.git/ctools/random.h: 15
for _lib in _libs.values():
    if not _lib.has("random_seed_with_current_time", "cdecl"):
        continue
    random_seed_with_current_time = _lib.get("random_seed_with_current_time", "cdecl")
    random_seed_with_current_time.argtypes = []
    random_seed_with_current_time.restype = None
    break

# /home/mcm/src/dss-opt/dss-opt.git/ctools/random.h: 18
for _lib in _libs.values():
    if not _lib.has("random_uint", "cdecl"):
        continue
    random_uint = _lib.get("random_uint", "cdecl")
    random_uint.argtypes = [uint, uint]
    random_uint.restype = uint
    break

# /home/mcm/src/dss-opt/dss-opt.git/ctools/random.h: 22
for _lib in _libs.values():
    if not _lib.has("random_double_uniform", "cdecl"):
        continue
    random_double_uniform = _lib.get("random_double_uniform", "cdecl")
    random_double_uniform.argtypes = []
    random_double_uniform.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/ctools/random.h: 25
for _lib in _libs.values():
    if not _lib.has("random_double_normal", "cdecl"):
        continue
    random_double_normal = _lib.get("random_double_normal", "cdecl")
    random_double_normal.argtypes = [c_double, c_double]
    random_double_normal.restype = c_double
    break

# /home/mcm/src/dss-opt/dss-opt.git/ctools/random.h: 31
for _lib in _libs.values():
    if not _lib.has("random_dvec_uniform_direction", "cdecl"):
        continue
    random_dvec_uniform_direction = _lib.get("random_dvec_uniform_direction", "cdecl")
    random_dvec_uniform_direction.argtypes = [uint, POINTER(c_double)]
    random_dvec_uniform_direction.restype = None
    break

# /usr/include/limits.h: 84
try:
    UINT_MAX = 4294967295
except:
    pass

# /usr/lib/gcc/x86_64-linux-gnu/11/include/limits.h: 123
# #undef UINT_MAX
try:
    del UINT_MAX
except NameError:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 5
try:
    NN_INF = 1000000
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 6
try:
    NN_LXC37 = 107.856
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 7
try:
    NN_N_HAIRPINLOOP = 31
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 8
try:
    NN_N_BULGELOOP = 31
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 9
try:
    NN_N_INTERNALLOOP = 31
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 10
try:
    NN_ML_OFFSET = 340
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 11
try:
    NN_ML_UNPAIRED = 0
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 12
try:
    NN_ML_STEMS = 40
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 13
try:
    NN_G_NON_GC_PENALTY = 50
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 14
try:
    NN_NINIO_M = 50
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 15
try:
    NN_NINIO_MAX = 300
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/nn_param.h: 16
try:
    NN_NTETRALOOP = 30
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 12
try:
    NA_UNDEF = (UINT_MAX / 2)
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 13
try:
    NA_UNPAIRED = (UINT_MAX / 2)
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 14
try:
    NA_BASE_N = (UINT_MAX / 2)
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 15
try:
    NA_BASE_A = (uint (ord_if_char(0))).value
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 16
try:
    NA_BASE_C = (uint (ord_if_char(1))).value
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 17
try:
    NA_BASE_G = (uint (ord_if_char(2))).value
except:
    pass

# /home/mcm/src/dss-opt/dss-opt.git/na.h: 18
try:
    NA_BASE_U = (uint (ord_if_char(3))).value
except:
    pass

nn_stack = struct_nn_stack# /home/mcm/src/dss-opt/dss-opt.git/na.h: 26

nn_hairpin = struct_nn_hairpin# /home/mcm/src/dss-opt/dss-opt.git/na.h: 31

nn_bulge = struct_nn_bulge# /home/mcm/src/dss-opt/dss-opt.git/na.h: 35

nn_intloop = struct_nn_intloop# /home/mcm/src/dss-opt/dss-opt.git/na.h: 39

nn_multiloop = struct_nn_multiloop# /home/mcm/src/dss-opt/dss-opt.git/na.h: 43

nn_inter = struct_nn_inter# /home/mcm/src/dss-opt/dss-opt.git/na.h: 51

# No inserted files

# No prefix-stripping

