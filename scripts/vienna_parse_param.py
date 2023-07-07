#!/usr/bin/env python

# developed for ViennaRNA-1.7.x/vienna1{3,7}.par
# TODO:
#     - make this module useable from command-line and interpreter:
#       - need to get rid of sys.exit calls, have to be replaced by exceptions
#     - handle printint float values instead of ints, with multiplier
#     - make DEF, INF values configurable
#     - fix the special handling for printing out constants instead of table (the tables
#       aren't really needed in that case
#       and fix the hack for prefixing the C constants
#     - perhaps for C output, put everythin in a struct so that we can have
#       more than one parameter set in the program at the same time, e.g.
#       for RNA, DNA and different versions (e.g. mfold 2.3 and 3.0)
#       python should be ok as we can put each parameter set into its own module
#     - write more robust parsing functions for reading in the tables

import sys, datetime

DEF = -50       # isn't really needed
INF = 1000000   # wrong number, should be removed or set to correct value

class ParseParamError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

#def print_dict(d, indent = ''):
#    if isinstance(d, dict):
#        for i in d.keys():
#            print indent, '%5s' % str(i),
#            if isinstance(d[i], dict):
#                print
#            print_dict(d[i], indent + "    ")
#        print
#    else:
#        print " %5s" % str(d)

def tab_read_line(lines, n, c):
    tab = {}
    found = False
    while not found:
        if lines[n][0] == '#':
            raise ParseParamError, 'read line ' + lines[n] + ', expected numbers'
        if lines[n].find('/*') == -1:
            tt = lines[n].split()
            if len(tt) != len(c):
                raise ParseParamError, 'read line with %d elements, expected %d\n(len(%s) == %d)\n' % (
                    len(tt), len(c), c, len(c)) \
                    + "line: '%s'\n" % lines[n] \
                    + "n = %s" % n
            for i in range(len(tt)):
                tab[c[i]] = tt[i]
            found = True
        n += 1
    return n, tab

def tab_read(lines, n, cfgstr):
    cfg = cfgstr.split(' * ')
    # perhaps some sanity checking on cfg
    # len(cfg) > 0
    # eval(cfg[i]) possible for all i in range(len(cfg))
    if len(cfg) == 1:
        # TODO: fix this. move to own function
        tab = {}
        tt = []
        c = eval(cfg[0])
        while len(tt) < len(c):
            if lines[n][0] == '#':
                print "ERROR: read line " + lines[n],
                print "       expected numbers"
                print len(tt), tt
                print len(c), c
                sys.exit(1)
            if lines[n].find('/*') == -1:
                tt.extend(lines[n].split())
            n += 1
        if len(tt) != len(c):
            print 'ERROR: wrong lengths'
            sys.exit(1)
        for i in range(len(tt)):
            tab[c[i]] = tt[i]
        return n, tab
    else:
        return tab_read_iter(lines, n, cfg)

def tab_read_iter(lines, n, cfg):
    tab = {}
    c = eval(cfg[0])
    if len(cfg) == 1:
        return tab_read_line(lines, n, c)
    for i in c:
        n, tab[i] = tab_read_iter(lines, n, cfg[1:])
    return n, tab

def printval(val):
    if val == 'DEF':
        return DEF
    elif val == 'INF':
        return INF
    else:
        return val

#def tab_print_c_line(tab, c, delim_open, delim_close, indent = ''):
#    print "    %s " % delim_open,
#    for i in c:
#        print "%5s," % printval(str(tab[i])),
#    print "%s," % delim_close

def tab_print_c(origname, name, type, tab, cfgstr, comment):
    cfg = cfgstr.split(' * ')
    print "/* %s: %s" % (name, comment)
    print "   vienna name: %s */" % origname
    dims = ''
    for i in cfg:
        dims += '[%d]' % len(eval(i))
    print "%s %s%s = {" % (type, name, dims)
    tab_print_iter(tab, cfg, True, '{', '}')
    print "};"

def tab_print_py(origname, name, type, tab, cfgstr, comment):
    cfg = cfgstr.split(' * ')
    print "# %s: %s" % (name, comment)
    print "#   vienna name: %s" % origname
    print name, '= ['
    tab_print_iter(tab, cfg, False, '[', ']')
    print "];"

