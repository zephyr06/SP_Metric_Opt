data_folder="/home/nvidia/workspace/sdcard/ROS2-SP-APPs/all_time_records"

mpcPath="mpc_response_time.txt"
rrtPath="rrt_response_time.txt"
slamPath="slam_response_time.txt"
tspPath="tsp_response_time.txt"

cd "/home/nvidia/workspace/sdcard/ROS2-SP-APPs/SP_Metric_OPT"

./build/tests/AnalyzeSP_Metric  --mpc_path "$data_folder$mpcPath" \
                                --rrt_path "$data_folder$rrtPath"\
                                --slam_path "$data_folder$slamPath"\
                                --tsp_path "$data_folder$tspPath"
