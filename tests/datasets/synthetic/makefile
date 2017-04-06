
DUMP_CST        = /Users/denas/Library/Developer/Xcode/DerivedData/fast_ms-dtwaybjykudaehehgvtglnvhcjbp/Build/Products/Debug/dump_cst
GENERATE_SCRIPT = ../../generate_input.py
MCH = 000000
GCH = $(MCH)000
ALP = abcdefghijklmnopqrst
SLENS  = 10 20 30 40 50 60 70 80 90 100 1000
MINPUT = $(addsuffix .s,$(addprefix mut_100Ms_5Mt_,$(SLENS)))
GINPUT = $(addsuffix .s,$(addprefix mut_1Gs_5Mt_,$(SLENS)))
.SECONDARY : $(MINPUT) $(GINPUT)

all : $(addsuffix .fwd.stree,$(MINPUT) $(GINPUT))

mut_100Ms_5Mt_%.s : $(GENERATE_SCRIPT)
	python $< mutation $(ALP) $@ 100$(MCH) $(subst .s,.t,$@) 5$(MCH) --mutation_period $(subst .s,,$(subst mut_100Ms_5Mt_,,$@))

mut_1Gs_5Mt_%.s : $(GENERATE_SCRIPT)
	python $< mutation $(ALP) $@ 1$(GCH) $(subst .s,.t,$@) 5$(MCH) --mutation_period $(subst .s,,$(subst mut_1Gs_5Mt_,,$@))

rnd_100Ms_5Mt_%.s : $(GENERATE_SCRIPT)
	python $< random $(ALP) $@ 100$(MCH) $(subst .s,.t,$@) 5$(MCH) --mutation_period $(subst .s,,$(subst mut_100Ms_5Mt_,,$@))

rnd_1Gs_5Mt.s : $(GENERATE_SCRIPT)
	python $< random $(ALP) $@ 1$(GCH) $(subst .s,.t,$@) 5$(MCH)

rnd_100Ms_5Mt.s : $(GENERATE_SCRIPT)
	python $< random $(ALP) $@ 100$(MCH) $(subst .s,.t,$@) 5$(MCH)

%.fwd.stree : $(DUMP_CST) %
	$^ 