#!/bin/bash
awk '/#ifdef SOLUTION_CODE/,/#endif/  { if ($0 ~ /#ifdef SOLUTION_CODE/) { keepLine = 0 ;next } else if ($0 ~ /#endif/) {next} else if (keepLine) {  } else { keepLine = $0 ~ /#else/; next }} 1' < $1 > ../02564_stud/$1
