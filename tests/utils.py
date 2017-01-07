import logging
import subprocess
import random
import os
from collections import namedtuple


LG = logging.getLogger(__name__)


def bwt(s):
    import numpy as np
    from collections import Counter
    s = s + "#"
    A = np.array([list(s[i:] + s[:i]) for i in range(len(s))])
    As = np.vstack(sorted(A, key=tuple))
    CF = Counter(As[:, 0])
    S = sorted(CF)
    C = [0] * (len(CF) + 1)

    for i in range(len(S)):
        if i == 0:
            continue
        C[i] = C[i-1] + CF[S[i-1]]
    C[i+1] = len(s)
    return "".join(As[:, len(s) - 1]), "".join(S), "-".join(map(str, C))


def _dump_to_file(s, f):
    LG.info("\tdumping to %s ..." % f)
    with open(f, 'w') as fd:
        fd.write(s)
    return f


def _read_chars(fname, skip, n):
    with open(fname) as fd:
        txt = []
        seen = 0
        for line in fd:
            for c in line.rstrip():
                seen += 1
                if seen <= skip:
                    continue
                assert seen >= skip
                if seen > n + skip:
                    break
                txt.append(c)
    return txt


def _gen_chars(alp, l, all_chars=True):
    s = "".join([random.choice(alp) for i in range(l)])
    if not all_chars:
        return s
    while len(set(s)) < len(alp):
        s = "".join([random.choice(alp) for i in range(l)])
    return s


def _dump_bp(inp_fname, out_fname, bp_exec="./bp"):
    subprocess.check_output("{bp_exec} {inp_fname} > {out_fname}"
                            .format(**locals()), shell=True)

def create_input(input_spec, len_t, len_s, source, input_type,
                 mutation_period):
    if input_type == "random":
        seed = random.randint(0, 10000000)
        LG.warning("SEED: %d", seed)
        random.seed(seed)
        alphabet = source
        t = _gen_chars(alphabet, len_t)
        s_fwd = _gen_chars(alphabet, len_s)
    elif input_type == "file":
        t = "".join(_read_chars(source, 0, len_t))
        s_fwd = _read_chars(source, len_t, len_s)
    elif input_type == "mutation":
        seed = random.randint(0, 10000000)
        LG.warning("SEED: %d", seed)
        random.seed(seed)
        alphabet = source
        t = _gen_chars(alphabet, len_t)
        s_fwd = [t[i] if i % mutation_period != 0 else random.choice(alphabet)
                 for i in range(len_t)]
    else:
        raise AttributeError("unknown input type %s" % input_type)

    LG.info("creating input wrt %s", str(input_spec))
    _dump_to_file(t, input_spec.t_path)
    _dump_to_file("".join(s_fwd), input_spec.s_path)


class InputSpec(namedtuple('iii', 'base_dir, prefix')):
    def _path(self, which):
        return os.path.join(self.base_dir, "%s%s.txt" % (self.prefix, which))

    @property
    def t_path(self): return self._path("t")

    @property
    def s_path(self): return self._path("s")


    @property
    def paths(self):
        return [self.t_path, self.s_path]

    @property
    def len_t(self):
        res = subprocess.check_output("/usr/bin/wc -c %s" % self.t_path, shell=True)
        return int(res.split()[0])

    @property
    def len_s(self):
        res = subprocess.check_output("/usr/bin/wc -c %s" % self.s_path, shell=True)
        return int(res.split()[0])


class MsCommand(object):
    @classmethod
    def fast(self, input_spec,
             lazy_wl, sada_st,
             space_usage, time_usage,
             answer, verb,
             path_to_exec):
        cmd_templ = ("{exec_path} -d {dir} -p {prefix} "
                     "-l {lazy} -sada {sada} -s {sp} -t {tm} -a {ans} -v {verb}")
        return (cmd_templ
                .format(exec_path=path_to_exec,
                        dir=input_spec.base_dir,
                        prefix=input_spec.prefix,
                        lazy=int(lazy_wl),
                        sada=int(sada_st),
                        sp=int(space_usage),
                        tm=int(time_usage),
                        ans=int(answer),
                        verb=int(verb)))

    @classmethod
    def slow(self, input_spec, path_to_exec):
        return ("python {exec_path} {dir} {prefix}"
                .format(exec_path=path_to_exec,
                        dir=input_spec.base_dir, prefix=input_spec.prefix))


def get_output(command, *args, **kwargs):
    LG.debug("running: " + str(command))
    res = subprocess.check_output(command, *args, shell=True, **kwargs)
    LG.debug("got: " + res)
    return res.strip().split("\n")
