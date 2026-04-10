#!/bin/bash
#
# Clang formatting check for Gitlab CI pipeline
#
# Adopted from mpdroot: https://git.jinr.ru/nica/mpdroot/-/blob/eecdbd76e4b3a24300209592d904aa57bee5f5e1/scripts/check_clangFormatting.sh

export TERM=xterm-color

main() {

  # assign basic variables
  CLANG_FORMAT_BIN="clang-format"
  CLANG_STYLE=file
  UNFORMATTED_LABEL="Unformatted"
  MERGE_REQUEST_URL="https://git.jinr.ru/api/v4/projects/$CI_MERGE_REQUEST_PROJECT_ID/merge_requests/$CI_MERGE_REQUEST_IID"
  echo $MERGE_REQUEST_URL
  CHANGED_FILES=""

  # do the actual work
  get_changed_files_list
  if [[ $CHANGED_FILES ]]; then
   check_formatting
   post_label_comment
  else
   echo "This merge request contains no files to format."
  fi

}

function get_changed_files_list() {

  CHANGED_FILES=$(git diff --diff-filter=d --name-only $CI_MERGE_REQUEST_DIFF_BASE_SHA...HEAD |grep -E '.*\.(H|h|hpp|hxx|hh|C|c|cpp|cxx|cc)$')
  echo "Changed files to be checked for formatting:"  $CHANGED_FILES

}

function check_formatting() {
  
  FORMATTED="true"
  UNFORMATTED_FILES=""
  changed_files_array=($CHANGED_FILES)

  for changed_file in "${changed_files_array[@]}"
  do
    output=$($CLANG_FORMAT_BIN -n --style=$CLANG_STYLE $changed_file |& grep -m1 warning)
    
    if [[ $output != "" ]]; then
      if [[ $UNFORMATTED_FILES != "" ]]; then
        UNFORMATTED_FILES="$UNFORMATTED_FILES $changed_file"
      else
        UNFORMATTED_FILES="$changed_file"
        FORMATTED="false"
      fi
    fi
  done

  if [[ $UNFORMATTED_FILES ]]; then
    echo -e "The unformatted files are:\n $UNFORMATTED_FILES \n"
    python services/tests/run-clang-format.py --color=always --style=$CLANG_STYLE  $UNFORMATTED_FILES
    python services/tests/run-clang-format.py --style=$CLANG_STYLE  $UNFORMATTED_FILES >> clang-format.log
  else
    echo -e "There are no unformatted files.\n"
  fi

}

function post_label_comment() {

  FormatInfo=$(curl -s -X GET -H "PRIVATE-TOKEN: $COMMENT_TOKEN" "$MERGE_REQUEST_URL" | grep labels.*\\[.*$UNFORMATTED_LABEL.*\\])

  if [[ $FORMATTED == false ]] && [[ -z $FormatInfo ]]; then
    curl -s -o /dev/null -S -f -X PUT -H "PRIVATE-TOKEN: $COMMENT_TOKEN" "$MERGE_REQUEST_URL?add_labels=$UNFORMATTED_LABEL"
    echo "Unformatted label added."
    author=$(curl -s -X GET -H "PRIVATE-TOKEN: $COMMENT_TOKEN" "$MERGE_REQUEST_URL" | jq -r ".author.username")
    comment="Dear @$author, you have submitted unformatted files. To format them, please run the following command in your bmnroot directory:<br><br>clang-format -i **$UNFORMATTED_FILES**<br><br>You can check the required changes in the <a href='$CI_JOB_URL'>job log</a> or the full <a href='$CI_JOB_URL/artifacts/raw/clang-format.log'>clang-format log</a>.<br>"
    curl -s -o /dev/null -S -f -X POST -d "body=$comment" -H "PRIVATE-TOKEN: $COMMENT_TOKEN" "$MERGE_REQUEST_URL/notes"
    echo "New comment added."
  fi

  if [[ $FORMATTED == true ]] && [[ $FormatInfo ]]; then
    curl -s -o /dev/null -S -f -X PUT -H "PRIVATE-TOKEN: $COMMENT_TOKEN" "$MERGE_REQUEST_URL?remove_labels=$UNFORMATTED_LABEL"
    echo "Unformatted label removed."
  fi

  if [[ $FORMATTED == true ]]; then
    exit 0
  else
    exit 1
  fi

}

main "$@"
