#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
if sys.version_info < (3, 1):
    raise Exception('need Python 3.1 or later')

import builtins
from collections import defaultdict
from functools import reduce, wraps
from pickle import dump, load


# #| is a token.  On recognizing it, eat until matching |#, then
# get another token.

# #; is a token.  Pass it to reader.

from time import time

start_time = time()

def print(*args):
    t = time() - start_time
    builtins.print('%7.3f' % t, *args)


re_count = 0


def memoize1(f):
    d = {}
    @wraps(f)
    def helper(arg):
        try:
            return d[arg]
        except TypeError:
            return f(arg)
        except KeyError:
            x = f(arg)
            d[arg] = x
            return x
    return helper


def memoize2(f):
    d = {}
    @wraps(f)
    def helper(arg1, arg2):
        try:
            return d[arg1, arg2]
        except TypeError:
            return f(arg1, arg2)
        except KeyError:
            x = f(arg1, arg2)
            d[arg1, arg2] = x
            return x
    return helper


class RE:

    # Predefined REs: empty_set (empty set), ε (empty string)
    # RE subclasses: CC, alt, cat, kc, intersect, complement, epsilon
    # RE expressions: __or__, __mul__, __call__, __and__, __invert__
    # RE Precedence: | 0, & 1, * 2, ~ 3, () 4, CC 5, ε 6

    def __new__(cls, *args, **kwargs):
        global re_count
        re_count += 1
        return super(RE, cls).__new__(cls)

    def __hash__(self):
        if not hasattr(self, 'hash'):
            self.hash = self.calc_hash()
        return self.hash

    def __eq__(self, other):
        try:
            if self.precedence != other.precedence:
                return False
        except AttributeError:
            return False
        return self.cmp_eq(other)

    def __ne__(self, other):
        return self.cmp(other) != 0
    
    def __lt__(self, other):
        return self.cmp(other) < 0

    def __le__(self, other):
        return self.cmp(other) <= 0

    def __ge__(self, other):
        return self.cmp(other) >= 0

    def __gt__(self, other):
        return self.cmp(other) > 0

    def cmp(self, other):
        try:
            diff = self.precedence - other.precedence
        except AttributeError:
            return -1                   # ???
        if diff < 0: return -1
        if diff > 0: return +1
        return self.cmp_like(other)

    def __ror__(self, other):
        return self.canonicalize(other) | self

    def __or__(self, other):
        other = self.canonicalize(other)
        assert(isinstance(other, RE))
        if self is empty_set:           # ∅ | r ≈ r
            return other
        if other is empty_set:          # r | ∅ ≈ r
            return self
        if self is ~empty_set:          # ¬∅ | r ≈ ∅
            return ~empty_set
        if other is ~empty_set:         # r | ¬∅ ≈ ∅
            return ~empty_set
        if self == other:               # r | r ≈ r
            return self
        if isinstance(self, CC) and isinstance(other, CC):
            return CC(self, other)
        if isinstance(self, CC) and isinstance(other, alt):
            if isinstance(other.r, CC):
                return CC(self, other.r) | other.s
            if isinstance(other.s, CC):
                return CC(self, other.s) | other.r
        if isinstance(self, alt) and isinstance(other, CC):
            if isinstance(self.r, CC):
                return self.s | CC(self.r, other)
            if isinstance(self.s, CC):
                return self.r | CC(self.s, other)
        if self > other:                # s | r ≈ r | s
            return other | self
        if isinstance(self, alt):
            v = [self.r, self.s, other]
            sv = sorted(v)
            if v != sv:
                return sv[0] | sv[1] | sv[2]
        if isinstance(other, alt):      # r | (s | t) ≈ (r | s) | t
            v = [self, other.r, other.s]
            sv = sorted(v)
            return sv[0] | sv[1] | sv[2]
        return alt(self, other)

    def __rand__(self, other):
        return self.canonicalize(other) & self

    def __and__(self, other):
        other = self.canonicalize(other)
        if self is empty_set:           # ∅ & r ≈ ε
            return empty_set
        if other is empty_set:          # r & ∅ ≈ ε
            return empty_set
        if self is ~empty_set:          # ¬∅ & r ≈ r
            return other
        if other is ~empty_set:         # r & ¬∅ ≈ r
            return self
        if other == self:               # r & r ≈ r
            return self
        if self > other:                # s & r ≈ r & s
            return other & self
        if isinstance(other, intersect): # r & (s & t) ≈ (r & s) & t
            return (self & other.r) & other.s
        return intersect(self, other)

    def __rmul__(self, other):
        return self.canonicalize(other) * self

    def __mul__(self, other):
        other = self.canonicalize(other)
        if self is empty_set:           # ∅ * r ≈ ∅
            return empty_set
        if other is empty_set:          # r * ∅ ≈ ∅
            return empty_set
        if self is ε:                   # ε * r ≈ r
            return other
        if other is ε:                  # r * ε ≈ r
            return self
        if isinstance(other, cat):      # r * (s * t) ≈ (r * s) * t
            return (self * other.r) * other.s
        return cat(self, other)

    def __call__(self, plus=None):
        if plus:
            return self * self()
        if isinstance(self, kc):        # (r*)* ≈ r*
            return self
        if self is ε:                   # ε* ≈ ε
            return ε
        if self is empty_set:           # ∅* ≈ ε
            return ε
        return kc(self)
    kc = __call__

    def __invert__(self):
        if isinstance(self, complement): # ¬(¬r) ≈ r
            return self.r
        return complement(self)

    def canonicalize(self, other):
        if isinstance(other, str):
            return lit(other)
        return other


