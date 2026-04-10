# Memory leeks tests

Checking memory leaks with [valgrind](https://valgrind.org/) utility.

## Recommended check procedure

1. Integrate latest changes from macros [run_sim_bmn.C](https://git.jinr.ru/nica/bmnroot/-/blob/dev/macro/run/run_sim_bmn.C) into [run_sim_bmn.cxx](https://git.jinr.ru/nica/bmnroot/-/blob/dev/macro/profiling/run_sim_bmn.cxx).
2. Make **run_sim_bmn** executable. Update [Makefile](https://git.jinr.ru/nica/bmnroot/-/blob/dev/macro/profiling/Makefile) in process.
3. Review [valgrind_sim.sh](https://git.jinr.ru/nica/bmnroot/-/blob/dev/macro/profiling/valgrind_sim.sh) script (see below).
4. Run **valgrind_sim.sh** script and get valgrind log.
5. Review valgrind log. Search for repeated patterns of calls, collect some of them into new suppress file (must be included into **valgrind_sim.sh**). See [Valgrind : Suppressing errors](https://valgrind.org/docs/manual/manual-core.html#manual-core.suppress) and [Valgrind : Writing suppression files](https://valgrind.org/docs/manual/mc-manual.html#mc-manual.suppfiles).
6. If errors from log contain calls for **BmnRoot** objects, methods etc, check it carefully to detect end correct errors in **BmnRoot** code.
7. Repeat from step 4. until you are satisfied with result.
8. When valgrind log is clean enough, run **valgrind_sim.sh** script again with deactivated non-bmn suppressions (see below) to check if new suppressions hides some bmn-related cases. Review and process such cases if found.
9. Repeat from step 1. for [run_reco_bmn](https://git.jinr.ru/nica/bmnroot/-/blob/dev/macro/profiling/run_reco_bmn.cxx) and other scripts / execs if necessary.

## Comments on valgrind_*.sh scripts

First three options are ON all the time, namely:

```
--leak-check=full
--show-leak-kinds=all
--track-origins=yes
```
Option num-callers defines how many entries of call stack to display. It is disputable how many callers to include, but in general, greater value will give better error detection rate.
```
--num-callers=64
```
Then you have multiple suppression files that come with FairRoot / FairSoft packages and must stay as is. Otherwise immense amount of irrelevant errors will be included in log:
```
--suppressions=$ROOTSYS/... .supp
```
Next option gives complete log of errors together with suppressions for each error:
```
--gen-suppressions=all
```
It looks like investigating such a log is more efficient in comparison with other ways. Thus, it is advised to keep this option, but it is not strictly required.

Then we have suppression files from last leaks search sessions:
```
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/bmn_valgrind_2025_06.supp
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/non-bmn_valgrind_2025_06.supp
...
```
Also it is possible to have suppression file with cases that was reviewed but left as is for future investigation. Normally it is deactivated, but can be used to clarify valgrind log:
```
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/left-asis_valgrind_2025_06.supp
```
And, finally, two suppression files from current session:
```
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/_ready.supp
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/_to_confirm.supp
```
One of them contains new **confirmed** suppression templates. Usually it consists of templates which does not include any references to BM@N objects. The other contains new suppression templates that **requires additional confirmation**. Usually it consists of templates which contains some references to BM@N objects. After successful session these two files will form new **bmn_valgrind_<year>_<month>.supp** and **non-bmn_valgrind_<year>_<month>.supp** files which will be added into **valgrind_*.sh** scripts.

Last option is self-explanatory:
```
--log-file=$VMCWORKDIR/macro/profiling/valgrind/val_sim.log
```
