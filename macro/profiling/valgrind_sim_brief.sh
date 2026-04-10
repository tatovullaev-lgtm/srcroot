#bin/bash
#. ~/bmnroot/build/config.sh

valgrind --tool=memcheck		\
	--leak-check=full		\
	--show-leak-kinds=all		\
	--error-limit=no		\
       	--log-file=run_sim_bmn_brief.valgrind	\
       	--suppressions=valgrind.supp	\
	--undef-value-errors=no		\
       	--track-origins=no		\
	./run_sim_bmn
