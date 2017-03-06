//
//  main.cpp
//  lazy_wl_performance
//
//  Created by denas on 1/28/17.
//  Copyright © 2017 denas. All rights reserved.
//

#include <iostream>
#include "basic.hpp"
#include "fd_ms.hpp"
#include "stree_sct3.hpp"

using namespace std;
using namespace fdms;
using timer = std::chrono::high_resolution_clock;


monitor::size_dict time_wl_calls(string& s_rev, StreeOhleb<>& st, const size_type ntrials, size_type trial_length, monitor::size_dict& time_usage){
    typedef typename StreeOhleb<>::node_type node_type;


    size_type nt = 0;
    size_type i = 0;
    size_type k = s_rev.size() - 1;
    node_type v = st.root();
    auto start_time = timer::now();
    while(nt++ < ntrials){
        for(i = 0; i < trial_length; i++)
            v = st.lazy_wl(v, s_rev[k--]);
        if(i > 0) // finish completing the new node
            st.lazy_wl_followup(v);

        if(k < ntrials)
            k = s_rev.size() - 1;
    }
    time_usage["lazy"] = std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - start_time).count();
    cerr << ntrials << " lazy calls of length " << trial_length << " took " << time_usage["lazy"] << " ms" << endl;


    nt = 0;
    k = s_rev.size() - 1;
    v = st.root();
    start_time = timer::now();
    while(nt++ < ntrials){
        for(i = 0; i < trial_length; i++)
            v = st.wl(v, s_rev[k--]);
        if(k < ntrials)
            k = s_rev.size() - 1;
    }
    time_usage["nonlazy"] = std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - start_time).count();
    cerr << ntrials << " non-lazy calls of length " << trial_length << " took " << time_usage["nonlazy"] << " ms" << endl;

    nt = 0;
    k = s_rev.size() - 1;
    v = st.root();
    start_time = timer::now();
    while(nt++ < ntrials){
        for(i = 0; i < trial_length; i++)
            v = st.lazy_wl(v, s_rev[k--]);
        // SKIP finish completing the new node

        if(k < ntrials)
            k = s_rev.size() - 1;
    }
    time_usage["lazy_nf"] = std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - start_time).count();
    cerr << ntrials << " lazy calls without followup of length " << trial_length << " took " << time_usage["lazy_nf"] << " ms" << endl;

    return time_usage;
}


int main(int argc, char **argv) {
    InputParser input(argc, argv);
    InputFlags flags(input);
    InputSpec S_fwd(input.getCmdOption("-s_path"));
    string s = S_fwd.load_s();

    monitor::size_dict time_usage;
    StreeOhleb<> st;
    auto runs_start = timer::now();
    if(flags.load_stree){
        cerr << "loading the CST T(s) from " << S_fwd.s_fname + ".fwd.stree" << "... ";
        sdsl::load_from_file(st, S_fwd.s_fname + ".fwd.stree");
    } else {
        cerr << "building the CST T(s) of lentgth " << s.size() << "... ";
        sdsl::construct_im(st, s, 1);
    }
    auto runs_stop = timer::now();
    time_usage["dstruct"] = std::chrono::duration_cast<std::chrono::milliseconds>(runs_stop - runs_start).count();
    cerr << "DONE (" << time_usage["dstruct"] / 1000 << " seconds)" << endl;


    cout << "s_path,s,ntrials,nwlcalls,item,time_ms" << endl;
    for(size_type i = 1; i < 500; i = i + 10){
        monitor::size_dict tu = time_wl_calls(s, st, 10000, i, time_usage);
        for(auto item : tu){
            cout << S_fwd.s_fname << "," << s.size() << ",10000," << i << "," << item.first << "," << item.second << endl;
        }
    }
    return 0;
}