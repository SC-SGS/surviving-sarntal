import subprocess

def run_command(command):
    """Run a shell command and check for errors."""
    result = subprocess.run(command, shell=True, text=True)
    if result.returncode != 0:
        print(f"Command failed: {command}")
        exit(result.returncode)

# Welcome message
print("Welcome to Bisection Hell")

# Get user inputs
last = input('Please enter the latest (bad) commit of the bisection: ').strip()
first = input('Please enter the earliest (good) commit of the bisection: ').strip()

# Start the bisection
print("Starting Bisection...")
run_command(f'git bisect start {last} {first}')

# Run the test script
try:
    run_command('git bisect run ./build-utils/test.sh')
finally:
    # Always reset bisect, even if the script fails
    print("Resetting bisection...")
    run_command('git bisect reset')
