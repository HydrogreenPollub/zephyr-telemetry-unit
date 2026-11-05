#!/usr/bin/env python3
"""Build and flash the telemetry app using the J-Link runner."""

import subprocess
import sys
from pathlib import Path

project_dir = Path(__file__).resolve().parent.parent
workspace_dir = project_dir.parent
west = workspace_dir / ".venv" / "Scripts" / "west.exe"
if not west.is_file():
    print(f"west executable not found: {west}", file=sys.stderr)
    sys.exit(1)

result = subprocess.run([sys.executable, str(project_dir / "tools" / "build_app.py")])
if result.returncode:
    sys.exit(result.returncode)

result = subprocess.run([
    str(west), "flash", "--runner", "jlink",
    "--build-dir", str(project_dir / "build"),
], cwd=str(workspace_dir))
if result.returncode:
    print("\nTelemetry app flash FAILED.", file=sys.stderr)
    sys.exit(result.returncode)

print("\nTelemetry app flashed successfully.")