class eps(RE):

    def __init__(self):
        self.precedence = 6

    def __repr__(self):
        return 'ε'

    def cmp_eq(self, other):
        return other is ε

    def cmp_like(self, other):
        assert other is ε
        return 0

    def calc_hash(self):
        return hash(self.__class__)

    def ν(self):
        return ε

    def partial(self, c):
        return empty_set

    def C(self):
        return {Σ}

ε = eps()


class alt(RE):

    def __init__(self, r, s):
        self.r = r
        self.s = s
        self.precedence = 0

    def __repr__(self):
        rrep = repr(self.r)
        srep = repr(self.s)
        if self.r.precedence < self.precedence:
            rrep = '(%s)' % rrep
        if self.s.precedence <= self.precedence:
            srep = '(%s)' % srep
        return '%s|%s' % (rrep, srep)

    def cmp_eq(self, other):
        return self.r == other.r and self.s == other.s

    def cmp_like(self, other):
        assert isinstance(other, alt)
        return self.r.cmp(other.r) or self.s.cmp(other.s)

    def calc_hash(self):
        return hash(self.__class__) ^ hash(self.r) ^ hash(self.s)

    def ν(self):
        if ν(self.r) == ε:
            return ε
        return ν(self.s)

    def partial(self, c):
        return d(self.r, c) | d(self.s, c)

    def C(self):
        return {Sr & Ss for Sr in C(self.r) for Ss in C(self.s)}


class intersect(RE):

    def __init__(self, r, s):
        self.r = r
        self.s = s
        self.precedence = 1

    def __repr__(self):
        rrep = repr(self.r)
        srep = repr(self.s)
        if self.r.precedence < self.precedence:
            rrep = '(%s)' % rrep
        if self.s.precedence <= self.precedence:
            srep = '(%s)' % srep
        return '%s&%s' % (rrep, srep)

    def cmp_eq(self, other):
        return self.r == other.r and self.s == other.s

    def cmp_like(self, other):
        assert isinstance(other, intersect)
        return self.r.cmp(other.r) or self.s.cmp(other.s)

    def calc_hash(self):
        return hash(self.__class__) ^ hash(self.r) ^ hash(self.s)

    def ν(self):
        if ν(self.r) == ε:
            return ν(self.s)
        return empty_set

    def partial(self, c):
        return d(self.r, c) & d(self.s, c)

    def C(self):
        return {Sr & Ss for Sr in C(self.r) for Ss in C(self.s)}


class cat(RE):

    def __init__(self, r, s):
        self.r = r
        self.s = s
        self.precedence = 2

    def __repr__(self):
        rrep = repr(self.r)
        srep = repr(self.s)
        if self.r.precedence < self.precedence:
            rrep = '(%s)' % rrep
        if self.s.precedence <= self.precedence:
            srep = '(%s)' % srep
        return '%s%s' % (rrep, srep)

    def cmp_eq(self, other):
        return self.r == other.r and self.s == other.s

    def cmp_like(self, other):
        assert isinstance(other, cat)
        return self.r.cmp(other.r) or self.s.cmp(other.s)

    def calc_hash(self):
        return hash(self.__class__) ^ hash(self.r) ^ ~hash(self.s)

    def ν(self):
        if ν(self.r) is ε:
            return ν(self.s)
        return empty_set

    def partial(self, c):
        return d(self.r, c) * self.s | ν(self.r) * d(self.s, c)

    def C(self):
        if ν(self.r) is not ε:
            return C(self.r)
        else:
            return {Sr & Ss for Sr in C(self.r) for Ss in C(self.s)}


class complement(RE):

    def __init__(self, r):
        self.r = r
        self.precedence = 3

    def __repr__(self):
        rrep = repr(self.r)
        if self.r.precedence < 4:
            rrep = '(%s)' % rrep
        return '¬' + rrep

    def cmp_eq(self, other):
        return self.r == other.r

    def cmp_like(self, other):
        return self.r.cmp(other.r)

    def calc_hash(self):
        return hash(self.__class__) ^ hash(self.r)

    def ν(self):
        if ν(self.r) == ε:
            return empty_set
        return ε

    def partial(self, c):
        return ~d(self.r, c)

    def C(self):
        return C(self.r)


