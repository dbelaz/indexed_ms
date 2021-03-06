//
//  main.cpp
//  fail_vs_nonfail
//
//  Created by denas on 4/18/17.
//  Copyright © 2017 denas. All rights reserved.
//


#include <iostream>
#include "utils.hpp"
#include "fd_ms.hpp"
#include "stree_sct3.hpp"
#define NTRIALS 500000

using namespace std;
using namespace fdms;

typedef typename StreeOhleb<>::node_type node_type;


size_type time_double_rank(string& s_rev, StreeOhleb<>& st, const size_type ntrials){
    size_type k = s_rev.size() - 1;
    size_type failures = 0;
    node_type v = st.root();

    for(size_type i=0; i<ntrials; i++){
        //st.m_csa.bwt.double_rank(v.i, v.j, s_rev[k--]);
        v = st.double_rank_nofail_wl(v, s_rev[k--]);
        if(st.is_root(v))
            failures += 1;
    }
    return failures;
}

size_type time_double_rank_and_fail(string& s_rev, StreeOhleb<>& st, const size_type ntrials){
    size_type k = s_rev.size() - 1;
    size_type failures = 0;
    node_type v = st.root();

    for(size_type i=0; i<ntrials; i++){
        //st.m_csa.bwt.double_rank_and_fail(v.i, v.j, s_rev[k--]);
        v = st.double_rank_fail_wl(v, s_rev[k--]);
        if(st.is_root(v))
            failures += 1;
    }
    return failures;
}


int main(int argc, char **argv) {
    OptParser input(argc, argv);
    InputFlags flags(input);
    //InputSpec S_fwd(input.getCmdOption("-s_path"));
    InputSpec S_fwd("/Users/denas/Desktop/FabioImplementation/software/indexed_ms/tests/lazy_vs_nonlazy_data/input_data/rnd_100Ms_5Mt.s");
    flags.load_stree = true;
    string s = S_fwd.load_s();
    size_type ntrials = (s.size() - 1 > NTRIALS ? NTRIALS : s.size() - 1);
    size_type close = 0;

    Counter time_usage;
    StreeOhleb<> st;

    time_usage["dstruct"] = fdms::load_st(st, s, S_fwd.fwd_cst_fname, true);
    cerr << "DONE (" << time_usage["dstruct"] / 1000 << " seconds)" << endl;

    cout << "s_path,s,ntrials,nwlcalls,item,time_ms" << endl;
    for(size_type i = 1; i < 20; i++){
        auto start_time = timer::now();
        close = time_double_rank(s, st, ntrials);
        time_usage["double_rank"] = std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - start_time).count();
        cerr << ntrials << " double rank wl calls (close = " << close << ") took " << time_usage["single_rank"] << " ms" << endl;
        cout << S_fwd.fwd_cst_fname << "," << s.size() << "," << ntrials << "," << i << ",single," << time_usage["single_rank"] << endl;

        start_time = timer::now();
        close = time_double_rank(s, st, ntrials);
        time_usage["double_rank"] = std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - start_time).count();
        cerr << ntrials << " double rank wl calls (close = " << close << ") took " << time_usage["double_rank"] << " ms" << endl;
        cout << S_fwd.fwd_cst_fname << "," << s.size() << "," << ntrials << "," << i << ",double," << time_usage["double_rank"] << endl;
    }
    return 0;
}
