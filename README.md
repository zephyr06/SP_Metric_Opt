# SP_Metric_Optimization

# Dependencies
- [CMake](https://cmake.org/download/)
- [Boost](https://www.boost.org/users/download/)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [GTSAM (optional, for unit tests only)](https://github.com/borglab/gtsam)

# Build and Run
```
cd SP_Metric_Opt
mkdir release
cd release
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
make testOptSingle.run # Optimize a task set described by the yaml file "/TaskData/test_robotics_v1.yaml"
```

# Modify the task set 
You can always directly modify the yaml file manually for some test purposes. Besides, you can also use the following command to modify it in a bash environment.
```
./tests/EditYamlFile --task_id 0 --parameter_name execution_time_mu --value 1.23 --file_path TaskData/test_robotics_v1.yaml
```
Although you cannot modify chains using the script above, I think this should not be nececssary during the run-time.



# Modify coefficients related to SP Metric
Go to the file "sources/Safety_Performance_Metric/Parameters.h", and modify the constructor functions