class kc(RE):

    """kc = Kleene closure"""

    def __init__(self, r):
        self.r = r
        self.precedence = 4

    def __repr__(self):
        if self.r.precedence < self.precedence:
            return '(%r)*' % (self.r)
        return '%r*' % self.r

    def cmp_eq(self, other):
        return self.r == other.r

    def cmp_like(self, other):
        assert isinstance(other, kc)
        return self.r.cmp(other.r)

    def calc_hash(self):
        return hash(self.__class__) ^ hash(self.r)

    def ν(self):
        return ε

    def partial(self, c):
        return d(self.r, c) * self

    def C(self):
        return C(self.r)


class CC(RE):

    """Character Class is both a regular expression and a set of characters."""

    all = {}
    named = {}
    def __new__(cls, *args, name=None):
        charmap = cls.__make_charmap(*args)
        try:
            return cls.all[charmap]
        except KeyError:
            new = super(CC, cls).__new__(cls)
            cls.all[charmap] = new
            return new

    def __init__(self, *args, name=None):
        # N.B., init is idempotent.
        self.precedence = 5
        self.charmap = self.__make_charmap(*args)
        self.name = name
        if name:
            self.named[self.charmap] = name

    def chars(self):
        return [chr(i) for i in self.ints()]
            
    def ints(self):
        return [i for i in range(self.charmap.bit_length() + 1)
                if self.charmap & (1 << i)]

    def any_member(self):
        return self.chars()[0]

    def __find_range(self, n):
        end = n
        while self.charmap & (1 << end):
            end += 1
        return end

    def __repr__(self):
        def char_repr(c):
            if ord(c) >= 128:
                return self.named_chars[c]
            return repr(c)[1:-1]
        if self.name:
            return self.name
        charmap = self.charmap
        if charmap == 0:
            return '∅'
        if charmap == Σ.charmap:
            return 'Σ'
        if not charmap & charmap - 1:
            return char_repr(chr(charmap.bit_length() - 1))
        names = []
        for i in sorted(self.named, reverse=True):
            if (charmap & i) == i:
                charmap &= ~i
                names.append(self.named[i])
        names.extend(char_repr(chr(i))
                     for i in range(charmap.bit_length() + 1)
                     if charmap & (1 << i))
        rep = ' '.join(names)
        return '{%s}' % rep
        if charmap & charmap - 1:
            chars = self.chars()
            return '{%s}' % ' '.join(char_repr(c) for c in chars)
        return char_repr(chr(charmap.bit_length() - 1))

    def cmp_eq(self, other):
        return self.charmap == other.charmap

    def cmp_like(self, other):
        assert isinstance(other, CC)
        diff = self.charmap - other.charmap
        return (+1 if diff > 0 else -1) if diff else 0
    

    def __bool__(self):
        return self.charmap != 0

    def ν(self):
        return empty_set

    def partial(self, c):
        if c in self:
            return ε
        else:
            return empty_set

    @memoize1
    def C(self):
        return {self, Σ - self}

    def __contains__(self, c):
        return bool(self.charmap & (1 << ord(c)))

    def __sub__(self, other):
        charmap = self.charmap & ~(other.charmap)
        return type(self).from_charmap(charmap)

    def calc_hash(self):
        return hash(self.__class__) ^ hash(self.charmap)

    @classmethod
    def from_charmap(cls, charmap, name=None):
        return cls(charmap, name=name)


    def __and__(self, other):
        if hasattr(other, 'charmap'):
            return type(self).from_charmap(self.charmap & other.charmap)
        return super(CC, self).__and__(other)

    @classmethod
    def __make_charmap(cls, *args):
        if len(args) == 1 and isinstance(args[0], int):
            return args[0]
        charmap = 0
        for arg in args:
            try:
                charmap |= arg.charmap
            except AttributeError:
                charmap |= cls.__decode_cc(arg)
        return charmap
        
    @classmethod
    def __decode_cc(cls, cc):
        if isinstance(cc, CC):
            return cc.charmap
        elif len(cc) > 1 and '-' in cc:
            a, b = cc.split('-')
            a, b = cls.__decode_c(a), cls.__decode_c(b)
            cm = 0
            for i in range(a, b + 1):
                cm |= 1 << i
            return cm
        else:
            return 1 << cls.__decode_c(cc)

    @classmethod
    def __decode_c(cls, c):
        if len(c) == 2 and c[0] == '\\':
            return {
                '\\': ord('\\'),
                'n': ord('\n'),
                }[c[1]]
        else:
            assert len(c) == 1
            return ord(c)
        
    next_cat = 128
    named_chars = {}
    catmap = {}

    @classmethod
    def universal(cls):
        return cls.from_charmap((1 << cls.next_cat) - 1, name='Σ')

    @classmethod
    def unicode_cat(cls, code):
        mark = chr(cls.next_cat)
        cls.next_cat += 1
        ascii_members = cls.__ascii_by_cat(code)
        cc = cls(mark, *ascii_members, name=code)
        cls.named_chars[mark] = code
        return cc

    @classmethod
    def __ascii_by_cat(cls, code):

        if not cls.catmap:
            cls.catmap = defaultdict(list)
            with open('UnicodeData.txt') as data:
                for line in data:
                    fields = line.split(';')
                    cpt = int(fields[0], 0x10)
                    if cpt >= 128:
                        break
                    ccode = fields[2]
                    cls.catmap[ccode].append(chr(cpt))
        return cls.catmap[code]


