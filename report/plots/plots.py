import matplotlib.pyplot as plt
import csv
import numpy as np

def sci_notation(num):
    """Returns a LaTeX formatted string for scientific notation."""
    return r'{:.2f} \times 10^{{{}}}'.format(num / 10**int(np.floor(np.log10(abs(num)))), int(np.floor(np.log10(abs(num)))))


# Initialize empty lists for storing x, y_line, and y_bar values
xSim = []
ySim = []
xSimFPS = []
ySimFPS = []
xTest = []
yTest = []

# Read data from CSV file
with open('report/plots/sim.csv', 'r') as file:
    csv_reader = csv.DictReader(file)  # Using DictReader to read data as dictionary
    for row in csv_reader:
        xSim.append(float(row['simTime']))
        ySim.append(float(row['stepTime']))
        
xSim = [x - xSim[0] for x in xSim]

regression = np.poly1d(np.polyfit(xSim, ySim, 4), variable='z')
coeffs = regression.c
#regLabel = r"$y = {}t^4 + {}t^3 + {}t^2 + {}t + {}$".format(sci_notation(coeffs[0]), sci_notation(coeffs[1]), sci_notation(coeffs[2]), sci_notation(coeffs[3]), sci_notation(coeffs[4]))
print(regression)
polyline = np.linspace(0, max(xSim), 1000)
yFit = regression(xSim)

# Compute residuals and the standard deviation of the residuals
residuals = ySim - yFit
std_res = np.std(residuals)

# Calculate the 95% confidence interval
# 95% confidence interval is approximately 1.96 * standard deviation for a normal distribution
confidence_interval = 1.96 * std_res

# Read data from CSV file
with open('report/plots/simFPS.csv', 'r') as file:
    csv_reader = csv.DictReader(file)  # Using DictReader to read data as dictionary
    for row in csv_reader:
        xSimFPS.append(float(row['simTime']))
        ySimFPS.append(float(row['fps']))
        
xSimFPS = [x - xSimFPS[0] for x in xSimFPS]

regression = np.poly1d(np.polyfit(xSimFPS, ySimFPS, 4), variable='z')
coeffs = regression.c
#regLabel = r"$y = {}t^4 + {}t^3 + {}t^2 + {}t + {}$".format(sci_notation(coeffs[0]), sci_notation(coeffs[1]), sci_notation(coeffs[2]), sci_notation(coeffs[3]), sci_notation(coeffs[4]))
print(regression)
polyline = np.linspace(0, max(xSimFPS), 1000)
yFitFPS = regression(xSimFPS)

# Compute residuals and the standard deviation of the residuals
residualsFPS = ySimFPS - yFitFPS
std_resFPS = np.std(residualsFPS)

# Calculate the 95% confidence interval
# 95% confidence interval is approximately 1.96 * standard deviation for a normal distribution
confidence_intervalFPS = 1.96 * std_resFPS


with open('report/plots/test.csv', 'r') as file:
    csv_reader = csv.DictReader(file)
    for row in csv_reader:
        xTest.append(float(row['numRocks']))
        yTest.append(float(row['stepTime']))



# Create a line graph
plt.figure(figsize=(10, 5))  # Set the figure size for better clarity
plt.subplot(1, 2, 1)  # Create subplot 1 (1 row, 2 columns, position 1)
#plt.plot(xSim, ySim, color='blue', linestyle='-', label="values")
plt.plot(polyline, regression(polyline), color='red', linestyle='-')#, label=regLabel)
# Plot confidence interval
plt.fill_between(xSim, yFit - confidence_interval, yFit + confidence_interval, color="gray", alpha=0.3, label="95% Confidence Interval")
plt.ylim(10e-5)
plt.title('Simulation')
plt.xlabel('Runtime')
plt.ylabel('Physics Step Time')
plt.legend()
plt.grid()
plt.yscale("log", base=10)

# Create a bar graph
plt.subplot(1, 2, 2)  # Create subplot 2 (1 row, 2 columns, position 2)
plt.bar(xTest, yTest, color='green')
plt.title('Test Cases')
plt.xlabel('Number of spawned Rocks')
plt.ylabel('Physics Step Time')
plt.legend()
plt.grid

# Show the graphs
plt.tight_layout()  # Adjust the layout to make space between plots
plt.show()