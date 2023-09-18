
def is_float(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def read_data(file_path, data_index):
    data_all=[]
    try:
        with open(file_path, 'r') as file:
            lines=file.readlines()
            for line in lines:
                line = line.strip()
                data_elements = line.split(" ")
                if(is_float(data_elements[data_index])):
                    data_all.append(float(data_elements[data_index]))
                    print(float(data_elements[data_index]))
            return data_all

    except FileNotFoundError:
        print(f"File '{file_path}' not found.")
    except Exception as e:
        print(f"An error occurred: {str(e)}")

def write_data(data_all, file_name):
    path="FormatedData/"+file_name+".txt"
    file=open(path,'w')
    for et in data_all:
        file.write(str(et)+"\n")

def main():
    data = read_data("MPC_time.txt", 1)
    write_data(data, "MPC")

    data = read_data("TSP_et_gap.txt", 0)
    write_data(data, "TSP")

    data = read_data("RRT_time.txt", 2)
    write_data(data, "RRT")

    data = read_data("SLAM_time.txt", 2)
    write_data(data, "SLAM")

main()