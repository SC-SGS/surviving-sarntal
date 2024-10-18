import matplotlib.pyplot as plt
import numpy as np
import csv

def readFile(name, xName, yName, fitDegree):
    x=[]
    y=[]
    # Read data from CSV file for Physics Step Time
    with open(name, 'r') as file:
        csv_reader = csv.DictReader(file)  # Using DictReader to read data as dictionary
        minX = 100000
        for row in csv_reader:
            newX=float(row[xName])
            if newX < minX:
                minX = newX
            if(newX - minX < 60):
                x.append(float(row[xName]))
                y.append(float(row[yName]))

    x = [i-x[0] for i in x]

    # Polynomial regression for Physics Step Time
    regression = np.poly1d(np.polyfit(x, y, fitDegree), variable='z')
    polyline = np.linspace(0, max(x), 1000)
    yFit = regression(x)

    # Compute residuals and the standard deviation of the residuals
    residuals = y - yFit
    std_res = np.std(residuals)

    # Calculate the 95% confidence interval for Physics Step Time
    confidence_interval = 1.96 * std_res
    return x, y, regression, polyline, yFit, confidence_interval

def plotSim(fileEnding, ax1, color, label):
    filename = 'report/plots/sim' + fileEnding + '.csv'
    xSim, ySim, regSim, polylineSim, yFitSim, confSim = readFile(filename, 'simTime', 'stepTime', 6)

    #ax1.plot(xSim, ySim, color=color, linestyle='-', label='Physics Step Time')
    ax1.plot(polylineSim, regSim(polylineSim), color=color, linestyle='-', label=label)
    ax1.fill_between(xSim, yFitSim - confSim, yFitSim + confSim, color=color, alpha=0.2)
    #ax1.tick_params(axis='y', labelcolor=color)
    #ax1.set_yscale("log", base=10)  # Log scale for Physics Step Time
    ax1.set_ylim([min(ySim) * 0.9, max(ySim) * 1.1])  # Adjust limits

def plotFPS(fileEnding, ax2, color, label):
    filename = 'report/plots/simFPS' + fileEnding + '.csv'
    xFPS, yFPS, regFPS, polylineFPS, yFitFPS, confFPS = readFile(filename, 'simTime', 'fps', 12)
    ax2.plot(xFPS, yFPS, color=color, linestyle='-', label=label)
    #ax2.plot(polylineFPS, regFPS(polylineFPS), color='orange', linestyle='--', label='FPS')
    #ax2.fill_between(xFPS, yFitFPS - confFPS, yFitFPS + confFPS, color="yellow", alpha=0.3, label="95% CI (FPS)")
    #ax2.tick_params(axis='y', labelcolor=color)
    ax2.set_ylim([min(yFPS) * 0.9, max(yFPS) * 1.1])  # Adjust limits


# Create a figure and two subplots side by side
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))  # 1 row, 2 columns

# First plot (Physics Step Time)
ax1.set_xlabel('Runtime (s)')  # Common x-axis label
ax1.set_ylabel('Physics Step Time (s)')
plotSim('10-2', ax1, 'red', r'$\delta t = 0.01, spf=2$')
plotSim('10-4', ax1, 'blue', r'$\delta t = 0.01, spf=4$')
plotSim('15-2', ax1, 'green', r'$\delta t = 0.015, spf=2$')
plotSim('15-4', ax1, 'purple', r'$\delta t = 0.015, spf=4$')

# Plot for FPS (right y-axis)
ax1.set_xlabel('Runtime (s)')
ax2.set_ylabel('FPS')  # Secondary y-axis label
plotFPS('10-2', ax2, 'red', '')
plotFPS('10-4', ax2, 'blue', '')
plotFPS('15-2', ax2, 'green', '')
plotFPS('15-4', ax2, 'purple', '')



# Add legends
fig.tight_layout()  # Adjust layout to make room for both plots
ax1.legend(loc='upper left')
ax2.legend(loc='upper right')

plt.grid(True)
plt.show()
