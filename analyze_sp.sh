# root="/home/nvidia/workspace/sdcard"
root="/home/zephyr/Programming"
data_folder="$root/ROS2-SP-APPs/all_time_records/"

mpcPath="mpc_response_time.txt"
rrtPath="rrt_response_time.txt"
slamPath="slam_response_time.txt"
tspPath="tsp_response_time.txt"

# cd "$root/SP_Metric_OPT"
echo "$data_folder$mpcPath"
cp $data_folder$mpcPath TaskData/AnalyzeSP_Metric/mpc.txt
cp $data_folder$rrtPath TaskData/AnalyzeSP_Metric/rrt.txt
cp $data_folder$slamPath TaskData/AnalyzeSP_Metric/slam.txt
cp $data_folder$tspPath TaskData/AnalyzeSP_Metric/tsp.txt
./build/tests/AnalyzeSP_Metric
