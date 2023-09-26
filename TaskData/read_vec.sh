#!/bin/bash

# Function to read a vector from a text file
read_vector_from_file() {
  local file_path="$1"
  local vector=()

  if [ -e "$file_path" ]; then
    while IFS= read -r line; do
      # Skip lines starting with "#"
      if [[ "$line" == "#"* ]]; then
        continue
      fi

      IFS=' ' read -ra elements <<< "$line"
      for element in "${elements[@]}"; do
        vector+=("$element")
      done
    done < "$file_path"

    # Return the vector
    echo "${vector[@]}"
  else
    echo "File not found: $file_path"
    return 1
  fi
}

# Specify the path to your text file
file_path="pa_res_test_robotics_v1.txt"

# Call the function to read the vector
result_vector=($(read_vector_from_file "$file_path"))

# Check if the function was successful
if [ $? -eq 0 ]; then
  echo "Vector read from file: ${result_vector[@]}"
fi

