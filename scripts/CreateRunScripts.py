import os
import sys
from pathlib import Path

def create_shell_script(
    script_path, queue_name="", nodes=1, processes_per_node=1, N=1000, variant=1
):
    script_path = Path(script_path)
    script_path.parent.mkdir(parents=True, exist_ok=True)

    with open(script_path, "w") as f:
        f.write(
            f"""#!/bin/bash
#PBS -q {queue_name}
#PBS -l walltime=00:05:00
#PBS -l select={nodes}:ncpus=8:mpiprocs={processes_per_node}
#PBS -m n
cd $PBS_O_WORKDIR
MPI_NP=$(wc -l $PBS_NODEFILE | awk '{{ print $1 }}')
echo “Number of MPI process: $MPI_NP“
echo 'File $PBS_NODEFILE:'
cat $PBS_NODEFILE
echo
mpirun -machinefile $PBS_NODEFILE -np $MPI_NP ~/lab1/build_release/app {N} {variant}"""
        )

    # Make script executable
    os.chmod(script_path, 0o755)
    print(f"Shell script created: {script_path}")


# Example usage
if __name__ == "__main__":
    queue_name = sys.argv[1] if len(sys.argv) > 1 else ""
    for i in range(1, 3):
        create_shell_script(
            f"scripts/run/run_variant_{i}.sh",
            queue_name=queue_name,
            nodes=1,
            processes_per_node=1,
            N=12500,
            variant=i,
        )
