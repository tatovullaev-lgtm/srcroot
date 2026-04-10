#!/bin/bash

shopt -s globstar

failed=false
results=()

function run_macro {
     if [ -f "$i" ];
     then
          if [ ! "$i" = "$CI_PROJECT_DIR/macro/run/CMakeLists.txt" ] && [ ! "$i" = "$CI_PROJECT_DIR/macro/run/run_sim_bmn.C" ] && [ ! "$i" = "$CI_PROJECT_DIR/macro/run/run_reco_bmn.C" ] && [[ $i = *.C ]];
          then
               macro_name=${i#"$CI_PROJECT_DIR"}
               macro_path=$(dirname ${i})

               mkdir -p logs/${macro_path#"$CI_PROJECT_DIR"}
               echo -e "\033[1;32mExecuting macro: $macro_name\033[0m"
               timeout 30m  root -b -q -l -x "$i" > logs/${macro_path#"$CI_PROJECT_DIR"}/$(basename ${i}).log 2>&1
               exit_code=$?

               if [ $exit_code = 0 ]
               then
                    echo -e "\033[1;32mOK\033[0m processing $i" | tee -a job.log
                    results+=("\033[1;32mOK\033[0m processing $i")
                    status_color="#75ff75"
               else
                    echo -e "\033[0;31mFAILED\033[0m processing $i" | tee -a job.log
                    results+=("\033[0;31mFAILED\033[0m processing $i")
                    status_color="#de5246"
               fi

               mkdir -p html/${macro_path#"$CI_PROJECT_DIR"}
               echo "<tr><td style=\"overflow-wrap: break-word; word-break: break-word\"><a href='$CI_JOB_URL/artifacts/raw/logs${macro_path#"$CI_PROJECT_DIR"}/$(basename ${macro_name}).log'>${macro_name}</a></td><td style=\"background-color: $status_color\">${exit_code}</td></tr>" > html/${macro_path#"$CI_PROJECT_DIR"}/$(basename ${i}).html
          fi
     fi    
}

table='<table class="tablepress"><colgroup><col  style="width: 90%;"><col span="1" style="width: 10%;"></colgroup><thead><tr><th>Macro name</th><th>Exit code</th></tr></thead><tbody>'
num_procs=4
num_jobs="\j"

for i in $CI_PROJECT_DIR/macro/**/*.C
do
     while (( ${num_jobs@P} >= num_procs )); do
          wait -n
     done

     time run_macro $i &
done

table+=$(find html -type f -exec cat {} +)
table+='</tbody></table>'

echo "<p>This page is updated nightly and displays the status of all macros test runs</p>" > result.html
echo "<p align='right'><a href='$CI_PIPELINE_URL'>Generated</a> on $(date)</p>" >> result.html
echo $table >> result.html