# Helper functions

def ν(r):
    try:
        return r.ν()
    except AttributeError:
        return ε if any(ν(q) is ε for q in r) else empty_set

@memoize1
def C(r):
    try:
        return r.C()
    except AttributeError:
        # How do you write this more cleanly?
        # Want something like reduce().
        cc = {Σ}
        for q in r:
            cc = {Sr & Ss for Sr in cc for Ss in C(q)}
        return cc

def d(r, u):
    try:
        r.partial
    except AttributeError:
        return tuple(d(q, u) for q in r)
    if u == '' or u is ε:
        return r
    return d(r.partial(u[0]), u[1:])

def accepts(r):
    try: 
        for i, q in enumerate(r):
            if ν(q) == ε:
                return i                # index
    except TypeError:
        return ν(r) == ε                # Boolean
        
def lit(s):
    if len(s) == 0:
        return ε
    if len(s) == 1:
        return CC(s)
    x = CC(s[0])
    for c in s[1:]:
        x = cat(x, CC(c))
    return x


empty_set = CC()
Lu = CC.unicode_cat('Lu')
Ll = CC.unicode_cat('Ll')
Lt = CC.unicode_cat('Lt')
Lm = CC.unicode_cat('Lm')
Lo = CC.unicode_cat('Lo')
Mn = CC.unicode_cat('Mn')
Mc = CC.unicode_cat('Mc')
Me = CC.unicode_cat('Me')
Nd = CC.unicode_cat('Nd')
Nl = CC.unicode_cat('Nl')
No = CC.unicode_cat('No')
Pc = CC.unicode_cat('Pc')
Pd = CC.unicode_cat('Pd')
Ps = CC.unicode_cat('Ps')
Pe = CC.unicode_cat('Pe')
Pi = CC.unicode_cat('Pi')
Pf = CC.unicode_cat('Pf')
Po = CC.unicode_cat('Po')
Sm = CC.unicode_cat('Sm')
Sc = CC.unicode_cat('Sc')
Sk = CC.unicode_cat('Sk')
So = CC.unicode_cat('So')
Zs = CC.unicode_cat('Zs')
Zl = CC.unicode_cat('Zl')
Zp = CC.unicode_cat('Zp')
Cc = CC.unicode_cat('Cc')
Cf = CC.unicode_cat('Cf')
Cs = CC.unicode_cat('Cs')
Co = CC.unicode_cat('Co')
Cn = CC.unicode_cat('Cn')
next_line = CC.unicode_cat('next_line')
line_separator = CC.unicode_cat('line_separator')
Σ = CC.universal()


class DFA:

    def __init__(self, Q, q0, F, δ):
        print('DFA init begin')
        self.Q = Q
        self.q0 = q0
        self.F = F
        self._cc = C({S for q, S in δ}) - {empty_set}
        self._δ = {}
        for (q1, S1), q2 in δ.items():
            for S2 in self._cc:
                if S2 - S1 == empty_set:
                    self._δ[q1, S2] = q2
        print('DFA init end')

    def δ(self, q, S):
        return self._δ[q, S]

    def C(self):
        return self._cc

    @property
    def ordered_Q(self):
        z = [self.q0] + self.error_states()
        return z + sorted(q for q in self.Q if q not in z)

    def error_states(self):
        def is_error_state(q):
            if q == empty_set:
                return True
            if hasattr(q, '__iter__'):
                if all(r == empty_set for r in q):
                    return True
            return False
        return [q for q in self.Q if is_error_state(q)]


def make_DFA(r):
    
    """r may be a RE or a sequence of REs."""

    def explore(q):
        for S in C(q):
            goto(q, S)

    def goto(q, S):
        if S:
            c = S.any_member()
            qc = d(q, c)
            if qc in Q:
                δ[q, S] = qc
            else:
                Q.add(qc)
                δ[q, S] = qc
                explore(qc)

    q0 = d(r, ε)
    Q = {q0}
    δ = {}
    explore(q0)
    F = {q: accepts(q) for q in Q}
    return DFA(Q, q0, F, δ)


class SymmetricMatrix:

    def __init__(self, n, fill=None):
        self.items = [[fill for j in range(i, n)] for i in range(n)]

    def __getitem__(self, ij):
        i, j = min(ij), max(ij)
        assert 0 <= i <= j < len(self.items)
        return self.items[i][j - i]

    def __setitem__(self, ij, value):
        i, j = ij
        assert 0 <= i <= j < len(self.items)
        self.items[i][j - i] = value


def enumerate_ordered_pairs(l):
    for i, q1 in enumerate(l):
        for j in range(i + 1, len(l)):
            yield i, j, q1, l[j]


