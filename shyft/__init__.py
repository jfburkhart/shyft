import os
from os import path

if os.name == 'nt':
    os.environ['PATH'] = path.join(path.dirname(__file__), 'lib') + ';' + os.environ['PATH']

this_dir = __path__[0]
__version__ = "development"
try:
    from .version import __version__
except:
    pass

if "SHYFT_DATA" in os.environ:
    shyftdata_dir = os.environ["SHYFT_DATA"]
else:
    # If SHYFT_DATA environment variable is not here, then use a decent guess
    shyftdata_dir = path.join(path.dirname(__file__), path.pardir, path.pardir, "shyft-data")
shyftdata_dir = path.normpath(shyftdata_dir)

def print_versions():
    """Print all the versions for packages that Shyft relies on."""
    import numpy
    import sys

    print("-=" * 38)
    print("Shyft version:     %s" % __version__)
    print("NumPy version:     %s" % numpy.__version__)
    print("Python version:    %s" % sys.version)
    if os.name == "posix":
        (sysname, nodename, release, version_, machine) = os.uname()
        print("Platform:          %s-%s" % (sys.platform, machine))
    print("Byte-ordering:     %s" % sys.byteorder)
    print("-=" * 38)


def run_tests():
    import nose
    print_versions()
    nose.main()
