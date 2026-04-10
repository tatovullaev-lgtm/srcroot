If you have time-consuming tasks, many simple tasks or a lot of files to process, you can use _batch systems_ on distributed clusters, such as _SLURM_ on the **HybriLIT platform** including Govorun supercomputer and JINR **CICC complex** or _Sun Grid Engine_ (SGE) on the NICA **ncx-cluster** to essentially accelerate your work.
<br>If you know how to work with SLURM ([SLURM on HybriLIT](http://hlit.jinr.ru/en/user_guide_eng/#_3en)), Sun Grid Engine ([SGE User Guide](https://bmn.jinr.ru/wp-content/%75ploads/2019/08/sge-userguide.pdf)) and Torque systems ([doc index](http://www.adaptivecomputing.com/support/documentation-index/)), you can use directly _**sbatch**_ or _**qsub**_ commands on the clusters to parallelize data processing. Simple examples of user jobs for SLURM, SGE and Torque can be found [here](https://git.jinr.ru/nica_modules/mpd_scheduler/tree/dev/examples/batch). Otherwise, NICA-Scheduler has been developed to simplify running user tasks in parallel.

**NICA-Scheduler** is a module package developed for the ROOT- and FairRoot-based frameworks, such as the BmnRoot and MpdRoot software. It uses an existing batch system (SLURM, SGE and Torque are supported) to distribute user jobs on clusters (as well as supports multithreading on local machines), and simplifies parallel job execution without knowledge of the batch system. NICA-Scheduler can be used as a submodule inside your project or can be compiled as a standalone package. 
<br>Jobs for distributed execution are described and passed to NICA-Scheduler as XML files, e.g.:
> $ nica-scheduler job_description.xml

Additional options can be used to run the scheduler:
<br>   -h/--help              : produce help message
<br>   -f/--file arg          : set XML file path with a job description
<br>   -d/--debug             : debug mode with detailed output
<br>   --db/--database arg    : predefined settings for the [Unified Condition Database](http://ceur-ws.org/Vol-3041/128-132-paper-23.pdf) of the experiment (now arg = bmn supported only)

An example of the NICA-Scheduler job description:
```
<job name="reco_job">
   <macro path="$VMCWORKDIR/macro/run/run_reco.C" start_event=”0” event_count=”1000”>
    <file input="$VMCWORKDIR/macro/run/sim1.root" output="$VMCWORKDIR/macro/run/dst1.root"/>
    <file input="$VMCWORKDIR/macro/run/sim2.root" output="$VMCWORKDIR/macro/run/dst2.root"/>
    <file sim_input="energy=3,gen=dcmqgsm" output="$VMCWORKDIR/macro/run/dst_${counter}.root"/>
   </macro>
   <run mode=“global" count=“25" config=“~/build/config.sh"/>
</job>
```

The XML description of a job to run in a batch system starts with _**&lt;job>**_ tag and ends with closing _&lt;/job>_ tag. The attribute _'name'_ defines the name of the job to identify it. The attribute _'batch'_ defines the batch system of the cluster. The possible values: ‘_sge_’ for Sun Grid Engine, ‘_slurm_’ for SLURM and ‘_torque_’ for the Torque batch system. **Attention!** The default value is ‘slurm’ (SLURM batch system) to run on a cluster, where SLURM is deployed. In addition, predefined settings of the [Unified Condition Database](http://ceur-ws.org/Vol-3041/128-132-paper-23.pdf) for the experiment can be used via _'database'_ attribute (at present, only 'bmn' value is supported).

Tag **_&lt;macro>_** sets information on a ROOT-macro to be executed:
- _path_ – a path of the ROOT macro for distributed execution.
<br>**Important!** Since a job can be started by the Scheduler on any machine of the cluster, the path must point to a shared space (e.g. _/eos/nica/users/$USER/_ volumes on the NICA cluster). This argument is required.
- _start_event_ – a number of the start event to process for all the input files. This argument is optional.
- _event_count_ – a number of events to process for all the input files. This argument is optional.
- _add_args_ – additional last arguments of the ROOT macro, if required.

Tag **_&lt;file>_** is included inside the &lt;macro> tag and contains information on input and output files to process by the macro:
- _input_ – a path to one input file or a set of files in case of regular expressions (?,*,+) to be processed.
- _file_input_ – a path to a text file containing a list of input files separated by new lines.
- _job_input_ – a name of one of the previous jobs, whose output files are input files for the current macro.
- _macro_input_ – a name of one of the previous macros in the same job, whose output files are input files for the current macro.
- _sim_input_ – a string to specify a list of input simulation files forming from the [Unified Condition Database](http://ceur-ws.org/Vol-3041/128-132-paper-23.pdf) if it is used.
- _exp_input_ – a string to specify a list of input experimental (raw) files forming from the [Unified Condition Database](http://ceur-ws.org/Vol-3041/128-132-paper-23.pdf) if it is used.
- _output_ – a path to result files.
<br>**Important!** Since a user job can be started by the NICA-Scheduler on any machine of the cluster, the path must point to a shared space, e.g. EOS volume being available on all cluster nodes.
- _start_event_ – a number of the start event specific for the file. This argument is optional.
- _event_count_ – a number of events of the file to be processed. This argument is optional.
<br>**Important!** If the start_event (or event_count) is set in _&lt;macro>_ and _&lt;file>_ tags then the value in the _&lt;file>_ tag is selected as preferable.
- _parallel_mode_ – a processor count to parallelize event processing for the given input file. This argument is optional.
- _merge_ – whether merge partial output files in the parallel_mode. Default value is _false_, possible values of the attribute: "false" (not merge result files), "true" (merge result files to one output file and delete the partial results), "preserve" (merge result files to one output file and not delete the partial results), "chain" (create the output file containing a TChain object including all partial files).

An example of _sim_input_ string is "energy=3,gen=urqmd", where the following parameters mean as follows: collision energy of selected events is equal to 3 GeV and event generator is UrQMD. All possible parameters are described in [database console utilities](http://bmn.jinr.ru/console-utilities). As a list of input files to be processed can include a lot of files in one _&lt;file>_ tag, special variables can be used in the _output_ argument to set a list of output files:
```
${counter} = counter is corresponding to a regular input file to be processed, the counter starts with 1 and increases by 1 for the next input file.
${input} = absolute path of the input file.
${file_name} = input file name without extension.
${file_name_with_ext} = input file name with extension.
${file_dir_name} = directory name of the input file.
${batch_temp_dir} = path to a batch temp directory.
${user} = username who runs the job.
${first_number} = first number in the input file path.
${last_number} = last number in the input file path.
${first_number_fn} = first number in the input file name (without path).
```
Also, you can use an additional possibility to exclude first ('~' symbol after colon) or last ('-' symbol after colon) characters for the above special variables, e.g. ${file_name_with_ext:~N} is used as an input file name with extension but without first N chars, or ${file_name:-N} - input file name without last N chars.

In case of mass data processing or production, input files are often pre-copied to an intermediate fast storage before processing on the cluster. The possibility is implemented via &lt;put>, &lt;get> and &lt;clean> tags included into the &lt;file> tag. The **_&lt;put>_** tag is used to preliminary copy input files to a temporary storage, and the **_&lt;get>_** tag – copy the result files back from the storage. In addition, the **_&lt;clean>_** tag can be used to list auxiliary files (except temporary input file and result one deleted by default), which will be removed after the processing.
The &lt;put> and &lt;get> tags contain the following attributes:
- _command_ – copy command (possible with parameters) used to transfer files to (&lt;put> tag) or from (&lt;get> tag) an intermediate storage, e.g. "xrdcp -f -p -N".
- _directory_ – a directory path at the intermediate storage to put input files and get result ones.

The &lt;clean> tag uses only _path_ attribute to list additional files to be deleted after processing, e.g. some log or dump files.

Tag _**&lt;run>**_ describes run parameters and allocated resources for the job. It can contain the following arguments:
- _mode_ – an execution mode. ‘_global_’ value specifies distributed processing on a cluster with the batch system, ‘_local_’ – multi-threaded execution on a user multicore computer, ‘_container_’ – same as ‘_global_’, but jobs computed in container environment. The default value is ‘local’.
- _count_ – a maximum number of processors allocated for the job. If the count is greater than a number of files to be processed, the count of the allocated processors is assigned to the number of the files. The default value is 1. You can assign the processor count to 0, then all cores of the user machine are employed in case of the local mode or all processors of the batch system are requested in case of the global mode.
- _config_ – comma separated list of config file paths, where the config represents a bash file setting environment variables (e.g., ROOT environment variables), which will be executed before running the macros. This argument is optional.
- _queue_ – a queue name of the batch system to be used for running the job if it is not as default, e.g. "mpd@bfsrv.jinr-t1.ru" for LIT CICC cluster queue.
- _qos_ –  quality of service (QOS) for each job if required.
- _hosts_ – selected host names separated by comma to process the job, e.g. "ncx201,ncx202" - for running the job only on _ncx201.jinr.ru_ and _ncx202.jinr.ru_ nodes of the NICA cluster. Add '~' (tilda) symbol at the first place for listing only host names, which will be excluded from the job processing.
- _priority_ – a priority of the job (an integer in the range -1023 to 1024 with default value: 0). It is optional argument.
- _memory1_ – a number of operative memory being allocated per one processor slot, but not less than 1 MB, e.g. "4 GB". It is optional.
- _logs_ – a log file path for multi-threaded (_local_) mode. This argument is optional.
- _core_dump_ - whether dump core to 'core.*' files in case of task failures. The default value is _false_, possible values: "false", "true".
- _container_os_ - base OS of the container. Works only with ‘_container_’ mode. ‘_alma9_’  value specifies the AlmaLinux 9 environment. ‘_centos7_’ – CentOS 7 environment. The default value is ‘_alma9_’.

If you want to run a non-ROOT (arbitrary) command by the NICA-Scheduler, use **_&lt;command>_** tag instead of the _&lt;macro>_ one with argument _line_ that means command line for distributed execution by a scheduling system. An example of a job with &lt;_command_> tag:
```
<job>
   <command line="show_simulation_files energy=5-9"/>
   <run mode="global" config="~/build/config.sh"/>
</job>
```
Another example of a job for local multi-threaded execution:
```
<job>
   <macro path=“$VMCWORKDIR/macro/run/run_reco.C">
    <file input=“$VMCWORKDIR/macro/run/sim1.root" output="$VMCWORKDIR/macro/run/dst1.root“ start_event=”0” event_count=”0”/>
    <file input="$VMCWORKDIR/macro/run/sim2.root" output="$VMCWORKDIR/macro/run/dst2.root“ start_event=”0” event_count=”1000” parallel_mode=“5” merge=“true”/>
   </macro>
   <run mode="local" count=“6" config=“~/build/config.sh" logs="processing.log"/>
</job>
```

An XML job description for the NICA-Scheduler can contain more than one job, and a list of user jobs for distributed execution. In this case _&lt;job>_ tags are included in a general **_&lt;jobs>_** tag. Dependencies can be set between different jobs, so the job depending on another one will not start its execution until the latter ends. To set dependency, use _'dependency'_ attribute assigned to the name of another job in the _&lt;job>_ tag.
In addition, a NICA-Scheduler job can contain more than one macro to execute, just use several &lt;macro> tags in the job.

The [examples](https://git.jinr.ru/nica_modules/mpd_scheduler/-/tree/dev/examples) directory of the NICA-Scheduler contains various XML examples. If you have any questions on the NICA-Scheduler, please, feel free to email to gertsen@jinr.ru.