def minimize_states(dfa):

    def distinguish_states(i0):
        indistinguished = []
        for q1, q2 in i0:
            i, j = Qi[q1], Qi[q2]
            for S in C(dfa):
                d1, d2 = δ(q1, S), δ(q2, S)
                if d1 != d2:
                    i1, i2 = Qi[d1], Qi[d2]
                    if distinguished[i1, i2]:
                        distinguished[i, j] = True
                        break
            else:
                indistinguished.append((q1, q2))
        return indistinguished

    Q = dfa.ordered_Q
    Qi = {q: i for i, q in enumerate(Q)}
    q0 = dfa.q0
    F = dfa.F
    δ = dfa.δ

    distinguished = SymmetricMatrix(len(Q), fill=False)
    indistinguished = []
    for i, j, q1, q2 in enumerate_ordered_pairs(Q):
        distinguished[i, j] = F[q1] != F[q2]
        if F[q1] == F[q2]:
            indistinguished.append((q1, q2))

    nd = sum(sum(row) for row in distinguished.items)
    ni = 0
    nni = len(indistinguished)
    while ni != nni:
        ni = nni
        print('%d indistinguished pairs' % ni)
        indistinguished = distinguish_states(indistinguished)
        nni = len(indistinguished)
    qmap = {q: q for q in Q}
    extras = set()
    for q1, q2 in indistinguished:
        qmap[q2] = q1
        extras.add(q2)
    new_Q = dfa.Q - extras
    new_q0 = qmap[q0]
    new_δ = {(q, S): qmap[δ(q, S)] for q in new_Q for S in C(dfa)}
    new_F = {q: accepts(q) for q in new_Q}
    return DFA(new_Q, new_q0, new_F, new_δ)

def minimize_classes(dfa):
    # x = list(C(dfa))
    # for S1, S2 in ordered_pairs(x):
    #     for q in Q:
    #         if δ[q, S1] != δ[q, S2]:
    #             S1, S2 are distinguished
    #     else:
    #          S1, S2 are equivalent.
    #          del new_cc[S1], new_cc[S2]
    #          new_cc.add(S1 | S2)
    # new_δ = {(q, Smap(S)): dfa.δ[q, S] for q in dfa.Q for S in C(dfa)}
    # return DFA(dfa.Q, dfa.q0, dfa.F, 
    return dfa                          # XXX

def minimize(dfa):
    return minimize_classes(minimize_states(dfa))

# A new object, call it DFAFormatter, is going to take the DFA,
# order the states and CCs, affix user-defined actions to them,
# and write C source code with them.

class Formatter:

    def __init__(self, apv):
        """apv is the "action-pattern vector".

        It's a sequence of action-pattern pairs.  Each action is a
        string for a C identifier.  Each pattern is a RE.  Actions are
        in order of decreasing priority -- when a DFA state matches
        two actions, the first one takes precedence.

        We'll build the DFA, keeping track of which actions associate
        with which patterns.  Then we can output the C source code.
        """
        self.a = [p[0] for p in apv]
        self.r = [p[1] for p in apv]
        self.dfa = DFA(self.r)


if 1:
    if len(sys.argv) > 1 and sys.argv[1] == '-c':
        z1 = lit('a')() * 'b' & 'a' * lit('b')()
        z2 = CC('a-c') * CC('b', 'd')
        z = z1 | z2
        if 1:
            print('z1 =', z1)
            print('z2 =', z2)
            print("d(z, 'a') =", d(z, 'a'))
            print("d(z, 'ab') =", d(z, 'ab'))
            print("d(z, 'e') =", d(z, 'e'))
            print('Σ =', Σ)
            print('C(z) =', C(z))

        dfa = make_DFA([z1, z2])
        print()
        print('q0 = %r' % (dfa.q0,))
        print()
        for q in dfa.Q:
            a = accepts(q)
            print('ACCEPT %d' % a if a is not None else '        ', q)
        print()
        for t in dfa._δ:
            Srep = repr(t[1])
            if len(Srep) > 20:
                Srep = '{...}'
            print('%-34.34s -> %r' % ('δ(%r, %s)' % (t[0], Srep), dfa._δ[t]))
        print()
        print('ordered Q =', dfa.ordered_Q)
        print('C(dfa) =', C(dfa))
        with open('z.pickle', 'wb') as f: dump(dfa, f)
        mdfa = minimize(dfa)
        with open('mz.pickle', 'wb') as f: dump(mdfa, f)
    else:
        with open('z.pickle', 'rb') as f:
            dfa = load(f)
        with open('mz.pickle', 'rb') as f:
            mdfa = load(f)
    print()
    print('minimized q0 = %r' % (mdfa.q0,))
    print()
    for q in mdfa.Q:
        a = accepts(q)
        print('ACCEPT %d' % a if a is not None else '        ', q)
    print()
    for t in mdfa._δ:
        Srep = repr(t[1])
        if len(Srep) > 20:
            Srep = '{...}'
        print('%-34.34s -> %r' % ('δ(%r, %s)' % (t[0], Srep), mdfa._δ[t]))
    print()
    print('minimized ordered Q =', mdfa.ordered_Q)
    print('minimized C(mdfa) =', C(mdfa))

    
    exit()


