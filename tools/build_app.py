#!/usr/bin/env python3
"""Build the telemetry application for the local Zephyr workspace."""

import subprocess
import sys
from pathlib import Path

project_dir = Path(__file__).resolve().parent.parent
workspace_dir = project_dir.parent
build_dir = project_dir / "build"
west = workspace_dir / ".venv" / "Scripts" / "west.exe"
if not west.is_file():
    print(f"west executable not found: {west}", file=sys.stderr)
    sys.exit(1)

result = subprocess.run([
    str(west), "build", "-b", "telemetry_unit",
    str(project_dir), "-d", str(build_dir),
], cwd=str(workspace_dir))
sys.exit(result.returncode)
