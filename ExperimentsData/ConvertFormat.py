import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import numpy as np
import seaborn as sns

def is_float(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def gaussian(x, amplitude, mean, std_dev):
    return amplitude * np.exp(-((x - mean) ** 2) / (2 * std_dev ** 2))

def gauss_fit(data):
    # initial_guess = [1.0, np.mean(data), np.std(data)]
    # params, covariance = curve_fit(gaussian, np.arange(len(data)), data, p0=initial_guess)
    # amplitude, mean, std_dev = params
    print("Mean: ", np.mean(data), "Variance: ", np.std(data))



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
                    # print(float(data_elements[data_index]))
            gauss_fit(data_all)
            return data_all

    except FileNotFoundError:
        print(f"File '{file_path}' not found.")
    except Exception as e:
        print(f"An error occurred: {str(e)}")

def write_data(data, file_name):
    path="FormatedData/"+file_name+".txt"
    file=open(path,'w')
    for et in data:
        file.write(str(et)+"\n")

    # sns.kdeplot(data, shade=True)
    # plt.xlim(min(data), max(data))
    print("Limits: ",min(data), max(data))
    plt.xlabel(file_name)
    plt.savefig(file_name+".png")
    # plt.show()

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