
LAZY_TIMMING_SCRIPT   = ../xcode_bin/lazy_wl_performance
RANK_TIMMING_SCRIPT   = ../xcode_bin/single_vs_double_rank
FAST_MS               = ../fast_ms.py
BDIR                  = input_data
REP                   = 10
## dataset
#TLEN                  = 200000
#SLEN                  = $(addsuffix 000000,32 64 128 256 512)
#RANDOM_DATA_TARGETS   = $(addsuffix _$(TLEN).s,$(addprefix $(BDIR)/random_,$(SLEN)))
#MAX_SLEN              = $(shell python -c "print max(map(int, '$(SLEN)'.split()))")
#MUT_PERIODS           = 10 100 1000 10000 100000
#MUTATION_DATA_TARGETS = $(addsuffix .s,$(addprefix $(BDIR)/mutation_$(MAX_SLEN)_$(TLEN)_,$(MUT_PERIODS)))
#DATA_TARGETS          = $(wildcard $(BDIR)/*.s)
DATA_TARGETS          = $(wildcard $(BDIR)/mut_100Ms_5Mt_1*.s)
RND_TARGETS           = $(wildcard $(BDIR)/rnd*.s)
PREFIXES              = $(notdir $(subst .s,,$(DATA_TARGETS)))
SANDBOX_TARGETS       = $(addsuffix .csv,$(addprefix $(BDIR)/sandbox_timming_,$(notdir $(DATA_TARGETS))))


.PHONY: all clean


all: lazy_vs_nonlazy.csv sandbox_timing.csv

rank_timing.csv : $(RND_TARGETS)
	$(RANK_TIMMING_SCRIPT) -load_cst 1 -s_path $< >$@

sandbox_timing.csv : $(RND_TARGETS)
	$(LAZY_TIMMING_SCRIPT) -load_cst 1 -s_path $< >$@

lazy_vs_nonlazy.csv : lazy.csv nonlazy.csv
	cat $^ | awk -F "," 'NR==1 || $$1!="len_s"' >$@

lazy.csv :  ./script_benchmark.py $(FAST_MS) $(DATA_TARGETS)
	python $< --load_cst --repeat $(REP) --output $@ $(BDIR) --label lazy --lazy_wl --runs_progress 5 --ms_progress 5 $(PREFIXES) --v

nonlazy.csv :  ./script_benchmark.py $(FAST_MS) $(DATA_TARGETS)
	python $< --load_cst --repeat $(REP) --output $@ $(BDIR) --label nonlazy --runs_progress 5 --ms_progress 5 $(PREFIXES) --v


$(BDIR)/mutation_$(MAX_SLEN)_$(TLEN)_%.s : 
	python ../generate_input.py random abcdefghijklmnopqrst \
		$@ $(MAX_SLEN) $(subst .s,.t,$@) $(TLEN) \
		--mutation_period $(subst .s,,$(subst $(BDIR)/mutation_$(MAX_SLEN)_$(TLEN)_,,$@))

$(BDIR)/random_%_$(TLEN).s : 
	python ../generate_input.py random abcdefghijklmnopqrst \
		$@ $(subst _$(TLEN).s,,$(subst $(BDIR)/random_,,$@)) $(subst .s,.t,$@) $(TLEN)


#rm -f $(subst .s,.t,$(DATA_TARGETS)) $(DATA_TARGETS) $(RND_TARGETS) lazy.csv nonlazy.csv lazy_vs_nonlazy.csv sandbox_timing.csv
clean  :
	rm -f lazy.csv nonlazy.csv lazy_vs_nonlazy.csv sandbox_timing.csv

