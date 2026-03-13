import os
import sys

from enum import Enum
from pathlib import Path

scripts = []

class ProgType(Enum):
    MPE = 1
    MPI = 2

def create_shell_script(
    script_path, queue_name="", nodes=1, processes_per_node=1, N=1000, variant=1, type=ProgType.MPI
):
    scripts.append(script_path)
    script_path = Path(script_path)
    script_path.parent.mkdir(parents=True, exist_ok=True)

    with open(script_path, "w") as f:
        f.write(
            f"""#!/bin/bash
#PBS -q {queue_name}
#PBS -l walltime=00:05:00
#PBS -l select={nodes}:ncpus=8:mpiprocs={processes_per_node}
#PBS -m n
#PBS -o ~/lab1/output/{script_path.name}.log;
#PBS -e ~/lab1/error/{script_path.name}.log;
cd $PBS_O_WORKDIR
mkdir -p ~/lab1/output
mkdir -p ~/lab1/error
MPI_NP=$(wc -l $PBS_NODEFILE | awk '{{ print $1 }}')
echo “Number of MPI process: $MPI_NP“
echo 'File $PBS_NODEFILE:'
cat $PBS_NODEFILE
echo
mpirun -machinefile $PBS_NODEFILE -np $MPI_NP ~/lab1/{"mpi_app" if type == ProgType.MPI else "mpe_app" } {N} {variant}"""
        )

    # Make script executable
    os.chmod(script_path, 0o755)
    print(f"Shell script created: {script_path}")

def create_run_scripts():
    script_path = Path("scripts/run.py")
    script_path.parent.mkdir(parents=True, exist_ok=True)
    with open(script_path, "w") as f:
        f.write("import subprocess\n\n")
        for script in scripts:
            f.write(f"""subprocess.Popen(["qsub", "{script}"])\n""")

pcounts = [1, 2, 4, 8]

if __name__ == "__main__":
    queue_name = sys.argv[1] if len(sys.argv) > 1 else ""
    for i in range(1, 3):
        create_shell_script(
            f"scripts/run/run_mpi_p1_v{i}.sh",
            queue_name=queue_name,
            nodes=1,
            processes_per_node=1,
            N=12500,
            variant=i,
        )

        for j in pcounts:
            create_shell_script(
                f"scripts/run/run_mpi_p{j*2}_v{i}.sh",
                queue_name=queue_name,
                nodes=2,
                processes_per_node=j,
                N=12500,
                variant=i,
            )

        create_shell_script(
            f"scripts/run/run_mpe_p16_v{i}.sh",
            queue_name=queue_name,
            nodes=2,
            processes_per_node=8,
            N=12500,
            variant=i,
        )

    create_run_scripts()