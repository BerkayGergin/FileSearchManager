
from pathlib import Path
import argparse

parser = argparse.ArgumentParser(
    description="Generate FileSearch benchmark data"
)

parser.add_argument("--files", type=int, default=100)
parser.add_argument("--lines", type=int, default=2000)
parser.add_argument("--output", default="benchmark_data")

args = parser.parse_args()

if args.files < 1 or args.lines < 1:
    parser.error("Files and lines must be positive.")

folder = Path(args.output)

# Prevent accidental overwriting of existing files.
if folder.exists() and any(folder.iterdir()):
    raise SystemExit(
        f"Folder '{folder}' is not empty. "
        "Use a new output folder."
    )

folder.mkdir(parents=True, exist_ok=True)

for file_number in range(1, args.files + 1):

    file_path = folder / f"file_{file_number:03d}.txt"

    with file_path.open("w", encoding="utf-8") as file:

        for line_number in range(1, args.lines + 1):

            if line_number % 100 == 0:
                text = "memory management in operating systems"

            elif line_number % 250 == 0:
                text = "Memory architecture notes"

            else:
                text = "CPU scheduling, threads and file systems"

            file.write(
                f"Line {line_number}: {text}\n"
            )

print("Benchmark dataset created successfully!")
print(f"Files created: {args.files}")
print(f"Total lines: {args.files * args.lines}")
print(
    f"Expected 'memory' matches: "
    f"{args.files * (args.lines // 100)}"
)