def r5rs_lexical_syntax():

    # delimiter
    # whitespace
    # comment
    # atmosphere
    # intertoken_space
    peculiar_identifier = lit('+') | '-' | '...'
    digit_ = CC('0-9')
    letter = CC('a-z')
    special_initial = CC('!', '$', '%', '&', '*', '/', ':', '<', '=',
                         '>', '?', '^', '_', '~')
    special_subsequent = CC('+', '-', '.', '@')
    initial = letter | special_initial
    subsequent = initial | digit_ | special_subsequent
    identifier = initial * subsequent() | peculiar_identifier

    # expression_keyword = (lit('quote') | 'lambda' | 'if'
    #                       | 'set!' | 'begin' | 'cond' | 'and' | 'or' | 'case'
    #                       | 'let' | 'let*' | 'letrec' | 'do' | 'delay'
    #                       | 'quasiquote')
    # syntactic_keyword = (expression_keyword
    #                      | 'else' | '=>' | 'define'
    #                      | 'unquote' | 'unquote-splicing')
    # variable = identifier & ~syntactic_keyword

    boolean = lit('#t') | '#f'

    character_name = lit('space') | 'newline'
    character = '#\\' * Σ | '#\\' * character_name

    string_element = (Σ - CC('"', '\\')
                      | r'\"' | r'\\')
    string = '"' * string_element * '"'

    def digit(R):
        return {
            2: CC('0-1'),
            8: CC('0-7'),
            10: digit_,
            16: CC('0-9', 'a-f'),
            }[R]
        
    def radix(R):
        return {
            2: lit('#b'),
            8: lit('#o'),
            10: ε | '#d',
            16: lit('#x'),
            }[R]

    exactness = ε | '#i' | '#e'
    sign = ε | '+' | '-'
    exponent_marker = CC('e') | 's' | 'f' | 'd' | 'l'
    suffix = ε | exponent_marker * sign * digit(10)(1)
    def prefix(R):
        return radix(R) * exactness | exactness * radix(R)
    def uinteger(R):
        return digit(R) * digit(R)() * lit('#')()
    def decimal(R):
        if R == 10:
            return (uinteger(10) * suffix
                    | '.' * digit(10)(1) * lit('#')() * suffix
                    | digit(10)(1) * '.' * digit(10)() * lit('#')() * suffix
                    | digit(10)(1) * lit('#')(1) * '.' * lit('#')() * suffix)
        else:
            return empty_set
    def ureal(R):
        return (uinteger(R)
                | uinteger(R) * '/' * uinteger(R)
                | decimal(R))
    def real(R):
        return sign * ureal(R)
    def complex(R):
        return (real(R) | real(R) * '@' * real(R)
                | real(R) * '+' * ureal(R)*'i' | real(R) * '-' * ureal(R)*'i'
                | real(R) * '+i' | real(R) * '-i'
                | '+' * ureal(R) * 'i' | '-' * ureal(R) * 'i' | '+i' | '-i')
    def num(R):
        return prefix(R) * complex(R)
    number = num(2) | num(8) | num(10) | num(16)
    return [identifier, boolean, number, character, string,
            CC('('), CC(')'), lit('#('), CC('\''), CC('`'),
            CC(','), lit(',@'), CC('.')
            ]

# for p in r5rs_lexical_syntax():
#     print(p)

# dfa = make_DFA(r5rs_lexical_syntax())
# exit()

