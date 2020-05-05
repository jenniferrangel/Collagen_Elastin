#!/bin/bash -l
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mail-user=useremail@address.com
#SBATCH --mail-type=ALL
#SBATCH --job-name="test"
#SBATCH -p gpu # This is the default partition, you can use any of the following; intel, batch, highmem, gpu


module load cmake/3.12.3
module load cuda/9.1
module load extra
module load GCCcore/6.3.0


srun -p gpu --gres=gpu:1 ./build/model -dpull=0.005 matlab_scripts/data_simple.xml