def tab_print_iter(tab, cfg, do_comment, delim_open, delim_close, indent = ''):
    indent += '    '
    c = eval(cfg[0])
    if len(cfg) == 1:
        print indent,
        nl = 0
        for i in c:
            print '%s,' % printval(tab[i]),
            nl += 1
            if nl == 10:
                nl = 0
                print '\n', indent,
        print
    elif len(cfg) == 2:
        c2 = eval(cfg[1])
        tab_print_2d(tab, c, c2, do_comment, delim_open, delim_close, indent)
    else:
        for i in c:
            if do_comment:
                print indent, "%s /* %s */ " % (delim_open, str(i))
            else:
                print indent, "%s " % delim_open
            tab_print_iter(tab[i], cfg[1:], do_comment, delim_open, delim_close, indent)
            print indent, "%s," % delim_close

def tab_print_2d(tab, c1, c2, do_comment, delim_open, delim_close, indent = ''):
    for i in c1:
        if do_comment:
            print indent, "/* %s */ %s" % (i, delim_open),
        else:
            print indent, "%s" % delim_open,
        for j in c2:
            print "%5s," % printval(str(tab[i][j])),
        print " %s," % delim_close

def print_header_c():
    print '/* autogenerated by %s' % sys.argv[0]
    print '   from: %s' % sys.argv[2]
    print '   on:   %s */' % datetime.datetime.now()
    print '#ifndef GUARD_NN_PARAM_H'
    print '#define GUARD_NN_PARAM_H'
    print
    #print '#define DEF %s' % DEF
    print '#define NN_INF %s' % INF
    print '#define NN_LXC37 107.856'
    print

def print_header_py():
    print '# autogenerated by %s' % sys.argv[0]
    print '#   from: %s' % sys.argv[2]
    print '#   on:   %s' % datetime.datetime.now()
    print
    print 'INF = %s' % INF
    print 'LXC37 = 107.856 # param for logarithmic loop energy extrapolation'
    print

def print_footer_c(unhandled, found_end):
    print '/* notes'
    if not found_end:
        print "WARNING: end marker '#END' not found"
    print '\nunhandled:'
    for i in unhandled:
        print '    ', i,
    print '*/'
    print
    print '#endif /* GUARD_NN_PARAM_H */'

def print_footer_py(unhandled, found_end):
    print '# notes'
    if not found_end:
        print "# WARNING: end marker '#END' not found"
    print '#\n# unhandled:'
    for i in unhandled:
        print '#    ', i,
    print

def print_constant_c(name, val):
    print '#define %s %s' % (name, val)

def print_constant_py(name, val):
    print '%s = %s' % (name, val)

# hack for tetraloops
def read_tetraloops(lines, i):
    loops = []
    energies = []
    for i in xrange(i + 1, len(lines)):
        if lines[i][0] == '#':
            break
        v = lines[i].split()
        if len(v) == 2:
            loops.append(v[0])
            energies.append(v[1])
    return i, { 'loops': loops, 'energies': energies }
def print_tetraloops(mode, t):
    if mode == '--py':
        print '# tetraloops'
        print '%s = [' % cfg_tetra_pyname
        for k in xrange(len(t['loops'])):
            loop = [ bases.index(b) for b in t['loops'][k][1:-1] ]
            bp = basepairs.index(t['loops'][k][0] + t['loops'][k][-1])
            print '    (%s, %s, %s),' % (bp, loop, t['energies'][k])
        print ']'
    if mode == '--c':
        print '/* tetraloops */'
        print '%s %s[] = {' % (cfg_tetra_bp['ctype'], cfg_tetra_bp['cname'])
        for k in t['loops']:
            bp = basepairs.index(k[0] + k[-1])
            print '    %s,' % bp
        print '};'
        print '%s %s[][4] = {' % (cfg_tetra_loops['ctype'], cfg_tetra_loops['cname'])
        for k in t['loops']:
            loop = [ bases.index(b) for b in k[1:-1] ]
            print '    { ',
            for b in loop:
                print '%s, ' % b,
            print '},'
            #print '    "%s",' % k
        print '};'
        print '%s %s[] = {' % (cfg_tetra_energies['ctype'], cfg_tetra_energies['cname'])
        for k in t['energies']:
            print '    %s,' % k
        print '};'
        print '#define NN_NTETRALOOP %s' % len(t['loops'])
    print

