import json

excluded_dirs = ["spdlog", "raylib", "googletest", "yaml-cpp"]  # Add all your library directories here

with open('../cmake-build-debug/compile_commands.json', 'r') as f:
    commands = json.load(f)

filtered_commands = [cmd for cmd in commands if all(excluded not in cmd['file'] for excluded in excluded_dirs)]

with open('../cmake-build-debug/compile_commands.json', 'w') as f:
    json.dump(filtered_commands, f, indent=2)
