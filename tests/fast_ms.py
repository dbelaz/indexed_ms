#!/usr/bin/env python


"""
compute matching statistics
"""


import logging
import sys
import argparse

from utils import MsCommand, get_output, InputSpec, FDMS_PATH

logging.basicConfig(level=logging.INFO)
LG = logging.getLogger(__name__)


def main(opt):
    logging.getLogger().setLevel(logging.DEBUG if opt.v else logging.INFO)

    ispec = InputSpec.infer(opt.base_dir, opt.prefix)
    LG.info("running on %s", ispec)
    res = get_output(MsCommand.fast(ispec,
                                    opt.lazy_wl, opt.sada,
                                    space_usage=True, time_usage=True,
                                    answer=True,
                                    verb=opt.vv,
                                    runs_prg=opt.runs_progress, ms_prg=opt.ms_progress,
                                    path_to_exec=opt.fast_prg))
    print res[-1]


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(
            description=__doc__,
            formatter_class=argparse.ArgumentDefaultsHelpFormatter,
            epilog="Olgert Denas (denas@adobe.com)")
    arg_parser.add_argument('base_dir', type=str, help="directory of input")
    arg_parser.add_argument('prefix', type=str, help="prefix")
    arg_parser.add_argument("--fast_prg", type=str, default=FDMS_PATH,
                            help="c++ program")
    arg_parser.add_argument("--runs_progress", type=int, default=0,
                            help="nr. of progress msgs for runs construction")
    arg_parser.add_argument("--ms_progress", type=int, default=0,
                            help="nr. of progress msgs for ms construction")
    arg_parser.add_argument("--sada", action='store_true',
                            default=False, help="use sdakane's CST")
    arg_parser.add_argument("--lazy_wl", action='store_true',
                            default=False, help="get lazy winer links")
    arg_parser.add_argument("--sada_st", action='store_true',
                            default=False, help="Sadakane's suffix tree")
    arg_parser.add_argument("--v", action='store_true',
                            default=False, help="verbose")
    arg_parser.add_argument("--vv", action='store_true',
                            default=False, help="very verbose")
    sys.exit(main(arg_parser.parse_args()))