def print_vienna_param(mode, lines, vienna_cfg, varkey, tab_print, print_header, print_footer, print_constant):
    if len(lines) == 0 or lines[0] != '## RNAfold parameter file\n':
        raise ParseParamError, "missing first line ('## RNAfold parameter file')"
    print_header()
    i = 1
    unhandled = []
    found_end = False
    while i < len(lines) and not found_end:
        line = lines[i]
        if line[0] != '#':
            i += 1
            continue
        if line == '#END\n':
            found_end = True
            continue
        if line[1] == ' ':
            name = line[2:-1]   # remove '# ' prefix and '\n' suffix
            if name == 'Tetraloops':
                # hack for tetraloops
                i, t = read_tetraloops(lines, i)
                print_tetraloops(mode, t)
                i += 1
                continue
            if not name in vienna_cfg:
                unhandled.append(line)
                i += 1
                continue
            i += 1
            cfg = vienna_cfg[name]
            i, t = tab_read(lines, i, cfg['cfgread'])
            if 'desc' in cfg:
                comment = cfg['desc']
            else:
                comment = ' x '.join(cfg['cfgwrite'].split(' * '))
            tab_print(name, cfg[varkey], cfg['ctype'], t, cfg['cfgwrite'], comment)
            # print out additional constants for sections

            # TODO: hack to prefix the constants printed out here for C output
            if varkey == 'cname':
                prefix = 'NN_'
            else:
                prefix = ''

            if name == 'ML_params':
                print_constant(prefix + 'ML_OFFSET', t['offset'])
                print_constant(prefix + 'ML_UNPAIRED', t['unpaired'])
                print_constant(prefix + 'ML_STEMS', t['stems'])
                print_constant(prefix + 'G_NON_GC_PENALTY', t['terminal_non_GC'])
            if name == 'NINIO':
                print_constant(prefix + 'NINIO_M', t['m'])
                print_constant(prefix + 'NINIO_MAX', t['max'])
            if name == 'hairpin':
                print_constant(prefix + 'N_HAIRPINLOOP', len(hairpin_len))
            if name == 'bulge':
                print_constant(prefix + 'N_BULGELOOP', len(bulge_len))
            if name == 'internal_loop':
                print_constant(prefix + 'N_INTERNALLOOP', len(internal_len))
            print
            continue
        else:
            unhandled.append(line)
            i += 1
            continue
    print_footer(unhandled, found_end)


bases = [ 'A', 'C', 'G', 'U' ]
bases_ext = [ 'N', 'A', 'C', 'G', 'U' ]
basepairs = [ 'CG', 'GC', 'GU', 'UG', 'AU', 'UA' ]
basepairs_ext = [ 'CG', 'GC', 'GU', 'UG', 'AU', 'UA', '@@' ]
basepairs_ext2 = [ '--', 'CG', 'GC', 'GU', 'UG', 'AU', 'UA', '@@' ]
hairpin_len = range(31)
internal_len = range(31)
bulge_len = range(31)
ml_param = [ 'unpaired', 'offset', 'stems', 'terminal_non_GC' ]
ninio_param = [ 'm', 'max' ]

# hack for tetraloops
cfg_tetra_pyname = 'TETRALOOPS'
cfg_tetra_bp = {
    'ctype': 'const unsigned int',
    'cname': 'nn_tetraloop_bp',
}
cfg_tetra_loops = {
    'ctype': 'const unsigned int',
    'cname': 'nn_tetraloop_loop',
}
cfg_tetra_energies = {
    'ctype': 'const int',
    'cname': 'nn_G_tetraloop',
}

