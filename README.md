
# Revisiting SATZilla in 2024

  
Code for the paper: "Revisiting SATZilla Features in 2024" Submitted to SAT 2024.

## Abstract
Boolean satisfiability (SAT) is an $\mathcal{NP}$-complete problem with important applications, notably in hardware and software verification.
Characterising a SAT instance by a set of features has shown great potential for various tasks, ranging from algorithm selection to benchmark generation.
In this work, we revisit the widely used SATZilla features and introduce a new version of the tool used to compute them.  
In particular, we utilise a new preprocessor and SAT solvers, adjust the code to accommodate larger formulas, and determine better settings of the feature extraction time limits.
We evaluate the extracted features on three downstream tasks: satisfiability prediction, running time prediction, and algorithm selection. 
We observe that our new tool is able to extract features from a broader range of instances than before. 
We show that the new version of the feature extractor produces features that achieve up to $25\%$ lower RMSE for running time prediction, up to $5\%$ higher accuracy for satisfiability prediction, and up to $80\%$ higher closed gap for algorithm selection on benchmarks from recent SAT competitions.


## Usage
We provide precompiled binaries for the SATZilla feature extraction tool for linux. Currently the SATZilla features extractor supports only linux.

SAT-feature-computation code contains the new SATZilla feature extraction tool.

Recompile by running make, then the executable features will be created

To compute features, simply run ./features [-base] [-dia] [-ls] [-lp] [-lobjois] INFILE OUTFILE
Where -lp, -dia etc are the feature groups:

 - base: Base feature group, including pre, KLB, and clause graph
 - dia: Diameter
 - ls: Local search (both GSAT and Sparrow)
 - lp: Linear programming
 - lobjois: Lobjois
 -  cl: Clause learning
 - unit: Unit propagation
 - sp: Survey propagation

  To compute all features use the -all option.
  The input INFILE is a DIMACS CNF file. NOTE: currently, only raw CNF files are supported, and not compressed one (like .cnf.xz)
  The output is a CSV file containing the features for one instance.


## Experiments
rt_pred.py contains the code for running time prediction. sat_pred.py contains the code for satisfiability prediction. Both are using submitit for execution on a SLURM cluster.
For algorithm selection, the scenarios are available in the aslib directory. The AutoFolio code is available at [https://github.com/hadarshavit/AutoFolio].
## Contact
To contact us, please send an email to [shavit@aim.rwth-aachen.de](mailto:shavit@aim.rwth-aachen.de)
