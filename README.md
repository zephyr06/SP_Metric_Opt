# SP_Metric_Optimization


# Dependencies
- [CMake](https://cmake.org/download/)
- [Boost](https://www.boost.org/users/download/)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
- [GTSAM (optional, for unit tests only)](https://github.com/borglab/gtsam)



# Build and Run
```
cd SP_Metric_Opt
mkdir release
cd release
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
./tests/AnalyzePriorityAssignment --file_path TaskData/test_robotics_v1.yaml --output_file_path TaskData/pa_res_test_robotics_v1.txt
```
After running the commmand above, the priority assignment results will be saved in `TaskData/pa_res_test_robotics_v1.txt`



# Read the priority assignment results in bash scripts
There is one script `TaskData/read_vec.sh`, which reads the priority assignment results into an array.



# Modify the task set 
You can always directly modify the yaml file manually for some test purposes. Besides, you can also use the following command to modify it in a bash environment.
```
cd release
./tests/EditYamlFile --task_id 0 --parameter_name execution_time_mu --value 1.23 --file_path TaskData/test_robotics_v1.yaml
```
Although you cannot modify chains using the script above, I think this should not be nececssary during the run-time.



# Modify coefficients related to SP Metric
Go to the file `sources/Safety_Performance_Metric/Parameters.h`, and modify the constructor functions



# Modify optimizaion settings
Go to the file `sources/parameters.yaml`, and modify the granularity parameter.