vienna_tab = {
    'stack_energies': {
        'ctype':    'const int',
        'cname':    'nn_G_stack',
        'pyname':   'G_stack',
        'cfgread':  'basepairs_ext * basepairs_ext',
        'cfgwrite': 'basepairs * basepairs',
    },
    'ML_params': {
        'ctype':    'const int',
        'cname':    'nn_G_ml_param',
        'pyname':   'G_ml_param',
        'cfgread':  'ml_param',
        'cfgwrite': 'ml_param',
        'desc':     str(ml_param),
    },
    'NINIO': {
        'ctype':    'const int',
        'cname':    'nn_G_ninio',
        'pyname':   'G_ninio',
        'cfgread':  'ninio_param',
        'cfgwrite': 'ninio_param',
        'desc':     str(ninio_param),
    },
    'hairpin': {
        'ctype':    'const int',
        'cname':    'nn_G_hairpin_loop',
        'pyname':   'G_hairpin_loop',
        'cfgread':  'hairpin_len',
        'cfgwrite': 'hairpin_len',
    },
    'internal_loop': {
        'ctype':    'const int',
        'cname':    'nn_G_internal_loop',
        'pyname':   'G_internal_loop',
        'cfgread':  'internal_len',
        'cfgwrite': 'internal_len',
    },
    'bulge': {
        'ctype':    'const int',
        'cname':    'nn_G_bulge_loop',
        'pyname':   'G_bulge_loop',
        'cfgread':  'bulge_len',
        'cfgwrite': 'bulge_len',
    },
    'dangle5': {
        'ctype':    'const int',
        'cname':    'nn_G_dangle5',
        'pyname':   'G_dangle5',
        'cfgread':  'basepairs_ext2 * bases_ext',
        'cfgwrite': 'basepairs * bases',
    },
    'dangle3': {
        'ctype':    'const int',
        'cname':    'nn_G_dangle3',
        'pyname':   'G_dangle3',
        'cfgread':  'basepairs_ext2 * bases_ext',
        'cfgwrite': 'basepairs * bases',
    },
    'mismatch_hairpin': {
        'ctype':    'const int',
        'cname':    'nn_G_mismatch_hairpin',
        'pyname':   'G_mismatch_hairpin',
        'cfgread':  'basepairs_ext * bases_ext * bases_ext',
        'cfgwrite': 'basepairs * bases * bases',
    },
    'mismatch_interior': {
        'ctype':    'const int',
        'cname':    'nn_G_mismatch_interior',
        'pyname':   'G_mismatch_interior',
        'cfgread':  'basepairs_ext * bases_ext * bases_ext',
        'cfgwrite': 'basepairs * bases * bases',
    },
    'int11_energies': {
        'ctype':    'const int',
        'cname':    'nn_G_int11',
        'pyname':   'G_int11',
        'cfgread':  'basepairs_ext * basepairs_ext * bases_ext * bases_ext',
        'cfgwrite': 'basepairs * basepairs * bases * bases',
    },
    'int21_energies': {
        'ctype':    'const int',
        'cname':    'nn_G_int21',
        'pyname':   'G_int21',
        'cfgread':  'basepairs_ext * basepairs_ext * bases_ext * bases_ext * bases_ext',
        'cfgwrite': 'basepairs * basepairs * bases * bases * bases',
    },
    'int22_energies': {
        'ctype':    'const int',
        'cname':    'nn_G_int22',
        'pyname':   'G_int22',
        'cfgread':  'basepairs_ext * basepairs_ext * bases * bases * bases * bases',
        'cfgwrite': 'basepairs * basepairs * bases * bases * bases * bases',
    },
}

def main(argv=None):
    if argv is None:
        argv = sys.argv
    # parse command-line arguments
    usage = "usage: " + argv[0] + " [--c|--py] param-file"
    if len(argv) != 3:
        print "ERROR: not enough arguments"
        print usage
        sys.exit(1)
    mode = argv[1]
    filename = argv[2]
    try:
        file = open(filename, 'r')
    except IOError:
        print "ERROR: could not open file " + filename + " for reading"
        sys.exit(1)
    # main program
    lines = file.readlines()
    try:
        if mode == '--c':
            print_vienna_param(mode, lines, vienna_tab, 'cname', tab_print_c, print_header_c,
                               print_footer_c, print_constant_c)
        elif mode == '--py':
            print_vienna_param(mode, lines, vienna_tab, 'pyname', tab_print_py, print_header_py,
                               print_footer_py, print_constant_py)
        else:
            print 'ERROR: unknown mode %s' % mode
            print usage
            sys.exit(1)
    except ParseParamError, err:
        print 'ERROR:', err
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main())
