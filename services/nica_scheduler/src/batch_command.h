#include <map>
#include <string>

struct batch_commands
{
    string scheduler_run_job;
    string array_dependency_option, job_dependency_option;
    string sheduler_priority_option, sheduler_queue_option, sheduler_qos_option, sheduler_workdir_option;
    string operative_memory1_option;
    string sheduler_hosts_option;
    string sheduler_exclude_hosts_option;
    char   sheduler_hosts_separation;
    string set_job_variables, check_status_command, run_command, container_run_command;
    string batch_temp_directory, batch_task_id;
    string job_begin;
    batch_commands(): scheduler_run_job(""), array_dependency_option(""), job_dependency_option(""),
            sheduler_priority_option(""), sheduler_queue_option(""), sheduler_qos_option(""),
            sheduler_workdir_option(""), operative_memory1_option(""),
            sheduler_hosts_option(""), sheduler_exclude_hosts_option(""), sheduler_hosts_separation(','), set_job_variables(""),
            check_status_command(""), run_command(""), container_run_command(""), batch_temp_directory(""),
            batch_task_id(""), job_begin("") {}
    batch_commands(string str_scheduler_run_job, string str_array_dependency_option, string str_job_dependency_option,
            string str_sheduler_priority_option, string str_sheduler_queue_option, string str_sheduler_qos_option,
            string str_sheduler_workdir_option, string str_operative_memory1_option,
            string str_sheduler_hosts_option, string str_sheduler_exclude_hosts_option, char c_sheduler_hosts_separation,
            string str_set_job_variables, string str_check_status_command, string str_run_command, string str_container_run_command,
            string str_batch_temp_directory, string str_batch_task_id, string str_job_begin):
                scheduler_run_job(str_scheduler_run_job), array_dependency_option(str_array_dependency_option),
                job_dependency_option(str_job_dependency_option), sheduler_priority_option(str_sheduler_priority_option),
                sheduler_queue_option(str_sheduler_queue_option), sheduler_qos_option(str_sheduler_qos_option),
                sheduler_workdir_option(str_sheduler_workdir_option), operative_memory1_option(str_operative_memory1_option),
                sheduler_hosts_option(str_sheduler_hosts_option), sheduler_exclude_hosts_option(str_sheduler_exclude_hosts_option),
                sheduler_hosts_separation(c_sheduler_hosts_separation),
                set_job_variables(str_set_job_variables), check_status_command(str_check_status_command),
                run_command(str_run_command), container_run_command(str_container_run_command), batch_temp_directory(str_batch_temp_directory),
                batch_task_id(str_batch_task_id), job_begin(str_job_begin) {}
};

map<BATCH_SYSTEM_NAME, batch_commands> mapBS = {
    { SGE_BATCH_SYSTEM,		// Sun Grid Engine commands
        {
            /*string scheduler_run_job = */"qsub -N %s %s -t 1-%d -tc %d",
            /*string array_dependency_option = */"-hold_jid_ad ",
            /*string job_dependency_option = */"-hold_jid ",
            /*string sheduler_priority_option = */" -p %d",
            /*string sheduler_queue_option = */" -q %s",
            /*string sheduler_qos_option = */"",
            /*string sheduler_workdir_option = */" -wd %s",
            /*string operative_memory1_option = */" -l h_vmem=%sM",
            /*string sheduler_hosts_option = */" -l h=\"(%s)\"",
            /*string sheduler_exclude_hosts_option = */" -l h=\"!(%s)\"",
            /*char   sheduler_hosts_separation = */'|',
            /*string set_job_variables = */" -v ",
            /*string check_status_command = */"qstat",
            /*string run_command = */"qsub",
            /*string container_run_command = */"qrsh",
            /*string batch_temp_directory = */"${TMPDIR}",
            /*string batch_task_id = */"$SGE_TASK_ID",
            /*string job_begin = */"#!/bin/bash\n#$ -j y\n#$ -cwd\n#$ -S /bin/bash\n\n"
                                           "echo \"Start date: `date` (host: `hostname`)\"\n"
                                   "echo \"Current directory: `pwd`\"\n\n"
        }
    },
    { SLURM_BATCH_SYSTEM,	// SLURM commands
        {
            /*string scheduler_run_job = */"sbatch -J %s %s -a 1-%d%%%d",
            /*string array_dependency_option = */"-d aftercorr:",
            /*string job_dependency_option = */"-d afterany:",
            /*string sheduler_priority_option = */" --nice=%d",
            /*string sheduler_queue_option = */" -p %s",
            /*string sheduler_qos_option = */" -q %s",
            /*string sheduler_workdir_option = */" -D %s",
            /*string operative_memory1_option = */" --mem-per-cpu=%sM",
            /*string sheduler_hosts_option = */" -w %s",
            /*string sheduler_exclude_hosts_option = */" -x %s",
            /*char   sheduler_hosts_separation = */',',
            /*string set_job_variables = */" --export=",
            /*string check_status_command = */"squeue",
            /*string run_command = */"sbatch",
            /*string container_run_command = */"srun",
            /*string batch_temp_directory = */"${TMPDIR}",
            /*string batch_task_id = */"$SLURM_ARRAY_TASK_ID",
            /*string job_begin = */"#!/bin/bash\n\n"
                                   "echo \"Start date: `date` (host: `hostname`)\"\n"
                                   "echo \"Current directory: `pwd`\"\n\n"
        }
    },
    { TORQUE_BATCH_SYSTEM,	// Torque commands
        {
            /*string scheduler_run_job = */"qsub -N %s %s -t 1-%d%%%d",
            /*string array_dependency_option = */"-W depend=afteranyarray:", //? unclear for implementation
            /*string job_dependency_option = */"-W depend=afterany:",
            /*string sheduler_priority_option = */" -p %d",
            /*string sheduler_queue_option = */" -q %s",
            /*string sheduler_qos_option = */" -l qos=%s",
            /*string sheduler_workdir_option = */" -w %s",
            /*string operative_memory1_option = */" -l pmem=%smb",
            /*string sheduler_hosts_option = */" -l nodes=%s",
            /*string sheduler_exclude_hosts_option = */" -l excludenodes=%s",
            /*char  sheduler_hosts_separation = */'+',
            /*string set_job_variables = */" -v ",
            /*string check_status_command = */"qstat",
            /*string run_command = */"qsub",
            /*string container_run_command = */"qrsh",
            /*string batch_temp_directory = */"${TMPDIR}",
            /*string batch_task_id = */"$PBS_ARRAYID",
            /*string job_begin = */	"#!/bin/bash\n#PBS -j oe\n#PBS -d .\n#PBS -S /bin/bash\n\n"
                                    "echo \"Start date: `date` (host: `hostname`)\"\n"
                                    "echo \"Current directory: `pwd`\"\n\n"
        }
    }
};
