#! /bin/bash
# SO IS1 220B LAB07
# Miłosz Szczepanik
# sm51115@zut.edu.pl



option_p () {
  pids=`ls -d /proc/*/ | tr " " "\n" | cut -c 7- | sed 's/.$//' | sort -n | tr " " "\n"`

  for pid in $pids
  do
    if [[ $pid =~ ^[0-9]+$ ]]; then
      if [ -f "/proc/$pid/status" ]; then
        parent=`cat /proc/$pid/stat | cut -d ' ' -f 4`

        if [ -f "/proc/$pid/status" ]; then
          user_id=`cat /proc/$pid/status | grep "Uid" | cut -f 2`
          username=`id -nu $user_id`
          process_name=`cat /proc/$parent/status | grep "Name" | cut -f 2`

          echo -e "$pid \t $parent \t $user_id ($username) \t $process_name"
        fi
      fi
    fi
  done
}

option_u () {
  pids=`ls -d /proc/*/ | tr " " "\n" | cut -c 7- | sed 's/.$//' | sort -n | tr " " "\n"`

  for pid in $pids
  do
    if [[ $pid =~ ^[0-9]+$ ]]; then
      if [ -f "/proc/$pid/status" ]; then
        user_id=`cat /proc/$pid/status | grep "Uid" | cut -f 2`
        username=`id -nu $user_id`

        if [ "$username" == `whoami` ]; then
          if [ -d "/proc/$pid/cwd" ]; then
            parent=`cat /proc/$pid/stat | cut -d ' ' -f 4`

            if [ -f "/proc/$parent/status" ]; then
              process_name=`cat /proc/$parent/status | grep "Name" | cut -f 2`
              cwd=`cd /proc/$pid/cwd; pwd -P`

              echo -e "$pid \t $process_name \t $cwd"
            fi
          fi
        fi
      fi
    fi
  done
}


while getopts 'pu' OPTION;
do
  case "${OPTION}" in
    p)option_p;;
    u)option_u;;
    *)
      echo "Cos musiales zle wpisac"
      exit 1
      ;;
  esac
done