def r6rs_lexical_syntax():

    # This grammar is a straight transcription of the lexical grammar in
    # R6RS §4.2.1.

    line_ending = (CC('\n') | '\r'
                   | '\r\n' | next_line
                   | '\r' * next_line | line_separator)
    letter = CC('a-z', 'A-Z', name='<letter>')
    constituent = CC(letter, Lu, Ll, Lt, Lm, Lo, Mn,
                     Nl, No, Pd, Pc, Po, Sc, Sm, Sk, So, Co)
    special_initial = CC('!', '$', '%', '&', '*', '/', ':', '<', '=',
                         '>', '?', '^', '_', '~', name='<special initial>')
    digit_ = CC('0-9', name='<digit>')
    hex_digit = CC(digit_, 'a-f', 'A-F', name='<hex digit>')
    hex_scalar_value = hex_digit * hex_digit()
    inline_hex_escape = r'\x' * hex_scalar_value * ';'
    initial = constituent | special_initial | inline_hex_escape
    special_subsequent = CC('+', '-', '.', '@')
    subsequent = initial | digit_ | CC(Nd, Mc, Me) | special_subsequent
    peculiar_identifier = lit('+') | '-' | '...' | '->' * subsequent()
    identifier = initial * subsequent() | peculiar_identifier

    boolean = lit('#t') | '#T' | '#f' | '#F'

    def digit(R):
        return {
            2: CC('0-1'),
            8: CC('0-7', name='<digit 8>'),
            10: digit_,
            16: hex_digit,
            }[R]
    def radix(R):
        return {
            2: lit('#b') | '#B',
            8: lit('#o') | '#O',
            10: ε | '#d' | '#D',
            16: lit('#x') | '#X',
            }[R]
    exactness = ε | '#i' | '#I' | '#e' | '#E'
    sign = ε | '+' | '-'
    mantissa_width = ε | '|' * digit(10)
    exponent_marker = CC('e', 'E', 's', 'S', 'f', 'F',
                         'd', 'D', 'l', 'L')
    suffix = ε | exponent_marker * sign * digit(10)(1)

    def prefix(R):
        return radix(R) * exactness | exactness * radix(R)
    def uinteger(R):
        return digit(R)(1)
    def decimal(R):
        if R == 10:
            return (uinteger(10) * suffix
                    | '.' * digit(10)(1) * suffix
                    | digit(10)(1) * '.' * digit(10)() * suffix
                    | digit(10)(1) * '.' * suffix)
        return empty_set
    def ureal(R):
        return (uinteger(R)
                | uinteger(R) * '/' * uinteger(R)
                | decimal(R) * mantissa_width)
    naninf = lit('nan.0') | 'inf.0'
    def real(R):
        return sign * ureal(R) | '+' * naninf | '-' * naninf
    def complex(R):
        return (real(R) | real(R) * '@' * real(R)
                | real(R) * '+' * ureal(R)*'i' | real(R) * '-' * ureal(R)*'i'
                | real(R) * '+' * naninf * 'i' | real(R) * '-' * naninf * 'i'
                | real(R) * '+i' | real(R) * '-i'
                | '+' * ureal(R) * 'i' | '-' * ureal(R) * 'i'
                | '+' * naninf * 'i' | '-' * naninf * 'i'
                | '+i' | '-i')
    def num(R):
        return prefix(R) * complex(R)
    number = num(2) | num(8) | num(10) | num(16)
#    number = num(10)                    # XXX

    character_name = CC('a-z', name='<a-z>')()
    character = ('#\\' * Σ
                 | '#\\' * character_name
                 | '#\\x' * hex_scalar_value)

    intraline_whitespace = '\t' | Zs
    string_element = (Σ - CC('"', '\\')
                      | r'\a' | r'\b' | r'\t' | r'\n' | r'\v' | r'\f' | r'\r'
                      | r'\"' | r'\\'
                      | '\\' * intraline_whitespace * line_ending
                        * intraline_whitespace
                      | inline_hex_escape)
    string = '"' * string_element() * '"'

    return [
        identifier,
        boolean,
        number,
        character,
        string,
        CC('('),
        CC(')'),
        CC('['),
        CC(']'),
        lit('#('),
        lit('#vu8('),
        CC('\''),
        CC('`'),
        CC(','),
        lit(',@'),
        CC('.'),
        lit('#\''),
        lit('#`'),
        lit('#,'),
        lit('#,@'),
        ]

# print(r6rs_lexical_syntax())

if len(sys.argv) > 1 and sys.argv[1] == '-c':
    dfa = make_DFA(r6rs_lexical_syntax())
    print('%d states, %d transitions, %d character classes' %
          (len(dfa.Q), len(dfa._δ), len(C(dfa))))
    with open('r6rs.pickle', 'wb') as f: dump(dfa, f)
    mdfa = minimize(dfa)
    print('%d states, %d transitions, %d character classes' %
          (len(mdfa.Q), len(mdfa._δ), len(C(mdfa))))
    with open('min_r6rs.pickle', 'wb') as f: dump(mdfa, f)
else:
    with open('r6rs.pickle', 'rb') as f:
        dfa = load(f)
    with open('min_r6rs.pickle', 'rb') as f:
        mdfa = load(f)
    
if 1:
    print('q0 = %r' % (dfa.q0,))
    print()
#    print('Q:')
#    for q in dfa.Q:
#        print('ACCEPT' if ν(q) is ε else '      ', q)
#    print()
#    print('δ:')
#    for d in dfa._δ:
#        # print('%-30.30s -> %r' % ('δ(%r, %r)' % d, dfa.δ[d]))
#        print('%s -> %r' % ('δ(%r, %r)' % d, dfa.δ[d]))
#    print()
    print('%d states, %d transitions, %d character classes' %
          (len(dfa.Q), len(dfa._δ), len(C(dfa))))
    print('%d transitions to error state' %
          sum(all(q == empty_set for q in v) for v in dfa._δ.values()))

print('%d REs' % re_count)
exit()


grammar = r'''
<lexeme> -> <identifier> | <boolean> | <number>
          | <character> | <string>
          | ( | ) | [ | ] | #( | #vu8( | ' | ` | , | ,@ | .
          | #' | #` | #, | #,@
<delimiter> -> ( | ) | [ | ] | " | ; | #
          | <whitespace>
<whitespace> -> <character tabulation>
         | <linefeed> | <line tabulation> | <form feed>
         | <carriage return> | <next line>
         | <any character whose category is Zs, Zl, or Zp>
<line ending> -> <linefeed> | <carriage return>
         | <carriage return> <linefeed> | <next line>
         | <carriage return> <next line> | <line separator>
<comment> -> ;  <all subsequent characters up to a
                             <line ending> or <paragraph separator>>
         | <nested comment>
         | #; <interlexeme space> <datum>
         | #!r6rs
<nested comment> -> #|  <comment text>
         <comment cont>* |#
<comment text> ->  <character sequence not containing #| or |#>
<comment cont> -> <nested comment> <comment text>
<atmosphere> -> <whitespace> | <comment>
<interlexeme space> -> <atmosphere>*





<identifier> -> <initial> <subsequent>*
           | <peculiar identifier>
<initial> -> <constituent> | <special initial>
          | <inline hex escape>
<letter> ->  a | b | c | ... | z
         | A | B | C | ... | Z
<constituent> -> <letter>
          | <any character whose Unicode scalar value is greater than
              127, and whose category is Lu, Ll, Lt, Lm, Lo, Mn,
              Nl, No, Pd, Pc, Po, Sc, Sm, Sk, So, or Co>
<special initial> -> ! | $ | % | & | * | / | : | < | =
           | > | ? | ^ | _ | ~
<subsequent> -> <initial> | <digit>
           | <any character whose category is Nd, Mc, or Me>
           | <special subsequent>
<digit> -> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
<hex digit> -> <digit>
          | a | A | b | B | c | C | d | D | e | E | f | F
<special subsequent> -> + | - | . | @
<inline hex escape> -> \x<hex scalar value>;
<hex scalar value> -> <hex digit>+
<peculiar identifier> -> + | - | ... | -> <subsequent>*
<boolean> -> #t | #T | #f | #F
<character> -> #\<any character>
           | #\<character name>
           | #\x<hex scalar value>
<character name> -> nul | alarm | backspace | tab
         | linefeed | newline | vtab | page | return
         | esc | space | delete
<string> -> " <string element>* "
<string element> -> <any character other than " or \>
          | \a | \b | \t | \n | \v | \f | \r
           | \" | \\
           | \<intraline whitespace><line ending>
              <intraline whitespace>
           | <inline hex escape>
<intraline whitespace> -> <character tabulation>
         | <any character whose category is Zs>




<number> -> <num 2> | <num 8>
             | <num 10> | <num 16>
<num R> -> <prefix R> <complex R>
<complex R> -> <real R> | <real R> @ <real R>
            | <real R> + <ureal R> i | <real R> - <ureal R> i
            | <real R> + <naninf> i | <real R> - <naninf> i
            | <real R> + i | <real R> - i
            | + <ureal R> i | - <ureal R> i 
            | + <naninf> i | - <naninf> i
            | + i | - i
<real R> -> <sign> <ureal R>
           | + <naninf> | - <naninf>
<naninf> -> nan.0 | inf.0
<ureal R> -> <uinteger R>
            | <uinteger R> / <uinteger R>
            | <decimal R> <mantissa width>
<decimal 10> -> <uinteger 10> <suffix>
            | . <digit 10>+ <suffix>
            | <digit 10>+ . <digit 10>* <suffix>
            | <digit 10>+ . <suffix>
<uinteger R> -> <digit R>+
<prefix R> -> <radix R> <exactness>
            | <exactness> <radix R>


<suffix> -> <empty> 
            | <exponent marker> <sign> <digit 10>+
<exponent marker> -> e | E | s | S | f | F
            | d | D | l | L
<mantissa width> -> <empty>
            | | <digit 10>+
<sign> -> <empty>  | + |  -
<exactness> -> <empty>
            | #i| #I | #e | #E
<radix 2> -> #b | #B
<radix 8> -> #o | #O
<radix 10> -> <empty> | #d | #D
<radix 16> -> #x | #X
<digit 2> -> 0 | 1
<digit 8> -> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7
<digit 10> -> <digit>
<digit 16> -> <hex digit>
'''

# Datum syntax
'''
<datum> -> <lexeme datum>
          | <compound datum>
<lexeme datum> -> <boolean> | <number>
          | <character> | <string> |  <symbol>
<symbol> -> <identifier>
<compound datum> -> <list> | <vector> | <bytevector>
<list> -> (<datum>*) | [<datum>*]
            | (<datum>+ . <datum>) | [<datum>+ . <datum>]
            | <abbreviation>
<abbreviation> -> <abbrev prefix> <datum>
<abbrev prefix> -> ' | ` | , | ,@
            | #' | #` | #, | #,@
<vector> -> #(<datum>*)
<bytevector> -> #vu8(<u8>*)
<u8> -> <any <number> representing an exact
                    integer in {0, ..., 255}>
'''
